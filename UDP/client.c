#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message_to_server[5000], message_from_server[5000];
    socklen_t server_addr_len = sizeof(server_addr);

    // Clear the message buffers
    memset(message_to_server, '\0', sizeof(message_to_server));
    memset(message_from_server, '\0', sizeof(message_from_server));

    // Create a UDP socket
    client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket < 0) {
        perror("Failed to create UDP socket");
        return 1;
    }
    printf("UDP socket created successfully.\n");

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Get message input from user
    printf("Enter message to send to server: ");
    fgets(message_to_server, sizeof(message_to_server), stdin);

    // Send message to the server
    if (sendto(client_socket, message_to_server, strlen(message_to_server), 0,
               (struct sockaddr*)&server_addr, server_addr_len) < 0) {
        perror("Failed to send message to server");
        close(client_socket);
        return 1;
    }

    // Receive response from the server
    if (recvfrom(client_socket, message_from_server, sizeof(message_from_server), 0, NULL, NULL) < 0) {
        perror("Failed to receive message from server");
        close(client_socket);
        return 1;
    }

    printf("Server replied: %s\n", message_from_server);

    // Close the socket
    close(client_socket);

    return 0;
}
