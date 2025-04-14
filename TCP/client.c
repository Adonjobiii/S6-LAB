#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char message_to_server[500], message_from_server[5000];

    // Clear message buffers
    memset(message_to_server, '\0', sizeof(message_to_server));
    memset(message_from_server, '\0', sizeof(message_from_server));

    // Create a TCP socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Failed to create socket");
        return 1;
    }
    printf("Client socket created successfully.\n");

    // Define server details
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(2000);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Attempt to connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Connection to server failed");
        close(client_socket);
        return 1;
    }
    printf("Connected to the server.\n");

    // Get message input from user
    printf("Enter message to send: ");
    fgets(message_to_server, sizeof(message_to_server), stdin);

    // Send the message to the server
    if (send(client_socket, message_to_server, strlen(message_to_server), 0) < 0) {
        perror("Failed to send message");
        close(client_socket);
        return 1;
    }

    // Receive message from the server
    if (recv(client_socket, message_from_server, sizeof(message_from_server), 0) < 0) {
        perror("Failed to receive message");
        close(client_socket);
        return 1;
    }

    printf("Message from server: %s\n", message_from_server);

    // Close the socket connection
    close(client_socket);
    return 0;
}
