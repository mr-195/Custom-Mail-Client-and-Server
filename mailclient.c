#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SMTP_SERVER "iitkgp.edu"
#define SMTP_PORT 25
#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    // the program takes 3 command line arguments
    // server IP,smtp port,pop3port
    if (argc != 4)
    {
        printf("Usage: %s <server IP> <smtp port> <pop3 port>\n", argv[0]);
        exit(1);
    }
    int sockfd;
    struct sockaddr_in serv_addr;

    /* Opening a socket is exactly similar to the server process */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Unable to create socket\n");
        exit(0);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_aton(argv[1], &serv_addr.sin_addr);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect with server

    char username[100];
    char password[100];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    while (1)
    {
        printf("Enter option:\n");
        printf("1. Manage Mail\n");
        printf("2. Send Mail\n");
        printf("3. Quit\n");
        int option;
        scanf("%d", &option);
        if (option == 1)
        {
            // manage mail
            // send username and password to server
            send(sockfd, username, strlen(username), 0);
            send(sockfd, password, strlen(password), 0);
            // receive number of mails
            int num_mails;
            recv(sockfd, &num_mails, sizeof(num_mails), 0);
            printf("Number of mails: %d\n", num_mails);
            // receive mails
            for (int i = 0; i < num_mails; i++)
            {
                // receive mail
                char mail[1000];
                recv(sockfd, mail, sizeof(mail), 0);
                printf("Mail %d:\n", i + 1);
                printf("%s\n", mail);
            }
        }
        else if (option == 2)
        {
            // connect to server
            connect(sockfd, (struct sockaddr *)&serv_addr,
                    sizeof(serv_addr));
            // use SMTP protocol to send mail
            char mail [MAX_BUFFER_SIZE];
            char line [MAX_BUFFER_SIZE];
            // get mail details from user 
            fgets(mail,sizeof(mail), stdin);
            // check format of mail
            if(!isvalidmail(mail)){
                printf("Invalid mail format\n");
                continue;
            }
            else{
                // write to server 
                char *msg = "client connects to SMTP port>\n";
                write(sockfd, msg, strlen(msg));
                // read from server
                read(sockfd, line, sizeof(line));
                printf("%s\n", line);
                // wrtie HELO domain
                char *helo = "HELO iitkgp.edu\n";
                write(sockfd, helo, strlen(helo));

            }

            
        }
        else if (option == 3)
        {
            // quit
            break;
        }
        else
        {
            printf("Invalid option\n");
        }
    }
    close(sockfd);
    return 0;
}
