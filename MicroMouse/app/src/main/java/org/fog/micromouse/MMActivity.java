package org.fog.micromouse;

import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

public class MMActivity extends Activity {
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
                c = CalibrationActivity.class;
                break;
            case R.id.show_automatic:
                c = AutomaticActivity.class;
                break;
            case R.id.show_manual:
                c = ManualActivity.class;
                break;
        }
        if (c == null)
            return super.onOptionsItemSelected(item);

        startActivity(new Intent(this, c));
        return true;
    }
}
