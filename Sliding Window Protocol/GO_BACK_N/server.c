#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 2000
#define MAX_BUFFER 1024

int main() {
    int serverSock, clientSock;
    socklen_t clientSize;
    struct sockaddr_in serverAddr, clientAddr;

    char buffer[MAX_BUFFER];
    int frameCount = 5;
    int frameNumber = 1;

    // Create socket
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }
    printf("✅ Server socket created successfully.\n");

    // Bind server to IP and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding failed");
        return EXIT_FAILURE;
    }
    printf("✅ Binding successful.\n");

    // Start listening
    if (listen(serverSock, 1) < 0) {
        perror("Listening failed");
        return EXIT_FAILURE;
    }
    printf("🔊 Listening for incoming connections...\n");

    // Accept client connection
    clientSize = sizeof(clientAddr);
    clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientSize);
    if (clientSock < 0) {
        perror("Connection accept failed");
        return EXIT_FAILURE;
    }
    printf("✅ Connected to client: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Start receiving frames
    while (frameCount--) {
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recv(clientSock, buffer, sizeof(buffer), 0);

        if (recvLen < 0) {
            perror("❌ Error receiving data");
            exit(EXIT_FAILURE);
        }

        // Simulate frame reception
        if (strncmp(buffer, "frame", 5) == 0) {
            printf("📥 Frame %d received\n", frameNumber);
        } else if (strncmp(buffer, "Exit", 4) == 0) {
            printf("🚪 Exit signal received from client. Closing connection.\n");
            break;
        } else {
            printf("⚠️  Invalid frame received\n");
        }

        // Simulate ACK loss for odd-numbered frames
        if (frameNumber % 2 == 0) {
            strcpy(buffer, "ack");
        } else {
            printf("⚠️  Simulating ACK loss for frame %d\n", frameNumber);
            strcpy


