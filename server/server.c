#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

    struct sockaddr_in server_addr;

    //Configure the server's IPv4 address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    printf("Server socket created: %d\n", server_fd);

    //Bind the server to our configured address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return 1;
    }

    printf("Server bound to port 8080.");

    return 0;
}