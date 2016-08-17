package org.fog.micromouse;

import org.florescu.android.rangeseekbar.RangeSeekBar;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;

public class CalibrationActivity extends MMActivity
        implements CvCameraViewListener2, RangeSeekBar.OnRangeSeekBarChangeListener<Integer> {
    private static final String TAG = "MicroMouse::Calibration";

    private Mat hsv;
    private Mat rgb;
    private Mat rgb2;
    private Mat mask;
    private Scalar minHSV;
    private Scalar maxHSV;
    private RangeSeekBar<Integer> hBar;
    private RangeSeekBar<Integer> sBar;
    private RangeSeekBar<Integer> vBar;
    private CameraBridgeViewBase mOpenCvCameraView;
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    public CalibrationActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    @Override
    public void onCreate(Bundle b) {
        super.onCreate(b);
        SharedPreferences p = getPreferences(Context.MODE_PRIVATE);
        minHSV = new Scalar(p.getInt("minH", 0), p.getInt("minS", 0), p.getInt("minV", 0));
        maxHSV = new Scalar(p.getInt("maxH", 255), p.getInt("maxS", 255), p.getInt("maxV", 255));

        setContentView(R.layout.camera_layout);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.camera_preview);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        hBar = initRangeBar(minHSV, maxHSV, 0, R.id.h_range);
        sBar = initRangeBar(minHSV, maxHSV, 1, R.id.s_range);
        vBar = initRangeBar(minHSV, maxHSV, 2, R.id.v_range);
    }

    private RangeSeekBar<Integer> initRangeBar(Scalar min, Scalar max, int ind, int view) {
        RangeSeekBar<Integer> rsb = (RangeSeekBar<Integer>) findViewById(view);
        rsb.setSelectedMinValue((int)min.val[ind]);
        rsb.setSelectedMaxValue((int)max.val[ind]);
        rsb.setRangeValues(0, 255);
        rsb.setOnRangeSeekBarChangeListener(this);
        return rsb;
    }

    @Override
    public void onPause()
    {
        super.onPause();

        SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putInt("minH", (int)minHSV.val[0]);
        editor.putInt("minS", (int)minHSV.val[1]);
        editor.putInt("minV", (int)minHSV.val[2]);
        editor.putInt("maxH", (int)maxHSV.val[0]);
        editor.putInt("maxS", (int)maxHSV.val[1]);
        editor.putInt("maxV", (int)maxHSV.val[2]);
        editor.commit();

        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        hsv = new Mat(width, height, CvType.CV_8UC3);
        mask = new Mat(width, height, CvType.CV_8U);
        rgb = new Mat(width, height, CvType.CV_8UC4);
        rgb2 = new Mat(width, height, CvType.CV_8UC4);
    }

    public void onCameraViewStopped() {
        hsv.release();
        mask.release();
        rgb.release();
        rgb2.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        Mat rgba = inputFrame.rgba();
        Imgproc.cvtColor(rgba, hsv, Imgproc.COLOR_RGB2HSV);
        Core.inRange(hsv, minHSV, maxHSV, mask);
        Imgproc.cvtColor(mask, rgb, Imgproc.COLOR_GRAY2RGBA);
        Core.bitwise_and(rgba, rgb, rgb2);
        return rgb2;
    }

    @Override
    public void onRangeSeekBarValuesChanged(RangeSeekBar<?> bar, Integer minValue, Integer maxValue) {
        minHSV.val[0] = hBar.getSelectedMinValue();
        minHSV.val[1] = sBar.getSelectedMinValue();
        minHSV.val[2] = vBar.getSelectedMinValue();
        maxHSV.val[0] = hBar.getSelectedMaxValue();
        maxHSV.val[1] = sBar.getSelectedMaxValue();
        maxHSV.val[2] = vBar.getSelectedMaxValue();
    }
}
