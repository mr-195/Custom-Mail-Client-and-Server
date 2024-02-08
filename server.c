#include <stdio.h>
#include <string.h>

int main() {
    char originalArray[] = "First line\nSecond line\nThird line\nFourth line";
    char newString[] = "Inserted line";

    // Counters for '\n' occurrences and index
    int newlineCount = 0;
    int currentIndex = 0;

    // Find the third '\n'
    while (originalArray[currentIndex] != '\0') {
        if (originalArray[currentIndex] == '\n') {
            newlineCount++;
            if (newlineCount == 3) {
                break; // Found the third '\n'
            }
        }
        currentIndex++;
    }

    // Check if we found the third '\n'
    if (newlineCount == 3) {
        // Calculate the length of the resulting array
        int newLength = currentIndex + strlen(newString) + strlen(originalArray + currentIndex);

        // Create a new array to store the result
        char resultArray[newLength + 1]; // +1 for the null terminator

        // Copy the characters before the insertion point
        strncpy(resultArray, originalArray, currentIndex);

        // Append the new string
        strcat(resultArray, newString);

        // Append the remaining characters
        strcat(resultArray, originalArray + currentIndex);

        // Print the result
        printf("Result:\n%s\n", resultArray);
    } else {
        printf("Error: Less than 3 newline characters found.\n");
    }

    return 0;
}
