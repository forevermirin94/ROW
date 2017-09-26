package com.example.jobush50.test2;

import android.os.Message;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Udpclient extends Thread {


    String ipaddress;
    int port;

    DatagramSocket socket;
    InetAddress address;

    String msg = "Hi, client here!";

    public Udpclient(String addr, int prt) {
        ipaddress = addr;
        port = prt;
    }

    public void message(String message) {
        msg = message;
    }

    public void run() {

        try {

            socket = new DatagramSocket();
            address = InetAddress.getByName(ipaddress);

            // send request
            byte[] buf = msg.getBytes();

            DatagramPacket packet =
                    new DatagramPacket(buf, buf.length, address, port);
            socket.send(packet);

        } catch (SocketException e) {
            e.printStackTrace();
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (socket != null) {
                socket.close();
            }
        }
    }
}
