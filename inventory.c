#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <unistd.h>
#include <sodium.h>

#include "invfunc.c"

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

#define buffer 51
#define query_buffer 200

typedef struct user {
    int session_id;
    char *username;
} user;

int main(void) 
{
    // Open DB connection (with the help of GPT)
    sqlite3 *db;
    char *err_msg = NULL;
    int rc;

    // Open DB
    rc = sqlite3_open("inventory.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }        

    //TODO: Initialize libsodium to use pwhash* API
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
        
        char *username = malloc(buffer * sizeof(char));
        if (username == NULL) {
            fprintf(stderr, RED "ERROR: Failed to allocate memory\n" RESET);
            return 1;
        } 
        //initialize all bytes to 0 to prevent unpredictable behavior 
        memset(username, 0 , buffer * sizeof(char));

        // Check the username
        get_username(username);

        // Check if username exists in database if not, place in DB (with help of GPT)
        char *get_exunames = "SELECT username from users";
        rc = sqlite3_exec(db, get_exunames, existence, (void *)username, &err_msg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, RED "SQL error: %s\n" RESET, err_msg);
            sqlite3_free(err_msg);
        }

        // Allocate memory to get password and password repeat
        char *password = malloc(buffer * sizeof(char));
        char *pw_repeat = malloc(buffer * sizeof(char));

        if (password == NULL || pw_repeat == NULL) {
            fprintf(stderr, RED "ERROR: Failed to allocate memory\n" RESET);
            return 1;
        }
        memset(password, 0 , buffer * sizeof(char));
        memset(pw_repeat, 0 , buffer * sizeof(char));
        
        // get password
        get_password(password);
        printf("%s\n", password);

        // get password repeat, function does this until password and password repeat match
        pwrepeat_compare(password, pw_repeat);
        printf("%s\n", pw_repeat);

        //TODO: hash password (unsigned char help with GPT)
        unsigned char hashed_password[crypto_pwhash_STRBYTES];

        if (crypto_pwhash_str((char *)hashed_password, password, strlen(password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            fprintf(stderr, RED "ERROR: Hashing Failed" RESET);
            return 1;
        }

        printf("%s", hashed_password);

        free(password);
        free(pw_repeat);

        //Create sql query with username
        char insert_registration[query_buffer];
        snprintf(insert_registration, sizeof(char) * query_buffer, "INSERT INTO users (username, hash) VALUES ('%s', '%s')", username, hashed_password);

         //TODO: place memory free's securely (passwords shouldn't stay in memory for long)
        printf("%s\n", insert_registration);

        //TODO: place username and hashed pw in DB
        rc = sqlite3_exec(db, insert_registration, 0, 0, &err_msg);

        if (rc == SQLITE_OK) {
            printf(BLU "Registration succesful\n" RESET);
            rc = sqlite3_close(db);
            free(username);
            return 0;
        }
        else {
            fprintf(stderr, RED "ERROR: Registration failed: %s\n" RESET, err_msg);
            return 1;
        }
        
        free(username);
        rc = sqlite3_close(db);
    }
    else if (input == 2) { // If input == 2 go to log in
        clearscr();
        display_login();
    }

    

}
