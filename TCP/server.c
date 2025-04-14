#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 5000
#define CLIENT_MESSAGE_SIZE 500
#define SERVER_PORT 2000

int main(void) {
    int server_socket, client_socket, client_size;
    struct sockaddr_in server_address, client_address;
    char server_message[MAX_MESSAGE_SIZE], client_message[CLIENT_MESSAGE_SIZE];

    // Initialize the message buffers
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return -1;
    }
    printf("Server socket created successfully.\n");

    // Set up the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket to the address
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Binding failed");
        return -1;
    }
    printf("Binding successful.\n");

    // Start listening for incoming connections
    if (listen(server_socket, 1) < 0) {
        perror("Listen failed");
        return -1;
    }
    printf("Server is listening for connections...\n");

    // Accept incoming client connection
    client_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_size);
    if (client_socket < 0) {
        perror("Failed to accept client connection");
        return -1;
    }
    printf("Client connected from IP: %s, Port: %i\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Receive a message from the client
    if (recv(client_socket, client_message, sizeof(client_message), 0) < 0) {
        perror("Failed to receive message from client");
        return -1;
    }
    printf("Received message from client: %s\n", client_message);

    // Send a response to the client
    printf("Enter the message to send to the client: ");
    fgets(server_message, sizeof(server_message), stdin);

    if (send(client_socket, server_message, strlen(server_message), 0) < 0) {
        perror("Failed to send message to client");
        return -1;
    }

    // Close the connections
    close(client_socket);
    close(server_socket);
    return 0;
}
