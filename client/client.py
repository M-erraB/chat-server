import socket

# Create an IPv4 TCP socket.
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server running on localhost and port 8080.
client_socket.connect(("127.0.0.1", 8080))

print("Connected to the server.")

# Keep sending messages until the user quits.
while True:

    # Get a message from the keyboard.
    message = input("You: ")

    # Exit the chat if the user types /quit.
    if message == "/quit":
        break

    # Convert the Python string into bytes and send it.
    client_socket.send(message.encode())

    # Wait for the server's response.
    response = client_socket.recv(1024)

    # If the server closes the connection, stop the loop.
    if not response:
        print("Server disconnected.")
        break

    # Convert the received bytes back into a Python string.
    print("Server:", response.decode())

# Close the connection.
client_socket.close()