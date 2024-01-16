#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int main()
{
    int sockfd,newsockfd;
    struct sockaddr_in serv_addr;

    // cannot read more than 100 bytes at a time
    char buffer[100]={'\0'};

    // creating socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed\n");
        exit(0);
    }

    // setting params
    serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(6969);

    char filename[100]; int fd;
    while(1)
    {
        for (int i = 0; i < 100; i++) filename[i] = '\0';
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
    scanf("%d", &k);

    // connection establishing
    if ((connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) < 0)
    {
		perror("Connection failed\n");
		exit(0);
	}

    // send key
    // convert to string and send
    sprintf(buffer, "%d", k);
    send(sockfd, buffer, sizeof(buffer), 0);

    printf("Key sent: %d\n", k);

    // set buffer to 0
    memset(buffer, 0, sizeof(buffer));

    // send filename
    send(sockfd, filename, sizeof(filename), 0);

    printf("Filename sent: %s\n", filename);

    memset(buffer, '\0', sizeof(buffer));

    // read file
    int rd = 1;
    while (rd)
    {
        // read 100 bytes into buffer
        rd = read(fd, buffer, 100);

        // transfer buffer data to server via send
        send(sockfd, buffer, rd, 0);

        memset(buffer, 0, sizeof(buffer));

        // exit if rd == 0 ie EOF reached
        if (rd == 0 || rd == -1) break;
    }

    printf("File data sent\n");

    // close file
    close(fd);

    // receive the encrypted file from server
    // save it as <original filename>.enc
    char encFilename[256];
    sprintf(encFilename, "%s.enc", filename);
    int encFile = open(encFilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    printf("Encrypted file created: %s\n", encFilename);

    // receive and write file data to the temporary file
    int br;
    while ((br = recv(sockfd, buffer, sizeof(buffer), 0)) > 0)
    {
        // printf("br: %d\n", br);
        if (br == 0 || br == -1) break;
        write(encFile, buffer, br);
    }

    printf("File data received\n");

    close(encFile);

    // print success message
    // stating the file is encrypted, giving the filenames of the original and the encrypted file
    printf("File encrypted successfully\n");
    printf("Original file: %s\n", filename);
    printf("Encrypted file: %s\n", encFilename);

    // close connection
    close(sockfd);

    return 0;
}