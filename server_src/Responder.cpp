
#include "Responder.hpp"

int MAX_MESSAGE_LENGTH = 30;

// Sends a formatted message to the client associated with the given player.
void Responder::deliver_message_to_client(Player* player, const std::string& message) {
    // Set the outgoing message for the player.
    player->set_message_out(message);

    // Log the message delivery attempt.
    Logger::log(__FILENAME__, __FUNCTION__, "Delivering message: " + message + " to player: " + player->get_name() + ", Socket: " + std::to_string(player->get_socket()));

    // Append a newline to the message and send it to the player's socket.
    std::string formatted_message = message + "\n";
    send(player->get_socket(), formatted_message.data(), formatted_message.length(), 0);
}

// Sends confirmation to the player about their move.
void Responder::confirm_player_move(Player* player, int row, int column) {
    // Log the move confirmation.
    Logger::log(__FILENAME__, __FUNCTION__, "Acknowledging move for player: " + player->get_name());

    // Format the move confirmation message.
    std::string move_message = "YOUR_TURN;" + std::to_string(row) + ";" + std::to_string(column) + ";";

    // Deliver the move confirmation message to the player.
    deliver_message_to_client(player, move_message);
}

// Notifies the player about the opponent's move.
void Responder::notify_opponent_move(Player* player, int row, int column) {
    // Log the notification of the opponent's move.
    Logger::log(__FILENAME__, __FUNCTION__, "Notifying player: " + player->get_name() + " of opponent's move.");

    // Format the opponent move notification message.
    std::string opponent_move_message = "OPPONENT_TURN;" + std::to_string(row) + ";" + std::to_string(column) + ";";

    // Deliver the opponent move notification message to the player.
    deliver_message_to_client(player, opponent_move_message);
}

// Updates the player's status with a given message.
void Responder::update_player_status(Player* player, const std::string& status_message) {
    // Log the status update.
    Logger::log(__FILENAME__, __FUNCTION__, "Updating status for player: " + player->get_name());

    // Format the status update message.
    std::string status_update = "STATUS;" + status_message + ";";

    // Deliver the status update message to the player.
    deliver_message_to_client(player, status_update);
}

// Updates the player's state with a given message.
void Responder::update_player_state(Player* player, const std::string& state_message) {
    // Log the state update.
    Logger::log(__FILENAME__, __FUNCTION__, "Updating state for player: " + player->get_name());

    // Format the state update message.
    std::string state_update = state_message + ";";

    // Deliver the state update message to the player.
    deliver_message_to_client(player, state_update);
}

// Sends the result of the game to the player.
void Responder::send_game_result(Player* player, const std::string& result_message) {
    // Log the game result delivery.
    Logger::log(__FILENAME__, __FUNCTION__, "Sending game result to player: " + player->get_name());

    // Deliver the game result message to the player.
    deliver_message_to_client(player, result_message + ";");
}

// Sends a message directly to a specific socket ID.
void Responder::send_to_socket(int socket_id, const std::string& message) {

    // Append a delimiter to the message and send it to the socket.
    std::string formatted_message = message + ";\n";
    send(socket_id, formatted_message.data(), formatted_message.length(), 0);


    // Log the attempt to send a message to the given socket ID.
    Logger::log(__FILENAME__, __FUNCTION__, "Sending message: " + formatted_message + " to socket ID: " + std::to_string(socket_id));
}

// Sends the full game state to the given player for reconnection purposes.
void Responder::send_full_game_to_player(Player* player, Game* game) {
    // Log the attempt to send the game state to the player.
    Logger::log(__FILENAME__, __FUNCTION__, "Sending full game state to player: " + player->get_name());

    // Retrieve the opponent's name and prepare the base of the game state message.
    Player* opponent = game->get_opponent(player);
    std::string game_state = "RECONNECT;" + opponent->get_name() + ";";

    // Append the game board state to the message.
    for (int i = 0; i < game->BOARD_SIZE; ++i) {
        for (int j = 0; j < game->BOARD_SIZE; ++j) {
            game_state += std::to_string(game->get_board_value(i, j));
            if (j + 1 < game->BOARD_SIZE) {
                game_state += ",";
            }
        }
        if (i + 1 < game->BOARD_SIZE) {
            game_state += ",";
        }
    }

    // Append the player's game marker to the message.
    game_state += ";" + std::to_string(player->get_game_marker()) + ";";

    // Log the final formatted game state for debugging purposes.
    Logger::log(__FILENAME__, __FUNCTION__, "Full game state: " + game_state);

    // Deliver the game state to the player.
    deliver_message_to_client(player, game_state);
}

// Sends a ping message to the player to check Connector.
void Responder::ping_player(Player* player) {
    // Log the ping attempt.
    Logger::log(__FILENAME__, __FUNCTION__, "Pinging player: " + player->get_name());

    // Send a PING message to the player.
    deliver_message_to_client(player, "PING;");
}

// Splits a string into tokens based on a specified delimiter.
std::vector<std::string> Responder::tokenize(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end;

    // Find and extract substrings separated by the delimiter.
    while ((end = input.find(delimiter, start)) != std::string::npos) {
        if (end > start) {
            tokens.push_back(input.substr(start, end - start));
        }
        start = end + delimiter.length();
    }
    // Add the remaining part of the string (if any).
    if (start < input.length()) {
        tokens.push_back(input.substr(start));
    }

    return tokens;
}

// Processes a message received from a player and performs appropriate actions.
void Responder::process_message(Player* player, const std::string& message) {
    // Store the received message in the player's input message buffer.
    player->set_message_in(message);

    // Tokenize the message into parts using ';' as a delimiter.
    std::vector<std::string> message_parts = tokenize(message, ";");

    if (message_parts.empty()) {
        // Handle invalid messages by incrementing the invalid message count.
        player->add_invalid_msg_count();
        Logger::log(__FILENAME__, __FUNCTION__, "Invalid message from player: " + player->get_name() + " (Socket: " + std::to_string(player->get_socket()) + "). Count: " + std::to_string(player->get_invalid_msg_count()));
        return;
    }


    const std::string& message_type = message_parts[0];
    // Kontrola na binární znaky
    bool contains_binary = false;
    for (char c : message_type) {
        if (c < 32 || c > 126) { // Znaky mimo rozsah viditelných ASCII znaků
            contains_binary = true;
            break;
        }
    }

    if (contains_binary) {
        Logger::log(__FILENAME__, __FUNCTION__, "Error: Message contains binary or invalid characters from player: " + player->get_name());
        return; // Nebo jiný způsob ukončení zpracování zprávy
    }

    // Extract the message type (first part of the message).
    Logger::log(__FILENAME__, __FUNCTION__, "Processing message: " + message_type + " from player: " + player->get_name());

    // Perform actions based on the message type.
    if (message_type == "NAME") {
        player->set_invalid_msg_count(0);
        if (message_parts.size() > 1 && !message_parts[1].empty()) {
            GameAdmin::resolve_player_login(player->get_socket(), message_parts[1]);
        }
    } else if (message_type == "WAITING_FOR_GAME") {
        player->set_invalid_msg_count(0);
        if (player->get_state() == "LOBBY") {
            GameAdmin::initiate_game_search(player);
        } else {
            Logger::log(__FILENAME__, __FUNCTION__, "Invalid operation: Player " + player->get_name() + " is not in LOBBY state.");
        }
    } else if (message_type == "TURN") {
        player->set_invalid_msg_count(0);
        if (player->get_state() == "IN_GAME") {
            try {
                int row = std::stoi(message_parts[1]);
                int column = std::stoi(message_parts[2]);
                GameAdmin::resolve_player_turn(player, row, column);
            } catch (const std::exception& e) {
                Logger::log(__FILENAME__, __FUNCTION__, "Invalid turn data from player: " + player->get_name() + ". Error: " + e.what());
            }
        } else {
            Logger::log(__FILENAME__, __FUNCTION__, "Invalid operation: Player " + player->get_name() + " is not in IN_GAME state.");
        }
    } else if (message_type == "REMATCH") {
        player->set_invalid_msg_count(0);
        if (player->get_state() == "RESULT") {
            GameAdmin::request_rematch(player);
        } else {
            Logger::log(__FILENAME__, __FUNCTION__, "Invalid operation: Player " + player->get_name() + " is not in RESULT state.");
        }
    } else if (message_type == "GAME_OVER") {
        player->set_invalid_msg_count(0);
        if (player->get_state() == "RESULT") {
            GameAdmin::terminate_game(player);
        } else {
            Logger::log(__FILENAME__, __FUNCTION__, "Invalid operation: Player " + player->get_name() + " is not in RESULT state.");
        }
    } else if (message_type == "EXIT") {
        player->set_invalid_msg_count(0);
        if (player->get_state() == "LOBBY" || player->get_state() == "WAITING") {
            GameAdmin::remove_player(player);
        } else {
            Logger::log(__FILENAME__, __FUNCTION__, "Invalid operation: Player " + player->get_name() + " cannot exit from state " + player->get_state());
        }
    } else if (message_type == "ACK") {
        player->ping = true;
    } else {
        player->add_invalid_msg_count();
        Logger::log(__FILENAME__, __FUNCTION__, "Unknown message type received from player: " + player->get_name() + ". Type: " + message_type);
    }
}

// Processes raw input from a player by splitting it into individual messages.
void Responder::process_input(Player* player, const std::string& message) {
    // Split the raw input using '|' as a delimiter.
    std::vector<std::string> message_parts = tokenize(message, "|");
    std::vector<std::string> processed_parts;

    // Process each unique message part.
    for (const auto& part : message_parts) {
        if (std::find(processed_parts.begin(), processed_parts.end(), part) == processed_parts.end()) {
            processed_parts.push_back(part);

            // Ensure the message length is within acceptable limits.
            if (message.length() < MAX_MESSAGE_LENGTH) {
                process_message(player, part);
            } else {
                player->add_invalid_msg_count();
                Logger::log(__FILENAME__, __FUNCTION__, "Message too long from player: " + player->get_name() + " (Socket: " + std::to_string(player->get_socket()) + "). Count: " + std::to_string(player->get_invalid_msg_count()));
            }
        }
    }
}
