package org.fog.manualmouse;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class CirclePadFragment extends Fragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.circle_layout, container, false);
    }

    public void setComs(MouseComs coms) {
        View view = getView();
        ((CirclePad) view.findViewById(R.id.circle_pad)).setComs(coms);
    }
}

