package org.fog.manualmouse;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTransaction;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import java.util.Set;

public class MainActivity extends FragmentActivity {
    private MouseComs coms;
    private Thread comThread;
    private ConfigureFragment configuration;
    private CirclePadFragment circlePad;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        configuration = new ConfigureFragment();
        configuration.loadPreferences(getPreferences(Context.MODE_PRIVATE));

        circlePad = new CirclePadFragment();
        getSupportFragmentManager().beginTransaction()
                .add(R.id.fragment_container, circlePad).commit();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
        switch (item.getItemId()) {
            case R.id.show_configuration:
                transaction.replace(R.id.fragment_container, configuration);
                break;
            case R.id.show_controls:
                transaction.replace(R.id.fragment_container, circlePad);
                configuration.savePreferences(getPreferences(Context.MODE_PRIVATE));
                circlePad.setComs(coms);
                coms.configureMotors(
                        configuration.left_motor, configuration.left_reversed,
                        configuration.right_motor, configuration.right_reversed);
                break;
            case R.id.reconnect:
                reconnect();
        }
        transaction.commit();
        return false;
    }

    @Override
    public void onStart() {
        super.onStart();
        reconnect();
    }

    private void reconnect() {
        BluetoothAdapter bluetooth = BluetoothAdapter.getDefaultAdapter();
        if (!bluetooth.isEnabled()) {
            bluetooth.enable();
        }

        if (coms == null) {
            Set<BluetoothDevice> pairedDevices = bluetooth.getBondedDevices();
            for (BluetoothDevice device : pairedDevices) {
                if ("Henry".equals(device.getName())) {
                    coms = new MouseComs(device);
                    comThread = new Thread(coms);
                    comThread.start();
                    circlePad.setComs(coms);
                    coms.configureMotors(
                            configuration.left_motor, configuration.left_reversed,
                            configuration.right_motor, configuration.right_reversed);
                }
            }
        }
        else if (comThread.getState() == Thread.State.TERMINATED){
            comThread = new Thread(coms);
            comThread.start();
        }
    }

    @Override
    public void onStop() {
        super.onStop();
            BluetoothAdapter.getDefaultAdapter().disable();
        if (coms != null)
            coms.close();
    }
}
