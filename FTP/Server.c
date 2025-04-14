#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 2000
#define BUFFER_SIZE 100

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;
    FILE *filePtr;
    char buffer[BUFFER_SIZE], fileName[BUFFER_SIZE];

    // Create the TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }
    printf("Server socket created successfully.\n");

    // Define server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket to the IP and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    printf("Binding completed.\n");

    // Start listening for incoming connections
    if (listen(serverSocket, 1) < 0) {
        perror("Listen failed");
        return 1;
    }
    printf("Server is listening on port %d...\n", PORT);

    // Accept a client connection
    addrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
    if (clientSocket < 0) {
        perror("Connection acceptance failed");
        return 1;
    }

    printf("Connected to client: %s:%d\n", 
        inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Receive filename from client
    int bytesReceived = recv(clientSocket, fileName, sizeof(fileName), 0);
    if (bytesReceived <= 0) {
        perror("Failed to receive filename");
        close(clientSocket);
        close(serverSocket);
        return 1;
    }
    fileName[bytesReceived] = '\0';

    // Try to open the requested file
    filePtr = fopen(fileName, "r");
    if (filePtr == NULL) {
        send(clientSocket, "error", strlen("error"), 0);
        perror("Requested file not found");
    } else {
        // Send the file contents line by line
        while (fgets(buffer, sizeof(buffer), filePtr)) {
            if (send(clientSocket, buffer, sizeof(buffer), 0) < 0) {
                perror("Failed to send file contents");
                break;
            }
            sleep(1);  // Simulate transmission delay
        }
        // Notify completion
        send(clientSocket, "completed", strlen("completed"), 0);
        printf("File sent successfully.\n");
        fclose(filePtr);
    }

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
