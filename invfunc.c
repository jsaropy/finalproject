#include <stdio.h>
#include <stdlib.h>

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"

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

//TODO
void display_register()
{
    printf(BLU "Register\n" RESET);
    drawline(30, '-');
}

//TODO
void display_login()
{
    printf(BLU "Login\n" RESET);
    drawline(30, '-');
}

//TODO
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

    if (atoi(num) < 1 || atoi(num) > 3)
    {
        printf(RED "ERROR: Please provide a number between 1-3\n" RESET);
        exit(0);
    }
    return atoi(num);
}

char* get_username()
{
    char *username;
    printf("Username: ");
    scanf("%p", &username);
    return username;
}

