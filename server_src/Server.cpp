#include "Server.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <cstring>
#include <iostream>

int MAX_INVALID_MESSAGES = 5;

// File descriptor sets for managing active and ready sockets.
fd_set Server::active_sockets, Server::ready_sockets;
struct sockaddr_in Server::peer_address, Server::client_address, Server::server_address;

// Constructor initializes the server with given IP, port, and max games allowed.
Server::Server(const std::string &ip, int port, int max_games)
    : server_ip(ip), server_port(port), max_allowed_games(max_games), server_socket_fd(-1), client_socket_fd(-1) {
    Logger::log(__FILENAME__, __FUNCTION__, "Server initialized: IP=" + ip + ", Port=" + std::to_string(port) + ", Max Games=" + std::to_string(max_games));
}

// Sets up the server, including socket creation, binding, and listening.
int Server::initialize() {
    Logger::log(__FILENAME__, __FUNCTION__, "Setting up server: IP=" + server_ip + ", Port=" + std::to_string(server_port) + ", Max Games=" + std::to_string(max_allowed_games));

    // Create a socket for the server.
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        Logger::log(__FILENAME__, __FUNCTION__, "Error: Unable to create socket");
        return -1;
    }

    // Set socket options to allow address reuse.
    int reuse_option = 1;
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_option, sizeof(reuse_option)) < 0) {
        Logger::log(__FILENAME__, __FUNCTION__, "Error: Failed to set socket options");
    }

    // Configure the server address structure.
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = (server_ip == "localhost") ? inet_addr("127.0.0.1") : (server_ip == "INADDR_ANY" ? INADDR_ANY : inet_addr(server_ip.c_str()));

    if (server_address.sin_addr.s_addr == INADDR_NONE) {
        Logger::log(__FILENAME__, __FUNCTION__, "Error: Invalid IP address");
        return -1;
    }

    // Bind the socket to the specified IP and port.
    if (bind(server_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        Logger::log(__FILENAME__, __FUNCTION__, "Error: Failed to bind socket");
        return -1;
    }

    // Start listening for incoming connections.
    if (listen(server_socket_fd, max_allowed_games) < 0) {
        Logger::log(__FILENAME__, __FUNCTION__, "Error: Listening failed");
        return -1;
    }

    // Configure the GameAdmin with the maximum number of games.
    GameAdmin::configure_max_games(max_allowed_games);
    Logger::log(__FILENAME__, __FUNCTION__, "Server is ready to accept connections");
    return 0;
}

// Waits for incoming client connections and processes their requests.
void Server::waitForConnections() {
    Logger::log(__FILENAME__, __FUNCTION__, "Waiting for incoming connections");

    FD_ZERO(&active_sockets);
    FD_SET(server_socket_fd, &active_sockets);

    while (true) {
        // Prepare the ready sockets set for select.
        ready_sockets = active_sockets;
        select(FD_SETSIZE, &ready_sockets, nullptr, nullptr, nullptr);

        // Iterate through all possible file descriptors to handle events.
        for (int fd = 0; fd < FD_SETSIZE; ++fd) {
            if (FD_ISSET(fd, &ready_sockets)) {
                if (fd == server_socket_fd) {
                    // Accept a new client connection.
                    acceptClientConnection();
                } else {
                    // Process an existing client request.
                    processClientRequest(fd);
                }
            }
        }
    }
}

// Accepts a new client connection and adds it to the active sockets set.
void Server::acceptClientConnection() {
    socklen_t client_len = sizeof(client_address);
    client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_address, &client_len);

    if (client_socket_fd >= 0) {
        FD_SET(client_socket_fd, &active_sockets);
        char *client_ip = inet_ntoa(client_address.sin_addr);
        GameAdmin::add_new_unregistered_player(client_ip, client_socket_fd);
        Logger::log(__FILENAME__, __FUNCTION__, "New client connected: IP=" + std::string(client_ip));
    } else {
        Logger::log(__FILENAME__, __FUNCTION__, "Error: Unable to accept connection");
    }
}

// Handles client requests based on their file descriptor.
void Server::processClientRequest(int client_fd) {
    int bytes_ready;
    ioctl(client_fd, FIONREAD, &bytes_ready);

    if (bytes_ready > 0) {
        // Manage incoming data from the client.
        manageIncomingData(client_fd);
    } else if (bytes_ready == 0) {
        // Terminate the connection if the client closed the socket.
        terminate_client_connection(client_fd);
    } else {
        // Close the connection if an error occurred.
        closeConnection(client_fd);
    }
}

// Processes incoming data from a client and handles invalid messages.
void Server::manageIncomingData(int client_fd) {
    char buffer[1024] = {0};
    recv(client_fd, buffer, sizeof(buffer), 0);
    std::string message(buffer);

    Player *player = GameAdmin::find_registered_player_by_socket(client_fd);
    if (!player) {
        player = GameAdmin::find_unregistered_player_by_socket(client_fd);
    }

    Responder::process_input(player, message);

    // Terminate the connection if the player exceeds the maximum invalid message count.
    if (player->get_invalid_msg_count() >= MAX_INVALID_MESSAGES) {
        terminate_client_connection(client_fd);
    }
}

// Terminates the connection for a given client.
void Server::terminate_client_connection(int client_fd) {
    Player *player = GameAdmin::find_registered_player_by_socket(client_fd);
    if (!player) {
        player = GameAdmin::find_unregistered_player_by_socket(client_fd);
    }

    if (player) {
        Logger::log(__FILENAME__, __FUNCTION__, 
                    "Terminating connection for player: " + player->get_name() +
                    ", Socket: " + std::to_string(client_fd) +
                    ", State: " + player->get_state() +
                    ", Active: " + std::to_string(player->is_active) +
                    ", Ping: " + std::to_string(player->ping));
    } else {
        Logger::log(__FILENAME__, __FUNCTION__, 
                    "No player found for Socket: " + std::to_string(client_fd));
    }

    if (player) {
        player->set_socket(-1);
        player->ping = false;
        GameAdmin::handle_player_disconnect(player->get_socket());

        // Log all registered players for debugging purposes.
        Logger::log(__FILENAME__, __FUNCTION__, "Logging registered players after disconnect:");
        for (const auto& [socket, registered_player] : GameAdmin::logged_players) {
            Logger::log(__FILENAME__, __FUNCTION__, 
                        "Player: " + registered_player->get_name() +
                        ", Socket: " + std::to_string(registered_player->get_socket()) +
                        ", Connection: " + std::to_string(registered_player->get_connection_status()) +
                        ", Active: " + std::to_string(registered_player->is_active));
        }

        // Log unregistered players.
        Logger::log(__FILENAME__, __FUNCTION__, "Logging unregistered players:");
        for (const auto& [socket, unregistered_player] : GameAdmin::unlogged_players) {
            Logger::log(__FILENAME__, __FUNCTION__, 
                        "Unregistered Player: Socket: " + std::to_string(socket));
        }
    }

    closeConnection(client_fd);
    if (player && player->get_state().compare("NEW") == 0) {
        GameAdmin::unlogged_players.erase(client_fd);
    }
}

// Closes the connection for a specific client file descriptor.
void Server::closeConnection(int client_fd) {
    Logger::log(__FILENAME__, __FUNCTION__, "Closing client connection: FD=" + std::to_string(client_fd));
    close(client_fd);
    FD_CLR(client_fd, &active_sockets);
}
