package org.fog.micromouse.maze;

public enum Direction {
    N(6.5, -1, 0),
    E(8.2, 0, 1),
    S(12.8, 1, 0),
    W(8.2, 0, -1);

    public final double distance;
    public final int dx;
    public final int dy;

    private Direction(double distance, int dx, int dy) {
        this.distance = distance;
        this.dx = dx;
        this.dy = dy;
    }

    public Direction opposite() {
        switch (this) {
            case N:
                return S;
            case E:
                return W;
            case S:
                return N;
            default:
                return E;
        }
    }
}
