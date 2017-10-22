package com.test.cppsurfaceviewtrans;

import android.view.Surface;

public class NativeFunc {
    static { System.loadLibrary("testlib"); }

    public native static int surfaceCreated(Surface surface);
    public native static void surfaceChanged(int id, int width, int height);
    public native static void draw(int id);
    public native static void surfaceDestroyed(int id);
}
