package Network;

import javafx.application.Platform;
import javafx.scene.control.Alert;

import java.io.IOException;

// The Listener class continuously listens for messages from the server
// and processes them through the associated messageHandler.
public class Listener implements Runnable {

    // Reference to the messageHandler responsible for resolving messages.
    private final messageHandler messageHandler;

    // Counter for tracking invalid messages received from the server.
    private int invalidMessageCount = 0;

    // Maximum number of invalid messages allowed before terminating the connection.
    private static final int MAX_INVALID_MESSAGES = 5;

    // Constructor initializes the Listener with a messageHandler instance.
    public Listener(messageHandler messageHandler) {
        this.messageHandler = messageHandler;
    }

    // The main logic for listening and handling server messages.
    @Override
    public void run() {
        while (true) {
            try {
                // Receive a message from the server.
                String incomingMessage = messageHandler.recvMessage();

                if (incomingMessage != null) {
                    // Check if the message length exceeds the allowed limit.
                    if (incomingMessage.length() > 300) {
                        System.out.println(incomingMessage);
                        throw new IOException("Message length exceeds the limit.");
                    }

                    // Process the message on the JavaFX application thread.
                    Platform.runLater(() -> {
                        int result = messageHandler.resolveMessage(incomingMessage);

                        // Increment or reset the invalid message count based on the result.
                        if (result == -1) {
                            invalidMessageCount++;
                        } else {
                            invalidMessageCount = 0;
                        }
                    });
                }
            } catch (IOException e) {
                // Handle invalid messages.
                invalidMessageCount++;
                System.err.println("Invalid message received: " + e.getMessage());
            } catch (Exception e) {
                // Handle unexpected connection loss.
                handleConnectionLoss();
            } finally {
                // Terminate the application if too many invalid messages are received.
                if (invalidMessageCount >= MAX_INVALID_MESSAGES) {
                    handleExcessiveInvalidMessages();
                    break;
                }
            }
        }
    }

    // Handles scenarios where the connection to the server is lost.
    private void handleConnectionLoss() {
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setHeaderText("Connection Lost");
            alert.setContentText("The connection to the server has been lost.");
            alert.showAndWait();

            System.exit(0);
        });
    }

    // Handles scenarios where the server sends too many invalid messages.
    private void handleExcessiveInvalidMessages() {
        Platform.runLater(() -> {
            System.out.println("Too many invalid messages received. Terminating application.");

            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setHeaderText("Application Terminated");
            alert.setContentText("The server sent too many invalid messages. Please try again later.");
            alert.showAndWait();

            System.exit(0);
        });
    }
}
