package com.angrycoding.splinter.webview;

import android.content.Context;
import android.view.View;
import android.webkit.WebSettings;

public class WebView extends android.webkit.WebView {

    public WebView(Context context, WebViewCallback callback) {
        super(context);
        WebSettings settings = getSettings();
        setLayerType(View.LAYER_TYPE_HARDWARE, null);
        settings.setJavaScriptEnabled(true);
        settings.setAllowUniversalAccessFromFileURLs(true);
        settings.setAllowContentAccess(true);
        settings.setAllowFileAccessFromFileURLs(true);
        settings.setAllowFileAccess(true);
        settings.setStandardFontFamily("Roboto-Light");
        addJavascriptInterface(new PreferencesInterface(context), "preferences");
        setWebViewClient(new WebViewClient(context, callback));
    }

    public void triggerEvent(String event) {
        this.evaluateJavascript("document.dispatchEvent(new Event('" + event + "'));", null);
    }

}
