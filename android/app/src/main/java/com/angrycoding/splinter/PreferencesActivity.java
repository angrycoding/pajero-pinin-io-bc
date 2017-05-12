package com.angrycoding.splinter;

import android.app.Activity;
import android.os.Bundle;

import com.angrycoding.splinter.webview.WebView;

public class PreferencesActivity extends Activity {

    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        webView = new WebView(this, null);
        webView.loadUrl("file:///android_asset/preferences.html");
        this.setContentView(webView);
    }
}
