# Using TCP sockets

This is a simple implementation of a client-server model using TCP sockets. It is used to implement a simple Caesar Cipher encryption where the client sends a file (along with the key) to the server and the server sends back the encrypted file to the client.

The server uses the fork() system call to handle multiple clients at the same time.

## Usage

Run the following commands to get the executable files:

```gcc file_server.c -o server```

```gcc file_client.c -o client```

Then, run the executable files in two different terminals. Note that the server should be run first.

```./server```

```./client```

Enter the name of the file to be encrypted and the key to be used for encryption as prompted by the program. The encrypted file will be stored in the same directory as the client program.

There is an option to either continue encrypting files or to exit the program. 

Note that the server can handle multiple clients at the same time, so you can run multiple clients on different terminals, until the server limit is reached.
