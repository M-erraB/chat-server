#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define USERNAME_SIZE 50
#define PORT 8080

// File descriptors for currently connected clients.
int clients[MAX_CLIENTS];
int client_count = 0;

// Protects clients[] and client_count from concurrent access.
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


// Remove a disconnected client from the global client list.
void remove_client(int client_fd)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] == client_fd)
        {
            // Shift remaining clients left to fill the gap.
            for (int j = i; j < client_count - 1; j++)
            {
                clients[j] = clients[j + 1];
            }

            client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}


// Send a message to every currently connected client.
// Send a message to every connected client except the sender.
void broadcast_message(const char *message, int sender_fd) {
    // Only one thread can access the client list at a time.
    pthread_mutex_lock(&clients_mutex);

    // Loop through every connected client.
    for (int i = 0; i < client_count; i++) {
        // Don't send the message back to the sender.
        if (clients[i] == sender_fd) {
            continue;
        }

        // Send the message to every other client.
        if (send(clients[i], message, strlen(message), 0) == -1) {
            perror("send");
        }
    }

    // Release the lock.
    pthread_mutex_unlock(&clients_mutex);
}


// Each connected client gets its own worker thread running this function.
void *handle_client(void *arg)
{
    // Copy the client's socket descriptor out of dynamically allocated memory.
    int client_fd = *(int *)arg;

    // The thread now owns the value, so the temporary allocation is no longer needed.
    free(arg);

    printf("Client connected. Client socket: %d\n", client_fd);

    char username[USERNAME_SIZE];

    // The first data sent by the client is treated as its username.
    int username_bytes =
        recv(client_fd,
             username,
             sizeof(username) - 1,
             0);

    if (username_bytes <= 0)
    {
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }

    // Turn the received bytes into a valid C string.
    username[username_bytes] = '\0';

    printf("%s joined the chat.\n", username);

    while (1)
    {
        char buffer[BUFFER_SIZE];

        // Wait for the next message from this client.
        int bytes_received =
            recv(client_fd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

        if (bytes_received == -1)
        {
            perror("recv");
            break;
        }

        if (bytes_received == 0) {
            printf("%s left the chat.\n", username);

        // Create a leave notification.
        char leave_message[100];

        snprintf(leave_message, sizeof(leave_message), "*** %s left the chat. ***", username);

        // Send the notification to the remaining clients.
        broadcast_message(leave_message, client_fd);

        break;
        }

        // Turn the received bytes into a valid C string.
        buffer[bytes_received] = '\0';
        
        // Ignore empty messages.
        if (strlen(buffer) == 0) {
            continue;
        }

        printf("%s: %s\n", username, buffer);

        // Build the message that every client will receive.
        char formatted_message[USERNAME_SIZE + BUFFER_SIZE + 3];

        // Create a message like:
        // "Mustafa: Hello everyone!"
        snprintf(formatted_message, sizeof(formatted_message), "%s: %s", username, buffer);

        // Send the message to every connected client except the sender.
        broadcast_message(formatted_message, client_fd);
    }

    // Remove the client before closing its socket.
    remove_client(client_fd);

    close(client_fd);

    return NULL;
}


int main(void)
{
    // Create an IPv4 TCP socket.
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("socket");
        return 1;
    }

    printf("Server socket created: %d\n", server_fd);

    // Allow port 8080 to be reused shortly after restarting the server.
    int opt = 1;

    if (setsockopt(server_fd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &opt,
                   sizeof(opt)) == -1)
    {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    // Describe the address where the server will listen.
    struct sockaddr_in server_addr;

    // Clear the structure before filling its fields.
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Associate the socket with 127.0.0.1:8080.
    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    printf("Server bound to port %d.\n", PORT);

    // Turn the bound socket into a listening socket.
    if (listen(server_fd, SOMAXCONN) == -1)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Main thread continuously accepts new clients.
    while (1)
    {
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        // Allocate a separate copy of client_fd for the new worker thread.
        int *client_ptr = malloc(sizeof(int));

        if (client_ptr == NULL)
        {
            perror("malloc");
            close(client_fd);
            continue;
        }

        *client_ptr = client_fd;

        // Add the client to the shared client list.
        pthread_mutex_lock(&clients_mutex);

        if (client_count >= MAX_CLIENTS)
        {
            pthread_mutex_unlock(&clients_mutex);

            printf("Maximum number of clients reached.\n");

            close(client_fd);
            free(client_ptr);

            continue;
        }

        clients[client_count] = client_fd;
        client_count++;

        pthread_mutex_unlock(&clients_mutex);

        // Create a worker thread for this client.
        pthread_t thread;

        int thread_result =
            pthread_create(&thread,
                           NULL,
                           handle_client,
                           client_ptr);

        if (thread_result != 0)
        {
            fprintf(stderr,
                    "pthread_create failed: %s\n",
                    strerror(thread_result));

            remove_client(client_fd);
            close(client_fd);
            free(client_ptr);

            continue;
        }

        // We don't need to join finished client threads later.
        pthread_detach(thread);
    }

    // Currently unreachable because the server runs indefinitely.
    close(server_fd);

    return 0;
}