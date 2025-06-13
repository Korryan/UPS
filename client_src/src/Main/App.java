package Main;

import Logic.Controller;
import Network.messageHandler;
import Object.Configuration;
import javafx.application.Application;
import javafx.stage.Stage;

// The main entry point of the TicTacToe client application.
public class App extends Application {

    // Holds the configuration details such as server address and port.
    private static Configuration configuration;

    // The main method serves as the application's starting point.
    public static void main(String[] args) {
        // Ensure the required arguments are provided (server address and port).
        if (args.length < 2) {
            displayUsageInstructions();
            System.exit(0);
        }

        try {
            // Initialize the configuration with the server address and port.
            configuration = new Configuration(args[0], Integer.parseInt(args[1]));
        } catch (NumberFormatException e) {
            // Handle invalid port number input.
            System.err.println("Invalid port number. Please provide a valid integer.");
            displayUsageInstructions();
            System.exit(0);
        }

        // Launch the JavaFX application.
        launch(args);
    }

    // Displays usage instructions for the application.
    private static void displayUsageInstructions() {
        System.out.println("Usage: java -jar TicTacToe <SERVER_ADDRESS> <SERVER_PORT>");
        System.out.println("SERVER_ADDRESS: The address of the TicTacToe server.");
        System.out.println("SERVER_PORT: The port number of the TicTacToe server.");
    }

    // Called after the JavaFX application is launched.
    @Override
    public void start(Stage stage) {
        // Create the game controller responsible for managing the game logic and UI.
        Controller gameController = new Controller(stage);

        // Create the message handler to manage server communication.
        messageHandler communicationManager = new messageHandler(gameController, configuration);

        // Associate the message handler with the game controller.
        gameController.assignmessageHandler(communicationManager);

        // Initialize event listeners and the default view for the game.
        gameController.initializeListeners();
        gameController.initializeDefaultView();

        // Ensure the application exits cleanly when the window is closed.
        stage.setOnCloseRequest(event -> System.exit(0));

        // Show the main application stage.
        stage.show();
    }
}
