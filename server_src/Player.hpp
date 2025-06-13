#ifndef Player_hpp
#define Player_hpp

#include <iostream>
#include <thread>
#include "Logger.hpp"

class Player
{
private:
    std::string ip_address;
    int socket;
    int game_id;
    int connection_status;
    int player_score;
    int game_marker;
    int invalid_msg_count;
    std::thread ping_handler;
    std::string player_name;
    std::string state;
    std::string message_in;
    std::string message_out;

public:
    Player(const std::string &ip, int socket);
    ~Player();
    bool ping;
    bool is_active;
    bool thread_running;
    bool rematch_requested;
    void set_name(const std::string &new_name);
    const std::string &get_name() const { return player_name; };
    const std::string &get_state() const { return state; };
    void set_state(const std::string &new_state) { state = new_state; };
    int get_game_marker() const { return game_marker; };
    void set_game_marker(int marker) { game_marker = marker; };
    const std::thread &get_ping_handler() const { return ping_handler; };
    void set_ping_handler(std::thread &&handler) { ping_handler = std::move(handler); };
    int get_connection_status() const { return connection_status; };
    void set_connection_status(int status) { connection_status = status; };
    int get_game_id() const { return game_id; };
    void set_game_id(int id) { game_id = id; };
    
    void set_message_in(const std::string &msg) { message_in = msg; };
    void set_message_out(const std::string &msg) { message_out = msg; };

    int get_socket() const { return socket; };
    void set_socket(int s) { socket = s; };
    int get_score() const { return player_score; };
    void add_score() { player_score++; };
    void set_score(int s) { player_score = s; };
    void reset_invalid_count() { invalid_msg_count = 0; };
    int get_invalid_msg_count() const { return invalid_msg_count; };
    void set_invalid_msg_count(int count) { invalid_msg_count = count; };
    void add_invalid_msg_count() { invalid_msg_count++; };
    void reset_game_stats();
};

#endif /* Player_hpp */
