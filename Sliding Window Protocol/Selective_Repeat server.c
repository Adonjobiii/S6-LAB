#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void processFrames(int connfd) {
    char buffer[MAX];
    int frameNo, action, ack;
    srand(time(NULL)); // Seed for random behavior

    while (1) {
        sleep(1);
        bzero(buffer, MAX);

        // Receive frame
        recv(connfd, buffer, MAX, 0);

        // Exit if "Exit" received
        if (strncmp("Exit", buffer, 4) == 0) {
            printf("🔚 Exit command received. Closing connection.\n");
            break;
        }

        // Parse frame number
        frameNo = atoi(buffer);
        action = rand() % 3;

        switch (action) {
            case 0:
                // Simulate frame not received
                printf("❌ Frame %d not received (simulated loss)\n", frameNo);
                ack = -1;
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(connfd, buffer, sizeof(buffer), 0);
                printf("📤 Sent Negative Acknowledgement for frame %d\n", frameNo);
                break;

            case 1:
                // Simulate delayed ACK
                ack = frameNo;
                sleep(2);
                printf("📥 Frame %d received (with delay)\n", frameNo);
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(connfd, buffer, sizeof(buffer), 0);
                printf("📤 Sent Acknowledgement: %d\n", ack);
                break;

            case 2:
                // Simulate immediate ACK
                ack = frameNo;
                printf("📥 Frame %d received\n", frameNo);
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(connfd, buffer, sizeof(buffer), 0);
                printf("📤 Sent Acknowledgement: %d\n", ack);
                break;
        }
    }
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("❌ Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("✅ Socket successfully created\n");

    // Prepare server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        perror("❌ Socket bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("✅ Socket successfully binded\n");

    // Listen for connections
    if (listen(sockfd, 5) != 0) {
        perror("❌ Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("🔊 Server listening on port %d...\n", PORT);

    // Accept client
    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        perror("❌ Server accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("🤝 Connection accepted from client\n");

    // Start frame processing
    processFrames(connfd);

    // Close socket
    close(sockfd);
    printf("🛑 Server socket closed\n");

    return 0;
}
