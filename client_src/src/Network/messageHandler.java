package Network;

import Logic.Controller;
import Enum.*;
import javafx.scene.control.Alert;
import javafx.stage.Stage;
import Object.*;

import java.io.*;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;

import java.io.*;
import java.net.*;
import javafx.scene.control.Alert;

// The messageHandler class is responsible for managing communication with the server.
public class messageHandler {

    private Socket socket; // Socket for the connection to the server.

    private BufferedWriter bw; // Writer for sending messages to the server.
    private BufferedReader br; // Reader for receiving messages from the server.

    private Controller controller; // Reference to the game controller for UI updates.

    private String lastMessage = ""; // Stores the last received message.

    private Configuration config; // Holds the server configuration (IP and port).

    private boolean allowCommunication = true; // Indicates if communication with the server is allowed.

    // Constructor initializes the messageHandler with a controller and configuration.
    public messageHandler(Controller controller, Configuration config) {
        this.controller = controller;
        this.config = config;

        // Attempt to create a connection with a 2-second timeout.
        createConnection(2000);
    }

    // Establishes a connection to the server.
    public void createConnection(int timeout) {
        try {
            socket = new Socket();

            // Define the server address and connect within the specified timeout.
            InetSocketAddress isa = new InetSocketAddress(this.config.getIp(), this.config.getPort());
            socket.connect(isa, timeout);

            // Log the connection details.
            InetAddress address = socket.getInetAddress();
            System.out.print("Connecting to: " + address.getHostAddress() + " with hostname: " + address.getHostName() + "\n");

            // Initialize the input and output streams for communication.
            bw = new BufferedWriter(new OutputStreamWriter(this.socket.getOutputStream()));
            br = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));
        } catch (IOException e) {
            // Display an error message if the connection fails and exit the application.
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setHeaderText("Server Error");
            alert.setContentText("Cannot connect to server. Try again later.");
            alert.showAndWait();

            System.exit(0);
        }
    }

    // Sends a message to the server.
    public void sendMessage(String msg) {
        System.out.println("Sending message: " + msg);

        if (this.allowCommunication) {
            // Check if the connection is active before sending the message.
            if (msg.equals("ACK;|") || testConnection()) {
                try {
                    bw.write(msg);
                    bw.flush();
                } catch (IOException e) {
                    // Handle lost connection scenarios.
                    printLostConnection();
                }
            } else {
                printLostConnection();
            }
        }
    }

    // Displays an alert and updates the UI when the connection is lost.
    private void printLostConnection() {
        this.allowCommunication = false;

        Alert alert = new Alert(Alert.AlertType.WARNING);
        alert.setHeaderText("Lost Connection");
        alert.setContentText("Connection with server is lost. Trying to reconnect.");
        alert.showAndWait();

        controller.updateStatus("Reconnecting...");
    }

    // Tests the connection by sending a ping message.
    private boolean testConnection() {
        try {
            bw.write("PING;|");
            bw.flush();

            return true;
        } catch (IOException e) {
            this.allowCommunication = false;
            return false;
        }
    }

    // Receives a message from the server.
    public String recvMessage() {
        try {
            String msg = br.readLine().trim();

            if (!msg.isEmpty()) {
                // Store and return the received message.
                this.lastMessage = msg;
                return this.lastMessage;
            } else {
                // Return null if the message is empty.
                return null;
            }
        } catch (Exception e) {
            // Return null in case of an exception.
            return null;
        }
    }

    // Sends a request to find a game on the server.
    public void findGame() {
        sendMessage(EMessage.WAITING_FOR_GAME.toString());
    }
// Logs the player into the server with a specified nickname.
public void loginToServer(String nickname) {
    this.controller.setPlayerNickname(nickname);
    sendMessage(EMessage.CONNECT + nickname + ";|");
}

// Sends the player's move (row and column) to the server.
public void sentMoveToServer(int row, int column) {
    sendMessage(EMessage.TURN.toString() + row + EMessage.DELIMETER + column + EMessage.DELIMETER + "|");
}

// Notifies the server that the game has ended.
public void closeGame() {
    sendMessage(EMessage.GAME_OVER.toString());
}

// Sends a message to exit the current session or game.
public void exit() {
    sendMessage(EMessage.EXIT.toString());
}

// Requests a rematch from the server.
public void replay() {
    sendMessage(EMessage.REMATCH.toString());
}

// Processes a composite message by splitting it into individual parts and resolving each part.
public void resolving(String msg) {
    System.out.println(msg);
    String parts[] = msg.split("%");
    ArrayList<String> usedParts = new ArrayList<>();
    boolean used;

    // Ensure each unique part is resolved only once.
    for (int i = 0; i < parts.length; i++) {
        used = false;

        for (int j = 0; j < usedParts.size(); j++) {
            if (usedParts.get(j).compareToIgnoreCase(parts[i]) == 0) {
                used = true;
            }
        }

        if (!used) {
            usedParts.add(parts[i]);
            resolveMessage(parts[i]);
        }
    }
}

// Resolves an individual message and updates the game state accordingly.
public int resolveMessage(String msg) {
    String parts[] = msg.split(";");

    if (parts.length == 0) {
        return -1; // Invalid message.
    }

    EState state;

    try {
        // Determine the message type by parsing the state.
        state = EState.valueOf(parts[0].toUpperCase());
    } catch (Exception e) {
        System.out.println(msg);
        System.out.println("Invalid message. Error: Unrecognised type of message");
        return -1;
    }

    System.out.println("Message Received: " + msg);

    ResponseData data;

    // Handle specific message types and extract additional data if needed.
    if (state.equals(EState.OPPONENT_TURN) || state.equals(EState.YOUR_TURN)) {
        int row = Integer.valueOf(parts[1]);
        int column = Integer.valueOf(parts[2]);

        data = new ResponseData(state, new Coordinates(row, column));
    } else if (state.equals(EState.WIN) || state.equals(EState.TIE) || state.equals(EState.LOSE)) {
        int yourScore = Integer.valueOf(parts[1]);
        int opponentScore = Integer.valueOf(parts[2]);

        data = new ResponseData(state, new Score(yourScore, opponentScore));
    } else if (state.equals(EState.RECONNECT)) {
        data = new ResponseData(state, parts[1], new Reconnect(parts[2], Integer.parseInt(parts[3])));
    } else if (state.equals(EState.STARTING_GAME)) {
        data = new ResponseData(state, parts[1]);
    } else if (state.equals(EState.STATUS)) {
        data = new ResponseData(state, parts[1]);
    } else if (state.equals(EState.PING)) {
        data = new ResponseData(state, 0);
    } else {
        int result = 0;
        if (parts.length > 1) {
            result = Integer.valueOf(parts[1]);
        }
        data = new ResponseData(state, result);
    }

    // Update the controller with the resolved state and data.
    controller.handleStateChange(data);

    return 0;
}

// Retrieves the associated controller.
public Controller getController() {
    return this.controller;
}

// Allows enabling or disabling communication with the server.
public void setAllowCommunication(boolean allowCommunication) {
    this.allowCommunication = allowCommunication;
}
}
