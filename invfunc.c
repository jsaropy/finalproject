#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sqlite3.h>
#include <sodium.h>
#include <time.h>

#include "invfunc.h"

#define RED "\x1B[31m"
#define RESET "\x1B[0m"

#define buffer 51
#define date_size 11

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
    printf("Inventory.c\n");
    drawline(30, '-');
    printf("\n");
    printf("- Enter '0' to exit the program\n");
    printf("- Enter '1' to register\n");
    printf("- Enter '2' to login\n");
    printf("- Enter '3' for a description\n");
    printf("\n");
    drawline(30, '-');
}

void display_description()
{
    printf("\n");
    printf( "This is an inventory management system.\n" );
    printf( "This system allows you to easily track the inventory of your business.\n");
    printf( "Simple functions: add/remove/modify the inventory of your business.\n");
    printf( "Please register or login to use the service.\n" );
    printf("\n");
}

void clearscr()
{
    system("@cls||clear");
}

int get_number()
{
    char num[10];

    // Check if the numbers provided are between 1-3, because there are only three options to choose from
    do 
    {
        printf("Enter number: ");
        scanf("%s", num);

        if (atoi(num) < 0 || atoi(num) > 3) 
        {
            fprintf(stderr, RED "ERROR: Please provide a number between 0-3\n" RESET);
        }

    } while (atoi(num) < 0 || atoi(num) > 3);

    return atoi(num);
}

void display_register()
{
    printf( "Register\n");
    drawline(30, '-');
    printf("\n");
    printf( "Provide an username and password.\n");
    printf( "The username/password must be between 5-50 characters.\n");
    printf("\n");
}

char *get_username(char *username) //Can be redesigned more efficiently
{
    printf("Username: ");
    scanf("%s", username);
    int length = strlen(username);

    // Check length of username and prompt again if length is not sufficient
    while (length < 5 || length > 49) 
    {
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
        
    // Check if username exists in database if not, place in DB (with help of GPT), SQL Prepare and bind to prevent SQL injection
    rc = sqlite3_prepare_v2(db, get_usernames, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return -1;
    }

    rc = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind username: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) 
    {
        return rc; //username exists
        sqlite3_reset(stmt);
    }
    else 
    {
        return rc; //username doesnt exist
        sqlite3_reset(stmt);
    }
}

void get_password(char *password) 
{        
    char *placeholder;
    // get password, repeat until its between 5-50 characters
    do 
    {
        placeholder = getpass("Password: ");
        if (strlen(placeholder) > 50) 
        {
            fprintf(stderr, RED "ERROR: Maximum of 50 characters\n" RESET);
        }
        else if (strlen(placeholder) < 5) 
        {
            fprintf(stderr, RED "ERROR: Your password should be between 5-49 characters\n" RESET);
        }
    } while (strlen(placeholder) < 5 || strlen(placeholder) > 49);
    strncpy(password, placeholder, 49);
}

void pwrepeat_compare(char *password, char *password_repeat)
{
    char *placeholder;

    while (strcmp(password, password_repeat) != 0) 
    {
        placeholder = getpass("Repeat Password: ");
        if (strcmp(placeholder, password) != 0) 
        {
            fprintf(stderr, RED"Error: Passwords don't match\n"RESET);
        }
        else 
        {
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
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind username: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_blob(stmt, 2, hash, crypto_pwhash_STRBYTES, SQLITE_STATIC);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind password: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) 
    {
        printf( "Registration succesful\n" RESET);
        return 0;
    }
    else 
    {
        fprintf(stderr, RED "ERROR: Registration failed: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_reset(stmt);
}

void display_login()
{
    printf( "Login\n" RESET);
    drawline(30, '-');
}

// compare hash with password
int prompt_compare_hash(char *login_password, const unsigned char *hash) 
{
    // prompt and compare password hash. This can be a function
    int max_password_tries = 0;

    do 
    {
        get_password(login_password);

        if (crypto_pwhash_str_verify ((const char *)hash, login_password, strlen(login_password)) != 0) 
        {
            fprintf(stderr, RED "ERROR: Wrong password\n" RESET);
            max_password_tries++;
        }

        if (max_password_tries > 2) 
        {
            fprintf(stderr, RED "ERROR: Max tries reached, exiting application\n" RESET);
            return 1;
        }

    } while (crypto_pwhash_str_verify ((const char *)hash, login_password, strlen(login_password)) != 0);
    return 0;
}

void display_main(char *username)
{
    drawline(70, '-');
    printf( "                 Hi %s, welcome to Inventory.c\n", username);
    drawline(70, '-');
    printf("\n");
    printf("                     Enter '0' to exit the program\n");
    printf( "                     Enter '1' to add an item\n");
    printf( "                     Enter '2' to remove an item\n");
    printf( "                     Enter '3' to modify stock\n");
    printf("\n");
}

// show inventory select * from view
int show_inventory(sqlite3 *db, int session_id) 
{
    char *user_inventory = "select * from user_inventory where user_id = ?;";

    rc = sqlite3_prepare_v2(db, user_inventory, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_int(stmt, 1, session_id);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind ID: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    drawline(70, '-');
    printf("%-10s %-20s %-14s %-14s %-14s\n", "Prod ID", "Name", "Prod Price", "Quantity", "Value");
    drawline(70, '-');

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        int prod_id = sqlite3_column_int(stmt, 1);
        const unsigned char *prod_name = sqlite3_column_text(stmt, 2);
        double prod_price = sqlite3_column_double(stmt, 3);
        int quantity = sqlite3_column_int(stmt, 4);
        float value = sqlite3_column_double(stmt, 5);

        printf("%-10d %-20s €%-14.2f %-14d €%-14.2f", prod_id, prod_name, prod_price, quantity, value);
        printf("\n");
        drawline(70, '-');
    }

    sqlite3_reset(stmt);
    return 0;
}

//Get product name
char *get_prod_name(char *product_name) 
{
    do 
    {
        printf("Product Name: ");
        scanf(" %[^\n]", product_name);

        if (strlen(product_name) < 3) 
        {
            fprintf(stderr, RED "ERROR: Please enter in a product name\n" RESET);
            while (getchar() != '\n');
        }
        else if (strlen(product_name) > buffer)
        {
            fprintf(stderr, RED "ERROR: Product name should be less than 50 characters\n" RESET);
            while (getchar() != '\n');
        }

    } while (strlen(product_name) < 3 || strlen(product_name) > buffer);
    
    while (getchar() != '\n');
    return product_name;
}

//Get product price
double get_price(double *price)
{
    int result;

    while (1) 
    {
        printf("Product Price: ");
        // Store the result of scanf in variable to check for character input by user
        result = scanf("%4lf", price);

        // check for positive and non numeric values
        if (result == 1 && *price > 0)
        {
            while (getchar() != '\n');
            break;
        }
        else 
        {
            fprintf(stderr, RED"ERROR: Please enter a (positive) numeric value.\n"RESET);
            while (getchar() != '\n');
        }
    }
    return *price;
}

//Get date format
int get_dates(char *date)
{
    scanf("%s", date);
    int length = strlen(date);

    if (length != 10)
    {
        fprintf(stderr, RED"ERROR: Please use date format DD/MM/YYYY\n"RESET);
        while (getchar() != '\n');
        return 1;
    }

    for (int i = 0; i < length; i++)
    {
        if (i == 2 || i == 5)
        {
            if (date[i] != '/')
            {
                fprintf(stderr, RED"ERROR: Please use date format DD/MM/YYYY\n"RESET);
                while (getchar() != '\n');
                return 1;
            }
        }
        else 
        {
            if (date[i] < '0' || date[i] > '9')
            {
                fprintf(stderr, RED"ERROR: Invalid character in date. Please use date format DD/MM/YYYY\n"RESET);
                while (getchar() != '\n');
                return 1;
            }
        }
    }
    
    int days = ((date[0] - 48) * 10) + (date[1] - 48);
    if (days > 31 || days < 0) 
    {        
        fprintf(stderr, RED"ERROR: Max days = 31\n"RESET);
        while (getchar() != '\n');
        return 1;
    }

    int months = ((date[3] - 48) * 10) + (date[4] - 48);
    if (months > 12 || months < 0) 
    {        
        fprintf(stderr, RED"ERROR: Max months = 12\n"RESET);
        return 1;
    }

    int years = ((date[6] - 48) * 1000) + ((date[7] - 48) * 100) + ((date[8] - 48) * 10) + (date[9] - 48);
    if (years < 0) 
    {        
        fprintf(stderr, RED"ERROR: Please provide a positive year\n"RESET);
        return 1;
    }
    
    return 0;
}

int get_quantity(int *quantity)
{
    int result;

    do 
    {
        printf("Quantity: ");
        // Scanf returns 1 if the scanf is succesful. 
        result = scanf("%i", quantity);

        if (*quantity <= 0 || result != 1)
        {
            fprintf(stderr, RED"ERROR: Please provide a sufficient quantity\n"RESET);
            while (getchar() != '\n');
        }
    } while (*quantity <= 0 || result != 1);

    return 0;
}

//TODO
int add_inventory(sqlite3 *db, int session_id, char *product_name, double *price, char *date_ordered, char *date_received, int *quantity)
{
    char *orders_query = "INSERT INTO orders (user_id, date_ordered, date_received) VALUES (?, ?, ?);";

    // Add the values into the orders table
    rc = sqlite3_prepare_v2(db, orders_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_int(stmt, 1, session_id);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind user_id: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_text(stmt, 2, date_ordered, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind date ordered: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }
    
    rc = sqlite3_bind_text(stmt, 3, date_received, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind date received: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, RED "ERROR: Adding Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    //because 
    int prod_id = sqlite3_last_insert_rowid(db);

    sqlite3_reset(stmt);

    char *products_query = "INSERT INTO products (prod_name, prod_price) VALUES (?, ?);";
    
    // Prepare SQL insert into products table, this needs to be repeated due to different tables and different values
    rc = sqlite3_prepare_v2(db, products_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_text(stmt, 1, product_name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "Failed to bind product name: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }
    
    rc = sqlite3_bind_double(stmt, 2, *price);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "Failed to bind product price: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, RED "ERROR: Adding Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    int order_id = sqlite3_last_insert_rowid(db);

    sqlite3_reset(stmt);

    char *op_query = "INSERT INTO orderproducts (order_id, prod_id, quantity) VALUES (?, ?, ?);";
    
    rc = sqlite3_prepare_v2(db, op_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_int(stmt, 1, order_id);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind order id: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }
    
    rc = sqlite3_bind_int(stmt, 2, prod_id);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind product id: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_int(stmt, 3, *quantity);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind qunatity: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, RED "ERROR: Adding Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_reset(stmt);
    return 0; 
}
 
int remove_inventory(sqlite3 *db, char *product_name)
{
    char *prod_id_query = "SELECT prod_id FROM products WHERE prod_name = ?;";
    
    rc = sqlite3_prepare_v2(db, prod_id_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_text(stmt, 1, product_name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind product name: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    int prod_id = -1;

    if (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        // Retrieve the prod_id (column 0 in the result)
        prod_id = sqlite3_column_int(stmt, 0);
    } 
    else 
    {
        printf(RED"ERROR: No product found with name: %s\n"RESET, product_name);
        return 1;
    }

    sqlite3_reset(stmt);

    char *delete_product = "DELETE FROM products WHERE prod_name = ?;";

    rc = sqlite3_prepare_v2(db, delete_product, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_text(stmt, 1, product_name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind product name: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, RED "ERROR: Removing Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_reset(stmt);

    char *delete_orderproducts = "DELETE FROM orderproducts WHERE prod_id = ?;";

    rc = sqlite3_prepare_v2(db, delete_orderproducts, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_int(stmt, 1, prod_id);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind product name: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, RED "ERROR: Removing Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_reset(stmt);
    return 0;
}

int modify_inventory(sqlite3 *db, char *product_name, double *price, int *quantity)
{
    int prod_id;
    printf("Please enter the Prod ID of the item to modify: ");
    scanf("%i", &prod_id);

    char *select_prodid = "SELECT prod_name, prod_price FROM products WHERE prod_id = ?;";

    rc = sqlite3_prepare_v2(db, select_prodid, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_bind_int(stmt, 1, prod_id);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, RED "ERROR: Failed to bind product id: %s\n" RESET, sqlite3_errmsg(db));
        return 1;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        while (getchar() != '\n');
        char user_choice[11];
        printf("Please enter the field you want to modify: ");
        scanf(" %[^\n]", user_choice);

        char options_arr[3][11] = {"Name", "Prod Price", "Quantity"};
        sqlite3_reset(stmt);

        for (int i = 0; i < 3; i++)
        {
            if (strcasecmp(user_choice, options_arr[0]) == 0)
            {
                printf("New ");
                get_prod_name(product_name);

                char *modify_name = "UPDATE products SET prod_name = ? WHERE prod_id = ?;";

                rc = sqlite3_prepare_v2(db, modify_name, -1, &stmt, NULL);
                if (rc != SQLITE_OK) 
                {
                    fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }   
                rc = sqlite3_bind_text(stmt, 1, product_name, -1, SQLITE_TRANSIENT);
                if (rc != SQLITE_OK) 
                {
                    fprintf(stderr, RED "ERROR: Failed to bind product name: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }   
                rc = sqlite3_bind_int(stmt, 2, prod_id);
                if (rc != SQLITE_OK) 
                {
                    fprintf(stderr, RED "ERROR: Failed to bind product id: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }   
                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) 
                {
                    fprintf(stderr, RED "ERROR: Modifying Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }
                sqlite3_reset(stmt);
                return 0;
            }
            else if (strcasecmp(user_choice, options_arr[1]) == 0)
            {
                printf("New ");
                get_price(price);

                char *modify_price = "UPDATE products SET prod_price = ? WHERE prod_id = ?;";

                rc = sqlite3_prepare_v2(db, modify_price, -1, &stmt, NULL);
                if (rc != SQLITE_OK) 
                {
                   fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
                   return 1;
                }

                rc = sqlite3_bind_double(stmt, 1, *price);
                if (rc != SQLITE_OK) 
                {
                    fprintf(stderr, RED "ERROR: Failed to bind product name: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }

                rc = sqlite3_bind_int(stmt, 2, prod_id);
                if (rc != SQLITE_OK) 
                {
                    fprintf(stderr, RED "ERROR: Failed to bind product id: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }

                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) 
                {
                    fprintf(stderr, RED "ERROR: Modifying Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }

                sqlite3_reset(stmt);
                return 0;
            }
            else if (strcasecmp(user_choice, options_arr[2]) == 0)
            {
               char *modify_quant = "UPDATE orderproducts SET quantity = ? WHERE prod_id = ?;";

                rc = sqlite3_prepare_v2(db, modify_quant, -1, &stmt, NULL);
                if (rc != SQLITE_OK) 
                {
                   fprintf(stderr, RED "ERROR: Failed to prepare statement: %s\n" RESET, sqlite3_errmsg(db));
                   return 1;
                }

                rc = sqlite3_bind_int(stmt, 1, *quantity);
                if (rc != SQLITE_OK) 
                {
                    fprintf(stderr, RED "ERROR: Failed to bind product name: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }

                rc = sqlite3_bind_int(stmt, 2, prod_id);
                if (rc != SQLITE_OK) 
                {
                    fprintf(stderr, RED "ERROR: Failed to bind product id: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }

                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) 
                {
                    fprintf(stderr, RED "ERROR: Modifying Inventory failed: %s\n" RESET, sqlite3_errmsg(db));
                    return 1;
                }
                
                sqlite3_reset(stmt);
                return 0;
            }
            else
            {
                fprintf(stderr, RED"ERROR: Column not found\n"RESET);
                return 1;
            }
        }
    } 
    else 
    {
        printf(RED"ERROR: No product found with id: %d\n"RESET, prod_id);
        while (getchar() != '\n');
        sqlite3_reset(stmt);
        return 1;
    }
    return 0;
}

