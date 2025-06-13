package Scene;

import Network.messageHandler;
import Object.*;
import Enum.*;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.SplitPane;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.scene.text.TextAlignment;

// The Game class represents the main game scene, managing the game board and interactions.
public class Game extends SceneTemplate {

    private final Reconnect gameReconnectData; // Data for restoring a previous game state.
    private static final int BOARD_SIZE = 11; // The size of the game board.
    private final Cell[][] gameBoard; // 2D array of cells representing the game board.
    private final String opponentName; // The name of the opponent player.

    // Constructor initializes the game scene with necessary data and dimensions.
    public Game(messageHandler messageHandler, int width, int height, Reconnect reconnect, String opponentName) {
        super(messageHandler, width, height + 25);
        this.gameReconnectData = reconnect;
        this.gameBoard = new Cell[BOARD_SIZE][BOARD_SIZE];
        this.opponentName = opponentName;
    }

    // Builds the controls and layout for the game scene.
    @Override
    public Node buildControls() {
        VBox mainLayout = new VBox();
        mainLayout.setStyle("-fx-background-color: white;");
        mainLayout.setSpacing(10);

        // Display opponent's name.
        Text opponentText = new Text("Playing against: " + this.opponentName);
        opponentText.setFont(Font.font("Arial", FontWeight.BOLD, 24));
        opponentText.setFill(Color.BLACK);
        opponentText.setTextAlignment(TextAlignment.CENTER);

        VBox opponentContainer = new VBox(opponentText);
        opponentContainer.setAlignment(Pos.CENTER);
        opponentContainer.setPadding(new Insets(20, 0, 10, 0)); // Add spacing around the text.

        // Create the game board.
        Pane boardPane = new Pane();
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                Cell cell = new Cell(this.messageHandler, row, col);
                cell.setTranslateX(col * 50);
                cell.setTranslateY(row * 50);
                boardPane.getChildren().add(cell);
                gameBoard[row][col] = cell;
            }
        }

        VBox.setVgrow(boardPane, Priority.ALWAYS); // Allow the board to take up available space.

        // Status box with a black border.
        VBox statusContainer = new VBox();
        statusContainer.setStyle("-fx-border-color: black; -fx-border-width: 1px 0 0 0; -fx-padding: 10;");

        HBox statusBox = new HBox();
        statusBox.setAlignment(Pos.CENTER);
        Text statusLabel = new Text("Status: ");
        statusLabel.setTextAlignment(TextAlignment.CENTER);
        statusLabel.setFill(Color.BLACK);

        this.statusDisplay = new Text("Ready");
        this.statusDisplay.setTextAlignment(TextAlignment.CENTER);
        this.statusDisplay.setFill(Color.BLACK);

        statusBox.getChildren().addAll(statusLabel, this.statusDisplay);
        statusContainer.getChildren().add(statusBox);

        // Prevent resizing of the status box.
        VBox.setVgrow(statusContainer, Priority.NEVER);

        // Add components to the main layout.
        mainLayout.getChildren().addAll(opponentContainer, boardPane, statusContainer);
        SplitPane.setResizableWithParent(statusContainer, false);

        return mainLayout;
    }

    // Updates the board to reflect a player's move.
    public void updatePlayerMove(int row, int col, boolean isPlayerX) {
        if (isPlayerX) {
            gameBoard[row][col].markAsX();
        } else {
            gameBoard[row][col].markAsO();
        }
    }

    // Restores the game state from the reconnect data.
    public void restoreGame() {
        int[][] boardData = gameReconnectData.getGameBoard();
        int playerIndicator = gameReconnectData.getMyIndicator();

        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (boardData[row][col] == playerIndicator) {
                    gameBoard[row][col].markAsX();
                } else if (boardData[row][col] != 0) {
                    gameBoard[row][col].markAsO();
                }
            }
        }
    }

    // Clears the game board for a new game.
    public void resetBoard() {
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                gameBoard[row][col].clearCell();
            }
        }
    }

    // Inner class representing a single cell on the game board.
    private class Cell extends StackPane {
        private final Text cellText; // Text to display X or O in the cell.
        private final int row; // Row index of the cell.
        private final int column; // Column index of the cell.

        // Constructor initializes the cell with its row, column, and message handler.
        public Cell(messageHandler manager, int row, int column) {
            this.row = row;
            this.column = column;

            Rectangle border = new Rectangle(50, 50);
            border.setFill(null);
            border.setStroke(Color.BLACK);

            this.cellText = new Text();
            this.cellText.setFont(Font.font(36));
            this.cellText.setFill(Color.WHITESMOKE);

            setAlignment(Pos.CENTER);
            getChildren().addAll(border, this.cellText);

            setOnMouseClicked(event -> {
                if (event.getButton() == MouseButton.PRIMARY) {
                    manager.sentMoveToServer(row, column);
                }
            });
        }

        // Marks the cell as X.
        public void markAsX() {
            cellText.setText("X");
            cellText.setFill(Color.RED);
        }

        // Marks the cell as O.
        public void markAsO() {
            cellText.setText("O");
            cellText.setFill(Color.BLUE);
        }

        // Clears the cell.
        public void clearCell() {
            cellText.setText("");
        }
    }
}
