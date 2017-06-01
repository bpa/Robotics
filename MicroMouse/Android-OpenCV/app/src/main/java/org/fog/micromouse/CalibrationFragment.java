package org.fog.micromouse;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import org.florescu.android.rangeseekbar.RangeSeekBar;
import org.opencv.core.Scalar;

public class CalibrationFragment extends Fragment implements RangeSeekBar.OnRangeSeekBarChangeListener<Integer> {
    private static final String TAG = "MicroMouse::Calibration";

    private Scalar minHSV;
    private Scalar maxHSV;
    private RangeSeekBar<Integer> hBar;
    private RangeSeekBar<Integer> sBar;
    private RangeSeekBar<Integer> vBar;

    @Override
    public void onCreate(Bundle b) {
        super.onCreate(b);

        MazeActivity camera = (MazeActivity) getActivity();
        minHSV = camera.minHSV;
        maxHSV = camera.maxHSV;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.calibration_layout, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        hBar = initRangeBar(minHSV, maxHSV, 0, R.id.h_range, view);
        sBar = initRangeBar(minHSV, maxHSV, 1, R.id.s_range, view);
        vBar = initRangeBar(minHSV, maxHSV, 2, R.id.v_range, view);
    }

    private RangeSeekBar<Integer> initRangeBar(Scalar min, Scalar max, int ind, int viewId, View view) {
        RangeSeekBar<Integer> rsb = (RangeSeekBar<Integer>) getView().findViewById(viewId);
        rsb.setSelectedMinValue((int)min.val[ind]);
        rsb.setSelectedMaxValue((int)max.val[ind]);
        rsb.setOnRangeSeekBarChangeListener(this);
        return rsb;
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
