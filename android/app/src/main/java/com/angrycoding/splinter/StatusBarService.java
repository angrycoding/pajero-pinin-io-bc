package com.angrycoding.splinter;

import android.accessibilityservice.AccessibilityService;
import android.accessibilityservice.AccessibilityServiceInfo;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.view.accessibility.AccessibilityEvent;

import com.angrycoding.splinter.arduino.Arduino;
import com.angrycoding.splinter.arduino.ArduinoListener;
import com.angrycoding.splinter.webview.WebView;

import java.lang.reflect.Method;

public class StatusBarService extends AccessibilityService {

    private WebView statusBarView;
    private byte STATUS_BAR_SIZE = 120;
    private WindowManager windowService;
    private LocalBroadcastManager lbm;

    private ArduinoListener arduinoListener = new ArduinoListener() {
        @Override
        public void onReceive() {
        }
    };


    private void setOverscan(int a, int b, int c, int d) {
        try {
            Class<?> serviceManager = Class.forName("android.os.ServiceManager");
            IBinder serviceBinder = (IBinder)serviceManager.getMethod("getService", String.class).invoke(serviceManager, "window");
            Class<?> stub = Class.forName("android.view.IWindowManager$Stub");
            Object windowManagerService = stub.getMethod("asInterface", IBinder.class).invoke(stub, serviceBinder);
            Method foo = windowManagerService.getClass().getDeclaredMethod("setOverscan", int.class, int.class, int.class, int.class, int.class);
            foo.setAccessible(true);
            foo.invoke(windowManagerService, Display.DEFAULT_DISPLAY, a, b, c, d);
        }

        catch (Exception e) {
            e.printStackTrace();
        }
    }

    private  int getNavigationBarHeight() {
        Resources resources = getResources();
        int resourceId = resources.getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            return resources.getDimensionPixelSize(resourceId);
        }
        return 0;
    }

    @Override
    protected void onServiceConnected() {
        super.onServiceConnected();
        AccessibilityServiceInfo config = new AccessibilityServiceInfo();
        config.eventTypes = AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED;
        config.feedbackType = AccessibilityServiceInfo.FEEDBACK_GENERIC;
        config.flags = AccessibilityServiceInfo.FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;
        setServiceInfo(config);

        Arduino.addListener(arduinoListener);

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


        setOverscan(0, 0, 0, STATUS_BAR_SIZE);


        lbm = LocalBroadcastManager.getInstance(getApplicationContext());

        lbm.registerReceiver(new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                statusBarView.triggerEvent("DONE_EDIT");
            }
        }, new IntentFilter("statusBar.AA"));

    }

    @Override
    public void onAccessibilityEvent(AccessibilityEvent event) {
//        localBroadcastManager.sendBroadcast(new Intent("USB_ENABLED"));
    }

    private void doCleanup() {
        Arduino.removeListener(arduinoListener);
        statusBarView.setVisibility(View.INVISIBLE);
        setOverscan(0, 0, 0, 0);
    }

    @Override
    public void onInterrupt() {
        doCleanup();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        doCleanup();
    }

    @Override
    public boolean onUnbind(Intent intent) {
        doCleanup();
        return false;
    }

}