package org.fog.micromouse.maze;

public class Maze {
    Room[] maze;
    int height;
    int width;
    int size;

    public Maze(int height, int width) {
        this.height = height;
        this.width = width;
        this.size = height * width;
        int i = 0;
        for (int y = 0; y < this.height; y++) {
            for (int x = 0; x < this.width; x++) {
                maze[i] = new Room(i, x, y, this.size);
                i++;
            }
        }
    }

    public void link(int x, int y, Direction d) {
        int id = y * width + x;
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return;
        }

        int x2 = x + d.dx;
        int y2 = y + d.dy;
        if (x2 < 0 || x2 >= width || y2 < 0 || y2 >= height) {
            return;
        }

        int id2 = y2 * width + x2;
        maze[id].neighbors.add(d);
        maze[id2].neighbors.add(d.opposite());
    }
}
