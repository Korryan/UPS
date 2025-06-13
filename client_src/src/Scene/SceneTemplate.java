package Scene;

import Network.messageHandler;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.scene.paint.Color;
import javafx.scene.text.Text;

// Abstract class serving as a template for creating scenes in the application.
public abstract class SceneTemplate {

    protected final messageHandler messageHandler; // Handles communication with the server.
    private final int sceneWidth; // Width of the scene.
    private final int sceneHeight; // Height of the scene.
    protected String playerNickname; // Stores the player's nickname.

    protected Text statusDisplay; // Text element for displaying the status of the current scene.

    // Constructor initializes the scene template with a message handler and dimensions.
    public SceneTemplate(messageHandler messageHandler, int width, int height) {
        this.messageHandler = messageHandler;
        this.sceneWidth = width;
        this.sceneHeight = height;
    }

    // Creates and returns a new JavaFX Scene with the defined layout and dimensions.
    public Scene createScene() {
        return new Scene(constructRootLayout(), this.sceneWidth, this.sceneHeight);
    }

    // Constructs the root layout of the scene, placing controls at the center.
    private Parent constructRootLayout() {
        BorderPane layout = new BorderPane();
        layout.setCenter(buildControls()); // The main controls are built by the derived class.
        return layout;
    }

    // Assigns the player's nickname to the template.
    public void assignNickname(String nickname) {
        this.playerNickname = nickname;
    }

    // Retrieves the player's nickname from the template.
    public String retrieveNickname() {
        return this.playerNickname;
    }

    // Updates the status display with a given message in black color.
    public void updatestatusDisplay(String message) {
        this.statusDisplay.setFill(Color.BLACK);
        this.statusDisplay.setText(message);
    }

    // Displays an error message in the status display with red color.
    public void displayErrorStatus(String message) {
        this.statusDisplay.setFill(Color.RED);
        this.statusDisplay.setText(message);
    }

    // Fetches the current text displayed in the status display.
    public String fetchstatusDisplay() {
        return this.statusDisplay.getText();
    }

    // Abstract method for building the controls of the scene. Must be implemented by derived classes.
    protected abstract Node buildControls();
}