# Simple Datagram Socket using POSIX C

This is a simple datagram socket program written in C.

## Usage

Run the following command to get the executable files:

```gcc wordclient.c -o wordclient```
```gcc wordserver.c -o wordserver```

Then, run the executable files in two different terminals. Note that the server should be run first.

```./wordserver```
```./wordclient```

Enter the filename in the client terminal and the server will send the contents of the file to the client. Output will be stored in the file "output.txt". The file "input.txt" is an example input file in the specified format.

## Is this a good file transfer protocol?

No, this is not a good file transfer protocol.

The file transmission protocol used in this program is not reliable as there is no error correction method available. The protocol also assumes that the file begins with "HELLO" and ends with "END", which is not true in most cases. The protocol also does not support file transfer of files larger than 1000 bytes.
