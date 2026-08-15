import socket
import threading

HOST = "127.0.0.1"
PORT = 8080
BUFFER_SIZE = 1024


# Continuously receive messages while the main thread handles keyboard input.
def receive_messages():
    while True:
        try:
            message = client_socket.recv(BUFFER_SIZE)

            # Empty bytes mean the server closed the connection.
            if not message:
                print("\nServer disconnected.")
                break

            print("\n" + message.decode())
            print("You: ", end="", flush=True)

        except OSError:
            # The socket may have been closed while this thread was waiting.
            break


# Create an IPv4 TCP socket.
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the chat server.
client_socket.connect((HOST, PORT))

print("Connected to the server.")

username = input("Enter your username: ").strip()

# Prevent an empty username.
while not username:
    username = input("Username cannot be empty. Enter your username: ").strip()

# The server expects the username first.
client_socket.sendall(username.encode())

# Separate thread listens for incoming broadcasts.
receiver_thread = threading.Thread(
    target=receive_messages,
    daemon=True
)

receiver_thread.start()

try:
    while True:
        message = input("You: ")

        if message == "/quit":
            break

        if not message:
            continue

        # sendall() keeps sending until all bytes have been handed to the socket.
        client_socket.sendall(message.encode())

except KeyboardInterrupt:
    print("\nLeaving chat...")

finally:
    # Shut down and close the client connection.
    try:
        client_socket.shutdown(socket.SHUT_RDWR)
    except OSError:
        pass

    client_socket.close()