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

/**
 * @brief Struct for parsed arguments
 *
 */
struct args_t
{
    std::string host;   // IPv4 address of the server
    std::string port;   // server port
    std::string mode;   // tcp or udp
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

int main(int argc, char **argv)
{
    args_t args = parse_args(argc, argv);

    std::cout << args.host << std::endl;
    std::cout << args.port << std::endl;
    std::cout << args.mode << std::endl;

    return 0;
}