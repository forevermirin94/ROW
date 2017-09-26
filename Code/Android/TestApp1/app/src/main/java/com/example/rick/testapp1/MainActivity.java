package com.example.rick.testapp1;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

import java.net.SocketException;

public class MainActivity extends AppCompatActivity {
    private String host;
    private int port;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void onSendButtonClick(View view) {
        EditText inputbox = (EditText) findViewById(R.id.inputMessage);
        String inputtext = inputbox.getText().toString();
        showMessage(inputtext);
        inputbox.setText("");

        String response = sendMessage(inputtext);
        showMessage("Response: " + response);
    }

    public void onConnectButtonClick (View view) {
        EditText ipbox = (EditText) findViewById(R.id.inputIP);
        EditText portbox = (EditText) findViewById(R.id.inputPort);

        host = ipbox.getText().toString();
        port = Integer.parseInt(portbox.getText().toString());
    }

    private void showMessage(String message) {
        TextView outputbox = (TextView) findViewById(R.id.output);
        outputbox.append(message + "\n");

        ScrollView scrollview = (ScrollView) findViewById(R.id.scrollView);
        scrollview.fullScroll(View.FOCUS_DOWN);
    }

    private String sendMessage(String message) {
        try {
            return Socket.getInstance().sendPacket(message, host, port);

        } catch (Exception e) {
            e.printStackTrace();
            return "err";
        }
    }
}