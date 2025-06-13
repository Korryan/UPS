#include "Player.hpp"

// Constructor for Player initializes all member variables and logs the creation of a new player.
Player::Player(const std::string &ip, int socket)
    : ip_address(ip), socket(socket), game_id(0), connection_status(0), player_score(0), game_marker(0),
      invalid_msg_count(0), is_active(true), rematch_requested(false), thread_running(false), player_name("Unknown"),
      state("NEW") {
    // Log the creation of the player with IP address and socket ID.
    Logger::log(__FILENAME__, __FUNCTION__, "Player created: IP=" + ip + ", Socket=" + std::to_string(socket));
}

// Destructor for Player logs the deletion of the player instance.
Player::~Player() {
    // Log the deletion of the player using their socket ID.
    Logger::log(__FILENAME__, __FUNCTION__, "Player deleted: Socket=" + std::to_string(socket));
}

// Sets the player's name and logs the name change.
void Player::set_name(const std::string &new_name) {
    // Log the player's name change from the current name to the new name.
    Logger::log(__FILENAME__, __FUNCTION__, "Player name changed from " + player_name + " to " + new_name);
    player_name = new_name; // Update the player's name.
}

// Resets the player's game-related statistics and logs the reset action.
void Player::reset_game_stats() {
    // Reset the player's game ID to 0.
    set_game_id(0);
    // Reset the player's score to 0.
    set_score(0);
    // Reset the player's game marker to 0.
    set_game_marker(0);
    // Log the reset of game stats for the player.
    Logger::log(__FILENAME__, __FUNCTION__, "Game stats reset for player: " + get_name());
}
