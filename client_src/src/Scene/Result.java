package Scene;

import Network.messageHandler;
import Object.*;
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

// The Result class represents the result screen displayed after a game ends.
public class Result extends SceneTemplate {

    private final String resultHeader; // The header text indicating the result (e.g., "You Win", "You Lose").
    private final Score matchScore; // The scores of the match.
    private Button replayButton; // Button to request a replay.

    // Constructor initializes the Result scene with the match scores and result header.
    public Result(messageHandler messageHandler, int width, int height, String resultHeader, Score matchScore) {
        super(messageHandler, width, height);
        this.resultHeader = resultHeader;
        this.matchScore = matchScore;
    }

    // Builds the controls and layout for the result scene.
    @Override
    public Node buildControls() {
        VBox mainLayout = new VBox();
        mainLayout.setStyle("-fx-background-color: white;");
        mainLayout.setSpacing(50);

        // Container for the result content (header, scores, buttons).
        VBox contentContainer = new VBox();
        Text headerText = new Text(resultHeader); // Display the result header.
        headerText.setFont(Font.font("Arial", FontWeight.BOLD, 20));

        // Display the match scores.
        HBox scoreContainer = new HBox();
        Text yourScoreText = new Text(Integer.toString(matchScore.getYourScore()));
        Text delimiterText = new Text("  -  ");
        Text opponentScoreText = new Text(Integer.toString(matchScore.getOpponentScore()));

        yourScoreText.setFont(Font.font("Arial", FontWeight.BOLD, 30));
        opponentScoreText.setFont(Font.font("Arial", FontWeight.BOLD, 30));
        delimiterText.setFont(Font.font("Arial", FontWeight.BOLD, 30));

        scoreContainer.setAlignment(Pos.CENTER);
        scoreContainer.getChildren().addAll(yourScoreText, delimiterText, opponentScoreText);

        // Buttons for replay and quitting the game.
        replayButton = new Button("Replay");
        replayButton.setPrefSize(80, 30);
        replayButton.setOnAction(event -> initiateReplay());

        Button quitButton = new Button("Exit");
        quitButton.setPrefSize(80, 30);
        quitButton.setOnAction(event -> quitGame());

        contentContainer.getChildren().addAll(headerText, scoreContainer, replayButton, quitButton);
        contentContainer.setSpacing(15);
        contentContainer.setAlignment(Pos.CENTER);
        contentContainer.setPadding(new Insets(10));

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
        mainLayout.getChildren().addAll(contentContainer, statusContainer);
        SplitPane.setResizableWithParent(statusContainer, false);

        return mainLayout;
    }

    // Updates the replay button to indicate that the game is waiting for an opponent.
    public void displaySearchingStatus() {
        replayButton.setText("Waiting for opponent");
        replayButton.setDisable(true);
        replayButton.setCursor(Cursor.WAIT);
    }

    // Sends a request to the server to replay the game.
    private void initiateReplay() {
        this.messageHandler.replay();
    }

    // Sends a message to the server to close the game.
    private void quitGame() {
        this.messageHandler.closeGame();
    }
}
