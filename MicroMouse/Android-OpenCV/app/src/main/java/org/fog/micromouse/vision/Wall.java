package org.fog.micromouse.vision;

import org.opencv.core.Mat;
import org.opencv.core.Point;

public class Wall {//implements Comparable<Wall> {
    double x1, y1, x2, y2, dx, dy, y_intercept;
    Point start;
    Point end;

    public Wall(double[] vec) {
        x1 = vec[0];
        x2 = vec[1];
        y1 = vec[2];
        y2 = vec[3];
        dx = x1 - x2;
        dy = y1 - y2;
        y_intercept = dx*y1 + dy*x1;
        start = new Point(x1, y1);
        end = new Point(x2, y2);
    }

    public boolean isEdge(Mat mask) {
        double iy = dy == 0 ? 0 : -1 / dy;
        double ix = dx == 0 ? 0 : -1 / dx;
        double mx = x1 + dx/2;
        double my = y1 + dy/2;
        double det = dx*dy - 1/dx*dy;
        this.px = -1/dx*y_intercept/det;
        this.py = -1/dy*y_intercept/det;
        this.distance_squared = dx * dx + dy * dy;
    }

//    @Override
//    public int compareTo(Wall o) {
//        return distance_squared > o.distance_squared ? 1 : -1;
//    }
}
