package com.angrycoding.splinter;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;

import com.angrycoding.splinter.webview.WebView;

public class SensorSettings extends Activity {

    private String area;
    private WebView webView;

    private void startEdit(Intent intent) {
        area = intent.getStringExtra("area");
        webView.loadUrl("file:///android_asset/sensorSettings.html#" + intent.getStringExtra("type"));
    }

    private void stopEdit() {
        Intent foo = new Intent(area + ".AA");
        LocalBroadcastManager.getInstance(this).sendBroadcast(foo);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        webView = new WebView(this, null);
        startEdit(getIntent());
        this.setContentView(webView);


    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        stopEdit();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        startEdit(intent);
    }

    @Override
    protected void onUserLeaveHint() {
        super.onUserLeaveHint();
        stopEdit();
    }
}
