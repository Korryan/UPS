

#include "GameAdmin.hpp"

std::map<string, Player*> GameAdmin::logged_players;
std::map<int, Player*> GameAdmin::unlogged_players;
stack<Player*> GameAdmin::players_queue;
std::map<int, Game*> GameAdmin::active_games;

int GameAdmin::game_id_counter = 1;
int GameAdmin::MAX_GAMES;

int TIMEOUT = 60;
int PING_INTERVAL = 1;

void GameAdmin::add_new_unregistered_player(const char *ip_address, int socket_id) {
    // Log the connection attempt with the provided IP address and socket ID.
    std::string formatted_ip(ip_address);
    Logger::log(__FILENAME__, __FUNCTION__, "New player connected: IP=" + formatted_ip + ", Socket=" + std::to_string(socket_id));

    // Create a new Player object for the unregistered player.
    Player* new_player = new Player(const_cast<char*>(ip_address), socket_id);

    // Attempt to add the player to the unregistered players map.
    auto insertion_result = GameAdmin::unlogged_players.emplace(socket_id, new_player);
    if (!insertion_result.second) {
        // If insertion fails (socket ID already exists), log the error and delete the created player.
        Logger::log(__FILENAME__, __FUNCTION__, "Failed to add player: Socket already exists.");
        delete new_player;
    } else {
        // Log success if the player was successfully added.
        Logger::log(__FILENAME__, __FUNCTION__, "Player successfully added to unregistered list.");
    }
}

void GameAdmin::authenticate_and_register_player(int client_socket, const std::string& player_name) {
    // Log the start of the authentication process for the player.
    Logger::log(__FILENAME__, __FUNCTION__, "Authenticating player: Name=" + player_name + ", Socket=" + std::to_string(client_socket));

    // Find the unregistered player associated with the socket ID.
    Player* unregistered_player = GameAdmin::find_unregistered_player_by_socket(client_socket);

    if (!unregistered_player) {
        // Log an error if no matching unregistered player is found.
        Logger::log(__FILENAME__, __FUNCTION__, "Error: No unregistered player found for the given socket.");
        return;
    }

    // Remove the player from the unregistered list.
    GameAdmin::unlogged_players.erase(client_socket);

    // Set the player's name and move them to the "LOBBY" state.
    unregistered_player->set_name(player_name);
    unregistered_player->set_state("LOBBY");

    // Add the player to the logged players map.
    GameAdmin::logged_players.insert(make_pair(player_name, unregistered_player));

    // Start a ping monitoring thread for the player to handle connection status.
    unregistered_player->set_ping_handler(thread(GameAdmin::monitor_player_ping, unregistered_player));
    unregistered_player->thread_running = true;

    // Notify the client about the successful connection.
    Responder::deliver_message_to_client(unregistered_player, "CONNECT");
    Logger::log(__FILENAME__, __FUNCTION__, "Player registered and moved to logged players: Name=" + player_name + ", Socket=" + std::to_string(client_socket));
}

Player* GameAdmin::find_unregistered_player_by_socket(int socket_id) {
    // Search the unregistered players map for a player with the given socket ID.
    auto it = GameAdmin::unlogged_players.find(socket_id);
    return (it != GameAdmin::unlogged_players.end()) ? it->second : nullptr;
}

Player* GameAdmin::find_registered_player_by_socket(int socket_id) {
    // Iterate through logged players to find a player with the matching socket ID.
    for (const auto& [key, player] : GameAdmin::logged_players) {
        if (player->get_socket() == socket_id) {
            return player;
        }
    }
    return nullptr; // Return null if no match is found.
}

Player* GameAdmin::find_registered_player_by_name(const std::string& player_name) {
    // Iterate through logged players to find a player with the matching name.
    map<string, Player*>::iterator it = GameAdmin::logged_players.begin();
    while (it != GameAdmin::logged_players.end())
    {
        auto *pl = it->second;
        
        if (pl->get_name().compare(player_name) == 0)
        {
            return pl;
        }
        
        it++;
    }
    
    return NULL;
}

void GameAdmin::initiate_game_search(Player* player) {
    // Log that the player has initiated a game search.
    Logger::log(__FILENAME__, __FUNCTION__, "Player searching for a game: " + player->get_name());

    // Check if there is capacity for a new game.
    if (active_games.size() < GameAdmin::MAX_GAMES) {
        // Try to find an opponent from the players queue.
        auto* opponent = search_for_opponent();

        if (!opponent) {
            // If no opponent is found, add the player to the queue and update their state.
            Logger::log(__FILENAME__, __FUNCTION__, "No opponent found. Adding player to the queue: " + player->get_name());

            players_queue.push(player);
            Responder::update_player_state(player, "WAITING");
            player->set_state("WAITING");
        } else {
            // If an opponent is found, start a new game.
            Logger::log(__FILENAME__, __FUNCTION__, "Opponent located. Opponent name: " + opponent->get_name());

            Responder::update_player_state(player, "STARTING_GAME;" + opponent->get_name());
            Responder::update_player_state(opponent, "STARTING_GAME;" + player->get_name());

            player->set_state("IN_GAME");
            opponent->set_state("IN_GAME");

            initialize_game(player, opponent);
        }
    } else {
        // If the maximum game limit is reached, inform the player.
        Logger::log(__FILENAME__, __FUNCTION__, "Maximum game limit reached. Player: " + player->get_name());
        Responder::update_player_state(player, "MAXIMUM_GAMES_REACHED");
    }
}

Player* GameAdmin::search_for_opponent() {
    // Check the queue for an available opponent.
    if (!players_queue.empty()) {
        Player* queued_player = players_queue.top();
        players_queue.pop();

        // Skip disconnected players.
        if (queued_player->get_connection_status() == -1) {
            Logger::log(__FILENAME__, __FUNCTION__, "Opponent is disconnected, skipping.");
            return nullptr;
        }

        return queued_player; // Return the available opponent.
    }

    return nullptr; // Return null if the queue is empty.
}

void GameAdmin::initialize_game(Player* player_one, Player* player_two) {
    // Log the initialization of a new game.
    Logger::log(__FILENAME__, __FUNCTION__, "Setting up new game for players: " + player_one->get_name() + " and " + player_two->get_name());

    // Create a new game instance and assign it to the active games map.
    Game* new_game = new Game(game_id_counter, player_one, player_two);
    new_game->set_previous_winner(player_one);

    active_games[game_id_counter] = new_game;
    game_id_counter++;

    // Notify players about the start of the game.
    Responder::update_player_status(player_one, "Your turn");
    Responder::update_player_status(player_two, "Opponent's turn");
}
void GameAdmin::resolve_player_turn(Player* player, int row, int column) {
    // Log the player's turn with row and column details.
    Logger::log(__FILENAME__, __FUNCTION__, "Processing turn for player: " + player->get_name() + ". Row: " + std::to_string(row) + ", Column: " + std::to_string(column));

    // Retrieve the active game associated with the player.
    Game* current_game = get_active_game(player->get_game_id());
    int action_result = current_game->execute_turn(row, column, player);

    // Handle the result of the turn execution.
    switch (action_result) {
        case 0: {
            // Successful turn.
            Logger::log(__FILENAME__, __FUNCTION__, "Turn accepted. Player: " + player->get_name());
            Player* next_player = current_game->get_opponent(player);
            current_game->active_turn = next_player->get_game_marker();

            // Update the game state and notify players.
            Responder::update_player_status(next_player, "Your turn");
            Responder::update_player_status(player, "Opponent's turn");

            Responder::confirm_player_move(player, row, column);
            Responder::notify_opponent_move(next_player, row, column);

            // Evaluate the game state for a win or draw.
            int game_status = current_game->evaluate_game_state();
            if (game_status == -1) {
                // Game ends in a tie.
                Logger::log(__FILENAME__, __FUNCTION__, "Game ended in a tie.");
                player->set_state("RESULT");
                next_player->set_state("RESULT");
                Responder::send_game_result(player, "TIE;" + std::to_string(player->get_score()) + ";" + std::to_string(next_player->get_score()));
                Responder::send_game_result(next_player, "TIE;" + std::to_string(next_player->get_score()) + ";" + std::to_string(player->get_score()));
            } else if (game_status == 1) {
                // Player wins the game.
                Logger::log(__FILENAME__, __FUNCTION__, "Player " + player->get_name() + " wins the game.");
                player->add_score();
                player->set_state("RESULT");
                next_player->set_state("RESULT");
                Responder::send_game_result(player, "WIN;" + std::to_string(player->get_score()) + ";" + std::to_string(next_player->get_score()));
                Responder::send_game_result(next_player, "LOSE;" + std::to_string(next_player->get_score()) + ";" + std::to_string(player->get_score()));
                current_game->set_previous_winner(player);
            }
            break;
        }
        case 1:
            // Cell already marked error.
            Logger::log(__FILENAME__, __FUNCTION__, "Error: Cell already marked by another player.");
            break;
        case 2:
            // Not the player's turn error.
            Logger::log(__FILENAME__, __FUNCTION__, "Error: Not player's turn.");
            break;
        case -1:
            // Invalid cell coordinates error.
            Logger::log(__FILENAME__, __FUNCTION__, "Error: Invalid cell coordinates.");
            break;
    }
}

Game* GameAdmin::get_active_game(int game_id) {
    // Retrieve the game instance from the active games map.
    auto it = active_games.find(game_id);
    return (it != active_games.end()) ? it->second : nullptr;
}

void GameAdmin::request_rematch(Player* player) {
    // Log the rematch request from the player.
    player->rematch_requested = true;
    Game* current_game = get_active_game(player->get_game_id());

    Logger::log(__FILENAME__, __FUNCTION__, "Player " + player->get_name() + " requested a rematch.");

    // Retrieve the opponent of the player in the current game.
    Player* opponent = current_game->get_opponent(player);

    if (player->rematch_requested && opponent->rematch_requested) {
        // Both players agreed to a rematch.
        Logger::log(__FILENAME__, __FUNCTION__, "Both players agreed to a rematch.");

        current_game->reset_game_board();
        current_game->active_turn = opponent->get_game_marker();

        // Notify players about the rematch.
        Responder::update_player_state(player, "STARTING_GAME;" + opponent->get_name());
        Responder::update_player_state(opponent, "STARTING_GAME;" + player->get_name());

        Responder::update_player_status(opponent, "Your turn");
        Responder::update_player_status(player, "Opponent's turn");

        // Reset rematch flags and set player states to "IN_GAME".
        player->rematch_requested = false;
        opponent->rematch_requested = false;

        player->set_state("IN_GAME");
        opponent->set_state("IN_GAME");
    } else {
        // Wait for the opponent's rematch confirmation.
        Logger::log(__FILENAME__, __FUNCTION__, "Waiting for opponent to confirm rematch");
        Responder::update_player_state(player, "WAITING");
        Responder::update_player_status(opponent, "Opponent requested a rematch");
    }
}

void GameAdmin::terminate_game(Player* player) {
    // Check if the player is associated with an active game.
    if (player->get_game_id() > 0) {
        Game* game_instance = get_active_game(player->get_game_id());

        // Log the game termination details.
        Logger::log(__FILENAME__, __FUNCTION__, "Closing game: " + std::to_string(game_instance->get_game_id()) + ", Player exiting: " + player->get_name());

        // Remove the game from the active games map.
        active_games.erase(game_instance->get_game_id());

        // Reset game stats for both players and notify them.
        player->reset_game_stats();
        Player* opponent = game_instance->get_opponent(player);
        opponent->reset_game_stats();

        Responder::send_game_result(player, "GAME_OVER;");
        Responder::send_game_result(opponent, "GAME_OVER;");
        Responder::update_player_status(opponent, "Opponent left the game.");

        // Set players' states to "LOBBY" and delete the game instance.
        player->set_state("LOBBY");
        opponent->set_state("LOBBY");

        delete game_instance;
    } else {
        // Log a message if the player is not in a game.
        Logger::log(__FILENAME__, __FUNCTION__, "Player " + player->get_name() + " is not in a game.");
    }
}

void GameAdmin::handle_player_disconnect(int socket_id) {
    // Locate the player associated with the given socket ID.
    Player* player = find_registered_player_by_socket(socket_id);

    if (player != NULL) {
        // Mark the player as disconnected and log the event.
        Logger::log(__FILENAME__, __FUNCTION__, 
            "Disconnecting player. Name=" + player->get_name() + 
            ", Game ID=" + std::to_string(player->get_game_id()));

        player->set_connection_status(-1);
        Logger::log(__FILENAME__, __FUNCTION__, "Player disconnected: " + player->get_name());

        // Handle disconnection based on the player's state.
        if (player->get_state() == "IN_GAME") {
            notify_opponent(player, "Opponent is disconnected");
        } else if (player->get_state() == "RESULT") {
            terminate_game(player);
        }
    }
}

void GameAdmin::notify_opponent(Player* player, const std::string& message) {
    // Notify the opponent in the player's game with a custom message.
    if (player->get_game_id() > 0) {
        auto game = get_active_game(player->get_game_id());
        auto opponent = game->get_opponent(player);

        // Log the notification details.
        Logger::log(__FILENAME__, __FUNCTION__, "Game ID: " + to_string(game->get_game_id()) + " notify " + player->get_name() + "'s opponent " + opponent->get_name() + " with message: " + message);

        Responder::update_player_status(opponent, message);
    }
}

void GameAdmin::restore_player_connection(Player* player, int new_socket) {
    // Restore the player's connection and update their socket.
    player->set_connection_status(0);
    player->set_socket(new_socket);
    player->ping = true;

    // Retrieve the game associated with the player.
    Game* associated_game = get_active_game(player->get_game_id());

    if (associated_game) {
        // Log the restoration and update the player's state.
        Logger::log(__FILENAME__, __FUNCTION__, "Restoring connection for player: " + player->get_name() + " to game ID: " + std::to_string(player->get_game_id()));

        player->set_state("IN_GAME");

        // Send the full game state to the player.
        Responder::send_full_game_to_player(player, associated_game);

        // Notify the player and opponent of the current turn.
        if (associated_game->active_turn == player->get_game_marker()) {
            Responder::update_player_status(player, "Your turn");
            notify_opponent(player, "Opponent's turn");
        } else {
            Responder::update_player_status(player, "Opponent's turn");
            notify_opponent(player, "Your turn");
        }
    } else {
        // If no game is associated, move the player to the lobby.
        Logger::log(__FILENAME__, __FUNCTION__, "No active game found for player: " + player->get_name() + ". Moving to lobby.");

        player->set_state("LOBBY");
        Responder::deliver_message_to_client(player, "CONNECT");
    }
}

void GameAdmin::resolve_player_login(int client_socket, const std::string& name) {
    // Log the player's login attempt with their name and socket ID.
    Logger::log(__FILENAME__, __FUNCTION__, "Processing login for socket: " + std::to_string(client_socket) + ", Player name: " + name);

    if (!name.empty() && name.length() < 14) {
        // Check if the player name already exists among registered players.
        Player* existing_player = find_registered_player_by_name(name);

        if (existing_player) {
            if (existing_player->get_connection_status() == 0) {
                // Log the case where the name is already in use.
                Logger::log(__FILENAME__, __FUNCTION__, "Name already in use: " + name + ", Connection status: " + std::to_string(existing_player->get_connection_status()));
                Responder::send_to_socket(client_socket, "NAME_TAKEN");
            } else {
                // Restore the connection for the existing player.
                GameAdmin::restore_player_connection(existing_player, client_socket);
                Logger::log(__FILENAME__, __FUNCTION__, "Reconnection successful for player: " + existing_player->get_name());
            }
        } else {
            // Authenticate and register a new player if the name is not in use.
            GameAdmin::authenticate_and_register_player(client_socket, name);
        }
    } else {
        // Log an error for invalid player names.
        Logger::log(__FILENAME__, __FUNCTION__, "Invalid name provided by socket: " + std::to_string(client_socket));
        Responder::send_to_socket(client_socket, "INVALID_NAME");
    }
}

void GameAdmin::display_active_games() {
    // Log the intention to display all active games.
    Logger::log(__FILENAME__, __FUNCTION__, "Displaying all active games:");

    // Iterate through active games and print their IDs to the console.
    for (const auto& [game_id, game_instance] : active_games) {
        std::cout << "Game ID: " << game_instance->get_game_id() << std::endl;
    }
}

void GameAdmin::remove_player(Player* player) {
    // Log the start of the player removal process.
    Logger::log(__FILENAME__, __FUNCTION__, "Removing player: " + player->get_name() + ", Socket: " + std::to_string(player->get_socket()));

    // Mark the player as inactive and remove them from the logged players map.
    player->is_active = false;
    logged_players.erase(player->get_name());

    Logger::log(__FILENAME__, __FUNCTION__, "Player removed from logged players. Checking queue.");

    // Remove the player from the queue if present.
    auto queue_size = static_cast<int>(players_queue.size());
    remove_player_from_queue(player, queue_size, 0);

    // Notify the player about the exit.
    Responder::update_player_state(player, "EXIT");

    // If the player is part of an active game, force the game exit.
    if (player->get_game_id() > 0) {
        force_game_exit(player);
    }

    // Log and manage the player's ping thread termination.
    Logger::log(__FILENAME__, __FUNCTION__, "Player's game ID reset. Shutting down ping thread.");
    while (player->thread_running) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // Close the player's socket connection if valid.
    if (player->get_socket() != -1) {
        Server::closeConnection(player->get_socket());
    }

    Logger::log(__FILENAME__, __FUNCTION__, "Player removal complete: " + player->get_name());
}

void GameAdmin::remove_player_from_queue(Player* player, int total, int current) {
    // Base case: Stop recursion if the queue is empty or all elements are processed.
    if (players_queue.empty() || current == total) {
        return;
    }

    // Pop the top player from the queue for comparison.
    Player* temp_player = players_queue.top();
    players_queue.pop();

    // Recursive call to process the next player in the queue.
    remove_player_from_queue(player, total, current + 1);

    // Reinsert the player back into the queue if they do not match the target player.
    if (temp_player != player) {
        players_queue.push(temp_player);
    } else {
        // Log the removal of the target player from the queue.
        Logger::log(__FILENAME__, __FUNCTION__, "Player " + player->get_name() + " removed from the queue.");
    }
}

void GameAdmin::configure_max_games(int max_games) {
    // Update the maximum number of active games allowed.
    GameAdmin::MAX_GAMES = max_games;
    Logger::log(__FILENAME__, __FUNCTION__, "Maximum games updated to " + std::to_string(max_games));
}

void GameAdmin::force_game_exit(Player* player) {
    // Retrieve the active game associated with the player.
    Game* game_instance = get_active_game(player->get_game_id());

    if (game_instance) {
        // Log the forced game exit details.
        Logger::log(__FILENAME__, __FUNCTION__, "Forcing game exit for player: " + player->get_name() + ", Game ID: " + std::to_string(game_instance->get_game_id()));

        // Remove the game from the active games map.
        active_games.erase(game_instance->get_game_id());

        // Reset the opponent's stats and state.
        Player* opponent = game_instance->get_opponent(player);
        opponent->reset_game_stats();
        opponent->set_state("LOBBY");

        // Notify the opponent about the game termination.
        Responder::update_player_state(opponent, "LOBBY");
        Responder::update_player_status(opponent, "Opponent did not return.");

        // Delete the game instance.
        delete game_instance;
    }
}

void GameAdmin::monitor_player_ping(Player* player)
{
    int i = 0;
    while (true) {
        // Break the loop if the player is no longer active.
        if (!player->is_active) {
            break;
        }

        // Check if the player's socket is valid and active.
        if (player->get_socket() > 0) {
            if (player->is_active) {
                // Send a ping to the player.
                Responder::ping_player(player);
            } else {
                break;
            }
        }

        // Wait for the specified ping interval before the next check.
        sleep(PING_INTERVAL);

        if (player->ping) {
            // Handle a successful ping response.
            if (player->get_connection_status() < 0) {
                player->ping = true;
                Logger::log(__FILENAME__, __FUNCTION__, "Player: " + player->get_name() + " has been reconnected");

                Game* game = get_active_game(player->get_game_id());

                if (game) {
                    // Reconnect the player to their active game.
                    Logger::log(__FILENAME__, __FUNCTION__, "Reconnecting player: " + player->get_name() + " to game: " + to_string(player->get_game_id()));

                    player->set_state("IN_GAME");

                    Responder::send_full_game_to_player(player, game);

                    if (game->active_turn == player->get_game_marker()) {
                        Responder::update_player_status(player, "You are on Turn");
                        notify_opponent(player, "Opponent is on Turn");
                    } else {
                        Responder::update_player_status(player, "Opponent is on Turn");
                        notify_opponent(player, "You are on Turn");
                    }
                } else {
                    Responder::update_player_status(player, "Reconnected");
                }
            }

            // Reset the ping flag and connection status.
            i = 0;
            player->ping = false;
            player->set_connection_status(0);
        } else {
            // Handle cases where the player does not respond to pings.
            if (player->get_connection_status() >= 0) {
                GameAdmin::handle_player_disconnect(player->get_socket());
            }

            i += PING_INTERVAL;
            if (i >= TIMEOUT) {
                // Remove the player after timeout and close the ping thread.
                player->thread_running = false;
                GameAdmin::remove_player(player);
                Logger::log(__FILENAME__, __FUNCTION__, "Ping thread for player: " + player->get_name() + " has been closed");
                pthread_exit(0);
            }
        }
    }

    // Ensure the thread exits cleanly when the player manually exits the game.
    player->thread_running = false;
    Logger::log(__FILENAME__, __FUNCTION__, "Ping thread for player: " + player->get_name() + " has been closed");
    pthread_exit(0);
}

