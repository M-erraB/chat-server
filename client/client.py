import socket

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client_socket.connect(("127.0.0.1", 8080))

print("Connected to the server.")

# Ask for a username.
username = input("Enter your username: ")

# Send the username to the server.
client_socket.send(username.encode())

while True:
    message = input("You: ")

    if message == "/quit":
        break

    client_socket.send(message.encode())

    response = client_socket.recv(1024)

    if not response:
        break

    print("Server:", response.decode())

client_socket.close()