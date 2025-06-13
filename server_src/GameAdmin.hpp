

#ifndef GameAdmin_hpp
#define GameAdmin_hpp

#include <stdio.h>
#include <map>
#include <stack>
#include <thread>
#include <unistd.h>

#include <algorithm>
#include <iostream>


#include "Player.hpp"
#include "Game.hpp"
#include "Responder.hpp"
#include "Server.hpp"
#include "Logger.hpp"

using namespace std;

class GameAdmin
{
    public:
        static Player* find_registered_player_by_socket(int socket_id);
        static Player* find_unregistered_player_by_socket(int socket_id);
        static Player* find_registered_player_by_name(const std::string& player_name);
    
        static void add_new_unregistered_player(const char *ip_address, int socket_id);
        static void initiate_game_search(Player* player);
        static void resolve_player_turn(Player* player, int row, int column);
    
        
        static void authenticate_and_register_player(int client_socket, const std::string& player_name);

        static Game* get_active_game(int game_id);
        static void request_rematch(Player* player);
        static void terminate_game(Player* player);
        static void handle_player_disconnect(int socket_id);
        static void restore_player_connection(Player* player, int new_socket);
        static void display_active_games();
    
        static void resolve_player_login(int client_socket, const std::string& name);
    
        static void player_ping(Player* pl);
        
        static void remove_player(Player* player);
        static void force_game_exit(Player* player);
    
        static int MAX_GAMES;
        static void configure_max_games(int max_games);
    
        static void remove_player_from_queue(Player* player, int total, int current);
        static void notify_opponent(Player* player, const std::string& message);
    
        static void monitor_player_ping(Player* player);
        static std::map<string, Player*> logged_players;
        static std::map<int, Player*> unlogged_players;
        
    private:
    
    
        static std::map<int, Game*> active_games;
        static stack<Player*> players_queue;
    
        static void initialize_game(Player* player_one, Player* player_two);
        static Player* search_for_opponent();
    
        static int game_id_counter;
        static void resolve_result(int client_socket, const std::string& name);
};



#endif /* GameAdmin_hpp */
