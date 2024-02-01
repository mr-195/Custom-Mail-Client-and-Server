#include <stdio.h>
#include <string.h>

int main() {
    char msg[] = "This is a\nsample string\nwith newline characters.";
    const char delimiter[2] = "\n";

    // Get the first token
    char *token = strtok(msg, delimiter);

    // Continue getting tokens until there are no more
    while (token != NULL) {
        printf("%s\n", token);

        // Get the next token
        token = strtok(NULL, delimiter);
    }

    return 0;
}
