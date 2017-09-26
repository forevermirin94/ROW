package com.example.jobush50.test2;

import android.os.AsyncTask;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

/**
 * @author Rick
 *
 * This class is used to send data using the UDP protocol.
 */
public class SocketOut {
    private int port;
    private String address;
    private DatagramSocket socket;

    /**
     * The constructor of this class saves the host IP address and port to their respective variables
     * and creates a datagram socket which is later used to send packets.
     *
     * @param address           The address of the destination.
     * @param port              The corresponding port.
     * @throws SocketException  If the socket could not be created.
     */
    public SocketOut(String address, int port) throws SocketException {
        this.port = port;
        this.address = address;
        socket = new DatagramSocket();
    }

    /**
     *  This method sends a string of data to the previously specified host and port. An AsyncTask is
     *  used as Android does not allow network stuff to run in the main thread.
     *
     * @param message   The message to be send over the socket, as a string.
     */
    public void sendPacket(final String message) {

        AsyncTask<String, Void, Void> task = new AsyncTask<String, Void, Void>() {

            @Override
            protected Void doInBackground(String... params) {
                try {
                    byte[] buffer = message.getBytes();
                    InetAddress host = InetAddress.getByName(address);

                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length, host, port);

                    socket.send(packet);
                } catch (UnknownHostException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                return null;
            }
        }.execute();
    }
}
