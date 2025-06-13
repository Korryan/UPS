package Logic;

import Network.Listener;
import Network.Connector;
import Network.messageHandler;
import Enum.*;
import Object.*;
import Scene.*;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class Controller {

    private final Stage applicationStage;

    private messageHandler messageHandler;

    private EState applicationState;

    private SceneTemplate activeScene;

    private String playerNickname;

    private Thread listenerThread;

    private Thread connectivityMonitorThread;

    private Connector connectivityMonitor;


    public Controller(Stage stage) {
        this.applicationStage = stage;
    }

    public void initializeListeners() {
        Listener clientListener = new Listener(messageHandler);
        listenerThread = new Thread(clientListener);
        listenerThread.start();

        connectivityMonitor = new Connector(messageHandler);
        connectivityMonitorThread = new Thread(connectivityMonitor);
    }

    public void setPlayerNickname(String nickname) {
        this.playerNickname = nickname;
    }

    public void assignmessageHandler(messageHandler manager) {
        this.messageHandler = manager;
    }

    public void initializeDefaultView() {
        this.activeScene = new Login(this.messageHandler, 380, 200);
        displayScene(this.activeScene.createScene());
    }

    private void displayScene(Scene scene) {
        applicationStage.setScene(scene);
        applicationStage.show();
    }

    
    private void handleSuccessfulLogin() {
        try {
            this.activeScene = new Lobby(this.messageHandler, 380, 200);
            this.activeScene.assignNickname(this.playerNickname);
            displayScene(this.activeScene.createScene());

            connectivityMonitorThread.start();

            System.out.println("Welcome " + this.playerNickname + "!");
        } catch (Exception e) {
            System.err.println("Error during login: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private void navigateToLobby() {
        this.activeScene = new Lobby(this.messageHandler, 380, 200);
        this.activeScene.assignNickname(this.playerNickname);
        displayScene(this.activeScene.createScene());
    }

    private void initiateGame(String opponentName) {
        this.activeScene = new Game(this.messageHandler, 550, 630, null, opponentName);
        displayScene(this.activeScene.createScene());
        System.out.println("Game started against: " + opponentName);
    }
    

    private void processPlayerMove(int row, int column) {
        if (this.activeScene instanceof Game) {
            ((Game) this.activeScene).updatePlayerMove(row, column, true); // true označuje hráče X
        }
    }
    
    private void processOpponentMove(int row, int column) {
        if (this.activeScene instanceof Game) {
            ((Game) this.activeScene).updatePlayerMove(row, column, false); // false označuje hráče O
        }
    }
    

    private void displayResult(String result, Score score) {
        this.activeScene = new Result(this.messageHandler, 300, 300, result, score);
        displayScene(this.activeScene.createScene());
    }

    private void terminateGameSession() {
        this.activeScene = new Lobby(this.messageHandler, 380, 200);
        this.activeScene.assignNickname(this.playerNickname);
        displayScene(this.activeScene.createScene());
    }

    private void displayErrorMessage(String message) {
        if (this.activeScene != null) {
            this.activeScene.displayErrorStatus(message);
        }
    }

    private void waitForOpponent() {
        System.out.println("Looking for game");
        System.out.println(this.activeScene.getClass().getName());

        if (this.activeScene instanceof Lobby) {
            ((Lobby) this.activeScene).displayGameSearchStatus();
        } else if (this.activeScene instanceof Result) {
            ((Result) this.activeScene).displaySearchingStatus();
        }

        this.activeScene.updatestatusDisplay("Waiting for opponent");
    }

    private void exitApplication() {
        listenerThread.interrupt();
        System.exit(0);
    }

    private void acknowledgePing() {
        messageHandler.setAllowCommunication(true);
        messageHandler.sendMessage("ACK;|");

        connectivityMonitor.recievedPing();
        connectivityMonitor.setCheck(true);
    }

    private void reconnectToGame(Reconnect reconnect, String opponent) {
        this.activeScene = new Game(this.messageHandler, 550, 630, reconnect, opponent);
        displayScene(this.activeScene.createScene());
        ((Game) this.activeScene).restoreGame();
        System.out.println("Reconnecting game");
    }

    public void renderScene(Scene scene) {
        applicationStage.setScene(scene);
    }

    public void updateStatus(String statusMessage) {
        if (this.activeScene != null) {
            this.activeScene.updatestatusDisplay(statusMessage);
        }
    }

    public void handleStateChange(ResponseData response) {
        switch (response.getState()) {
            case CONNECT:
                handleSuccessfulLogin();
                break;
            case LOBBY:
                navigateToLobby();
                break;
            case WAITING:
                waitForOpponent();
                break;
            case STARTING_GAME:
                initiateGame(response.getMsg());
                break;
            case YOUR_TURN:
                if (response.getResult() == 0) {
                    processPlayerMove(response.getCoordinates().getRow(), response.getCoordinates().getColumn());
                }
                break;
            case OPPONENT_TURN:
                processOpponentMove(response.getCoordinates().getRow(), response.getCoordinates().getColumn());
                break;
            case WIN:
                displayResult("YOU WIN!", response.getScore());
                break;
            case LOSE:
                displayResult("YOU LOSE", response.getScore());
                break;
            case TIE:
                displayResult("TIE", response.getScore());
                break;
            case GAME_OVER:
                terminateGameSession();
                break;
            case NAME_TAKEN:
                displayErrorMessage("Name is taken. Please choose a different one.");
                break;
            case EXIT:
                exitApplication();
                break;
            case RECONNECT:
                reconnectToGame(response.getReconnect(), response.getMsg());
                break;
            case STATUS:
                updateStatus(response.getMsg());
                break;
            case INVALID_NAME:
                displayErrorMessage("Name cannot be empty or contain \";\". Maximum length is 13 characters.");
                break;
            case MAXIMUM_GAMES_REACHED:
                displayErrorMessage("Maximum limit of games reached. Please try again later.");
                break;
            case PING:
                acknowledgePing();
                break;
            default:
                System.out.println("Unknown state received: " + response.getState());
        }
    }
}
