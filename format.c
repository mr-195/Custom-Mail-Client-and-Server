#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    // Sample message
    const char msg[] = "From: Utsav@jdd\nTo: Utsav@jdd\nSubject: Hello\nHello, World!\n\nWOW helll";

    // Assuming username is set
    const char *username = "sample_user";
    
    FILE *fp = fopen(username, "a");

    // write msg in file
    if (fp != NULL) {
        const char delimiter[2] = "\n";

        // Get the first token
        char *token = strtok((char *)msg, delimiter);
        int cnt = 0;

        // Continue getting tokens until there are no more
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        // Get current time
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        // Format time as a string
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        while (token != NULL) {
            // Print every line (including empty lines)
            fprintf(fp, "%s\n", token);

            cnt++;
            if (cnt == 3) {
                fprintf(fp, "Received at: %s\n", buffer);
                delimiter
            }

            // Get the next token
            token = strtok(NULL, delimiter);
        }

        fclose(fp);
        printf("Message stored in file with timestamp.\n");
    }

    return 0;
}
