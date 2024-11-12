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
    printf("Enter number: ");
    scanf("%s", num);

    // Check if the numbers provided are between 1-3
    while (atoi(num) < 1 || atoi(num) > 3)
    {
        fprintf(stderr, RED "ERROR: Please provide a number between 1-3\n" RESET);
        printf("Enter number: ");
        scanf("%s", num);
    }
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

char *get_username(char *usrname)
{
    printf("Username: ");
    scanf("%s", usrname);
    int length = strlen(usrname);

    // Check length of username and prompt again if length is not sufficient
    while (length < 5 || length > 49) {
        fprintf(stderr, RED "ERROR: Username should be between 5-49 characters\n" RESET);
        printf("Username: ");
        scanf("%s", usrname);
        length = strlen(usrname);
    }
    return usrname;
}

int compare_usernames(sqlite3 *db, char *username)
{
    //SQL query to select usernames from table
    char *get_unames = "SELECT username FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;
    int rc;

    // Get & Check the username
    get_username(username);
        
    // Check if username exists in database if not, place in DB (with help of GPT)
    rc = sqlite3_prepare_v2(db, get_unames, -1, &stmt, NULL);
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
        return 0; //username exists
    }
    else {
        return 1; //username doesnt exist
    }

    sqlite3_reset(stmt);
}

void get_password(char *pwd) 
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
    strncpy(pwd, placeholder, 49);
}

void pwrepeat_compare(char *passw, char *passw_repeat)
{
    char *placeholder;

    while (strcmp(passw, passw_repeat) != 0) {
        placeholder = getpass("Repeat Password: ");
        if (strcmp(placeholder, passw) != 0) {
            fprintf(stderr, RED"Error: Passwords don't match\n"RESET);
        }
        else {
            strncpy(passw_repeat, placeholder, 49);
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

// get hash & id & compare

// compare hash with password
int prompt_compare_hash(char *login_password, const unsigned char *account_hash) 
{
    // prompt and compare password hash. This can be a function
    int max_password_tries = 0;

    do {
        get_password(login_password);

        if (crypto_pwhash_str_verify ((const char *)account_hash, login_password, strlen(login_password)) != 0) {
        fprintf(stderr, RED "ERROR: Wrong password\n" RESET);
        max_password_tries++;
        }

        if (max_password_tries > 2) {
            fprintf(stderr, RED "Max tries reached, exiting application\n" RESET);
            return 1;
        }

    } while (crypto_pwhash_str_verify ((const char *)account_hash, login_password, strlen(login_password)) != 0);
    return 0;
}

void display_main(char *username)
{
    drawline(30, '-');
    printf(BLU "Hi %s, welcome to Inventory.c\n" RESET, username);
    drawline(30, '-');
    printf(BLU "Enter '1' to add an item\n" RESET);
    printf(BLU "Enter '2' to remove an item\n" RESET);
    printf(BLU "Enter '3' to modify stock\n" RESET);
    drawline(30, '-');
}

// show current inventory select * from user_inventory

// check date format

// check quantity format

// 