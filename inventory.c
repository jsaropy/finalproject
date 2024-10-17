#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "invfunc.c"

int main(void) 
{
    // Print menu
    clearscr();
    display_menu();

    // Ask for user input
    int input = get_number();
    
    while (input == 3)
    {
        display_description();
        input = get_number();
    }

    if (input == 1)
    {
        clearscr();
        display_register();
        char *un_register = get_username();
    }
    else if (input == 2)
    {
        clearscr();
        display_login();
        char *un_login = get_username();
    }   
}
