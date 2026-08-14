import socket

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client_socket.connect(("127.0.0.1", 8080))
client_socket.send(b"Hello!")

response = client_socket.recv(1024)
print(response.decode())

client_socket.close()