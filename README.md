# Multithreaded TCP Chat Server

A TCP chat application consisting of a multithreaded server written in C and a client written in Python. The server supports multiple simultaneous connections and real-time message broadcasting between connected clients.

## Features

- Multiple concurrent clients
- Usernames
- Real-time message broadcasting
- Join notifications
- Leave notifications
- Automatic cleanup of disconnected clients
- Thread-safe client management

## Technologies

- C
- Python
- TCP/IP
- POSIX sockets
- pthreads

## Prerequisites

- Python 3
- A C compiler that supports POSIX threads (Clang or GCC)
- A POSIX-compatible operating system (macOS or Linux)

## Architecture

```text
                     Server

                  Main Thread
                       │
                       ▼
                    accept()
                       │
                       ▼
               Create Worker Thread
                       │
        ┌──────────────┼──────────────┐
        ▼              ▼              ▼
    Client 1       Client 2       Client 3
      Thread         Thread         Thread
         │              │              │
      recv()         recv()         recv()
         │              │              │
         └────────── broadcast ────────┘
```

## Project Structure

```text
chat-server/
├── client/
│   └── client.py
├── server/
│   └── server.c
└── README.md
```

## Build

### Server

Compile the server with any C compiler that supports POSIX threads.

**Clang**

```bash
cd server

clang -Wall -Wextra -Wpedantic server.c -o server -pthread
```

**GCC**

```bash
cd server

gcc -Wall -Wextra -Wpedantic server.c -o server -pthread
```

## Usage

### Start the server

```bash
./server
```

### Start a client

Open a separate terminal for each client.

```bash
cd client

python3 client.py
```

### Example Chat Session

**Client 1**

```text
Connected to the server.

Enter your username: H

You: Hi, everyone! I'm H

J: Hey H, I'm J

*** J left the chat. ***
```

**Client 2**

```text
Connected to the server.

Enter your username: J

H: Hi, everyone! I'm H

You: Hey H, I'm J
```

## Implementation Details

- Thread-per-client architecture implemented with `pthread_create()`
- Connected clients are stored in a shared global client array
- A shared client registry protected by a mutex
- Message broadcasting between connected clients
- Automatic cleanup of disconnected clients
- A dedicated receive thread in the Python client for asynchronous message handling

## Future Improvements

- Private messaging
- Chat rooms
- Message history
- A custom application-layer protocol