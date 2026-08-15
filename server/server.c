#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;

    free(arg);

    printf("Client connected. Client socket: %d\n", client_fd);
    
    char username[50];

    int username_bytes = recv(client_fd, username, sizeof(username) - 1, 0);

    if (username_bytes <= 0) {
    close(client_fd);
    return NULL;
    }

    username[username_bytes] = '\0';

    printf("%s joined the chat.\n", username);

    while (1) {
        char buffer[1024];

        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == -1) {
            perror("recv");
            break;
        }

        if (bytes_received == 0) {
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0';

        printf("%s: %s\n", username, buffer);

        char response[] = "Message received";

        if (send(client_fd, response, strlen(response), 0) == -1) {
            perror("send");
            break;
        }
    }

    close(client_fd);

    return NULL;
}
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
    while (1) {
    int client_fd = accept(server_fd, NULL, NULL);

    if (client_fd == -1) {
        perror("accept");
        continue;
    }

    int *client_ptr = malloc(sizeof(int));

    *client_ptr = client_fd;

    pthread_t thread;

    if (pthread_create(&thread, NULL, handle_client, client_ptr) != 0) {
        perror("pthread_create");

        close(client_fd);

        free(client_ptr);

        continue;
    }

    pthread_detach(thread);
}

    // Close the server socket.
    close(server_fd);

    return 0;
}