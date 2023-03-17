/**
 * @file ipkcpc.cpp
 *
 * @name IPK project
 * @brief Remote calculator UDP or TCP client
 *
 * @author Matyas Strelec (xstrel03)
 * @date 2023-03-12
 */

 // Cross-platform includes, will work on both Unix and Windows
#include <iostream>
#include <string>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// Unix-specific includes, will work only on Linux, macOS, etc.
// Would need to be changed for Windows (Along with the rest of the code)
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFSIZE 8092

int client_socket;

// No need to write std:: for everything
using namespace std;

/**
 * @brief Struct for parsed command-line arguments
 *
 */
struct args_t
{
    string host; // IPv4 address of the server
    string port; // Port of the server
    string mode; // 'tcp' or 'udp'
};

void close_socket() {
    close(client_socket);
}

/**
 * @brief Parses command-line arguments
 *
 * Usage: ipkcpc -h <host> -p <port> -m <mode>
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return Struct containing parsed arguments
 */
args_t parse_args(int argc, char** argv) {
    args_t args;

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-h") {
            args.host = string(argv[i + 1]);
        }
        else if (string(argv[i]) == "-p") {
            args.port = string(argv[i + 1]);
        }
        else if (string(argv[i]) == "-m") {
            args.mode = string(argv[i + 1]);
        }
    }

    return args;
}

int tcp_client(string host, string port) {

    return 0;
}

int udp_client(string host, string port) {
    const char* server_hostname = host.c_str();
    int port_number = atoi(port.c_str());

    int bytestx;
    int bytesrx;
    socklen_t serverlen;
    struct hostent* server;
    struct sockaddr_in server_address;

    server = gethostbyname2(server_hostname, AF_INET);
    if (server == NULL) {
        cout << "Error: No such host as " << server_hostname << endl;
        return EXIT_FAILURE;
    }

    // Set the server address
    // Zero the structure
    bzero((char*)&server_address, sizeof(server_address));
    // Set the address family, AF_INET for IPv4
    server_address.sin_family = AF_INET;
    // Copy the server address to the structure
    bcopy((char*)server->h_addr, (char*)&server_address.sin_addr.s_addr, server->h_length);
    // Set the port number
    server_address.sin_port = htons(port_number);

    // Create the socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket <= 0) {
        perror("Error: Socket");
        return EXIT_FAILURE;
    }

    char buffer[BUFSIZE];
    char payload[BUFSIZE];

    // Get the payload data from the user
    cin.getline(payload, BUFSIZE);

    // Set the opcode to 0 for request
    buffer[0] = 0;

    // Get the length of the payload data
    size_t payload_length = strlen(payload);

    // Set the payload length in the buffer
    buffer[1] = static_cast<char>(payload_length);

    // Copy the payload data to the buffer
    strcpy(&buffer[2], payload);

    /* Send message to the server */
    serverlen = sizeof(server_address);
    bytestx = sendto(client_socket, buffer, payload_length + 2, 0, (struct sockaddr*)&server_address, serverlen);
    if (bytestx < 0) {
        perror("ERROR: sendto");
        return EXIT_FAILURE;
    }

    bzero(buffer, BUFSIZE);

    /* Receive and print the response */
    bytesrx = recvfrom(client_socket, buffer, BUFSIZE, 0, (struct sockaddr*)&server_address, &serverlen);
    if (bytesrx < 0) {
        perror("ERROR: recvfrom");
        return EXIT_FAILURE;
    }

    char output_op_code = buffer[0];
    char output_status_code = buffer[1];
    char output_payload_length = buffer[2];
    char output_final[BUFSIZE];

    for (int i = 0; i < output_payload_length; i++) {
        output_final[i] = buffer[i + 3];
    }

    if (output_status_code == 0)
        cout << "OK:" << output_final << endl;
    else if (output_status_code == 1)
        cout << "ERR:" << output_final << endl;

    return 0;
}


/**
 * @brief Main function
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return Exit code
 */
int main(int argc, char** argv) {
    args_t args = parse_args(argc, argv);

    // Check if host is correct
    if (args.host.empty()) {
        cerr << "Error: Host is not specified." << endl;
        return 1;
    }

    // Check if port is in range
    if (!(stoi(args.port) > 1024 && stoi(args.port) < 65535)) {
        cerr << "Error: Port must be in range 1024-65535." << endl;
        return 1;
    }

    // Select mode based on the parsed arguments
    // Handle error if mode is not 'tcp' or 'udp'
    if (args.mode == "tcp")
        tcp_client(args.host, args.port);
    else if (args.mode == "udp")
        udp_client(args.host, args.port);
    else {
        cerr << "Error: Unknown mode '" << args.mode << "', must be either 'udp' or 'tcp'." << endl;
        return 1;
    }
}