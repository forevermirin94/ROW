package com.example.jobush50.test2;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    EditText editTextAddress, editTextPort;
    Button buttonConnect;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        editTextAddress = (EditText) findViewById(R.id.editText_ip);
        editTextPort = (EditText) findViewById(R.id.editText_port);
        buttonConnect = (Button) findViewById(R.id.Connect_button);

        buttonConnect.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){

                String ip = editTextAddress.getText().toString();
                int port =  Integer.parseInt(editTextPort.getText().toString());

                //udpclient = new Udpclient(ip,port);
                //udpclient.start();

                Intent intent = new Intent(MainActivity.this,SecondActivity.class);

                intent.putExtra("ip", ip);
                intent.putExtra("port", port);

                startActivity(intent);

            }

        });


    }



}

