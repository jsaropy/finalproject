#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sqlite3.h>
#include <sodium.h>

#include "invfunc.h"

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"

sqlite3_stmt *stmt;
int rc;

void drawline(int a, char line)
{
    for (int i = 0; i < a; i++)
    {
        printf("%c", line);
    }
    printf("\n");
}

void display_menu()
{
    printf("\n");
    printf(BLU "Inventory.c\n" RESET);
    drawline(30, '-');
    printf("\n");
    printf("- Enter '1' to register\n");
    printf("- Enter '2' to login\n");
    printf("- Enter '3' for a description\n");
    printf("\n");
    drawline(30, '-');
}

void display_description()
{
    printf("\n");
    printf(BLU "This is an inventory management system.\n" RESET);
    printf(BLU "This system allows you to easily track the inventory of your business.\n" RESET);
    printf(BLU "Simple functions: add/remove/modify/track the inventory of your business.\n" RESET);
    printf(BLU "PLease register or login to use the service.\n" RESET);
    printf("\n");
}

void clearscr()
{
    system("@cls||clear");
}

int get_number()
{
    char num[10];

    // Check if the numbers provided are between 1-3
    do {
        printf("Enter number: ");
        scanf("%s", num);

        if (atoi(num) < 1 || atoi(num) > 3) {
            fprintf(stderr, RED "ERROR: Please provide a number between 1-3\n" RESET);
        }

    } while (atoi(num) < 1 || atoi(num) > 3);

    return atoi(num);
}

void display_register()
{
    printf(BLU "Register\n" RESET);
    drawline(30, '-');
    printf("\n");
    printf(BLU "Provide an username and password.\n" RESET);
    printf(BLU "The username/password must be between 5-50 characters.\n" RESET);
    printf("\n");
}

char *get_username(char *username) //Can be redesigned more efficiently
{
    printf("Username: ");
    scanf("%s", username);
    int length = strlen(username);

    // Check length of username and prompt again if length is not sufficient
    while (length < 5 || length > 49) {
        fprintf(stderr, RED "ERROR: Username should be between 5-49 characters\n" RESET);
        printf("Username: ");
        scanf("%s", username);
        length = strlen(username);
    }
    return username;
}

int compare_usernames(sqlite3 *db, char *username)
{
    //SQL query to select usernames from table
    char *get_usernames = "SELECT username FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;
    int rc;
        
    // Check if username exists in database if not, place in DB (with help of GPT)
    rc = sqlite3_prepare_v2(db, get_usernames, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, RED "Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return -1;
    }

    rc = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, RED "Failed to bind username: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        return rc; //username exists
        sqlite3_reset(stmt);
    }
    else {
        return rc; //username doesnt exist
        sqlite3_reset(stmt);
    }
}

void get_password(char *password) 
{        
    char *placeholder;
    // get password, repeat until its between 5-50 characters
    do {
    placeholder = getpass("Password: ");
        if (strlen(placeholder) > 50) {
            fprintf(stderr, RED "ERROR: Maximum of 50 characters\n" RESET);
        }
        else if (strlen(placeholder) < 5) {
            fprintf(stderr, RED "ERROR: Your password should be between 5-49 characters\n" RESET);
        }
    } while (strlen(placeholder) < 5 || strlen(placeholder) > 49);
    strncpy(password, placeholder, 49);
}

void pwrepeat_compare(char *password, char *password_repeat)
{
    char *placeholder;

    while (strcmp(password, password_repeat) != 0) {
        placeholder = getpass("Repeat Password: ");
        if (strcmp(placeholder, password) != 0) {
            fprintf(stderr, RED"Error: Passwords don't match\n"RESET);
        }
        else {
            strncpy(password_repeat, placeholder, 49);
        }
    }
}

// create user
int create_user(sqlite3 *db, char *username, unsigned char hash[crypto_pwhash_STRBYTES])
{
    //Create sql query with username
    char *insert_registration = "INSERT INTO users (username, hash) VALUES (?, ?);";
    
    rc = sqlite3_prepare_v2(db, insert_registration, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, RED "Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, RED "Failed to bind username: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_blob(stmt, 2, hash, crypto_pwhash_STRBYTES, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, RED "Failed to bind password: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        printf(BLU "Registration succesful\n" RESET);
        return 0;
    }
    else {
        fprintf(stderr, RED "ERROR: Registration failed: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_reset(stmt);
}

void display_login()
{
    printf(BLU "Login\n" RESET);
    drawline(30, '-');
}

// compare hash with password
int prompt_compare_hash(char *login_password, const unsigned char *hash) 
{
    // prompt and compare password hash. This can be a function
    int max_password_tries = 0;

    do {
        get_password(login_password);

        if (crypto_pwhash_str_verify ((const char *)hash, login_password, strlen(login_password)) != 0) 
        {
            fprintf(stderr, RED "ERROR: Wrong password\n" RESET);
            max_password_tries++;
        }

        if (max_password_tries > 2) 
        {
            fprintf(stderr, RED "Max tries reached, exiting application\n" RESET);
            return 1;
        }

    } while (crypto_pwhash_str_verify ((const char *)hash, login_password, strlen(login_password)) != 0);
    return 0;
}

void display_main(char *username)
{
    drawline(70, '-');
    printf(BLU "                 Hi %s, welcome to Inventory.c\n" RESET, username);
    drawline(70, '-');
    printf("\n");
    printf(BLU "                     Enter '1' to add an item\n" RESET);
    printf(BLU "                     Enter '2' to remove an item\n" RESET);
    printf(BLU "                     Enter '3' to modify stock\n" RESET);
    printf("\n");
}

// show inventory select * from view
int show_inventory(sqlite3 *db, int session_id) 
{
    char *user_inventory = "select * from user_inventory where user_id = ?;";

    rc = sqlite3_prepare_v2(db, user_inventory, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_int(stmt, 1, session_id);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "Failed to bind ID: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    int col_count = sqlite3_column_count(stmt);

    drawline(70, '-');
    printf("%-10s %-10s %-22s %-14s %-14s\n", "User ID", "Prod ID", "Name", "Quantity", "Value");
    drawline(70, '-');

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        int user_id = sqlite3_column_int(stmt, 0);
        int prod_id = sqlite3_column_int(stmt, 1);
        const unsigned char *prod_name = sqlite3_column_text(stmt, 2);
        int quantity = sqlite3_column_int(stmt, 3);
        float value = sqlite3_column_double(stmt, 4);

        printf("%-10d %-10d %-22s %-14d €%-14.2f", user_id, prod_id, prod_name, quantity, value);
        printf("\n");
        drawline(70, '-');
    }

    sqlite3_reset(stmt);
    return 0;
}


