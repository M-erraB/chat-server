#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void)
{
    //File descriptor used to reference the server socket
    int server_fd;

    //Create an IPv4 TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    //socket() returns -1 if socket creation fails
    if (server_fd == -1)
    {
        perror("socket");
        return 1;
        
    }

    printf("Server socket created: %d\n", server_fd);


    struct sockaddr_in server_addr;

    //Configure the server's IPv4 address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    //Bind the server to our configured address
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

    // Accept clients forever.
    while (1) {
        // File descriptor used to reference the connected client.
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1) {
        perror("accept");
        continue;
        }

        printf("Client connected successfully. Client socket: %d\n", client_fd);

        char buffer[1024];
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == -1) {
            perror("recv");
            close(client_fd);
            continue;
        }

        buffer[bytes_received] = '\0';

        printf("Received: %s\n", buffer);

        char response[] = "Hello from the server";

        if (send(client_fd, response, strlen(response), 0) == -1) {
            perror("send");
        }

        printf("Response sent.\n");

        close(client_fd);
    }

return 0;

}