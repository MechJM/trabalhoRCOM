#!/bin/bash
touch results_writer.csv
# Parameters: port filename baudrate packet_size FER T_prop
# Changing FER
echo "CHANGING FER!"
echo "FER = 10"
./write 0 pinguim.gif B38400 1000 10 0
./write 0 pinguim.gif B38400 1000 10 0
./write 0 pinguim.gif B38400 1000 10 0
echo "FER = 20"
./write 0 pinguim.gif B38400 1000 20 0
./write 0 pinguim.gif B38400 1000 20 0
./write 0 pinguim.gif B38400 1000 20 0
echo "FER = 30"
./write 0 pinguim.gif B38400 1000 30 0
./write 0 pinguim.gif B38400 1000 30 0
./write 0 pinguim.gif B38400 1000 30 0
echo "FER = 40"
./write 0 pinguim.gif B38400 1000 40 0
./write 0 pinguim.gif B38400 1000 40 0
./write 0 pinguim.gif B38400 1000 40 0
echo "FER = 50"
./write 0 pinguim.gif B38400 1000 50 0
./write 0 pinguim.gif B38400 1000 50 0
./write 0 pinguim.gif B38400 1000 50 0
echo "FER = 60"
./write 0 pinguim.gif B38400 1000 60 0
./write 0 pinguim.gif B38400 1000 60 0
./write 0 pinguim.gif B38400 1000 60 0
echo "FER = 70"
./write 0 pinguim.gif B38400 1000 70 0
./write 0 pinguim.gif B38400 1000 70 0
./write 0 pinguim.gif B38400 1000 70 0
echo "FER = 80"
./write 0 pinguim.gif B38400 1000 80 0
./write 0 pinguim.gif B38400 1000 80 0
./write 0 pinguim.gif B38400 1000 80 0
echo "FER = 90"
./write 0 pinguim.gif B38400 1000 90 0
./write 0 pinguim.gif B38400 1000 90 0
./write 0 pinguim.gif B38400 1000 90 0
echo "FER = 100"
./write 0 pinguim.gif B38400 1000 100 0
./write 0 pinguim.gif B38400 1000 100 0
./write 0 pinguim.gif B38400 1000 100 0
# Changing T_prop
echo "CHANGING T_PROP!"
echo "T_PROP = 1"
./write 0 pinguim.gif B38400 1000 0 1
./write 0 pinguim.gif B38400 1000 0 1
./write 0 pinguim.gif B38400 1000 0 1
echo "T_PROP = 2"
./write 0 pinguim.gif B38400 1000 0 2
./write 0 pinguim.gif B38400 1000 0 2
./write 0 pinguim.gif B38400 1000 0 2
echo "T_PROP = 3"
./write 0 pinguim.gif B38400 1000 0 3
./write 0 pinguim.gif B38400 1000 0 3
./write 0 pinguim.gif B38400 1000 0 3
echo "T_PROP = 4"
./write 0 pinguim.gif B38400 1000 0 4
./write 0 pinguim.gif B38400 1000 0 4
./write 0 pinguim.gif B38400 1000 0 4
echo "T_PROP = 5"
./write 0 pinguim.gif B38400 1000 0 5
./write 0 pinguim.gif B38400 1000 0 5
./write 0 pinguim.gif B38400 1000 0 5
# Changing baudrate
echo "CHANGING BAUDRATE!"
echo "BAUDRATE = 2400"
./write 0 pinguim.gif B2400 1000 0 0
./write 0 pinguim.gif B2400 1000 0 0
./write 0 pinguim.gif B2400 1000 0 0
echo "BAUDRATE = 4800"
./write 0 pinguim.gif B4800 1000 0 0
./write 0 pinguim.gif B4800 1000 0 0
./write 0 pinguim.gif B4800 1000 0 0
echo "BAUDRATE = 9600"
./write 0 pinguim.gif B9600 1000 0 0
./write 0 pinguim.gif B9600 1000 0 0
./write 0 pinguim.gif B9600 1000 0 0
echo "BAUDRATE = 19200"
./write 0 pinguim.gif B19200 1000 0 0
./write 0 pinguim.gif B19200 1000 0 0
./write 0 pinguim.gif B19200 1000 0 0
echo "BAUDRATE = 38400"
./write 0 pinguim.gif B38400 1000 0 0
./write 0 pinguim.gif B38400 1000 0 0
./write 0 pinguim.gif B38400 1000 0 0
echo "BAUDRATE = 57600"
./write 0 pinguim.gif B57600 1000 0 0
./write 0 pinguim.gif B57600 1000 0 0
./write 0 pinguim.gif B57600 1000 0 0
echo "BAUDRATE = 115200"
./write 0 pinguim.gif B115200 1000 0 0
./write 0 pinguim.gif B115200 1000 0 0
./write 0 pinguim.gif B115200 1000 0 0
# Changing packet size
echo "CHANGING PACKET SIZE!"
echo "PACKET SIZE = 512"
./write 0 pinguim.gif B38400 512 0 0
./write 0 pinguim.gif B38400 512 0 0
./write 0 pinguim.gif B38400 512 0 0
echo "PACKET SIZE = 1024"
./write 0 pinguim.gif B38400 1024 0 0
./write 0 pinguim.gif B38400 1024 0 0
./write 0 pinguim.gif B38400 1024 0 0
echo "PACKET SIZE = 2048"
./write 0 pinguim.gif B38400 2048 0 0
./write 0 pinguim.gif B38400 2048 0 0
./write 0 pinguim.gif B38400 2048 0 0
echo "PACKET SIZE = 4096"
./write 0 pinguim.gif B38400 4096 0 0
./write 0 pinguim.gif B38400 4096 0 0
./write 0 pinguim.gif B38400 4096 0 0
echo "PACKET SIZE = 8192"
./write 0 pinguim.gif B38400 8192 0 0
./write 0 pinguim.gif B38400 8192 0 0
./write 0 pinguim.gif B38400 8192 0 0
echo "PACKET SIZE = 16384"
./write 0 pinguim.gif B38400 16384 0 0
./write 0 pinguim.gif B38400 16384 0 0
./write 0 pinguim.gif B38400 16384 0 0
echo "PACKET SIZE = 32768"
./write 0 pinguim.gif B38400 32768 0 0
./write 0 pinguim.gif B38400 32768 0 0
./write 0 pinguim.gif B38400 32768 0 0