#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer_from_client[5000], buffer_to_client[5000];

    // Initialize message buffers
    memset(buffer_from_client, '\0', sizeof(buffer_from_client));
    memset(buffer_to_client, '\0', sizeof(buffer_to_client));

    // Create UDP socket
    server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket < 0) {
        perror("Failed to create UDP socket");
        return 1;
    }
    printf("UDP socket successfully created.\n");

    // Set server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket to the IP and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(server_socket);
        return 1;
    }
    printf("Socket bound to 127.0.0.1:2000\nWaiting for client message...\n");

    // Receive message from client
    if (recvfrom(server_socket, buffer_from_client, sizeof(buffer_from_client), 0,
                 (struct sockaddr*)&client_addr, &client_addr_len) < 0) {
        perror("Failed to receive message from client");
        close(server_socket);
        return 1;
    }

    printf("Received message from client [%s:%d]: %s\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer_from_client);

    // Get server's reply
    printf("Enter message to send to client: ");
    fgets(buffer_to_client, sizeof(buffer_to_client), stdin);

    // Send message to client
    if (sendto(server_socket, buffer_to_client, sizeof(buffer_to_client), 0,
               (struct sockaddr*)&client_addr, client_addr_len) < 0) {
        perror("Failed to send message to client");
        close(server_socket);
        return 1;
    }

    // Close the socket
    close(server_socket);
    printf("Connection closed.\n");

    return 0;
}
