#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 2000
#define MAX_BUFFER 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER];
    int frameCount = 5;
    int frameNumber = 1;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("❌ Socket creation failed");
        return EXIT_FAILURE;
    }
    printf("✅ Client socket created successfully.\n");

    // Define server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("❌ Connection failed");
        return EXIT_FAILURE;
    }
    printf("🔗 Connected to the server.\n");

    while (frameCount--) {
        memset(buffer, 0, sizeof(buffer));

        if (frameNumber % 2 == 0) {
            // Send normal frame
            strcpy(buffer, "frame");
            printf("📤 Sending frame %d\n", frameNumber);
        } else {
            // Simulate packet loss
            printf("⚠️  Packet lost for frame %d\n", frameNumber);
            strcpy(buffer, "error");

            for (int i = 0; i < 3; i++) {
                printf("🕒 Waiting for %d sec\n", i + 1);
                sleep(1);
            }

            printf("🔁 Retransmitting frame %d...\n", frameNumber);
            strcpy(buffer, "frame");
            sleep(1); // Extra delay before sending
        }

        // Send frame
        if (send(sockfd, buffer, strlen(buffer) + 1, 0) < 0) {
            perror("❌ Error sending frame");
            exit(EXIT_FAILURE);
        }

        printf("✅ Frame %d sent\n", frameNumber);

        // Receive ACK
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
        if (recvLen < 0) {
            perror("❌
