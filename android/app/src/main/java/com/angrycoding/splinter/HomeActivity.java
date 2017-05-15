package com.angrycoding.splinter;

import android.app.Activity;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.view.Gravity;
import android.view.WindowManager;

import com.angrycoding.splinter.webview.WebView;


public class HomeActivity extends Activity {

    private WebView statusBarView;
    private byte STATUS_BAR_SIZE = 60;
    private WindowManager windowService;

    private  int getNavigationBarHeight() {
        Resources resources = getResources();
        int resourceId = resources.getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            return resources.getDimensionPixelSize(resourceId);
        }
        return 0;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        windowService = (WindowManager)getSystemService(WINDOW_SERVICE);
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.TYPE_SYSTEM_ALERT,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
                        WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL |
                        WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR |
                        WindowManager.LayoutParams.FLAG_LAYOUT_ATTACHED_IN_DECOR |
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN |
                        WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS |
                        WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH |
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_OVERSCAN
                , PixelFormat.TRANSLUCENT
        );

        params.gravity = Gravity.LEFT | Gravity.BOTTOM | Gravity.FILL_HORIZONTAL;
        params.y = -(STATUS_BAR_SIZE + getNavigationBarHeight());
        params.height = STATUS_BAR_SIZE;

        statusBarView = new WebView(this, null);
        statusBarView.setBackgroundColor(Color.TRANSPARENT);
        statusBarView.loadUrl("file:///android_asset/status.html");
        windowService.addView(statusBarView, params);
    }

}