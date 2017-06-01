package org.fog.micromouse.body;

import java.util.ArrayList;
import java.util.List;

public class Brain {
    private static List<Action> actions = new ArrayList<Action>();
    private static int x;
    private static int y;
    private static double r;
    private static Action currentAction;
    //accumulator = (alpha * new_value) + (1.0 - alpha) * accumulator

    public static void setGoal(Action action) {
        actions.clear();
        actions.add(action);
    }
}
