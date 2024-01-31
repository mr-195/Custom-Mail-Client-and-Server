#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Declare variables to store each line
    char from_line[100], to_line[100], subject_line[100];
    char message_lines[4][100]; // Assuming a maximum of 4 lines for the message

    // Prompt the user for input
    printf("Enter the email details:\n");

    // Use fgets to read each line into the respective variables
    printf("From: ");
    fgets(from_line, sizeof(from_line), stdin);

    printf("To: ");
    fgets(to_line, sizeof(to_line), stdin);

    printf("Subject: ");
    fgets(subject_line, sizeof(subject_line), stdin);

    printf("Message:\n");
    for (int i = 0; i < 4; ++i) {
        printf("Line %d: ", i + 1);
        fgets(message_lines[i], sizeof(message_lines[i]), stdin);
        if(strcmp(message_lines[i], ".\n") == 0) {
            break;
        }
    }

    // Display the user's input
    printf("\nYou entered:\n");
    printf("From: %s", from_line);
    printf("To: %s", to_line);
    printf("Subject: %s", subject_line);
    printf("Message:\n");
    for (int i = 0; i < 4; ++i) {
        printf("Line %d: %s", i + 1, message_lines[i]);
    }

    // Check if the "From" line is valid
   if (strncmp(from_line, "From:", 5) != 0 || strncmp(to_line, "To:", 3) != 0 || strncmp(subject_line, "Subject:", 8) != 0)
    {
        printf("Incorrect Format \n");
        return 0;
    }
    // Check the presence of '@' in the "From" line
    char *from_at_symbol = strchr(from_line, '@');
    if (from_at_symbol == NULL)
    {
        printf("Incorrect Format \n");
        return 0;
    }

    // Check the presence of '@' in the "To" line
    char *to_at_symbol = strchr(to_line, '@');
    if (to_at_symbol == NULL)
    {
        printf("Incorrect Format \n");
        return 0;
    }

    return 0;
}
