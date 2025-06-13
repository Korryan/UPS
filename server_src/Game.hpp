#ifndef Game_hpp
#define Game_hpp

#include <iostream>
#include "Player.hpp"
#include "Logger.hpp"

class Game
{
private:
    Player *player_one;
    Player *player_two;
    int game_id;
    int **game_board;
    Player *previous_winner;
    int **create_board() const;
    void release_board(int **board) const;

public:
    static const int BOARD_SIZE = 11;

    Game(int game_id, Player *first_player, Player *second_player);
    ~Game();

    Player *get_opponent(Player *player) const;
    Player *get_first_player() const { return player_one; };
    Player *get_second_player() const { return player_two; };
    Player *get_previous_winner() const { return previous_winner; };
    void set_previous_winner(Player *winner) { previous_winner = winner; };
    int get_game_id() const { return game_id; };

    void reset_game_board();
    int execute_turn(int row, int column, Player *player);
    int evaluate_game_state() const;
    int get_board_value(int row, int column) const;

    int active_turn;
};

#endif /* Game_hpp */