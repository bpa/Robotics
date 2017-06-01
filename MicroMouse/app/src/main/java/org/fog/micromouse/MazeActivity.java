package org.fog.micromouse;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTransaction;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.TextView;

import org.fog.micromouse.body.Brain;
import org.fog.micromouse.body.Wheels;
import org.fog.micromouse.vision.CalibrateColor;
import org.fog.micromouse.vision.DetectWalls;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Scalar;

import java.util.Arrays;
import java.util.Set;

public class MazeActivity extends FragmentActivity implements SensorEventListener {
    private static final String TAG = "MicroMouse:Automatic";
    private static final double GRAVITY = 9.80665;
    private boolean disableBluetoothWhenStopped = false;
    public CameraBridgeViewBase mOpenCvCameraView;
    private SensorManager sensorManager;
    private Sensor sensor;
    public Scalar minHSV;
    public Scalar maxHSV;
    public Brain brain;
    private Wheels wheels;
    private int mode = R.id.fully_automatic;
    private CalibrateColor calibrateColor;
    private DetectWalls detectWalls;

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvCameraView.enableView();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    @Override
    public void onCreate(Bundle b) {
        super.onCreate(b);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.camera_layout);

        SharedPreferences p = getPreferences(Context.MODE_PRIVATE);
        minHSV = new Scalar(p.getInt("minH", 0), p.getInt("minS", 0), p.getInt("minV", 0));
        maxHSV = new Scalar(p.getInt("maxH", 180), p.getInt("maxS", 255), p.getInt("maxV", 255));

        calibrateColor = new CalibrateColor(minHSV, maxHSV);
        detectWalls = new DetectWalls(minHSV, maxHSV, (TextView) findViewById(R.id.distance));

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.camera_preview);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(detectWalls);

        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        sensor = sensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);

        Fragment fragment = new AutomaticFragment();
        getSupportFragmentManager().beginTransaction()
                .add(R.id.fragment_container, fragment).commit();
    }

    @Override
    public void onStart() {
        super.onStart();
        BluetoothAdapter bluetooth = BluetoothAdapter.getDefaultAdapter();
        disableBluetoothWhenStopped = false;
        if (!bluetooth.isEnabled()) {
            bluetooth.enable();
            disableBluetoothWhenStopped = true;
        }

        Set<BluetoothDevice> pairedDevices = bluetooth.getBondedDevices();
        for (BluetoothDevice device : pairedDevices) {
            if ("Henry".equals(device.getName())) {
                wheels = new Wheels(device);
                new Thread(wheels).start();
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == mode)
            return true;

        savePreferences();
        if (mode == R.id.show_calibration || id == R.id.show_calibration) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            if (id == R.id.show_calibration) {
                transaction.replace(R.id.fragment_container, new CalibrationFragment());
                mOpenCvCameraView.setCvCameraViewListener(calibrateColor);
            }
            else {
                transaction.replace(R.id.fragment_container, new AutomaticFragment());
                mOpenCvCameraView.setCvCameraViewListener(detectWalls);
            }
            transaction.commit();
        }
        switch (id) {
            case R.id.show_calibration:
                break;
            case R.id.fully_automatic:
                break;
            case R.id.random_path:
                break;
            default:
                return false;
        }
        mode = id;
        return false;
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
        sensorManager.unregisterListener(this);
        calibrateColor.onCameraViewStopped();
        detectWalls.onCameraViewStopped();
    }

    @Override
    public void onResume() {
        super.onResume();
        enableCamera();
        sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_NORMAL);
    }

    private void enableCamera() {
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        if (disableBluetoothWhenStopped) {
            BluetoothAdapter.getDefaultAdapter().disable();
        }
        if (wheels != null)
            wheels.close();
    }

    public void savePreferences() {
        SharedPreferences p = getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor b = p.edit();
        b.putInt("minH", (int) minHSV.val[0]);
        b.putInt("minS", (int) minHSV.val[1]);
        b.putInt("minV", (int) minHSV.val[2]);
        b.putInt("maxH", (int) maxHSV.val[0]);
        b.putInt("maxS", (int) maxHSV.val[1]);
        b.putInt("maxV", (int) maxHSV.val[2]);
        b.apply();
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
         detectWalls.angle_x = Math.asin(event.values[0]/GRAVITY);
         detectWalls.angle_y = Math.asin(event.values[1]/GRAVITY);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        Log.i(TAG, "AccuracyChanged:" + accuracy);
    }
}