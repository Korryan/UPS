#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netinet/in.h>
#include "Logger.hpp"
#include "GameAdmin.hpp"
#include "Responder.hpp"

class Server {
private:
    std::string server_ip;
    int server_port;
    int max_allowed_games;
    int server_socket_fd;
    int client_socket_fd;
    static fd_set active_sockets, ready_sockets;
    static struct sockaddr_in peer_address, client_address, server_address;

    void acceptClientConnection();
    void processClientRequest(int client_fd);
    void manageIncomingData(int client_fd);
    void terminate_client_connection(int client_fd);

public:
    Server(const std::string &ip, int port, int max_games);
    int initialize();
    void waitForConnections();
    static void closeConnection(int client_fd);
};

#endif // SERVER_HPP