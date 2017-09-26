package com.example.jobush50.test2;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;
import java.net.SocketException;

/**
 * @author Rick
 * @author Lamar
 * @author Jonathan
 * @author Hung
 */

public class SecondActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);

        Intent intent = getIntent();
        final String ip = intent.getStringExtra("ip");
        final int port = intent.getIntExtra("port", 0);

        //Load the camera steam in the webview.
        WebView webView = (WebView) findViewById(R.id.camera);
        webView.loadUrl("http://" + ip + ":8080/?action=stream");
        //Resizes the content correctly without scrollbars.
        webView.getSettings().setLoadWithOverviewMode(true);
        webView.getSettings().setUseWideViewPort(true);


        //Show the address and port that the app is connected to.
        TextView textHost = (TextView) findViewById(R.id.textViewHost);
        textHost.setText("Host: " + ip + ":" + port);

        //Start the thread that listens for incoming data.
        final SocketIn socketThread = new SocketIn();
        socketThread.start();

        final RelativeLayout layout_joystick = (RelativeLayout) findViewById(R.id.layout_joystick);

        final JoyStickClass js = new JoyStickClass(getApplicationContext(), layout_joystick, R.drawable.image_button);
        js.setStickSize(150, 150);
        js.setLayoutSize(800, 800);
        js.setLayoutAlpha(150);
        js.setStickAlpha(100);
        js.setOffset(90);
        js.setMinimumDistance(50);

        layout_joystick.setOnTouchListener(new View.OnTouchListener() {
            long lastDataTimestamp = System.currentTimeMillis();

            public boolean onTouch(View arg0, MotionEvent arg1) {
                js.drawStick(arg1);

                //If-statement is used to limit the number of packages to 1 per 50 miliseconds.
                if (System.currentTimeMillis() - lastDataTimestamp > 50) {
                    //Get the x and y coordinates of the joystick and scale them.
                    int x = js.getX();
                    int y = js.getY();
                    x /= 2;
                    y /= 2;

                    //x and y range from -200..200. Adding 400 makes sure the numbers are always positive.
                    //Both numbers are combined in a long to make data transmission easier.
                    x += 400;
                    y += 400;
                    long data = x << 10 | y;
                    String message = Long.toString(data);

                    //Send a command to the robot.
                    try {
                        SocketOut socketOut = new SocketOut(ip, port);
                        socketOut.sendPacket(message);
                    } catch (SocketException e) {
                        e.printStackTrace();
                    }
                    lastDataTimestamp = System.currentTimeMillis();

                    //Check for new incoming data from the distance sensor and display it.
                    String received = socketThread.getMostRecentData();
                    if (received != null && !received.isEmpty()) {
                        TextView textDist = (TextView) findViewById(R.id.textViewDist);
                        textDist.setText("Distance: " + received + " cm");
                    }
                }
                return true;
            }
        });
    }
}