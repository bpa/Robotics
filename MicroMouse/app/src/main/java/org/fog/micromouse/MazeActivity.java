package org.fog.micromouse;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.WindowManager;

import org.fog.micromouse.body.Brain;
import org.fog.micromouse.body.Wheels;

import java.util.Set;

public class MazeActivity extends Activity {
    private static final String TAG = "MicroMouse:Automatic";
    private boolean disableBluetoothWhenStopped = false;
    public Brain brain;
    private Wheels wheels;
    public CameraFragment camera;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        camera = (CameraFragment) getFragmentManager().findFragmentById(R.id.camera_preview);
        setContentView(R.layout.automatic_layout);
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
        Class c = null;
        switch (item.getItemId()) {
            case R.id.show_calibration:
                c = CalibrationFragment.class;
                break;
            case R.id.show_automatic:
                c = MazeActivity.class;
                break;
            case R.id.show_manual:
                c = ManualFragment.class;
                break;
        }
        if (c == null)
            return super.onOptionsItemSelected(item);

        startActivity(new Intent(this, c));
        return true;
    }

    @Override
    public void onStop() {
        super.onStop();
        if (disableBluetoothWhenStopped) {
            BluetoothAdapter.getDefaultAdapter().disable();
        }
        wheels.close();
    }
}