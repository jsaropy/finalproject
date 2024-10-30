#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"

#define esc 27

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
    printf(BLU "Inventory\n" RESET);
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
    printf(BLU "The username/password must be at least 5 characters.\n" RESET);
    printf("\n");
}

char *get_username(char *usrname)
{
    printf("Username: ");
    scanf("%49s", usrname);
    int length = strlen(usrname);

    // Check length of username and prompt again if length is not sufficient
    while (length < 5) {
        fprintf(stderr, RED "ERROR: Username should be at least 5 characters\n" RESET);
        printf("Username: ");
        scanf("%49s", usrname);
        length = strlen(usrname);
    }
    return usrname;
}

int existence(void *input_username, int argc, char **argv, char **azColName)
{
    // Call back username to extra data input (void *input_username)
    char * username = (char *)input_username;

    for (int i = 0; i < argc; i++) {
        // If the column name is username
        if (strcasecmp(azColName[i], "username") == 0) {
            // While the username exists in db, reprompt.
            while (strcasecmp(username, argv[i]) == 0) {
                printf(RED "ERROR: Username already exists\n" RESET);
                printf("Username: ");
                scanf("%49s", username);
            }
        }
    }
    return 0;
}

char *get_password(char *pwd) 
{        
    char *placeholder;
    // get password, repeat until its between 5-50 characters
    do {
    placeholder = getpass("Password: ");
        if (strlen(placeholder) > 50) {
            fprintf(stderr, RED "ERROR: Maximum of 50 characters\n" RESET);
        }
        else if (strlen(placeholder) < 5) {
            fprintf(stderr, RED "ERROR: Your password should be atleast 5 characters\n" RESET);
        }
    } while (strlen(placeholder) < 5 || strlen(placeholder) > 49);
    strncpy(pwd, placeholder, 49);
}

char *pwrepeat_compare(char *passw, char *passw_repeat)
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

//TODO
void display_login()
{
    printf(BLU "Login\n" RESET);
    drawline(30, '-');
}