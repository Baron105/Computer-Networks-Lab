#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

void set_nonblocking(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

char buf[2048];

void message_220(int client_socket)
{
    int n = recv(client_socket, buf, sizeof(buf), 0);
    buf[n] = '\0';
    printf("%s\n", buf);
}

int main()
{
    char server_ip[100];
    char smtp_port[100];
    char pop3_port[100];

    printf("Enter the server ip : ");
    scanf("%s", server_ip);

    printf("Enter the smtp port : ");
    scanf("%s", smtp_port);

    printf("Enter the pop3 port : ");
    scanf("%s", pop3_port);

    char username[100];
    char password[100];

    printf("Enter the username : ");
    scanf("%s", username);

    printf("Enter the password : ");
    scanf("%s", password);

    int client_socket;
    struct sockaddr_in server_addr, client_addr;

    // opening a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Unable to create socket\n");
        exit(0);
    }

    // binding the client (optional in this case)

    // server info
    server_addr.sin_family = AF_INET;
    inet_aton(server_ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(smtp_port));

    while (1)
    {
        printf("\n1.Manage mails\n2.Send mail\n3.Quit\n");

        int choice;
        scanf("%d", &choice);

        if (choice == 3)
        {
            // send receiver that quit is requested

            // also print the message response from server

            // break any connection is open
            return 0;
        }

        else if (choice == 1)
        {
        }
        else if (choice == 2)
        {
            // connect to the server
            if ((connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
            {
                perror("Unable to connect to server");
                exit(0);
            }

            // set the socket to non blocking
            set_nonblocking(client_socket);

            // so tcp connection is accepted , so receiver will send the message of 220
            message_220(client_socket);

            // if the message is not 220 , then close the connection and exit
            if (strncmp("220", buf, 3) != 0)
            {
                printf("Error in connection\n");
                close(client_socket);
                exit(0);
            }
            // send the HELO message with server domain name as <ip>@<port>
            char message[2048];
            sprintf(message, "HELO %s@%s\n", inet_ntoa(server_addr.sin_addr), smtp_port);
            send(client_socket, message, strlen(message), 0);
        }
    }
}