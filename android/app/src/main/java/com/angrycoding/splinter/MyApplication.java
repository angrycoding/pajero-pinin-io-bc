package com.angrycoding.splinter;

import android.app.Application;
import android.content.Context;

public class MyApplication extends Application {

    private static Context context;

    public void onCreate() {
        super.onCreate();
        context = getApplicationContext();
    }

    public static Context getAppContext() {
        return MyApplication.context;
    }

}
