#include <stdio.h>
#include <stdlib.h>

void clearscr()
{
    system("@cls||clear");
}

int get_int()
{
    int num;
    printf("Enter number: ");
    scanf("%i", &num);
    return num;
}

char* get_username()
{
    char *username;
    printf("Username: ");
    scanf("%p", &username);

    return username;
}