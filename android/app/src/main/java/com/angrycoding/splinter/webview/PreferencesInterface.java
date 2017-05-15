package com.angrycoding.splinter.webview;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.preference.PreferenceManager;
import android.util.Log;
import android.webkit.JavascriptInterface;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;

public class PreferencesInterface {

    SharedPreferences preferences;
    Context context;

    public PreferencesInterface(Context context) {
        this.context = context;
        preferences = PreferenceManager.getDefaultSharedPreferences(context);
    }

    @JavascriptInterface
    public String getSensors() throws JSONException {
        JSONArray result = new JSONArray();

        JSONObject x = new JSONObject();
        x.put("type", "1");
        x.put("label", "Внешняя температура");
        result.put(x);

        x = new JSONObject();
        x.put("type", "2");
        x.put("label", "Запас топлива в литрах");
        result.put(x);

        x = new JSONObject();
        x.put("type", "3");
        x.put("label", "Средний расход");
        result.put(x);

        x = new JSONObject();
        x.put("type", "4");
        x.put("label", "Средняя скорость");
        result.put(x);

        x = new JSONObject();
        x.put("type", "5");
        x.put("label", "Температура ОЖ");
        result.put(x);

        x = new JSONObject();
        x.put("type", "6");
        x.put("label", "Пробег за поездку");
        result.put(x);

        x = new JSONObject();
        x.put("type", "7");
        x.put("label", "Пробег с замены масла");
        result.put(x);

        x = new JSONObject();
        x.put("type", "0");
        x.put("label", "Средняя скорость");
        result.put(x);



        return result.toString();
    }



    @JavascriptInterface
    public boolean getBoolean(String key) {
        return preferences.getBoolean(key, false);
    }

    @JavascriptInterface
    public void setBoolean(String key, boolean value) {
        SharedPreferences.Editor preferences = this.preferences.edit();
        preferences.putBoolean(key, value);
        preferences.commit();
    }


    @JavascriptInterface
    public void runApplication(String pkgName) {
        Intent LaunchIntent = this.context.getPackageManager().getLaunchIntentForPackage(pkgName);
        this.context.startActivity( LaunchIntent );

    }


    @JavascriptInterface
    public String getApplications() {
        final PackageManager pm = this.context.getPackageManager();
        //get a list of installed apps.
//        List<ApplicationInfo> packages = pm.getInstalledApplications(PackageManager.GET_META_DATA);


        Intent mainIntent = new Intent(Intent.ACTION_MAIN, null);
        mainIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        List<ResolveInfo> pkgAppsList = context.getPackageManager().queryIntentActivities( mainIntent, 0);

        String out = "[";

//        out += "{\"label\": \"BLABLA\", \"name\": \"com.angrycoding.carlauncher\", \"icon\": \"icon:///com.angrycoding.carlauncher\"},";

        for (int c = 0; c < pkgAppsList.size(); c++) try {
            String pkgName = pkgAppsList.get(c).activityInfo.packageName;
            Log.v("V", pkgName);
            String pkgIcon = "icon:///" + pkgName;
            String label = pkgAppsList.get(c).activityInfo.loadLabel(pm).toString();
//            String pkgIcon = encodeIcon(pm.getApplicationIcon(pkgName));
            out += "{\"label\": \"" + label + "\", \"name\": \"" + pkgName + "\", \"icon\": \"" + pkgIcon + "\"},";
        }

        catch (Exception e) {
            e.printStackTrace();
        }




        out = out.substring(0, out.length() - 1);
        out += "]";

        Log.v("V", out);
        return out;


    }

}
