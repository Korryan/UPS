#include "Game.hpp"

Game::Game(int game_id, Player *first_player, Player *second_player)
    : game_id(game_id), player_one(first_player), player_two(second_player), previous_winner(nullptr)
{
    // Initialize the game by associating players with game markers and ID.
    first_player->set_game_id(game_id);
    first_player->set_game_marker(1);

    second_player->set_game_id(game_id);
    second_player->set_game_marker(2);

    // Create a game board and set the active turn to the first player.
    game_board = create_board();
    this->active_turn = first_player->get_game_marker();

    // Log the game initialization details.
    Logger::log(__FILENAME__, __FUNCTION__, "Game initialized: ID " + std::to_string(game_id) + ", Players: " + first_player->get_name() + " and " + second_player->get_name());
}

Game::~Game()
{
    // Release the allocated memory for the game board and log the termination.
    release_board(game_board);
    Logger::log(__FILENAME__, __FUNCTION__, "Game terminated: ID " + std::to_string(game_id));
}

Player *Game::get_opponent(Player *player) const
{
    // Return the opponent of the provided player.
    return (player == player_one) ? player_two : player_one;
}

int **Game::create_board() const
{
    // Dynamically allocate memory for the game board.
    int **new_board = new int *[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        new_board[i] = new int[BOARD_SIZE](); // Initialize with zeros.
    }
    return new_board;
}

void Game::release_board(int **board) const
{
    // Deallocate memory for the game board to avoid memory leaks.
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        delete[] board[i];
    }
    delete[] board;
}

void Game::reset_game_board()
{
    // Reset all cells of the game board to 0.
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            game_board[i][j] = 0;
        }
    }
}

int Game::execute_turn(int row, int column, Player *player)
{
    // Check if the selected cell is within the board limits.
    if (row < 0 || column < 0 || row >= BOARD_SIZE || column >= BOARD_SIZE)
    {
        return -1; // Invalid move.
    }

    // Check if it is the player's turn.
    if (player->get_game_marker() == active_turn)
    {
        // Ensure the selected cell is empty.
        if (game_board[row][column] == 0)
        {
            game_board[row][column] = player->get_game_marker(); // Mark the cell.
            active_turn = (active_turn == 1) ? 2 : 1; // Switch the turn.
            return 0; // Successful move.
        }
        else
        {
            return 1; // Cell already occupied.
        }
    }
    else
    {
        return 2; // Not the player's turn.
    }
}

int Game::evaluate_game_state() const
{
    const int WIN_CONDITION = 5; // Number of consecutive markers needed to win.

    // Check for horizontal win.
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j <= BOARD_SIZE - WIN_CONDITION; ++j)
        {
            if (game_board[i][j] != 0)
            {
                bool win = true;
                for (int k = 1; k < WIN_CONDITION; ++k)
                {
                    if (game_board[i][j] != game_board[i][j + k])
                    {
                        win = false;
                        break;
                    }
                }
                if (win) return 1;
            }
        }
    }

    // Check for vertical win.
    for (int i = 0; i <= BOARD_SIZE - WIN_CONDITION; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (game_board[i][j] != 0)
            {
                bool win = true;
                for (int k = 1; k < WIN_CONDITION; ++k)
                {
                    if (game_board[i][j] != game_board[i + k][j])
                    {
                        win = false;
                        break;
                    }
                }
                if (win) return 1;
            }
        }
    }

    // Check for diagonal win (top-left to bottom-right).
    for (int i = 0; i <= BOARD_SIZE - WIN_CONDITION; ++i)
    {
        for (int j = 0; j <= BOARD_SIZE - WIN_CONDITION; ++j)
        {
            if (game_board[i][j] != 0)
            {
                bool win = true;
                for (int k = 1; k < WIN_CONDITION; ++k)
                {
                    if (game_board[i][j] != game_board[i + k][j + k])
                    {
                        win = false;
                        break;
                    }
                }
                if (win) return 1;
            }
        }
    }

    // Check for diagonal win (bottom-left to top-right).
    for (int i = WIN_CONDITION - 1; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j <= BOARD_SIZE - WIN_CONDITION; ++j)
        {
            if (game_board[i][j] != 0)
            {
                bool win = true;
                for (int k = 1; k < WIN_CONDITION; ++k)
                {
                    if (game_board[i][j] != game_board[i - k][j + k])
                    {
                        win = false;
                        break;
                    }
                }
                if (win) return 1;
            }
        }
    }

    // Check if the board is full.
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (game_board[i][j] == 0)
            {
                return 0; // Game still in progress.
            }
        }
    }

    return -1; // Game is a draw.
}

int Game::get_board_value(int row, int column) const
{
    // Return the value of the specified cell on the game board.
    return game_board[row][column];
}
