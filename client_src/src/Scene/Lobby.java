package Scene;

import Network.messageHandler;
import Enum.EMessage;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Cursor;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.SplitPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.scene.text.TextAlignment;

// The Lobby class represents the main menu or lobby scene where players can initiate a game or exit.
public class Lobby extends SceneTemplate {

    private Button playGameButton; // Button to start searching for a game.

    // Constructor initializes the Lobby scene with the specified dimensions and message handler.
    public Lobby(messageHandler messageHandler, int width, int height) {
        super(messageHandler, width, height);
    }

    // Builds the controls and layout for the lobby scene.
    @Override
    public Node buildControls() {
        VBox mainLayout = new VBox();
        mainLayout.setStyle("-fx-background-color: white;");
        mainLayout.setSpacing(50);

        // Container for buttons and welcome message.
        VBox buttonContainer = new VBox();
        Text welcomeMessage = new Text("Welcome, " + super.retrieveNickname() + "!");
        welcomeMessage.setFont(Font.font("Arial", FontWeight.BOLD, 20));

        playGameButton = new Button("Start");
        playGameButton.setPrefSize(130, 30);
        playGameButton.setOnAction(event -> initiateGameSearch()); // Action to start searching for a game.

        Button quitButton = new Button("Exit");
        quitButton.setPrefSize(130, 30);
        quitButton.setOnAction(event -> quitApplication()); // Action to quit the application.

        buttonContainer.getChildren().addAll(welcomeMessage, playGameButton, quitButton);
        buttonContainer.setSpacing(15);
        buttonContainer.setAlignment(Pos.CENTER);
        buttonContainer.setPadding(new Insets(10));

        // Status box with a black border to display the current status.
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
        mainLayout.getChildren().addAll(buttonContainer, statusContainer);
        SplitPane.setResizableWithParent(statusContainer, false);

        return mainLayout;
    }

    // Updates the playGameButton to indicate the game search is in progress.
    public void displayGameSearchStatus() {
        playGameButton.setText("Searching for Game...");
        playGameButton.setDisable(true);
        playGameButton.setCursor(Cursor.WAIT);
    }

    // Initiates the game search by sending a message to the server.
    private void initiateGameSearch() {
        this.messageHandler.findGame();
    }

    // Quits the application by sending an exit message to the server.
    private void quitApplication() {
        this.messageHandler.exit();
    }
}
