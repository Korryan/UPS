package Object;

// The Player class represents a player in the game, identified by their name.
public class Player {

    private String name; // The name of the player.

    // Constructor initializes the Player with a specified name.
    Player(String name) {
        this.name = name; // Set the player's name.
    }

    // Returns the name of the player.
    public String getName() {
        return name;
    }

    // Updates the player's name.
    public void setName(String name) {
        this.name = name;
    }
}
