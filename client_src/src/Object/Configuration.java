package Object;

// The Configuration class holds server connection details such as IP address and port number.
public class Configuration {

    private final String ip; // The IP address of the server.
    private final int port; // The port number of the server.

    // Constructor initializes the Configuration with the specified IP address and port number.
    public Configuration(String ip, int port) {
        this.ip = ip; // Set the server IP address.
        this.port = port; // Set the server port number.
    }

    // Returns the IP address of the server.
    public String getIp() {
        return ip;
    }

    // Returns the port number of the server.
    public int getPort() {
        return port;
    }
}