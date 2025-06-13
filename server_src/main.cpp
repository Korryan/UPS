#include <iostream>
#include "Server.hpp"
#include "Logger.hpp"

void tutorial();

int main(int argc, const char *argv[]) {
    // Log the initialization of the server.
    Logger::log(__FILENAME__, __FUNCTION__, "Initializing server...");

    if (argc == 4) {
        // Parse and validate command-line arguments.
        const std::string ip_address = argv[1];
        int port = 0;
        int max_games = 0;

        try {
            port = std::stoi(argv[2]);
            // Validate port range.
            if (port < 0 || port > 65535) {
                Logger::log(__FILENAME__, __FUNCTION__, "Error: Port must be in range <0; 65535>");
                tutorial();
                return EXIT_FAILURE;
            }

            max_games = std::stoi(argv[3]);
            // Ensure max games is a positive number.
            if (max_games <= 0) {
                Logger::log(__FILENAME__, __FUNCTION__, "Error: Max games must be greater than 0");
                tutorial();
                return EXIT_FAILURE;
            }
        } catch (const std::exception &e) {
            // Handle invalid argument errors.
            Logger::log(__FILENAME__, __FUNCTION__, "Error: Invalid argument(s) provided");
            tutorial();
            return EXIT_FAILURE;
        }

        // Initialize and run the server.
        Server server(ip_address, port, max_games);
        if (server.initialize() == 0) {
            server.waitForConnections();
        } else {
            Logger::log(__FILENAME__, __FUNCTION__, "Error: Failed to set up the server");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    } else {
        // Display usage instructions if arguments are invalid.
        tutorial();
        return EXIT_FAILURE;
    }
}

// Display usage instructions for the server program.
void tutorial() {
    std::cout << "Usage: ./server <IP_ADDR> <PORT> <MAX_GAMES>\n" << std::endl;
    std::cout << "  IP_ADDR    - The IP address of the server\n";
    std::cout << "  PORT       - The port number to bind the server\n";
    std::cout << "  MAX_GAMES  - The maximum number of concurrent games\n" << std::endl;
}
