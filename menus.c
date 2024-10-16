#include <stdio.h>

#define RED "\x1B[31m"
#define BLU   "\x1B[34m"
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

void display_register()
{
    printf(BLU "Register\n" RESET);
    drawline(30, '-');
}

void display_login()
{
    printf(BLU "Login\n" RESET);
    drawline(30, '-');
}

void display_description()
{
    printf("This is an inventory management system.\n");
    printf("This system allows you to easily track the inventory of your business.\n");
    printf("Simple functions: add/remove/modify/track the inventory of your business.\n");
}