#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 2000
#define BUFFER_SIZE 100

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    FILE *filePtr;
    char fileToRequest[BUFFER_SIZE], fileToSave[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }
    printf("Client socket created successfully.\n");

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to server failed");
        return 1;
    }
    printf("Connected to the server at 127.0.0.1:%d\n", SERVER_PORT);

    // Get filename to request and local filename to store
    printf("Enter the filename to request from server: ");
    scanf("%s", fileToRequest);
    printf("Enter the name of the local file to save the content: ");
    scanf("%s", fileToSave);

    // Open file for writing
    filePtr = fopen(fileToSave, "w");
    if (filePtr == NULL) {
        perror("Failed to create local file");
        close(clientSocket);
        return 1;
    }

    // Send filename to server
    send(clientSocket, fileToRequest, strlen(fileToRequest), 0);

    // Receive and write content
    while (1) {
        bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            perror("Error receiving data");
            break;
        }

        buffer[bytesReceived] = '\0';

        if (strcmp(buffer, "error") == 0) {
            printf("\nFile does not exist on server.\n");
            fclose(filePtr);
            remove(fileToSave);  // delete empty local file
            break;
        } else if (strcmp(buffer, "completed") == 0) {
            printf("\nFile transfer completed successfully.\n");
            fclose(filePtr);
            break;
        } else {
            printf("%s", buffer);  // Print to console
            fprintf(filePtr, "%s", buffer);  // Write to file
        }
    }

    // Close the socket
    close(clientSocket);
    return 0;
}
