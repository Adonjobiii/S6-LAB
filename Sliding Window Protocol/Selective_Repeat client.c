#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

struct timeval timeout;

void sendFrames(int sockfd, int totalFrames, int windowSize) {
    char buffer[MAX];
    int ack, sent = 0, n, i;
    int windowStart = 0, windowEnd = windowSize - 1;
    int acksReceived = 0;
    int sendStatus, flag = 0;

    // Set timeout for receiving
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
    }

    // Send initial window frames
    for (i = 0; i < totalFrames && i <= windowEnd; i++) {
        bzero(buffer, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d", i);
        send(sockfd, buffer, sizeof(buffer), 0);
        printf("📤 Frame %d sent\n", i);
    }

    while (1) {
        if (acksReceived == totalFrames) {
            strcpy(buffer, "Exit");
            send(sockfd, buffer, sizeof(buffer), 0);
            break;
        }

        // Fill up window with new frames if space available
        if ((windowEnd - windowStart != windowSize - 1) && flag == 0 && sent < totalFrames) {
            bzero(buffer, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%d", sent);
            send(sockfd, buffer, sizeof(buffer), 0);
            printf("📤 Frame %d sent\n", sent);
            windowEnd++;
            sent++;
        }

        flag = 0;

        // Try receiving ACK
        bzero(buffer, sizeof(buffer));
        n = recv(sockfd, buffer, MAX, 0);
        ack = atoi(buffer);

        if (n > 0) {
            if (ack == -1) {
                // Negative ACK: resend window start
                printf("❌ ACK not received for Frame %d. Resending...\n", windowStart);
                bzero(buffer, sizeof(buffer));
                snprintf(buffer, sizeof(buffer), "%d", windowStart);
                send(sockfd, buffer, sizeof(buffer), 0);
                printf("📤 Frame %d resent\n", windowStart);
            } else {
                // Positive ACK
                printf("✅ ACK received for Frame %d\n", ack);
                windowStart++;
                acksReceived++;
            }
        } else {
            // Timeout case: optional to handle
            printf("⏳ Timeout waiting for ACK...\n");
        }
    }
}

int main() {
    int sockfd, totalFrames, windowSize;
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("❌ Socket creation failed");
        exit(0);
    }
    printf("✅ Socket successfully created\n");

    // Setup server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    servaddr.sin_port = htons(PORT);

    // Set receive timeout (3 seconds)
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // Connect to server
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        perror("❌ Connection to the server failed");
        exit(0);
    }
    printf("🤝 Connected to server\n");

    // Get frame & window details
    printf("Enter the number of frames: ");
    scanf("%d", &totalFrames);
    printf("Enter the window size: ");
    scanf("%d", &windowSize);

    // Start sending frames
    sendFrames(sockfd, totalFrames, windowSize);

    // Close socket
    close(sockfd);
    printf("🔒 Connection closed\n");

    return 0;
}
