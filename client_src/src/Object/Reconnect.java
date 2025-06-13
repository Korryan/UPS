package Object;

// The Reconnect class represents the state of the game during a reconnection event.
// It includes the game board and the player's indicator.
public class Reconnect {

    private int[][] gameBoard; // The game board represented as a 2D array.

    private int myIndicator; // The player's unique indicator on the game board.

    // Constructor initializes the Reconnect object with a serialized game board and the player's indicator.
    public Reconnect(String gameBoard, int myIndicator) {
        this.gameBoard = generateGameboard(gameBoard); // Parse the serialized game board into a 2D array.
        this.myIndicator = myIndicator; // Set the player's indicator.
    }

    // Parses a serialized game board string into a 2D integer array.
    private int[][] generateGameboard(String gameBoard) {
        int[][] data = new int[11][11]; // Initialize a fixed-size 11x11 game board.

        String[] split = gameBoard.split(","); // Split the serialized string by commas.

        int k = 0; // Column index.
        int j = 0; // Row index.
        for (int i = 0; i < split.length; i++) {
            data[j][k] = Integer.parseInt(split[i].trim()); // Parse and assign the value to the board.

            k++;

            // Move to the next row after every 11 columns.
            if ((i + 1) % 11 == 0) {
                k = 0;
                j++;
            }
        }

        return data; // Return the populated 2D game board.
    }

    // Returns the current state of the game board.
    public int[][] getGameBoard() {
        return gameBoard;
    }

    // Returns the player's unique indicator.
    public int getMyIndicator() {
        return myIndicator;
    }
}