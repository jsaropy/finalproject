# CLI INVENTORY MANAGEMENT
#### Video Demo: <URL HERE>
#### Description: 
Final Project CS50x ~ Shvan Jaro
Hi this is my final project for CS50x: Introduction to Computer Science.
My final project is an cli program, that entails an inventory management system to easily track the inventory of businesses. 
The cli program allows the user to add/remove/modify and track the inventory of their business.

TODO:
## ✅ 1. Make the display for the starting page of the CLI program 

## ✅ 2. Complete get user input number
- Don't use cs50.h
- Create get input yourself 

## 3. Create DB
✅ - Create DB with sqlite3
✅- Link db into C program
✅- READ SQLite C api docs

![Database](assets/DB-finalproject.drawio.png)

# 4. Redirect user to pages
## Redirect user to register page (If user types 1 display the following things):
    - First clear terminal screen ✅
    - display_register next ✅
    - Username (Must be unique, so check DB): ✅
        - For username, malloc a place in memory for the maximum length of the string -> username ✅
        - ⛔ ERROR: username accepts whitespace 
        - Check if username exists in DB ✅
        - If exist, output error ✅
    - Password  ✅
    - ⛔ ERROR: password accepts whitespace
    - No terminal output ✅
    - Password (Repeat) ✅
    - Store username in database ✅
    - Hash password, and store in database (function hash) ✅

> [!NOTE]
 > I need to provide a way to exit at any time during the program, because the user can get stuck.
 > Additionally fix errors?

## Redirect user to login page (If user types 2 display the following things):
    - First clear terminal screen ✅
    - Username: (get_username) ✅
        - Compare if username exists in DB ✅
        - If not print error (while loop) ✅
    - Password (*) (get_password) ✅
        - Compare password with password hashes (while loop) ✅
        - libsodium has a way to compare the password hashes. ✅

    - When log in successful, display the main screen
        - assign the struct user the logged in username
        - assign the struct user the user_id which becomes the session ID
        - The goal is to show/add/modify products based on this session ID
        - Showing current inventory is the standard procedure 

    - The main screen contains an menu, but shows the current inventory as a standard
    - Allow to add inventory items
        - Product name
        - Product price
        - Supplier
        - Field with date ordered
        - Date received
        - Quantity

✅ - Show the user the description (If user types 3 display the following things):
    - display_description
    - display menu
    - ONly add the description at the top of the menu, and refresh the page. 


# 5. Complete inventory UI
- User has the option to add inventory
- Put this item in DB
- Show DB items in home 
- Remove Inventory
- Remove from DB
- Remove from UI
  
# Dependencies (Add tutorials for installing these before using project)
- SQLite3
- Libsodium
- Unistd.h -> for Windows
