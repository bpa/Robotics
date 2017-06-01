package org.fog.micromouse.body;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class Wheels implements Runnable {
    private static final String TAG = "MicroMouse:Wheels";
    private static final UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private BluetoothDevice device;
    private BluetoothSocket socket;
    private OutputStream outputStream;
    private byte speed;
    private byte[] cmd = new byte[2];

    public Wheels(BluetoothDevice device) {
        this.device = device;
    }

    @Override
    public void run() {
        InputStream inputStream;
        try {
            socket = this.device.createRfcommSocketToServiceRecord(uuid);
            socket.connect();
            try {
                inputStream = socket.getInputStream();
                outputStream = socket.getOutputStream();
            } catch (IOException streamError) {
                Log.e(TAG, "Error when getting input or output Stream");
                return;
            }

            byte[] buffer = new byte[1024];
            int bytes;

            while (true) {
                try {
                    bytes = inputStream.read(buffer);
                } catch (IOException e) {
                    Log.e(TAG, "Error reading from socket");
                    break;
                }
            }

        } catch (IOException e) {
            try {
                socket.close();
            } catch (IOException ex) {
            }
        }
    }

    /**
     * Max speed to move
     *
     * @param speed 1 - 255
     */
    public void setSpeed(int speed) {
        if (speed > 0 && speed < 256)
            this.speed = (byte) speed;
    }

    /**
     * Set the
     *
     * @param rotation
     */
    public void setRotation(int rotation) {
        cmd[0] = speed;
        cmd[1] = speed;
        try {
            outputStream.write(cmd);
        } catch (IOException e) {
            Log.e(TAG, "Error writing to socket");
        }
    }

    public void stop() {
        cmd[0] = 0;
        cmd[1] = 0;
        try {
            outputStream.write(cmd);
        } catch (IOException e) {
            Log.e(TAG, "Error writing to socket");
        }
    }

    public void close() {
        try {
            if (socket != null)
                socket.close();
        } catch (IOException e) {
        }
    }
}
