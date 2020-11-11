#!/bin/bash
touch results_reader.csv
# Parameters: port baudrate packet_size FER T_prop
# Changing FER
echo "CHANGING FER!"
:'
echo "FER = 10"
./read 0 B38400 1000 10 0 && rm pinguim_clone.gif
./read 0 B38400 1000 10 0 && rm pinguim_clone.gif
./read 0 B38400 1000 10 0 && rm pinguim_clone.gif
echo "FER = 20"
./read 0 B38400 1000 20 0 && rm pinguim_clone.gif
./read 0 B38400 1000 20 0 && rm pinguim_clone.gif
./read 0 B38400 1000 20 0 && rm pinguim_clone.gif
echo "FER = 30"
./read 0 B38400 1000 30 0 && rm pinguim_clone.gif
./read 0 B38400 1000 30 0 && rm pinguim_clone.gif
./read 0 B38400 1000 30 0 && rm pinguim_clone.gif
echo "FER = 40"
./read 0 B38400 1000 40 0 && rm pinguim_clone.gif
./read 0 B38400 1000 40 0 && rm pinguim_clone.gif
./read 0 B38400 1000 40 0 && rm pinguim_clone.gif
echo "FER = 50"
./read 0 B38400 1000 50 0 && rm pinguim_clone.gif
./read 0 B38400 1000 50 0 && rm pinguim_clone.gif
./read 0 B38400 1000 50 0 && rm pinguim_clone.gif
'
echo "FER = 60"
./read 0 B38400 1000 60 0 && rm pinguim_clone.gif
./read 0 B38400 1000 60 0 && rm pinguim_clone.gif
./read 0 B38400 1000 60 0 && rm pinguim_clone.gif
echo "FER = 70"
./read 0 B38400 1000 70 0 && rm pinguim_clone.gif
./read 0 B38400 1000 70 0 && rm pinguim_clone.gif
./read 0 B38400 1000 70 0 && rm pinguim_clone.gif
echo "FER = 80"
./read 0 B38400 1000 80 0 && rm pinguim_clone.gif
./read 0 B38400 1000 80 0 && rm pinguim_clone.gif
./read 0 B38400 1000 80 0 && rm pinguim_clone.gif
echo "FER = 90"
./read 0 B38400 1000 90 0 && rm pinguim_clone.gif
./read 0 B38400 1000 90 0 && rm pinguim_clone.gif
./read 0 B38400 1000 90 0 && rm pinguim_clone.gif
echo "FER = 100"
./read 0 B38400 1000 100 0 && rm pinguim_clone.gif
./read 0 B38400 1000 100 0 && rm pinguim_clone.gif
./read 0 B38400 1000 100 0 && rm pinguim_clone.gif
# Changing T_prop
echo "CHANGING T_PROP!"
echo "T_PROP = 1"
./read 0 B38400 1000 0 1 && rm pinguim_clone.gif
./read 0 B38400 1000 0 1 && rm pinguim_clone.gif
./read 0 B38400 1000 0 1 && rm pinguim_clone.gif
echo "T_PROP = 2"
./read 0 B38400 1000 0 2 && rm pinguim_clone.gif
./read 0 B38400 1000 0 2 && rm pinguim_clone.gif
./read 0 B38400 1000 0 2 && rm pinguim_clone.gif
echo "T_PROP = 3"
./read 0 B38400 1000 0 3 && rm pinguim_clone.gif
./read 0 B38400 1000 0 3 && rm pinguim_clone.gif
./read 0 B38400 1000 0 3 && rm pinguim_clone.gif
echo "T_PROP = 4"
./read 0 B38400 1000 0 4 && rm pinguim_clone.gif
./read 0 B38400 1000 0 4 && rm pinguim_clone.gif
./read 0 B38400 1000 0 4 && rm pinguim_clone.gif
echo "T_PROP = 5"
./read 0 B38400 1000 0 5 && rm pinguim_clone.gif
./read 0 B38400 1000 0 5 && rm pinguim_clone.gif
./read 0 B38400 1000 0 5 && rm pinguim_clone.gif
# Changing baudrate
echo "CHANGING BAUDRATE!"
echo "BAUDRATE = 300"
./read 0 B300 1000 0 0 && rm pinguim_clone.gif
./read 0 B300 1000 0 0 && rm pinguim_clone.gif
./read 0 B300 1000 0 0 && rm pinguim_clone.gif
echo "BAUDRATE = 900"
./read 0 B900 1000 0 0 && rm pinguim_clone.gif
./read 0 B900 1000 0 0 && rm pinguim_clone.gif
./read 0 B900 1000 0 0 && rm pinguim_clone.gif
echo "BAUDRATE = 2700"
./read 0 B2700 1000 0 0 && rm pinguim_clone.gif
./read 0 B2700 1000 0 0 && rm pinguim_clone.gif
./read 0 B2700 1000 0 0 && rm pinguim_clone.gif
echo "BAUDRATE = 8100"
./read 0 B8100 1000 0 0 && rm pinguim_clone.gif
./read 0 B8100 1000 0 0 && rm pinguim_clone.gif
./read 0 B8100 1000 0 0 && rm pinguim_clone.gif
echo "BAUDRATE = 19200"
./read 0 B19200 1000 0 0 && rm pinguim_clone.gif
./read 0 B19200 1000 0 0 && rm pinguim_clone.gif
./read 0 B19200 1000 0 0 && rm pinguim_clone.gif
echo "BAUDRATE = 38400"
./read 0 B38400 1000 0 0 && rm pinguim_clone.gif
./read 0 B38400 1000 0 0 && rm pinguim_clone.gif
./read 0 B38400 1000 0 0 && rm pinguim_clone.gif
echo "BAUDRATE = 76800"
./read 0 B76800 1000 0 0 && rm pinguim_clone.gif
./read 0 B76800 1000 0 0 && rm pinguim_clone.gif
./read 0 B76800 1000 0 0 && rm pinguim_clone.gif
echo "BAUDRATE = 115200"
./read 0 B115200 1000 0 0 && rm pinguim_clone.gif
./read 0 B115200 1000 0 0 && rm pinguim_clone.gif
./read 0 B115200 1000 0 0 && rm pinguim_clone.gif
# Changing packet size
echo "CHANGING PACKET SIZE!"
echo "PACKET SIZE = 512"
./read 0 B38400 512 0 0 && rm pinguim_clone.gif
./read 0 B38400 512 0 0 && rm pinguim_clone.gif
./read 0 B38400 512 0 0 && rm pinguim_clone.gif
echo "PACKET SIZE = 1024"
./read 0 B38400 1024 0 0 && rm pinguim_clone.gif
./read 0 B38400 1024 0 0 && rm pinguim_clone.gif
./read 0 B38400 1024 0 0 && rm pinguim_clone.gif
echo "PACKET SIZE = 2048"
./read 0 B38400 2048 0 0 && rm pinguim_clone.gif
./read 0 B38400 2048 0 0 && rm pinguim_clone.gif
./read 0 B38400 2048 0 0 && rm pinguim_clone.gif
echo "PACKET SIZE = 4096"
./read 0 B38400 4096 0 0 && rm pinguim_clone.gif
./read 0 B38400 4096 0 0 && rm pinguim_clone.gif
./read 0 B38400 4096 0 0 && rm pinguim_clone.gif
echo "PACKET SIZE = 8192"
./read 0 B38400 8192 0 0 && rm pinguim_clone.gif
./read 0 B38400 8192 0 0 && rm pinguim_clone.gif
./read 0 B38400 8192 0 0 && rm pinguim_clone.gif
echo "PACKET SIZE = 16384"
./read 0 B38400 16384 0 0 && rm pinguim_clone.gif
./read 0 B38400 16384 0 0 && rm pinguim_clone.gif
./read 0 B38400 16384 0 0 && rm pinguim_clone.gif
echo "PACKET SIZE = 32768"
./read 0 B38400 32768 0 0 && rm pinguim_clone.gif
./read 0 B38400 32768 0 0 && rm pinguim_clone.gif
./read 0 B38400 32768 0 0 && rm pinguim_clone.gif