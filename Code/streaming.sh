#!/bin/sh
# Author: Hung 
# een script voor het aanzetten van streaming server
# streaming: http://+ HOST +:8080/?action=stream

cd /usr/src/mjpg-streamer/mjpg-streamer-experimental
export LD_LIBRARY_PATH=.
./mjpg_streamer -o "output_http.so -w ./www" -i "input_raspicam.so -x 640 -y 480 -fps 15 -q 5 -vf -hf"