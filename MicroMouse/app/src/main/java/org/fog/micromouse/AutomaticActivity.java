package org.fog.micromouse;

import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;


public class AutomaticActivity extends MMActivity {
    private static final String TAG = "MicroMouse:Automatic";
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.automatic_layout);
    }}
