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

// Unix-specific includes, will work only on Linux, macOS, etc.
// Would need to be changed for Windows (Along with the rest of the code)
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define UDP_BUFSIZE 255
#define TCP_BUFSIZE 1024

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

// Global variables
args_t args;
bool continue_loop = true;
int client_socket;

/**
 * @brief Close connection and exit
 *
 * If the mode is TCP, send 'BYE' to the server and receive the response
 */
void close_connection() {
    if (args.mode == "tcp") {
        // Send 'BYE' to the server
        send(client_socket, "BYE\n", 4, 0);

        // Receive the response
        char buffer[TCP_BUFSIZE];
        recv(client_socket, buffer, TCP_BUFSIZE, 0);
        cout << buffer;
    }

    close(client_socket);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Signal handler
 *
 * @cite https://stackoverflow.com/a/1641223
 *
 * @param sig Signal number
 */
void sighandler(int sig) {
    continue_loop = false;
    close_connection();
    return;
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

/**
 * @brief TCP client
 *
 * @cite https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoTcp
 * @cite https://linux.die.net/man/3/gethostbyname2
 *
 * @param host Hostname or address of the server
 * @param port Port number of the server
 * @return Return code
 */
int tcp_client(string host, string port) {
    const char* server_hostname = host.c_str(); // Hostname of the server
    int port_number = atoi(port.c_str());       // Port number of the server
    int bytestx;                                // Bytes sent
    int bytesrx;                                // Bytes received
    struct hostent* server;                     // Server structure
    struct sockaddr_in server_address;          // Server address

    // Get the server IPv4 address
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
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket <= 0) {
        perror("Error: Socket");
        return EXIT_FAILURE;
    }

    // Connect to the server
    int connect_result = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (connect_result < 0) {
        perror("Error: Connect");
        return EXIT_FAILURE;
    }

    char buffer[TCP_BUFSIZE];       // Buffer for the calculation data

    while (continue_loop) {
        // Clear the buffer
        bzero(buffer, TCP_BUFSIZE);

        // Get the data from the user
        fgets(buffer, TCP_BUFSIZE - 1, stdin);

        // Send the data to the server
        bytestx = send(client_socket, buffer, strlen(buffer), 0);
        if (bytestx < 0) {
            perror("Error: Send");
            close(client_socket);
            return EXIT_FAILURE;
        }
        // Clear the buffer
        bzero(buffer, TCP_BUFSIZE);

        // Receive the data from the server
        bytesrx = recv(client_socket, buffer, TCP_BUFSIZE, 0);
        if (bytesrx < 0) {
            perror("Error: Receive");
            close(client_socket);
            return EXIT_FAILURE;
        }

        // Print the result
        cout << buffer;

        // Check for 'BYE' message
        if (buffer[0] == 'B' && buffer[1] == 'Y' && buffer[2] == 'E') {
            continue_loop = false;
        }
    }

    close(client_socket);

    return 0;
}

/**
 * @brief UDP client
 *
 * @cite https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp
 * @cite https://linux.die.net/man/3/gethostbyname2
 *
 * @param host Hostname or address of the server
 * @param port Port number of the server
 * @return Return code
 */
int udp_client(string host, string port) {
    const char* server_hostname = host.c_str(); // Hostname of the server
    int port_number = atoi(port.c_str());       // Port number of the server
    int bytestx;                                // Bytes sent
    int bytesrx;                                // Bytes received
    socklen_t server_len;                       // Length of the server address
    struct hostent* server;                     // Server structure
    struct sockaddr_in server_address;          // Server address

    // Get the server IPv4 address
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

    char buffer[UDP_BUFSIZE];       // Buffer for the calculation data
    char payload[UDP_BUFSIZE + 2];  // Buffer for the payload (opcode + data length + calculation data)


    while (continue_loop) {
        // Clear the buffers
        bzero(buffer, UDP_BUFSIZE);
        bzero(payload, UDP_BUFSIZE + 2);

        // Get the payload data from the user
        cin.getline(buffer, UDP_BUFSIZE);
        // If got EOF, exit
        if (cin.eof()) {
            close_connection();
        }

        // Set the opcode to 0 for request
        payload[0] = 0;

        // Get the length of the payload data
        // Set the payload length in the buffer
        int buffer_length = strlen(buffer);
        payload[1] = (char)buffer_length;

        // Copy the buffer data to the payload
        strcpy(&payload[2], buffer);

        // Send the data to the server
        server_len = sizeof(server_address);
        bytestx = sendto(client_socket, payload, buffer_length + 2, 0, (struct sockaddr*)&server_address, server_len);
        if (bytestx < 0) {
            perror("ERROR: Sendto");
            close(client_socket);
            return EXIT_FAILURE;
        }

        // Clear the buffers
        bzero(buffer, UDP_BUFSIZE);
        bzero(payload, UDP_BUFSIZE + 2);

        // Receive the data from the server
        bytesrx = recvfrom(client_socket, buffer, UDP_BUFSIZE, 0, (struct sockaddr*)&server_address, &server_len);
        if (bytesrx < 0) {
            perror("ERROR: Recvfrom");
            close(client_socket);
            return EXIT_FAILURE;
        }

        // Get the status of the response
        char out_status = buffer[1];

        if (out_status == 0)
            cout << "OK:" << buffer + 3 << endl;
        else if (out_status == 1)
            cout << "ERR:" << buffer + 3 << endl;
    }

    close(client_socket);
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
    // Load arguments to global variable
    // To be able to close conenction correctly on signal
    args = parse_args(argc, argv);

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

    signal(SIGINT, &sighandler);

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