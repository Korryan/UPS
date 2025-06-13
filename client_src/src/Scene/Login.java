package Scene;

import Network.messageHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.SplitPane;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.scene.text.TextAlignment;

// The Login class represents the login screen where players enter their nickname to log in.
public class Login extends SceneTemplate {

    // Constructor initializes the Login scene with the specified dimensions and message handler.
    public Login(messageHandler messageHandler, int width, int height) {
        super(messageHandler, width, height);
    }

    // Builds the controls and layout for the login scene.
    @Override
    public Node buildControls() {
        VBox mainLayout = new VBox();
        mainLayout.setStyle("-fx-background-color: white;");
        mainLayout.setSpacing(50);

        // Container for input and login button.
        VBox inputContainer = new VBox();
        Text headerText = new Text("Please log in");
        headerText.setFont(Font.font("Arial", FontWeight.BOLD, 20));

        TextField nicknameInputField = new TextField(); // Input field for entering the player's nickname.
        nicknameInputField.setMaxWidth(240);
        nicknameInputField.setMinWidth(240);

        Button loginButton = new Button("Log In"); // Button to trigger the login action.
        loginButton.setPrefSize(80, 30);
        loginButton.setOnAction(event -> authenticatePlayer(nicknameInputField.getText()));

        inputContainer.getChildren().addAll(headerText, nicknameInputField, loginButton);
        inputContainer.setSpacing(15);
        inputContainer.setAlignment(Pos.CENTER);
        inputContainer.setPadding(new Insets(10));

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
        mainLayout.getChildren().addAll(inputContainer, statusContainer);
        SplitPane.setResizableWithParent(statusContainer, false);

        return mainLayout;
    }

    // Authenticates the player by sending their nickname to the server.
    private void authenticatePlayer(String nickname) {
        this.messageHandler.loginToServer(nickname);
    }
}
