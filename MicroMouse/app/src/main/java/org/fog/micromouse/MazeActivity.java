package org.fog.micromouse;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTransaction;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.WindowManager;

import org.fog.micromouse.body.Brain;
import org.fog.micromouse.body.Wheels;
import org.fog.micromouse.vision.CalibrateColor;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Scalar;

import java.util.Set;

public class MazeActivity extends FragmentActivity {
    private static final String TAG = "MicroMouse:Automatic";
    private boolean disableBluetoothWhenStopped = false;
    public CameraBridgeViewBase mOpenCvCameraView;
    public Scalar minHSV;
    public Scalar maxHSV;
    public Brain brain;
    private Wheels wheels;
    private int mode = R.id.fully_automatic;

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

        if (b == null) {
            minHSV = new Scalar(0, 0, 0);
            maxHSV = new Scalar(180, 255, 255);
        } else {
            minHSV = new Scalar(b.getInt("minH", 0), b.getInt("minS", 0), b.getInt("minV", 0));
            maxHSV = new Scalar(b.getInt("maxH", 180), b.getInt("maxS", 255), b.getInt("maxV", 255));
        }

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.camera_preview);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(new CalibrateColor(minHSV, maxHSV));

        AutomaticFragment fragment = new AutomaticFragment();
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

        if (mode == R.id.show_calibration || id == R.id.show_calibration) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            if (id == R.id.show_calibration)
                transaction.replace(R.id.fragment_container, new CalibrationFragment());
            else
                transaction.replace(R.id.fragment_container, new AutomaticFragment());
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
    }

    @Override
    public void onResume() {
        super.onResume();
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
        wheels.close();
    }

    @Override
    public void onSaveInstanceState(Bundle b) {
        super.onSaveInstanceState(b);
        b.putInt("minH", (int) minHSV.val[0]);
        b.putInt("minS", (int) minHSV.val[1]);
        b.putInt("minV", (int) minHSV.val[2]);
        b.putInt("maxH", (int) maxHSV.val[0]);
        b.putInt("maxS", (int) maxHSV.val[1]);
        b.putInt("maxV", (int) maxHSV.val[2]);
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }
}