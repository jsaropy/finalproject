#include <stdio.h>

void drawline(int a, char line)
{
    for (int i = 0; i < a; i++)
    {
        printf("%c", line);
    }
}

void display_menu()
{
    printf("\n");
    printf("Press '1' to register\n");
    printf("Press '2' to login\n");
    printf("Press '3' for a description\n");
    printf("\n");
}

int main(void) 
{
    printf("Inventory\n");
    drawline(30, '-');
    display_menu();

    
}
