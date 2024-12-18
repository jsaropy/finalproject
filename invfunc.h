

// Drawline function for the display of each menu
void drawline(int a, char line);

// Display starting menu of application
void display_menu();

//Display of description if input == 3
void display_description();

// Clear screen, to clear the terminal screen when loading in different menu's
void clearscr();

// Get number (A way to make the user choose which menu to access)
int get_number();

//Display register menu
void display_register();

// Get username, allows user to input username
char *get_username(char *username);

// Check existence of username in database
int existence(void *username, int argc, char **argv, char **azColName);

// compare usernames
int compare_usernames(sqlite3 *db, char *username);

// User input of password
void get_password(char *password);

// Repeat password function of user, with comparison of previousy provided password
void pwrepeat_compare(char *password, char *password_repeat);

// create user
int create_user(sqlite3 *db, char *username, unsigned char hash[crypto_pwhash_STRBYTES]);

// Display login menu
void display_login();

// Prompt for login and compare password to acc hash
int prompt_compare_hash(char *login_password, const unsigned char *hash); 

// diplay main menu when logged in
void display_main(char *username);

// show inventory select * from view
int show_inventory(sqlite3 *db, int session_id);

// Get product name
char *get_prod_name(char *product_name); 

// Get product price
double get_price(double *price);

// Get order and receive dates
int get_dates(char *date);

// Get quantity
int get_quantity(int *quantity);

// Add inventory to DB
int add_inventory(sqlite3 *db, int session_id, char *product_name, double *price, char *date_ordered, char *date_received, int *quantity);

int remove_inventory(sqlite3 *db, int session_id, char *product_name);

int modify_inventory(sqlite3 *db, int session_id, char *product_name, double *price, int *quantity);


