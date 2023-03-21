# **IPK Project 1**

## Description

The project is an implementation of a client for simple calculator server. The client and server can communicate via TCP or UDP. The protocol is described in the assignment repository.

## Usage

The client can be compiled using `make` command. The binary is called `ipkcpc`. The client can be run using `./ipkcpc` command. The client should be run with the following arguments:

- `-h` - hostname of the server (either IPv4 address or hostname)
- `-p` - port of the server
- `-m` - mode of communication, either `tcp` or `udp`



### UDP client

### TCP client



## Limitations

The client source code is using libraries only found on Unix systems, therefore, the code cannot be compiled on Windows. On Windows instead of `sys/socket.h`, `arpa/inet.h` and `netinet/in.h`, the `winsock2.h` library should be used which offers similar functionality.

## Testing

The client was tested both on macOS and the provided reference Linux VM against the reference server running on the VM. Testing was made by hand, running all the test cases described in the `testing.md` file, as I was unable to run a Python test script on the VM. Test cases are for basic functionality of error handling, UDP and TCP communication. On top of this, after implementing some change, the client was tested again with many inputs to make sure that the change did not break any functionality, including edge case inputs.

