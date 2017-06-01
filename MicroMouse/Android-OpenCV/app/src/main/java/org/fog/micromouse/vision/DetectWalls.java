package org.fog.micromouse.vision;

import android.util.Log;
import android.widget.TextView;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

public class DetectWalls implements CameraBridgeViewBase.CvCameraViewListener2 {
    private Mat hsv;
    private Mat hsv2;
    private Mat canny;
    private Mat mask;
    private Scalar minHSV;
    private Scalar maxHSV;
    public double angle_x = 0;
    public double angle_y = 0;
    private TextView tv;
    private final Scalar RED = new Scalar(255, 0, 0);

    public DetectWalls(Scalar minHSV, Scalar maxHSV, TextView view) {
        this.minHSV = minHSV;
        this.maxHSV = maxHSV;
        tv = view;
    }

    @Override
    public void onCameraViewStarted(int width, int height) {

    }

    public void ensureInitialized() {
        if (hsv == null) {
            hsv = new Mat();
            hsv2 = new Mat();
            mask = new Mat();
            canny = new Mat();
        }
    }

    @Override
    public void onCameraViewStopped() {
        hsv.release();
        mask.release();
        canny.release();
    }

    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        ensureInitialized();
        Mat rgba = inputFrame.rgba();
        Imgproc.cvtColor(rgba, hsv, Imgproc.COLOR_RGB2HSV);
        Core.inRange(hsv, minHSV, maxHSV, mask);
        Imgproc.Canny(mask, canny, 3, 9);
        Mat lines = new Mat();

        Imgproc.HoughLinesP(canny, lines, 1, Math.PI / 180, 50, 50, 20);
//        Imgproc.cvtColor(canny, rgba, Imgproc.COLOR_GRAY2RGBA);
        Imgproc.cvtColor(mask, rgba, Imgproc.COLOR_GRAY2RGBA);

        List<Wall> walls = new ArrayList<>(lines.rows());
        for (int x = 0; x < lines.rows(); x++) {
            Wall wall = new Wall(lines.get(x, 0));
            Imgproc.line(rgba, wall.start, wall.end, RED, 3);
        }
        Collections.sort(walls);
        
        return rgba;
    }
}
