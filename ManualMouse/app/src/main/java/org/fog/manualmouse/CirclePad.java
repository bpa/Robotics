package org.fog.manualmouse;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;

public class CirclePad extends View implements SurfaceHolder.Callback {
    private static final float BORDER_WIDTH = 5f;
    private static final float TOUCH_RADIUS = 50f;
    private static final double PI_4 = Math.PI / 4;

    private SurfaceHolder surfaceHolder;
    private Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private Paint text = new Paint(Paint.ANTI_ALIAS_FLAG);
    private float x, y, radius;
    private float tx, ty;
    private MouseComs coms = null;
    double r, lp, rp;

    public CirclePad(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
        paint.setColor(Color.WHITE);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5);
        text.setColor(Color.WHITE);
        text.setTextSize(15);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        float xpad = (float) (getPaddingLeft() + getPaddingRight());
        float ypad = (float) (getPaddingTop() + getPaddingBottom());

        float ww = (float) w - xpad - BORDER_WIDTH;
        float hh = (float) h - ypad - BORDER_WIDTH;

        x = w / 2f;
        y = h / 2f;
        tx = x;
        ty = y;
        radius = Math.min(ww, hh) / 2f;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawCircle(x, y, radius, paint);
        canvas.drawCircle(tx, ty, TOUCH_RADIUS, paint);
        canvas.drawText(String.format("%.0f\u00b0 %3.0f %3.0f", Math.toDegrees(r), lp*100, rp*100), 2, 15, text);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_UP:
                tx = x;
                ty = y;
                if (coms != null)
                    coms.stop();
                invalidate();
                return true;
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                float nx = event.getX();
                float ny = event.getY();
                float dx = nx - x;
                float dy = ny - y;
                r = Math.atan(dy/dx);
                if (dx < 0) {
                    r += Math.PI;
                }
                float speed = (dx*dx + dy*dy) / (radius*radius);
                if (speed > 1) {
                    speed = 1f;
                    dx = (float) Math.cos(r) * radius;
                    dy = (float) Math.sin(r) * radius;
                    nx = dx + x;
                    ny = dy + y;
                }
                if (Math.abs(nx - tx) > 1 || Math.abs(ny - ty) > 1) {
                    lp = calc(dx > 0, dy, r, speed);
                    rp = calc(dx < 0, dy, r, speed);

                    if (coms != null) {
                        coms.setSpeed(lp, rp);
                    }
                    tx = nx;
                    ty = ny;
                    invalidate();
                }
        }
        return true;
    }

    public double calc(boolean fullSpeed, float dy, double r, float speed) {
        if (fullSpeed)
            return dy < 0 ? speed : -speed;

        if (dy < 0)
            return Math.sin((r - PI_4) * 2);
        else
            return Math.sin((r - PI_4) * 2) * -1;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        surfaceHolder = holder;
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        //
    }

    public void setComs(MouseComs coms) {
        this.coms = coms;
    }
}
