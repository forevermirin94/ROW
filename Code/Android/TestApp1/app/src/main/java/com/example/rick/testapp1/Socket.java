package com.example.rick.testapp1;

import android.os.AsyncTask;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.concurrent.ExecutionException;

public class Socket {
    static Socket instance = null;
    static DatagramSocket socket_out;

    private Socket() throws SocketException {
        socket_out = new DatagramSocket();
    }

    public String sendPacket(String msg, String host, int port) throws ExecutionException, InterruptedException {
        AsyncTask<String, Void, Void> async;
        String args[] = new String[3];
        args[0] = msg;
        args[1] = host;
        args[2] = Integer.toString(port);

        String a = new AsyncTask<String, Void, String>() {
            String msg;
            @Override
            protected String doInBackground(String... args) {

                try {
                    byte[] buffer = args[0].getBytes();
                    InetAddress address = InetAddress.getByName(args[1]);

                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length, address, Integer.parseInt(args[2]));

                    socket_out.send(packet);

                    //Wait for response
                    byte[] recvbuffer = new byte[1024];
                    DatagramPacket recvpacket = new DatagramPacket(recvbuffer, recvbuffer.length);
                    socket_out.setSoTimeout(1000);
                    socket_out.receive(recvpacket);
                    msg = new String(recvbuffer, 0, recvpacket.getLength());
                    System.out.println("Response: " + recvpacket.getAddress().getHostName() + ": " + msg);

                }
                catch (SocketTimeoutException e) {
                    System.out.println("Socket timed out.");
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
                return msg;
            }
        }.execute(args).get();
        return a;
    }

    public static Socket getInstance() throws SocketException {
        if (instance == null) {
            instance = new Socket();
        }
        return instance;
    }
}