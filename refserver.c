#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

typedef struct {
    char username[50];
    char password[50];
} User;
int markedfordel[100];
int nom;
// Function to handle POP3 connection for each client
void handlePOPConnection(int clientSocket,User* users);



void handleSTAT(int clientSocket, const char *username) {
    // Open the user's mailbox file
    char mailboxPath[100];
    sprintf(mailboxPath, "%s/mymailbox.txt", username);
    printf("%s",mailboxPath);

    FILE *mailbox = fopen(mailboxPath, "r");
    if (mailbox == NULL) {
        perror("Error opening mailbox file");
        send(clientSocket, "-ERR Cannot open mailbox file\r\n", 32, 0);
        return;
    }

    // Count the number of messages and calculate the total size
    int numMessages = 0;
    int totalSize = 0;

    while (1) {
        char line[MAX_BUFFER_SIZE];
        if (fgets(line, sizeof(line), mailbox) == NULL ) {
            break;  // End of mailbox
        }
          totalSize += strlen(line);
        // Assume each message is separated by a line containing only a fullstop character
        // This might need modification based on the actual mailbox format
       line[strcspn(line, "\r\n")] = '\0';

                                // Check if the line contains only a period
                                if (strcmp(line, ".END") == 0) {
                                    numMessages++;
                                     // Assuming you want to exit after the first occurrence of a line with a period
                                }

        // Calculate size by adding the length of each line
        
    }

    fclose(mailbox);

    // Respond with STAT information
    char response[MAX_BUFFER_SIZE];
    sprintf(response, "+OK %d %d\r", numMessages, totalSize);
    send(clientSocket, response, strlen(response), 0);
}

void handleLIST(int clientSocket, const char *username) {
    // Open the user's mailbox file
    char mailboxPath[100];
    sprintf(mailboxPath, "%s/mymailbox.txt", username);

    FILE *mailbox = fopen(mailboxPath, "r");
    if (mailbox == NULL) {
        perror("Error opening mailbox file");
        send(clientSocket, "-ERR Cannot open mailbox file\r\n", 32, 0);
        return;
    }

    // Respond with LIST information
    send(clientSocket, "+OK Mailbox listing follows\r\n", 29, 0);

    int messageNumber = 1;
    int totalSize = 0;

    while (1) {
        char line[MAX_BUFFER_SIZE];
        if (fgets(line, sizeof(line), mailbox) == NULL  )  {
            break;  // End of mailbox
        }
         totalSize += strlen(line);
        // Assume each message is separated by a line containing only a fullstop character
        // This might need modification based on the actual mailbox format
        line[strcspn(line, "\r\n")] = '\0';
        if(strcmp(line, ".END")==0)
        {
              char response[MAX_BUFFER_SIZE];
        sprintf(response, "%d %d\r\n", messageNumber, totalSize);
        send(clientSocket, response, strlen(response), 0);

         messageNumber++;
         totalSize=0;
        }

        // Calculate size by adding the length of each line
        

        // Respond with message number and size
      

        
    }

    fclose(mailbox);

    // Respond with a line containing only a fullstop character to end the list
    send(clientSocket, "-END-", 6, 0);
}
void handleRETR(int clientSocket, const char *username, int messageNumber) {

    printf("%d\n",messageNumber);
    // Open the user's mailbox file
    char mailboxPath[100];
    sprintf(mailboxPath, "%s/mymailbox.txt", username);

    FILE *mailbox = fopen(mailboxPath, "r");
    if (mailbox == NULL) {
        perror("Error opening mailbox file");
        send(clientSocket, "-ERR Cannot open mailbox file\r\n", 32, 0);
        return;
    }
     char line[MAX_BUFFER_SIZE];
      memset(line, 0, sizeof(line));

    // Skip to the requested message number
    int currentMessage = 1;
    while (currentMessage < messageNumber) {
       
       
        if (fgets(line, sizeof(line), mailbox) == NULL) {
            // Error or message not found
            fclose(mailbox);
            send(clientSocket, "-ERR Message not found\r\n", 24, 0);
            return;
        }
        line[strcspn(line, "\r\n")] = '\0';

        // Check for lines with only a full stop and handle accordingly
        if (strstr(line ,".END")!=NULL) {
            printf("passing meaasage %d",currentMessage);
            currentMessage++;
        }
    }
    

      

    while (fgets(line, sizeof(line), mailbox)) {
        printf("%s",line);
        
         send(clientSocket, line, strlen(line), 0);
       
     
        if ( strstr(line,".END")!=NULL) {
            printf("END OF SENDING");
            break;  // End of message
        }
    }

        
        
    // }
     

    fclose(mailbox);
}

void handleDELE(int clientSocket, const char *username, int messageNumber) {
    // Open the user's mailbox file
    markedfordel[messageNumber]=1;
      send(clientSocket, "message marked for deletion\r\n", 29, 0);
}

void handleQUIT(int clientSocket, const char *username) {
    // Open the user's mailbox file
    char mailboxPath[100],tempmailboxPath[100];
    sprintf(mailboxPath, "%s/mymailbox.txt", username);
    sprintf(tempmailboxPath, "%s/temp.txt", username);

    FILE *mailbox = fopen(mailboxPath, "r");
    if (mailbox == NULL) {
        perror("Error opening mailbox file");
        send(clientSocket, "-ERR Cannot open mailbox file\r\n", 32, 0);
        return;
    }

    // Create a temporary mailbox file
  
     FILE* file = fopen(tempmailboxPath, "w+");

    if (file == NULL) {
        perror("Error creating temporary mailbox file");
        fclose(mailbox);
        send(clientSocket, "-ERR Cannot create temporary mailbox file\r\n", 43, 0);
        return;
    }

    // Copy messages to the temporary mailbox, excluding marked messages
    int currentMessage = 1;
    while (1) {
        char line[MAX_BUFFER_SIZE];
        if (fgets(line, sizeof(line), mailbox) == NULL) {
            break;  // End of mailbox
        }
            if (markedfordel[currentMessage] != 1) {
                // Append the message to the temporary mailbox
                fputs(line, file);
            }

        // Check for lines wonly a full stop and handle accordingly
        if (strstr(line, ".END") != NULL) {
        
            currentMessage++;
        } 
    }

    fclose(mailbox);
    fclose(file);

    // Replace the original mailbox file with the temporary one
    remove(mailboxPath);
    rename(tempmailboxPath,mailboxPath);

    // Respond with QUIT confirmation
    send(clientSocket, "+OK Goodbye\r\n", 13, 0);

    // Close the connection
    close(clientSocket);
}

void handleRSET(int clientSocket) {
    // Reset deletion status of all messages
    memset(markedfordel, 0, sizeof(markedfordel));

    // Respond with +OK
    send(clientSocket, "+OK Resetting marked messages\r\n", 32, 0);
}

int main() {
    // Read user credentials from user.txt file
    FILE *userFile = fopen("user.txt", "r");
    if (userFile == NULL) {
        perror("Error opening user.txt");
        exit(EXIT_FAILURE);
    }
    User users[100];

    int numUsers = 0;
    while (fscanf(userFile, "%s %s", users[numUsers].username, users[numUsers].password) == 2) {
        numUsers++;
    }
    fclose(userFile);

    // Setup server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);  // Use your desired port number
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind server socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening for connections...\n");

    while (1) {
        // Accept client connection
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Create a child process to handle the client
        if (fork() == 0) {
            close(serverSocket); // Close server socket in child process
            handlePOPConnection(clientSocket,users);
            close(clientSocket);
            exit(EXIT_SUCCESS);
        } else {
            close(clientSocket); // Close client socket in parent process
        }
    }

    close(serverSocket);
    return 0;
}

void handlePOPConnection(int clientSocket,User* users) {
    char buffer[MAX_BUFFER_SIZE];

    // Send greeting
    send(clientSocket, "+OK POP3 server ready\r\n", 24, 0);

    // Authentication
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);

    if (strstr(buffer, "USER") != NULL) {
        // Extract username
        char username[50];
        sscanf(buffer, "USER %s\r\n", username);

        // Check credentials
        int authenticated = 0;
        for (int i = 0; i < 100; i++) {
            if (strcmp(users[i].username, username) == 0) {
                authenticated = 1;
                break;
            }
        }

        // Respond to authentication
        if (authenticated) {
            send(clientSocket, "+OK\r\n", 5, 0);

            // Receive and check password
            memset(buffer, 0, sizeof(buffer));
            recv(clientSocket, buffer, sizeof(buffer), 0);
            if (strstr(buffer, "PASS") != NULL) {
                char password[50];
                sscanf(buffer, "PASS %s\r\n", password);

                // Check password
                int validPassword = 0;
                for (int i = 0; i < 100; i++) {
                    if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
                        validPassword = 1;
                        break;
                    }
                }

                if (validPassword) {
                    printf("Authentication succesful\n");
                    
                    send(clientSocket, "+OK Authentication successful\r\n", 35, 0);
                    printf("1");

                        char mailboxPath[100];
                            sprintf(mailboxPath, "%s/mymailbox.txt", username);


                            printf("2");
                            printf("\n%s\n",mailboxPath);

                            FILE *mailbox = fopen(mailboxPath, "r");
                            if (mailbox == NULL) {
                                perror("Error opening mailbox file");
                                send(clientSocket, "-ERR Cannot open mailbox file\r\n", 32, 0);
                                return;
                            }

                            // Count the number of messages and calculate the total size
                            nom = 0;
                            for(int i=0;i<100;i++)
                            {
                                markedfordel[i]=0;
                            }
                        

                            while (1) {
                                printf("3");
                                char line[MAX_BUFFER_SIZE];
                                if (fgets(line, sizeof(line), mailbox) == NULL ) {
                                    break;  // End of mailbox
                                }

                                // Assume each message is separated by a line containing only a fullstop character
                               
                                 line[strcspn(line, "\r\n")] = '\0';

                                // Check if the line contains only a period
                                if (strcmp(line, ".") == 0) {
                                    nom++;
                                     // Assuming you want to exit after the first occurrence of a line with a period
                                }
                            // if( strcmp(line, ".\r\n") == 0)
                            // {
                            //     nom++;
                            // }
                            printf("number of messages =%d\n",nom);

                                
                            }
                            printf("4");

                            // Command loop
                            while (1) {
                                printf("5");
                                // Receive command from client
                                memset(buffer, 0, sizeof(buffer));
                                recv(clientSocket, buffer, sizeof(buffer), 0);

                                // Handle QUIT command
                               
                                 if(strstr(buffer, "STAT") != NULL)
                                {
                                    printf("server running the stat command");
                                    handleSTAT(clientSocket, username);
                                }
                                else if (strstr(buffer, "LIST") != NULL) {
                                    // Handle LIST command
                                    handleLIST(clientSocket, username);
                                }
                                else if (strstr(buffer, "RETR") != NULL) {
                                    // Handle RETR command
                                    int messageNumber;
                                    sscanf(buffer, "RETR %d", &messageNumber);
                                    handleRETR(clientSocket, username, messageNumber);
                                }
                                else if (strstr(buffer, "DELE") != NULL) {
                                    // Handle RETR command
                                    int messageNumber;
                                    sscanf(buffer, "DELE %d", &messageNumber);
                                    handleDELE(clientSocket, username, messageNumber);
                                }
                                else if (strstr(buffer, "QUIT") != NULL) {
                                    // Handle LIST command
                                    handleQUIT(clientSocket, username);
                                }
                                else if(strstr(buffer, "RSET") != NULL){
                                    handleRSET(clientSocket);
                                }

                                
                            }





                } else {
                    send(clientSocket, "-ERR Authentication failed\r\n", 31, 0);
                }
            } else {
                send(clientSocket, "-ERR Invalid command\r\n", 23, 0);
            }
        } else {
            send(clientSocket, "-ERR Invalid username\r\n", 25, 0);
        }
    } else {
        // Invalid command
        send(clientSocket, "-ERR Invalid command\r\n", 23, 0);
    }

   
}
