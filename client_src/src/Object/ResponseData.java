package Object;

import Enum.*;

// The ResponseData class encapsulates data received from the server in response to various events.
// It supports multiple types of responses based on the game state and associated data.
public class ResponseData {

    private EState state; // The state of the game or server response.
    private int result; // Numeric result for specific states (e.g., success or failure codes).
    private String msg; // Additional message associated with the response.

    private Coordinates coordinates; // Coordinates data for moves or positions.
    private Score score; // Score data for win/lose/tie responses.

    private Reconnect reconnect; // Reconnection data, including the game board state.

    // Constructor for numeric results.
    public ResponseData(EState state, int result) {
        this.state = state; // Set the state.
        this.result = result; // Set the numeric result.
    }

    // Constructor for coordinates-based responses.
    public ResponseData(EState state, Coordinates coordinates) {
        this.state = state; // Set the state.
        this.coordinates = coordinates; // Set the coordinates.
    }

    // Constructor for score-based responses (e.g., game results).
    public ResponseData(EState state, Score score) {
        this.state = state; // Set the state.
        this.score = score; // Set the score.
    }

    // Constructor for reconnection responses with an additional message.
    public ResponseData(EState state, String msg, Reconnect reconnect) {
        this.state = state; // Set the state.
        this.msg = msg; // Set the message.
        this.reconnect = reconnect; // Set the reconnection data.
    }

    // Constructor for simple message-based responses.
    public ResponseData(EState state, String msg) {
        this.state = state; // Set the state.
        this.msg = msg; // Set the message.
    }

    // Returns the state of the response.
    public EState getState() {
        return state;
    }

    // Returns the numeric result associated with the response.
    public int getResult() {
        return result;
    }

    // Returns the coordinates associated with the response.
    public Coordinates getCoordinates() {
        return coordinates;
    }

    // Returns the score associated with the response.
    public Score getScore() {
        return score;
    }

    // Returns the reconnection data associated with the response.
    public Reconnect getReconnect() {
        return reconnect;
    }

    // Returns the message associated with the response.
    public String getMsg() {
        return msg;
    }
}
