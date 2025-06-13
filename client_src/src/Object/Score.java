package Object;

// The Score class represents the scores of the player and their opponent in a game.
public class Score {
    int yourScore; // The player's score.
    int opponentScore; // The opponent's score.

    // Constructor initializes the scores for both the player and the opponent.
    public Score(int your_score, int opponent_score) {
        this.yourScore = your_score; // Set the player's score.
        this.opponentScore = opponent_score; // Set the opponent's score.
    }

    // Returns the player's score.
    public int getYourScore() {
        return yourScore;
    }

    // Returns the opponent's score.
    public int getOpponentScore() {
        return opponentScore;
    }
}
