package Enum;

public enum EMessage {
    DELIMETER(";"),
    CONNECT("NAME;"),
    WAITING_FOR_GAME("WAITING_FOR_GAME;|"),
    TURN("TURN;"),
    REMATCH("REMATCH;|"),
    GAME_OVER("GAME_OVER;|"),
    EXIT("EXIT;|")
    ;



    private final String text;

    EMessage(final String text) {
        this.text = text;
    }

    @Override
    public String toString() {
        return text;
    }


}
