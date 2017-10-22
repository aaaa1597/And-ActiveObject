package com.test.cppsurfaceviewtrans;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.RelativeLayout;

public class MainActivity extends Activity {
    int mSurfaceid = -1;
    Handler mHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SurfaceView surface = ((SurfaceView)findViewById(R.id.surface));

        /* 透過設定 */
        surface.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        surface.setZOrderOnTop(true);

        /* コールバック設定 */
        surface.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                /* C++ */
                mSurfaceid = NativeFunc.surfaceCreated(holder.getSurface());
                return;
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                NativeFunc.surfaceChanged(mSurfaceid, width, height);
                mHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        NativeFunc.draw(mSurfaceid);
                        mHandler.postDelayed(this, 10);
                    }
                }, 10);

//TODO タイマーイベント完了時の処理
//                mHandler.removeCallbacksAndMessages(null);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                NativeFunc.surfaceDestroyed(mSurfaceid);
            }
        });

    }
}
