package com.angrycoding.splinter.arduino;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.widget.Toast;

import com.angrycoding.splinter.MyApplication;
import com.felhr.usbserial.UsbSerialDevice;
import com.felhr.usbserial.UsbSerialInterface;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

public class Arduino {

    private static Context context = MyApplication.getAppContext();
    private static UsbManager usbManager = (UsbManager)context.getSystemService(Context.USB_SERVICE);

    private static UsbDevice device;
    private static UsbDeviceConnection connection;
    private static UsbSerialDevice serialPort;
    private static String ACTION_USB_PERMISSION = "com.hariharan.arduinousb.USB_PERMISSION";

    private static LinkedList<ArduinoListener> listeners = new LinkedList<ArduinoListener>();


    private static void updateState() {
        if (listeners.size() > 0) {
            start();
        } else {
            stop();
        }
    }

    public static void addListener(ArduinoListener listener) {
        listeners.push(listener);
        updateState();
    }

    public static void removeListener(ArduinoListener listener) {
        listeners.remove(listener);
        updateState();
    }

    /*
    UsbSerialInterface.UsbReadCallback mCallback = new UsbSerialInterface.UsbReadCallback() {
        @Override
        public void onReceivedData(byte[] arg0) {
            try {
                final String data = new String(arg0, "UTF-8");
                Intent x = new Intent("ON_DATA");
                x.putExtra("data", data);
                LocalBroadcastManager.getInstance(context).sendBroadcast(x);

//                Toast.makeText(Arduino.this.context, data, Toast.LENGTH_LONG).show();
//                runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//
////                   statusBar.evaluateJavascript("window.deviceAttached('" + data + "')", null);
//
//
//                    }
//                });
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }


        }
    };
    */

    private static BroadcastReceiver usbReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            if (intent.getAction().equals(ACTION_USB_PERMISSION)) {
                if (intent.getExtras().getBoolean(UsbManager.EXTRA_PERMISSION_GRANTED)) {
                    connection = usbManager.openDevice(device);
                    serialPort = UsbSerialDevice.createUsbSerialDevice(device, connection);
                    if (serialPort != null) {

                        if (serialPort.open()) {
                            Toast.makeText(context, "OPEN!!!!", Toast.LENGTH_LONG).show();
                            serialPort.setBaudRate(9600);
                            serialPort.setDataBits(UsbSerialInterface.DATA_BITS_8);
                            serialPort.setStopBits(UsbSerialInterface.STOP_BITS_1);
                            serialPort.setParity(UsbSerialInterface.PARITY_NONE);
                            serialPort.setFlowControl(UsbSerialInterface.FLOW_CONTROL_OFF);
//                            serialPort.read(mCallback);
                        }

                        else {
                            Toast.makeText(context, "PORT_NOT_OPEN", Toast.LENGTH_LONG).show();
                        }

                    }

                    else {
                        Toast.makeText(context, "PORT_IS_NULL", Toast.LENGTH_LONG).show();
                    }

                }

                else {
                    Toast.makeText(context, "PERM_NOT_GRANTED", Toast.LENGTH_LONG).show();
                }

            }

            else if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)) {
                device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                requestUserPermission();
            }

            else if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_DETACHED)) {
                serialPort.close();
                connection.close();
            }

        }
    };

    private static void requestUserPermission() {
        PendingIntent pi = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), 0);
        usbManager.requestPermission(device, pi);
    }

    private static void start() {

        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_PERMISSION);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);

        context.registerReceiver(usbReceiver, filter);
        HashMap<String, UsbDevice> usbDevices = usbManager.getDeviceList();
        if (!usbDevices.isEmpty()) for (Map.Entry<String, UsbDevice> entry : usbDevices.entrySet()) {
            device = entry.getValue();
            int deviceVID = device.getVendorId();
            int devicePID = device.getProductId();
            if (deviceVID == 1027 && devicePID == 24577) {
                requestUserPermission();
                break;
            }
        }
    }

    private static void stop() {
        try { context.unregisterReceiver(usbReceiver); }
        catch (Exception e) { }
    }

}
