# **IPK Project 1**

## Description

The project is an implementation of a client for simple calculator server. The client and server can communicate via TCP or UDP. The protocol is described in the assignment repository.

## Usage

The client can be compiled using `make` command. The binary is called `ipkcpc`. The client can be run using `./ipkcpc` command. The client should be run with the following arguments:

- `-h` - hostname of the server (either IPv4 address or hostname)
- `-p` - port of the server
- `-m` - mode of communication, either `tcp` or `udp`

## Implementation

The client is implemented in C++ in the C++20 standard. All of the code is in the `ipkcpc.cpp` source file. Everything is documented to the best detail, so this documentation will only describe the higher level structure and functionality of the code, for a more detailed explanation of parts of the code, please refer to the source code.

### Main function

Main function first parses command line arguments with `parse_args()` function into a global variable of `args_t` type. The arguments are checked if they are valid — non-empty hostname, valid port (range 1024-65535) and valid mode (either `tcp` or `udp`). If any of the arguments is invalid, the program exits with an error message.

Depending on the mode of communication, the appropriate function is called. If the mode is `tcp`, the `tcp_client()` function is called. If the mode is `udp`, the `udp_client()` function is called.

### TCP client

The TCP client first finds the server using the `gethostbyname2()` function. The server is then stored in the `server` structure. The structure is then used to open a TCP socket with the `socket()` function. The socket is then connected to the server with the `connect()` function. This connection lasts for the whole duration of the program. A buffer is created to store both sent and received data. Buffer of size `TCP_BUFSIZE` is created, which is 1024 bytes which should be more than enough for this application.

The program operates in a loop. First it gets the input from the user. This data is sent to the server as text with the `send()` function. The client then waits for a response from the server with the `recv()` function. The response is then printed to the user. The loop starts over.

The client can be terminated by sending `BYE` or pressing `Ctrl+C`, which also sends the `BYE` message to the server.
If the server's response is `BYE`, the client closes the socket, thus terminating the connection, and exits the program.


### UDP client

The UDP client finds the server and creates the structure `server` the same as the TCP client. Then it opens a UDP socket. There are two buffers, one of `UDP_BUFSIZE`, which is 255 bytes, given by the limitation of the protocol which sends the length encoded on 8 bits. The payload is 257 bytes to account for the opcode and data length information.

The client also operates in a loop. First, it gets input from the user. The 0th byte is the operation code, in the request it is set to `0`. The 1st byte is the length of the data following, calculated as `strlen()` of the buffer. The data is sent to the server with the `sendto()` function. The client then waits for a response from the server with the `recvfrom()` function. The 1st byte of the response is the status code, which says whether to print `OK:` or `ERR:`, then the result is printed. The loop starts over.

The client can be terminated by pressing `Ctrl+C`, which closes the socket, thus terminating the connection, and exits the program.

### Error handling

Client prints the appropriate error message standard error output and exits with the code 1. In case of server errors, the client prints the error message from the server, but exits with the code 0.

### Signal handling

The `Ctrl+C` signal is handled by the `sighandler()` function. The code is based on the [answer by Gab Royer on Stack Overflow][3]. The function sets the `continue_loop` global variable to `true`, which is then checked in the main loop. Then, the socket is closed. In TCP mode, the `BYE` message is sent to the server.

## Limitations

The client source code is using libraries only found on Unix systems, therefore, the code cannot be compiled on Windows. On Windows instead of `sys/socket.h`, `arpa/inet.h` and `netinet/in.h`, the `winsock2.h` library should be used which offers similar functionality.

## Testing

The client was tested both on macOS and the provided reference Linux VM against the reference server running on the VM. Testing was made by hand, running all the test cases described in the `testing.md` file, as I was unable to run a Python test script on the VM. Test cases are for basic functionality of error handling, UDP and TCP communication. On top of this, after implementing some change, the client was tested again with many inputs to make sure that the change did not break any functionality, including edge case inputs.

## References

[1]: KOUTENSKÝ, Michal. IPK-Projekty/Project 1 at master - IPK-Projekty - FIT - VUT Brno - git [online]. [cit. 2023-03-21]. Avaiable at: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201

[2]: KOUTENSKÝ, Michal. NESFIT/IPK-Projekty - IPK-Projekty - FIT - VUT Brno - git [online]. [cit. 2023-03-21]. Avaiable at: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/

[3]: ROYER, Gab. C++ - How can I catch a ctrl-c event? - Stack Overflow [online]. Oct 29, 2009 [cit. 2023-03-21]. Avaiable at: https://stackoverflow.com/a/1641223