#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in cli_addr, serv_addr;

    // buffer size < 100 recommended in assignment
    char buffer[64] = {'\0'};

    // creating socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(0);
    }

    // setting params of server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(6969);

    // binding
    if (bind(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0) {
        perror("Binding failed");
        exit(0);
    }

    // listening limit
    listen(sockfd, 10);

    // start accepting
    while (1) {
        // accept connection from client
        int cli_len = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr * ) & cli_addr, & (cli_len));

        if (newsockfd < 0) {
            perror("Accepting failed");
            exit(0);
        }

        // handling multiple clients using fork
        if (fork() == 0) {

            // close the original socket    
            close(sockfd);

            // now start communicating

            // receive the value of key
            int k;

            // it is received as a string, so we need to convert it to int
            recv(newsockfd, buffer, sizeof(buffer), 0);
            k = atoi(buffer);
            printf("Key received: %d\n", k);
            memset(buffer, '\0', sizeof(buffer));

            // get the filename from client
            char filename[64];
            recv(newsockfd, filename, sizeof(buffer), 0);
            memset(buffer, '\0', sizeof(buffer));

            printf("Filename received: %s\n", filename);

            // now create a new file with name <IP.port of client>.txt
            char tempfilename[100];
            sprintf(tempfilename, "%s.%d.txt", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            int tempfile = open(tempfilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);

            printf("Temporary file created: %s\n", tempfilename);

            // receive and write file data to the temporary file
            int br = 1;
            while (1) {
                br = recv(newsockfd, buffer, sizeof(buffer), 0);
                // printf("br: %d\n", br);

                // if buffer is less than 64, it means that the file has no more data after this
                // so write the remaining data and break
                if (br < 64 && br > 0) {
                    write(tempfile, buffer, br);
                    break;
                }

                // if br == 0, it means that the file has been completely received
                // if br == -1, it means that there was an error
                else if (br == 0 || br == -1) break;

                write(tempfile, buffer, br);
            }

            printf("File data received\n");

            close(tempfile);

            // encrypt the file
            // encrypted file will be named <IP.port of client>.txt.enc
            char encfilename[100];
            sprintf(encfilename, "%s.%d.txt.enc", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            int encfile = open(encfilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);

            printf("Encrypted file created: %s\n", encfilename);

            // write encrypted data to the new file
            int fd = open(tempfilename, O_RDONLY);
            while ((br = read(fd, buffer, sizeof(buffer))) > 0) {
                for (int i = 0; i < br; i++) {
                    // caesar cipher
                    // lowercase
                    if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                        buffer[i] = (buffer[i] - 'a' + k) % 26 + 'a';
                    }

                    // uppercase
                    else if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
                        buffer[i] = (buffer[i] - 'A' + k) % 26 + 'A';
                    }

                    // else do nothing
                }
                write(encfile, buffer, br);
            }

            close(fd);

            printf("File encryption complete\n");

            // now send the encrypted file back to the client
            int efd = open(encfilename, O_RDONLY);
            while ((br = read(efd, buffer, sizeof(buffer))) > 0) {
                send(newsockfd, buffer, br, 0);
                // printf("br: %d\n", br);
            }

            memset(buffer, '\0', sizeof(buffer));

            close(efd);

            printf("Encrypted file sent\n\n");

            // close the connection
            close(newsockfd);

            exit(0);
        }

        close(newsockfd);
    }
}