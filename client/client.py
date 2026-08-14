import socket

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client_socket.connect(("127.0.0.1", 8080))
client_socket.send(b"Hello!")

print("Connected to server!")

client_socket.close()