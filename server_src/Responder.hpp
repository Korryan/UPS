

#ifndef Responder_hpp
#define Responder_hpp

#include "Player.hpp"
#include "Game.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include "Logger.hpp"
#include "GameAdmin.hpp"

class Responder
{
public:
    static void deliver_message_to_client(Player* player, const std::string& message);
    static void confirm_player_move(Player* player, int row, int column);
    static void notify_opponent_move(Player* player, int row, int column);
    static void update_player_state(Player* player, const std::string& state_message);
    static void send_game_result(Player* player, const std::string& result_message);
    static void send_full_game_to_player(Player *player, Game *game);
    static void send_to_socket(int socket_id, const std::string &message);
    
    static void update_player_status(Player* player, const std::string& status_message);
    static void ping_player(Player* player);
    static void process_message(Player* player, const std::string& message);
    static void process_input(Player* player, const std::string& message);
    static std::vector<std::string> tokenize(const std::string& input, const std::string& delimiter);
};


#endif /* Responder_hpp */
