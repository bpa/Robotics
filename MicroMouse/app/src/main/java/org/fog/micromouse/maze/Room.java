package org.fog.micromouse.maze;

import java.util.EnumSet;

public class Room {
    public final int id;
    public final int x;
    public final int y;
    public EnumSet<Direction> neighbors = EnumSet.noneOf(Direction.class);

    public Room(int id, int x, int y, int rooms) {
        this.id = id;
        this.x = x;
        this.y = y;
    }
}
