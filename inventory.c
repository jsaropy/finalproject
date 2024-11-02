#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "invfunc.c"

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

#define buffer 50

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

        // Check the username
        get_username(username);

        // Check if username exists in database if not, place in DB (with help of GPT)
        char *get_unames = "SELECT username from users";
        rc = sqlite3_exec(db, get_unames, existence, (void *)username, &err_msg);

        if (rc == SQLITE_OK) {
            printf("Succes\n");
            printf("%s\n", username);
        }
        else {
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
        
        // get password
        get_password(password);
        printf("%s\n", password);

        // get password repeat, function does this until password and password repeat match
        pwrepeat_compare(password, pw_repeat);
        printf("%s\n", pw_repeat);

        //Create sql query with username
        char *insert_username;
        snprintf(insert_username, sizeof(char) * buffer, "INSERT INTO users (username) VALUES (%s)", username);

        //TODO: place username in DB

        //TODO: Initialize libsodium and use pwhash* API

        //TODO: Salt pw??

        //TODO: hash password and place hash in database

        //TODO: place memory free's securely (passwords shouldn't stay in memory for long)

        printf("%s\n", insert_username);

        free(password);
        free(pw_repeat);
        free(username);
        rc = sqlite3_close(db);
    }
}
