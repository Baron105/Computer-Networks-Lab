#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_FILENAME_LENGTH 100
#define MAX_WORD_LENGTH 100
#define PORT 8888

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // loopback address, can change to different IP address if server is running on a different machine

    char filename[MAX_FILENAME_LENGTH];
    printf("Enter filename: ");
    fgets(filename, MAX_FILENAME_LENGTH, stdin);
    filename[strcspn(filename, "\n")] = '\0';

    sendto(sockfd, filename, strlen(filename), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    char response[MAX_WORD_LENGTH];
    int n = recvfrom(sockfd, response, MAX_WORD_LENGTH, 0, NULL, NULL);
    if (n == -1) {
        perror("Reception failed");
        exit(EXIT_FAILURE);
    }
    response[n] = '\0';

    if (strncmp(response, "NOTFOUND", 8) == 0) {
        printf("\nFile %s Not Found\n", filename);
        exit(EXIT_FAILURE);
    } else {
        printf("\nReceived: %s\n", response);
    }

    char word_request[MAX_WORD_LENGTH];
    sprintf(word_request, "WORD%d", 0);
    sendto(sockfd, word_request, strlen(word_request), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Sent: %s\n", word_request);

    char word[MAX_WORD_LENGTH];
    int lines = 1;
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("File creation failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        n = recvfrom(sockfd, word, MAX_WORD_LENGTH, 0, NULL, NULL);
        if (n == -1) {
            perror("Reception failed");
            exit(EXIT_FAILURE);
        }
        word[n] = '\0';
        printf("Received: %s\n", word);
        if (strncmp(word, "END", 3) == 0) {
            break;
        }

        fprintf(file, "%s", word);

        word_request[MAX_WORD_LENGTH];
        sprintf(word_request, "WORD%d", lines);
        sendto(sockfd, word_request, strlen(word_request), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        printf("Sent: %s\n", word_request);

        ++lines;
    }

    fclose(file);
    close(sockfd);
    printf("\nFile received and saved as output.txt\n");

    return 0;
}
