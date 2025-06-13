package Network;

import javafx.application.Platform;
import javafx.scene.control.Alert;

// The Connector class is responsible for monitoring the connection to the server
// and handling reconnection attempts if the connection is lost.
public class Connector implements Runnable {

    // Indicates if the server has responded to the last ping.
    private boolean ping;

    // Indicates if the connection should continue being checked.
    private boolean check;

    // Timeout limit for reconnection attempts, in seconds.
    private static final int TIMEOUT = 60;

    // Reference to the messageHandler for communication with the server.
    private messageHandler messageHandler;

    // Constructor initializes the Connector with a messageHandler instance and default states for ping and check.
    public Connector(messageHandler messageHandler) {
        this.messageHandler = messageHandler;
        this.ping = true; // Initially assume the server is responsive.
        this.check = true; // Initially enable connection monitoring.
    }

    // The main logic for monitoring the server connection.
    @Override
    public void run() {
        while (true) {
            try {
                // If no ping is received and connection monitoring is enabled, throw an exception to handle disconnection.
                if (!this.ping && this.check) {
                    throw new Exception();
                }

                // Reset the ping flag and pause for 3 seconds before the next check.
                this.ping = false;
                Thread.sleep(3000);
            } catch (Exception e) {
                // Handle connection loss.
                this.check = false; // Stop monitoring the connection.

                // Notify the user of the disconnection on the UI thread.
                Platform.runLater(() -> {
                    Alert alert = new Alert(Alert.AlertType.ERROR);
                    alert.setHeaderText("Connection lost");
                    alert.setContentText("Connection with server lost");
                    alert.showAndWait();
                });

                int i = 0; // Counter for reconnection attempts.

                // Update the controller with the reconnecting status.
                this.messageHandler.getController().updateStatus("Reconnecting...");
                this.messageHandler.setAllowCommunication(false);

                // Attempt to reconnect until the connection is re-established or timeout is reached.
                while (!this.check) {
                    try {
                        i++;

                        // If the timeout is reached, notify the user and exit the application.
                        if (i == TIMEOUT) {
                            Platform.runLater(() -> {
                                Alert alert = new Alert(Alert.AlertType.ERROR);
                                alert.setHeaderText("Connection timeout");
                                alert.setContentText("Connection timeout.");
                                alert.showAndWait();

                                System.exit(0);
                            });
                            break;
                        }

                        // Wait 1 second before the next reconnection attempt.
                        Thread.sleep(1000);
                    } catch (Exception ex) {
                        // Log unhandled exceptions during reconnection.
                        System.out.println("Unhandled exception");
                    }
                }
            }
        }
    }

    // Method to signal that a ping was received from the server.
    public void recievedPing() {
        this.ping = true;
    }

    // Method to enable or disable connection monitoring.
    public void setCheck(boolean check) {
        this.check = check;
    }
}
