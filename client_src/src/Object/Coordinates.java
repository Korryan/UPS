package Object;

// The Coordinates class represents a position on a grid, defined by a row and column.
public class Coordinates {

    int row; // The row index of the coordinate.
    int column; // The column index of the coordinate.

    // Constructor initializes the Coordinates with a specified row and column.
    public Coordinates(int row, int column) {
        this.row = row; // Set the row index.
        this.column = column; // Set the column index.
    }

    // Returns the row index of the coordinate.
    public int getRow() {
        return row;
    }

    // Returns the column index of the coordinate.
    public int getColumn() {
        return column;
    }
}
