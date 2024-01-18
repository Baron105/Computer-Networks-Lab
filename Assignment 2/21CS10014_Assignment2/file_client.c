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
    struct sockaddr_in serv_addr;

    // buffer size < 100 recommended in assignment
    char buffer[64] = {'\0'};

    while (1) {

        // creating socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation failed");
            exit(0);
        }

        // setting params for client
        serv_addr.sin_family = AF_INET;
        inet_aton("127.0.0.1", & serv_addr.sin_addr);
        serv_addr.sin_port = htons(6969);

        // getting filename
        char filename[64];
        int fd;
        while (1) {
            memset(filename, '\0', sizeof(filename));
            printf("Enter filename: ");
            scanf("%s", filename);

            // open the file if it exists
            fd = open(filename, O_RDONLY);
            if (fd != -1) break;

            // else print error and ask again
            printf("File doesnt exist, please try again\n");
        }

        // filename accessed
        // ask for key
        int k;
        printf("Enter the key: ");
        scanf("%d", & k);

        // connection establishing
        if ((connect(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr))) < 0) {
            perror("Connection failed");
            exit(0);
        }

        // send key
        // convert to string and send
        sprintf(buffer, "%d", k);
        send(sockfd, buffer, sizeof(buffer), 0);
        printf("Key sent: %d\n", k);

        memset(buffer, '\0', sizeof(buffer));

        // send filename
        send(sockfd, filename, sizeof(filename), 0);
        printf("Filename sent: %s\n", filename);

        memset(buffer, '\0', sizeof(buffer));

        // read file
        int rd = 1;
        while (rd) {
            // read 64 bytes into buffer
            rd = read(fd, buffer, 64);
            // printf("rd: %d\n", rd);

            // transfer buffer data to server via send
            send(sockfd, buffer, rd, 0);

            memset(buffer, 0, sizeof(buffer));

            // exit if rd == 0 ie EOF reached
            // or if rd == -1 ie error
            if (rd == 0 || rd == -1) break;
        }

        printf("File data sent\n");

        // close file
        close(fd);

        // receive the encrypted file from server
        // save it as <original filename>.enc
        char encfilename[100];
        sprintf(encfilename, "%s.enc", filename);
        int encfile = open(encfilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);

        printf("Encrypted file created: %s\n", encfilename);

        // receive and write file data to the created file
        int br;
        while ((br = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            // printf("br: %d\n", br);

            // if br == 0, it means that the file has been completely received
            // if br == -1, it means that there was an error
            if (br == 0 || br == -1) break;
            write(encfile, buffer, br);
        }

        printf("File data received\n");

        close(encfile);

        // print success message
        // stating the file is encrypted, giving the filenames of the original and the encrypted file
        printf("\nFile encrypted successfully and received\n");
        printf("Original file: %s\n", filename);
        printf("Encrypted file: %s\n", encfilename);

        // close connection
        close(sockfd);

        // ask if user wants to continue
        printf("\nDo you want to continue? (y/n): ");
        char ch;
        scanf(" %c", & ch);
        if (ch == 'n') {
            printf("Exiting...\n");
            break;
        }
        else if (ch == 'y') printf("Continuing...\n\n");
        else printf("Invalid input, exiting...\n");
    }

    return 0;
}