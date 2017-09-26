package com.example.jobush50.test2;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.ArrayList;


/**
 * @author Rick
 * This class is used to receive data using the UDP protocol.
 */
public class SocketIn extends Thread {
    private final int port = 4444;
    private ArrayList data = new ArrayList();

    /*
        to start
        SocketIn socketThread = new SocketIn();
        socketThread.start();

        Get data
        socketThread.getData();
     */

    /**
     * Starts a new thread and listens for incoming packages on the fixed port. New data is added to
     * an arraylist.
     */
    @Override
    public void run() {
        byte[] buffer = new byte[256];
        try {
            DatagramSocket socket = new DatagramSocket(port);
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
            while (true) {
                socket.receive(packet);
                String message = new String(packet.getData(), packet.getOffset(), packet.getLength());
                this.data.add(message);
            }
        } catch (SocketException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    /**
     * Returns the string from the most recent package received and clears the old data from the arraylist.
     *
     * @return  The data received as a string.
     */
    public String getMostRecentData() {
        if (data.isEmpty()) {
            return "";
        }
        String temp = data.get(data.size() - 1).toString();
        data.clear();
        return temp;
    }
}
