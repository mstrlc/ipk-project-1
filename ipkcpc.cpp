/**
 * @file ipkcpc.cpp
 *
 * @name IPK project
 * @brief Remote calculator UDP or TCP client
 *
 * @author Matyas Strelec (xstrel03)
 * @date 2023-03-12
 */

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define BUFSIZE 8092
#define UDP_BUFSIZE 256

bool forever = true;

/**
 * @brief Struct for parsed arguments
 *
 */
struct args_t
{
    const char *host; // IPv4 address of the server
    const char *port; // server port
    const char *mode; // "tcp" or "udp"
};

/**
 * @brief Parse the command-line arguments
 * ipkcpc -h <host> -p <port> -m <mode>
 *
 * @param argc number of arguments
 * @param argv array of arguments
 * @return args struct with parsed arguments
 */
args_t parse_args(int argc, char **argv)
{
    args_t args;

    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-h")
        {
            args.host = argv[i + 1];
        }
        else if (std::string(argv[i]) == "-p")
        {
            args.port = argv[i + 1];
        }
        else if (std::string(argv[i]) == "-m")
        {
            args.mode = argv[i + 1];
        }
    }

    return args;
}

/**
 * @brief TCP client
 * @cite https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoTcp
 *
 * @param args struct with parsed arguments
 */
int tcp_client(args_t args)
{
    int client_socket, port_number, bytestx, bytesrx;
    socklen_t serverlen;
    const char *server_hostname;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buf[BUFSIZE];

    server_hostname = args.host;
    port_number = atoi(args.port);

    if ((server = gethostbyname(server_hostname)) == NULL)
    {
        fprintf(stderr, "ERROR: no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }

    /* 3. nalezeni IP adresy serveru a inicializace struktury server_address */
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    /* tiskne informace o vzdalenem soketu */
    // printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    /* Vytvoreni soketu */
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }

    while (forever)
    {

        /* nacteni zpravy od uzivatele */
        bzero(buf, BUFSIZE);
        // printf("Please enter msg: ");
        fgets(buf, BUFSIZE, stdin);

        /* odeslani zpravy na server */
        bytestx = send(client_socket, buf, strlen(buf), 0);
        // if (bytestx < 0)
        //     perror("ERROR in sendto");

        // clear buffer
        bzero(buf, BUFSIZE);

        /* prijeti odpovedi a jeji vypsani */
        bytesrx = recv(client_socket, buf, BUFSIZE, 0);
        printf("%s", buf);
        // while(std::string(buf).find("\0") != std::string::npos)
        // {
        //     bytesrx = recv(client_socket, buf, BUFSIZE, 0);
        //     printf("%s", buf);
        // }
        if (buf[0] == 'B' && buf[1] == 'Y' && buf[2] == 'E')
        {
            break;
        }
    }

    close(client_socket);

    return 0;
}

/**
 * @brief UDP client
 * @cite https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp
 *
 * @param args struct with parsed arguments
 */
int udp_client(args_t args)
{
    printf("yay udp");

    int client_socket, port_number, bytestx, bytesrx;
    socklen_t serverlen;
    const char *server_hostname;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buf[UDP_BUFSIZE];

    server_hostname = args.host;
    port_number = atoi(args.port);

    /* 2. ziskani adresy serveru pomoci DNS */

    if ((server = gethostbyname(server_hostname)) == NULL)
    {
        fprintf(stderr, "ERROR: no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }

    /* 3. nalezeni IP adresy serveru a inicializace struktury server_address */
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    /* tiskne informace o vzdalenem soketu */
    printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    /* Vytvoreni soketu */
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    /* nacteni zpravy od uzivatele */
    bzero(buf, UDP_BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, UDP_BUFSIZE, stdin);

    /* odeslani zpravy na server */
    serverlen = sizeof(server_address);
    bytestx = sendto(client_socket, buf, strlen(buf), 0, (struct sockaddr *)&server_address, serverlen);
    if (bytestx < 0)
        perror("ERROR: sendto");

    /* prijeti odpovedi a jeji vypsani */
    bytesrx = recvfrom(client_socket, buf, UDP_BUFSIZE, 0, (struct sockaddr *)&server_address, &serverlen);
    if (bytesrx < 0)
        perror("ERROR: recvfrom");
    printf("Echo from server: %s", buf);
    return 0;
}

int main(int argc, char **argv)
{
    args_t args = parse_args(argc, argv);

    if (args.mode == std::string("tcp"))
    {
        tcp_client(args);
    }
    if (args.mode == std::string("udp"))
    {
        udp_client(args);
    }
}