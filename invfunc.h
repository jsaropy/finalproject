

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
char *get_username(char *usrname);

// Check existence of username in database
int existence(void *input_username, int argc, char **argv, char **azColName);

// User input of password
void get_password(char *pwd);

// Repeat password function of user, with comparison of previousy provided password
void pwrepeat_compare(char *passw, char *passw_repeat);

// Prompt for login and compare password to acc hash
int prompt_compare_hash(char *login_password, const unsigned char *account_hash); 

// Display login menu
void display_login();
