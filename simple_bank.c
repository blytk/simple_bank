#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct user
{
    char username[25];
    char password[25];
}
user;

bool login(void);
bool registration(int user_count);
bool bank_operations(char *LOGGED_USER);

int main(int argc, char *argv[])
{
    int user_count;
    // Get number of active users;
    FILE *user_count_file = fopen("user_count.txt", "r");
    if (user_count_file == NULL)
    {
        user_count_file = fopen("user_count.txt", "w");
        fprintf(user_count_file, "0");
        fclose(user_count_file);
        user_count_file = fopen("user_count.txt", "r");
    }
    fscanf(user_count_file, "%i", &user_count);
    fclose(user_count_file);

    // Create array of users, size of user_count
    user registered_users[user_count + user_count / 2];

    // LOGIN / REGISTER
    printf("Press 1 to register a new user. Press 2 to login with an existing user.\n");
    int user_option;
    scanf("%i", &user_option);
    if (user_option == 1)
    {
        registration(user_count);
    }
    else if (user_option == 2)
    {
        login();
    }

    // DEPOSIT / WITHDRAW / CHECK LOGS
    return 0;
}

bool registration(int user_count)
{
    user new_user;

    // Get username from user
    printf("Enter your username: ");
    scanf("%s", new_user.username);
    char c;
    for (int i = 0, length = strlen(new_user.username); i < length; i++)
    {
        c = tolower(new_user.username[i]);
        new_user.username[i] = c;
    }
    // Check that the username does not already exists
    
    // 1. Open the file with the usernames and passwords (users.csv)
    FILE *users_file = fopen("users.csv", "r");
    if (users_file == NULL)
    {
        printf("Error opening users file\n");
        return false;
    }
    // 2. Read the file, check for the username
    char buffer[50]; // 25 username + 25 password
    char substring[25];
    // fgets will return NULL if an error is encountered or ata the EOF
    while (fgets(buffer, sizeof(buffer), users_file) != NULL)
    {
        // I need to grab the username, that is, until the comma
        char *comma_position = strchr(buffer, ',');
        if (comma_position != NULL)
        {
            // Get the relevant part (before the comma, that is in comma_position)
            strncpy(substring, buffer, comma_position - buffer);
            substring[comma_position - buffer] = '\0';

            // Now substring should store the username
            if (strcasecmp(substring, new_user.username) == 0)
            {
                printf("Username already exists\n");
                fclose(users_file);
                return false;
            }
        }
    }
    fclose(users_file);
    

    // Read the file with the users
    do
    {
        printf("Enter your password: ");
        scanf("%s", new_user.password);
    }
    while (strlen(new_user.password) < 0 || strlen(new_user.password) > 24); // 25 - 1 for the '\0' terminator
    
    printf("Confirm your password: ");
    char password_confirmation[25];
    scanf("%s", password_confirmation);
    // check that password_confirmation == new_user.password
    if (strcmp(new_user.password, password_confirmation) == 0)
    {
        // success // create new user
        int array_index = user_count;
        user registered_users[array_index];
        strcpy(registered_users[array_index].username, new_user.username);
        strcpy(registered_users[array_index].password, new_user.password);

        // ADD +1 to user_count in "user_count.txt"
        FILE *f = fopen("user_count.txt", "r");
        fscanf(f, "%i", &user_count);
        fclose(f);
        user_count++;
        f = fopen("user_count.txt", "w");
        fprintf(f, "%i", user_count);
        fclose(f);  
        // store the user / password in a file
        FILE *users_file = fopen("users.csv", "a");
        if (users_file == NULL)
        {
            printf("Error opening users.csv file\n");
            return false;
        }
        fprintf(users_file, "%s,%s", new_user.username, new_user.password);
        printf("User successfully created\n");
        // Need to add a record for the new user to bank_operations.csv (username _ funds)
        // FILE *bank_records = fopen("bank_records.csv", "a");

        // !!!!!!!!!!!! CHANGE - TEST
        char filename[50];
        sprintf(filename, "%s_funds.txt", new_user.username);
        FILE *bank_records = fopen(filename, "w");
        
        if (bank_records == NULL)
        {
            printf("Error opening %s.csv\n", new_user.username);
            return false;
        }
        fprintf(bank_records, "%i", 0);
        return true;
    }
    else
    {
        // password did not match confirmation
        printf("The password confirmation does not match\n");
        return false;
    }
}

bool login(void)
{
    // capture username
    printf("Enter your username: ");
    char username_input[25];
    scanf("%s", username_input);
    // check username exists (in users.csv);
    FILE *users_file = fopen("users.csv", "r");
    if (users_file == NULL)
    {
        printf("Error opening users file\n");
        return 1;
    }
    char buffer[50];
    char substring[25];
    // read file, line by line (or 50 chars max from each line (size of buffer))
    while (fgets(buffer, sizeof(buffer), users_file))
    {
        char *comma_position = strchr(buffer, ',');
        if (comma_position != NULL)
        {
            // comma_position is a pointer to the comma's location within the buffer, buffer (as a char buffer / string) is a pointer to the first char of the string
            // pointer arithmethic allows us to subtract pointers: we are not subtracting address values, but C subtracts the number of addresses in between
            // it helps to draw / visualize a number line 
            // address1 -------- address2 ------- address3 -------- address4 ------- address5
            // each address is a unit, so for instance if comma position would be pointing to address 5, buffer to address 1: address5 - address1 = 4

            // so this line, copies into substring, from buffer position[0] until buffer position[where the comma is]
            strncpy(substring, buffer, comma_position - buffer); 
            substring[comma_position - buffer] = '\0';
            // so now we compare and check if the user was found
            if (strcasecmp(substring, username_input) == 0)
            {
                // username found, we can proceed to the password check
                printf("Username found\n");
                // password check
                // instead of closing and opening the file, maybe we can rewind to the beginning for fgets? yes, rewind(file pointer) accomplishes this
                // no need, the cursor should be right where it's needed (password goes right after name,password)
                char password_input[25];
                printf("Enter your password: ");
                scanf("%s", password_input); // WHAT HAPPENS IF USER ENTERS PASSWORD LONGER THAN BUFFER (OVERFLOW)?? Probably should prevent this somehow
                // I need to check the password for that user, not just check that the password exists within the users file
                // comma_position + 1 points to the source to copy (the first char we want to copy into the destination)
                // How many chars do we want to copy? We calculate strlen, but instead of starting from the beginning of the string, we start after the comma
                // it will start counting after the comma (first char of the password) until the end of the string ('\0')
                strncpy(substring, comma_position + 1, strlen(comma_position + 1));
                substring[strlen(substring) - 1] = '\0';

                if (strcasecmp(substring, password_input) == 0)
                {
                    printf("Password is correct, logging in user %s\n", username_input);
                    fclose(users_file);

                    // allocate memory to store the logged user
                    // need to pass the pointer to free the memory before finishing the program
                    char LOGGED_USER[25];
                    strcpy(LOGGED_USER, username_input);
                    bank_operations(LOGGED_USER);
                    return true;
                }
            }
        }
    }
    printf("Password does not match\n");
    return false;
}

// Deposit // Withdraw // Check logs
bool bank_operations(char *LOGGED_USER)
{
    char filename[25];
    sprintf(filename, "%s_funds.txt", LOGGED_USER);
    
    FILE *fund_records = fopen(filename, "r");
    if (fund_records == NULL)
    {
        fund_records = fopen(filename, "w");
        fprintf(fund_records, "%i", 0);
        fclose(fund_records);
        fund_records = fopen(filename, "w");
    }
    // Search name (passed in *LOGGED_USER)
    char buffer_funds[50];
    fread(buffer_funds, sizeof(buffer_funds), 1, fund_records);
    int funds = atoi(buffer_funds);
    
    // At this point, we should be logged as user LOGGED_USER with int funds for funds;
    // We can manipulate the funds easily as an int, but we will to convert to string and overwrite the record with the new amount
    int bank_operations_option;
    do
    {
        printf("Choose an option:\n");
        printf("1 --> DEPOSIT FUNDS\n");
        printf("2 --> WITHDRAW FUNDS\n");
        printf("3 --> CHECK BALANCE\n");
        scanf("%i", &bank_operations_option);
    }
    while (bank_operations_option < 1 || bank_operations_option > 3);
    if (bank_operations_option == 1)
    {
        // DEPOSIT FUNDS
        // Open the file LOGGED_USER_funds.txt (already open)
        // Read the funds parts (already read)

        // WHAT IF I USE AN INDIVIDUAL .TXT FILE FOR EACH USER, FOR THE FUNDS? THAT SHOULD MAKE IT MUCH EASIER TO EDIT

        printf("Your current balance is: %i\n", funds);
        // Add funds
        printf("How much money would you like to deposit?\n");
        int deposit_funds;
        scanf("%i", &deposit_funds);
        // Store new funds amount
        funds = funds + deposit_funds;
        printf("Your new balance is: %i\n", funds);
        fclose(fund_records);
        fund_records = fopen(filename, "w");
        fprintf(fund_records, "%i", funds);
        fclose(fund_records);
        return true;
    }
    if (bank_operations_option == 2)
    {
        // WITHDRAW FUNDS
        fclose(fund_records);
        return true;
    }
    if (bank_operations_option == 3)
    {
        // CHECK LOGS
        printf("Your current balance is: %i\n", funds);
        fclose(fund_records);
        return true;
    }
    fclose(fund_records);
    return true;
}