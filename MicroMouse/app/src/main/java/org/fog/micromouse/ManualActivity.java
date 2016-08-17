package org.fog.micromouse;

import android.os.Bundle;
import android.util.Log;

public class ManualActivity extends MMActivity {
    private static final String TAG = "MicroMouse:Manual";
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.manual_layout);
    }}
