package org.fog.micromouse.vision;

import android.util.Log;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

public class CalibrateColor implements CameraBridgeViewBase.CvCameraViewListener2 {
    private Mat hsv;
    private Mat rgb;
    private Mat rgb2;
    private Mat mask;
    private Scalar minHSV;
    private Scalar maxHSV;

    public CalibrateColor(Scalar minHSV, Scalar maxHSV) {
        this.minHSV = minHSV;
        this.maxHSV = maxHSV;
    }

    public void ensureInitialized() {
        if (hsv == null) {
            hsv = new Mat();
            mask = new Mat();
            rgb = new Mat();
            rgb2 = new Mat();
        }
    }

    @Override
    public void onCameraViewStarted(int width, int height) {

    }

    public void onCameraViewStopped() {
        hsv.release();
        mask.release();
        rgb.release();
        rgb2.release();
    }

    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        ensureInitialized();
        Mat rgba = inputFrame.rgba();
        Imgproc.cvtColor(rgba, hsv, Imgproc.COLOR_RGB2HSV);
        Core.inRange(hsv, minHSV, maxHSV, mask);
        Imgproc.cvtColor(mask, rgb, Imgproc.COLOR_GRAY2RGBA);
        Core.bitwise_and(rgba, rgb, rgb2);
        return rgb2;
    }
}
