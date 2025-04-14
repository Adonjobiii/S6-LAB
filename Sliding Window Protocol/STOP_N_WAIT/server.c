#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 2000
#define BUFFER_SIZE 80

// Function to simulate server-side Stop-and-Wait ARQ logic
void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    int frame, choice, ack = 0, expected = 0;

    while (1) {
        sleep(1);
        memset(buffer, 0, BUFFER_SIZE);
        recv(clientSocket, buffer, BUFFER_SIZE, 0);

        if (strcmp(buffer, "Exit") == 0) {
            printf("Client requested exit. Terminating connection.\n");
            break;
        }

        frame = atoi(buffer);

        if (frame != expected) {
            printf("Frame %d discarded. Expected frame: %d\n", frame, expected);
            snprintf(buffer, BUFFER_SIZE, "%d", ack);
            send(clientSocket, buffer, strlen(buffer), 0);
            continue;
        }

        choice = rand() % 3;
        ack = frame;

        if (choice == 1) {
            sleep(2);  // Simulate delay
        }

        printf("Frame %d received. Acknowledgement sent: %d\n", frame, ack);
        snprintf(buffer, BUFFER_SIZE, "%d", ack);
        send(clientSocket, buffer, strlen(buffer), 0);
        expected = ack + 1;
    }
}

int main() {
    int serverSocket, clientSocket, clientLen;
    struct sockaddr_in serverAddr, clientAddr;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Server socket created successfully.\n");

    // Prepare server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind socket to IP/port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to port %d.\n", PORT);

    // Start listening
    if (listen(serverSocket, 1) < 0) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    clientLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, (socklen_t *)&clientLen);
    if (clientSocket < 0) {
        perror("Failed to accept client connection");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Client connected: %s:%d\n",
           inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Handle client communication
    handleClient(clientSocket);

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
