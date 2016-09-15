package org.fog.manualmouse;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;

import static android.widget.AdapterView.OnItemSelectedListener;
import static android.widget.SeekBar.OnSeekBarChangeListener;
import static android.widget.CompoundButton.OnCheckedChangeListener;

public class ConfigureFragment extends Fragment implements OnItemSelectedListener, OnSeekBarChangeListener, OnCheckedChangeListener {
    public int left_motor, right_motor;
    public boolean left_reversed, right_reversed;
    public int maxSpeed;

    public void loadPreferences(SharedPreferences p) {
        left_motor = p.getInt("left_motor", 0);
        left_reversed = p.getBoolean("left_reversed", false);
        right_motor = p.getInt("right_motor", 1);
        right_reversed = p.getBoolean("right_reversed", false);
        maxSpeed = p.getInt("maxSpeed", 255);
    }

    public void savePreferences(SharedPreferences p) {
        SharedPreferences.Editor b = p.edit();
        b.putInt("left_motor", left_motor);
        b.putBoolean("left_reversed", left_reversed);
        b.putInt("right_motor", right_motor);
        b.putBoolean("right_reversed", right_reversed);
        b.putInt("maxSpeed", maxSpeed);
        b.apply();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.configure_layout, container, false);

        Spinner spinner = (Spinner)view.findViewById(R.id.left_motor);
        spinner.setOnItemSelectedListener(this);
        spinner.setSelection(left_motor);

        spinner = (Spinner)view.findViewById(R.id.right_motor);
        spinner.setOnItemSelectedListener(this);
        spinner.setSelection(right_motor);

        Switch reversed = (Switch)view.findViewById(R.id.left_motor_backwards);
        reversed.setOnCheckedChangeListener(this);
        reversed.setChecked(left_reversed);

        reversed = (Switch)view.findViewById(R.id.right_motor_backwards);
        reversed.setOnCheckedChangeListener(this);
        reversed.setChecked(right_reversed);

        SeekBar seekBar = (SeekBar)view.findViewById(R.id.max_speed);
        seekBar.setOnSeekBarChangeListener(this);
        seekBar.setProgress(maxSpeed);

        return view;
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        if (parent.getId() == R.id.left_motor) {
            left_motor = parent.getSelectedItemPosition();
        }
        else if (parent.getId() == R.id.right_motor) {
            right_motor = parent.getSelectedItemPosition();
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        maxSpeed = seekBar.getProgress();
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        maxSpeed = progress;
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (buttonView.getId() == R.id.left_motor_backwards) {
            left_reversed = isChecked;
        }
        else if (buttonView.getId() == R.id.right_motor_backwards) {
            right_reversed = isChecked;
        }
    }
}