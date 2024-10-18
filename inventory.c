#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "invfunc.c"

#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"

int main(void) 
{
    // Print menu
    clearscr();
    display_menu();

    // Ask for user input
    int input = get_number();

    printf("%i", input);

    while (input == 3)
    {
        clearscr();
        display_description();
        display_menu();
        input = get_number();
    }

    if (input == 1)
    {
        clearscr();
        display_register();
    }
    else if (input == 2)
    {
        clearscr();
        display_login();
    }
    
}
