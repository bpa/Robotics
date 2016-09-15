package org.fog.manualmouse;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class MouseComs implements Runnable {
    private static final String TAG = "ManualMouse";
    private static final UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private static final byte SYNC_ERROR    = 0b00000001;
    private static final byte LEFT_MOTOR    = 0b00000010;
    private static final byte RIGHT_MOTOR   = 0b00000100;
    private static final byte MOVE_FORWARD  = 0b00001000;
    private static final byte MOVE_BACKWARD = 0b00010000;
    private static final byte STOP          = 0b00100000;
    private static final byte CONFIGURE     = 0b01000000;

    private static final byte[] STOP_CMD = new byte[] { STOP | RIGHT_MOTOR | LEFT_MOTOR, SYNC_ERROR };
    private byte[] cmd = new byte[4];

    private BluetoothDevice device;
    private BluetoothSocket socket;
    private OutputStream outputStream;
    private int maxSpeed = 255;
    private int left_motor, right_motor;
    private boolean left_reversed = false, right_reversed = false;

    public MouseComs(BluetoothDevice device) {
        this.device = device;
    }

    /**
     * Set motor speed as fraction of max speed
     * @param left -1 to 1
     * @param right -1 to 1
     */
    public void setSpeed(double left, double right) {
        if (left_reversed)
            left *= -1;
        if (right_reversed)
            right *= -1;

        byte lSpeed = (byte) (Math.abs(left) * maxSpeed);
        byte rSpeed = (byte) (Math.abs(right) * maxSpeed);

        if (left < 0)
            cmd[0] = LEFT_MOTOR | MOVE_BACKWARD;
        else
            cmd[0] = LEFT_MOTOR | MOVE_FORWARD;
        cmd[1] = (byte) (lSpeed | SYNC_ERROR);

        if (right < 0)
            cmd[2] = RIGHT_MOTOR | MOVE_BACKWARD;
        else
            cmd[2] = RIGHT_MOTOR | MOVE_FORWARD;
        cmd[3] = (byte) (rSpeed | SYNC_ERROR);

        try {
            outputStream.write(cmd);
        } catch (IOException e) {
            Log.e(TAG, "Error writing to socket");
        }
     }

    public void stop() {
        try {
            outputStream.write(STOP_CMD);
        } catch (IOException e) {
            Log.e(TAG, "Error writing to socket");
        }
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
                configureMotors();
            } catch (IOException streamError) {
                Log.e(TAG, "Error when getting input or output Stream");
                return;
            }

            int read;
            byte[] buffer = new byte[1024];

            while (true) {
                try {
                    read = inputStream.read(buffer);
                    Log.i("Mouse", new String(buffer, 0, read));
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

    public void configureMotors(int left_motor, boolean left_reversed, int right_motor, boolean right_reversed) {
        this.left_motor = left_motor + 1;
        this.left_reversed = left_reversed;
        this.right_motor = right_motor + 1;
        this.right_reversed = right_reversed;

        if (outputStream != null) {
            configureMotors();
        }
    }

    private void configureMotors() {
        //Have to shift left because the first bit in the payload is reserved
        cmd[0] = CONFIGURE | LEFT_MOTOR;
        cmd[1] = (byte)((left_motor << 1) | SYNC_ERROR);
        cmd[2] = CONFIGURE | RIGHT_MOTOR;
        cmd[3] = (byte)((right_motor << 1) | SYNC_ERROR);

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

    /**
     * Max maxSpeed to move
     *
     * @param maxSpeed 1 - 255
     */
    public void setMaxSpeed(int maxSpeed) {
        if (maxSpeed > 0 && maxSpeed < 256)
            this.maxSpeed = maxSpeed;
    }
}


