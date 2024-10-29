#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <strings.h>
#include "invfunc.c"

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

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

    // Ask for user input
    int input = get_number();

    // If input = 3 display the description 
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
        
        int buffer = 50;
        char *username = malloc(buffer * sizeof(char));

        if (username == NULL) {
            fprintf(stderr, RED "ERROR: Failed to allocate memory\n" RESET);
            return 1;
        } 

        // You can put this in a function named check length
        printf("Username: ");
        scanf("%49s", username);

        // Check the length of the username
        check_length(username);

        // Check if username exists in database if not, place in DB (with help of GPT)
        char *sql = "SELECT username from users";
        rc = sqlite3_exec(db, sql, existence, (void *)username, &err_msg);

        if (rc == SQLITE_OK) {
            printf("Succes\n");
            printf("%s\n", username);
        }
        else {
            fprintf(stderr, RED "SQL error: %s\n" RESET, err_msg);
            sqlite3_free(err_msg);
        }

        free(username);
        rc = sqlite3_close(db);
    }
}
