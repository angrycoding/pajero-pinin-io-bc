package com.angrycoding.splinter.webview;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.util.DisplayMetrics;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebView;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;

public class WebViewClient extends android.webkit.WebViewClient {

    private PackageManager pm;
    private WebViewCallback callback;

    public WebViewClient(Context context, WebViewCallback callback) {
        this.callback = callback;
        pm = context.getPackageManager();
    }

    @Override
    public void onPageFinished(WebView view, String url) {
        super.onPageFinished(view, url);
        if (callback != null) {
            callback.onPageFinished();
        }
    }

    @Override
    public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {

          if (request.getUrl().getScheme().equals("icon")) try {

              ApplicationInfo item = pm.getApplicationInfo(request.getUrl().getPath().substring(1), PackageManager.GET_META_DATA);


              try {
                  Resources resources = pm.getResourcesForApplication(item);
                  Resources resourcesForApplication = pm.getResourcesForApplication(item);
                  Drawable icon = resourcesForApplication.getDrawableForDensity(item.icon, DisplayMetrics.DENSITY_XXXHIGH);

                  BitmapDrawable bitmapDrawable = ((BitmapDrawable)icon);
                  Bitmap bitmap = bitmapDrawable.getBitmap();
                  ByteArrayOutputStream stream = new ByteArrayOutputStream();
                  bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream);
                  InputStream is = new ByteArrayInputStream(stream.toByteArray());
                  return new WebResourceResponse("image/png", "UTF-8", is);

              } catch (Exception e) {
              }


          }

          catch (Exception e) {

          }

        return super.shouldInterceptRequest(view, request);
    }

}
