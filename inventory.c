#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <sodium.h>

#include "invfunc.h"

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"

#define buffer 51
#define query_buffer 200

typedef struct {
    char *username;
    int session_id;
} user;

int main(void) 
{
    // Open DB connection (with the help of GPT)
    sqlite3 *db;
    char *err_msg = NULL;
    int rc;
    sqlite3_stmt *stmt;

    // Open DB
    rc = sqlite3_open("inventory.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }        

    // Initialize libsodium to use pwhash* API
    if (sodium_init() < 0) {
        fprintf(stderr, RED "ERROR: Failed to initialize Libsodium" RESET);
        return 1;
    }
    // Print menu
    clearscr();
    display_menu();

    // Ask for user input // !! CHECK THIS FOR BETTER DESIGN
    int input = get_number();

    // If input = 3 display the description // !! CHECK THIS FOR BETTER DESIGN
    while (input == 3) {
        clearscr();
        display_description();
        display_menu();
        input = get_number();
    }

    // If input = 1 go to register
    if (input == 1) {
        clearscr();
        display_register();
        
        char *register_username = malloc(buffer * sizeof(char));
        if (register_username == NULL) {
            fprintf(stderr, RED "ERROR: Failed to allocate memory\n" RESET);
            return 1;
        } 
        //initialize all bytes to 0 to prevent unpredictable behavior 
        memset(register_username, 0 , buffer * sizeof(char));

        do {
                compare_usernames(db, register_username);
                fprintf(stderr, RED "ERROR: Username exists\n" RESET);
        } while (compare_usernames(db, register_username) == 0);

        // Allocate memory to get password and password repeat
        char *register_password = malloc(buffer * sizeof(char));
        char *pw_repeat = malloc(buffer * sizeof(char));

        if (register_password == NULL || pw_repeat == NULL) {
            fprintf(stderr, RED "ERROR: Failed to allocate memory\n" RESET);
            return 1;
        }
        memset(register_password, 0 , buffer * sizeof(char));
        memset(pw_repeat, 0 , buffer * sizeof(char));
        
        // get register_password
        get_password(register_password);

        // get register_password repeat, function does this until register_password and register_password repeat match
        pwrepeat_compare(register_password, pw_repeat);

        // hash register_password (unsigned char help with GPT)
        unsigned char hashed_password[crypto_pwhash_STRBYTES];

        if (crypto_pwhash_str((char *)hashed_password, register_password, strlen(register_password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            fprintf(stderr, RED "ERROR: Hashing Failed" RESET);
            return 1;
        }

        free(register_password);
        free(pw_repeat);

        if (create_user(db, register_username, hashed_password) != 0) {
            return 1;
        }
        
        free(register_username);
        sqlite3_finalize(stmt);
        rc = sqlite3_close(db);
    }
    else if (input == 2) { // If input == 2 go to log in
        clearscr();
        display_login();

        char *login_username = malloc(buffer * sizeof(char));
        if (login_username == NULL) {
            fprintf(stderr, RED "ERROR: Failed to allocate memory\n" RESET);
            return 1;
        }
        memset(login_username, 0, buffer * sizeof(char));

        do {
            compare_usernames(db, login_username);
            fprintf(stderr, RED "ERROR: Wrong username\n" RESET);
        } while (compare_usernames(db, login_username) == 1);

        char *login_password = malloc(buffer * sizeof(char));
        if (login_password == NULL) {
            fprintf(stderr, RED "ERROR: Failed to allocate memory\n" RESET);
            return 1;
        }
        memset(login_password, 0, buffer * sizeof(char));
        
        // Get hash linked to username in DB
        char *get_hash = "SELECT user_id, hash FROM users WHERE username LIKE ?;";

        rc = sqlite3_prepare_v2(db, get_hash, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, RED "Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
            return 1;
        }

        rc = sqlite3_bind_text(stmt, 1, login_username, -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            fprintf(stderr, RED "Failed to bind username: %s\n" RESET, sqlite3_errmsg(db));
            return 1;
        }
        
        rc = sqlite3_step(stmt);
        int user_id = sqlite3_column_int(stmt, 0);
        const unsigned char *account_hash = sqlite3_column_text(stmt, 1);

        // sqlite3_step finalize of reset?

        //prompt and compare pw with input
        if (prompt_compare_hash(login_password, account_hash) != 0) {
            return 1;
        }

        free(login_password);

        user activeuser = {login_username, user_id};

        clearscr();
        printf("username = %s\n", activeuser.username);
        printf("id = %i\n", activeuser.session_id);
        display_main(activeuser.username);

        //show current inventory

        //add inventory

        //modify inventory
        
        free(login_username);

    }
}
