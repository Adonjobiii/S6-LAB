#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

#define PORT 2000
#define BUFFER_SIZE 80

// Function to handle Go-Back-N ARQ logic
void goBackNClient(int sockfd, int totalFrames, int windowSize) {
    struct timeval timeout = {3, 0}; // 3 seconds timeout
    char buffer[BUFFER_SIZE];
    int ack, i = 0, n, k;
    int windowStart = 0, windowEnd = windowSize - 1;
    int j, resendFlag = 0;

    // Set socket receive timeout
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
        return;
    }

    // Send initial frames within the window
    for (i = 0; i < totalFrames && i <= windowEnd; i++) {
        snprintf(buffer, sizeof(buffer), "%d", i);
        send(sockfd, buffer, sizeof(buffer), 0);
        printf("Frame %d sent\n", i);
    }

    while (1) {
        if (windowEnd - windowStart != windowSize - 1 && resendFlag == 0 && i < totalFrames) {
            snprintf(buffer, sizeof(buffer), "%d", i);
            send(sockfd, buffer, sizeof(buffer), 0);
            printf("Frame %d sent\n", i);
            windowEnd++;
            i++;
        }

        resendFlag = 0;
        memset(buffer, 0, sizeof(buffer));
        n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        ack = atoi(buffer);

        if (n > 0) {
            if (ack + 1 == totalFrames) {
                printf("Acknowledgement received: %d\nAll frames sent successfully. Exiting.\n", ack);
                strcpy(buffer, "Exit");
                send(sockfd, buffer, sizeof(buffer), 0);
                break;
            }

            if (ack == windowStart) {
                windowStart++;
                printf("Acknowledgement received: %d\n", ack);
            }
        } else {
            printf("Acknowledgement not received for frame %d. Resending window...\n", windowStart);
            for (j = windowStart; j < totalFrames && j < windowStart + windowSize; j++) {
                snprintf(buffer, sizeof(buffer), "%d", j);
                send(sockfd, buffer, sizeof(buffer), 0);
                printf("Frame %d resent\n", j);
            }
            resendFlag = 1;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Client socket created successfully.\n");

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server at 127.0.0.1:%d\n", PORT);

    // Input from user
    int totalFrames, windowSize;
    printf("Enter number of frames to send: ");
    scanf("%d", &totalFrames);
    printf("Enter the window size:
