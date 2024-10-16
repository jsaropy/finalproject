#include <stdio.h>
#include <stdlib.h>

#include "invfunc.c"
#include "menus.c"

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"

int main(void) 
{
    // Print menu
    clearscr();
    display_menu();

    // Ask for user input
    int input;

    do {
        input = get_int();
        if (input > 3 || input < 1)
        {
            printf(RED "Error: Please provide a number between 1-3\n" RESET);
            clearscr();
            display_menu();
        }
    } while (input > 3 || input < 1);

    switch (input) {
        case 1:
            clearscr();
            display_register();
            char *un_register = get_username();
        case 2:
            clearscr();
            display_login();
            char *un_login = get_username();
    }
}
