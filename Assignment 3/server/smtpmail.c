#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// make a mailbox for a user
void mkmailbox(const char *username) {
    char path[42];
    snprintf(path, sizeof(path), "%s/mailbox", username);

    FILE *mailbox = fopen(path, "w");
    if (mailbox == NULL) {
        perror("Error creating mailbox");
        exit(EXIT_FAILURE);
    }

    fclose(mailbox);
}

// make mailboxes for all users from user.txt
void mkuserdir(const char *filename) {
    FILE *file;
    char line[42];
    int n = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening user.txt");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char username[20];
        sscanf(line, "%s", username);

        if (mkdir(username, 0700) != 0) {
            perror("Error creating user directory");
            exit(EXIT_FAILURE);
        }

        mkmailbox(username);

        n++;
    }

    fclose(file);
}

// set the socket to non-blocking
void set_nonblocking(int *sockfd)
{
    int flags = fcntl(*sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    if (fcntl(*sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    // make mailboxes for all users
    mkuserdir("user.txt");

    int server_socket, client_socket;
    int new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);

    char buf[2048];

    // creating the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Error in creating the socket\n");
        exit(1);
    }

    // setting the server address
    int port;
    printf("Enter the port number : ");
    scanf("%d", &port);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error in binding\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // listen for the connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Error in listening\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("SMTP server ready and listening for connections\n");

    while (1)
    {
        // accept the connection
        new_sock = accept(server_socket, (struct sockaddr *)&client_addr, &sin_len);

        printf("check\n");
        if (new_sock < 0)
        {
            perror("Error in accepting the connection\n");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        // making a fork for concurrency
        int pid;
        if ((pid = fork()) == 0)
        {
            // close the old socket in the child process
            close(server_socket);

            // connection established
            char message[2048] = "220 <Domain_name> Service ready\n";

            send(new_sock, message, strlen(message), 0);
            close(new_sock);
            exit(0);
        }
        close(new_sock);
    }
}