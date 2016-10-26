package org.fog.manualmouse;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class CirclePadFragment extends Fragment {

    private View view;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.circle_layout, container, false);
        return view;
    }

    public void setComs(MouseComs coms) {
        if (view != null)
            ((CirclePad) view.findViewById(R.id.circle_pad)).setComs(coms);
    }
}

