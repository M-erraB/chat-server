#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void)
{
    // File descriptor used to reference the server socket.
    int server_fd;

    // Create an IPv4 TCP socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // socket() returns -1 if socket creation fails.
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    printf("Server socket created: %d\n", server_fd);

    // Structure used to store the server's address information.
    struct sockaddr_in server_addr;

    // Configure the server's IPv4 address and port.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Bind the socket to the configured address.
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return 1;
    }

    printf("Server bound to port 8080.\n");

    // Mark the socket as a listening socket.
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        return 1;
    }

    printf("Server listening on port 8080...\n");

    // Keep the server running forever.
    while (1)
    {
        // Create a new socket for the connected client.
        int client_fd = accept(server_fd, NULL, NULL);

        // Check whether accept() failed.
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        printf("Client connected. Client socket: %d\n", client_fd);

        // Keep communicating with this client until it disconnects.
        while (1)
        {
            // Buffer used to store incoming messages.
            char buffer[1024];

            // Receive data from the client.
            int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

            // recv() returns -1 if an error occurs.
            if (bytes_received == -1) {
                perror("recv");
                break;
            }

            // recv() returns 0 if the client disconnects.
            if (bytes_received == 0)
            {
                printf("Client disconnected.\n");
                break;
            }

            // Add a null terminator to the received data.
            buffer[bytes_received] = '\0';

            // Print the client's message.
            printf("Received: %s\n", buffer);

            // Message sent back to the client.
            char response[] = "Message received";

            // Send a response to the client.
            if (send(client_fd, response, strlen(response), 0) == -1){
                perror("send");
                break;
            }
        }

        // Close the connected client's socket.
        close(client_fd);
    }

    // Close the server socket.
    close(server_fd);

    return 0;
}