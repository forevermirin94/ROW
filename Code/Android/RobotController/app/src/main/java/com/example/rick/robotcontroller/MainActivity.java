package com.example.rick.robotcontroller;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.webkit.WebView;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {
    String host = "172.24.1.1"; //Default 172.24.1.1
    int port = 8888;            //Default 8888
    Long lastMessageTimestamp = System.currentTimeMillis();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        WebView myWebView = (WebView) findViewById(R.id.camera);
        myWebView.loadUrl("http://" + host + ":8080/?action=stream");
        myWebView.getSettings().setLoadWithOverviewMode(true);
        myWebView.getSettings().setUseWideViewPort(true);

        Button buttonForward = (Button) findViewById(R.id.buttonFoward);
        buttonForward.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (System.currentTimeMillis() - lastMessageTimestamp > 10) {
                    sendData("F");
                }
                lastMessageTimestamp = System.currentTimeMillis();
                return false;
            }
        });

        Button buttonBackward = (Button) findViewById(R.id.buttonBackward);
        buttonBackward.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (System.currentTimeMillis() - lastMessageTimestamp > 50) {
                    sendData("B");
                }
                lastMessageTimestamp = System.currentTimeMillis();
                return false;
            }
        });

        Button buttonLeft = (Button) findViewById(R.id.buttonLeft);
        buttonLeft.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (System.currentTimeMillis() - lastMessageTimestamp > 50) {
                    sendData("L");
                }
                lastMessageTimestamp = System.currentTimeMillis();
                return false;
            }
        });

        Button buttonRight = (Button) findViewById(R.id.buttonRight);
        buttonRight.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (System.currentTimeMillis() - lastMessageTimestamp > 50) {
                    sendData("R");
                }
                lastMessageTimestamp = System.currentTimeMillis();
                return false;
            }
        });
    }

   /* public void moveForward(View view) {
        //sendData("F");
        System.out.println("Touched");
    }

    public void moveBackward(View view) {
        sendData("B");
    }

    public void moveLeft(View view) {
        sendData("L");
    }

    public void moveRight(View view) {
        sendData("R");
    }*/

    private String sendData(String message) {
        try {
            return Socket.getInstance().sendPacket(message, host, port);

        } catch (Exception e) {
            e.printStackTrace();
            return "err";
        }
    }
}
