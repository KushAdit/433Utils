



sudo nano /lib/systemd/system/RFmqtt.service


[Unit]
Description=433MHz Receiver on RPI GPIO sending to MQTT Broker
After=network.target multi-user.target

[Service]
Type=simple
User=root
Restart=always
RestartSec=30
ExecStart=/home/dietpi/WiringOP/433Utils/RPi_utils/RFmqtt -c 145000

[Install]
WantedBy=multi-user.target

Make exec using
sudo chmod 644 /lib/systemd/system/RFmqtt.service

sudo systemctl daemon-reload
sudo systemctl enable RFmqtt.service
sudo systemctl start RFmqtt.service

Check log of service
journalctl -u RFmqtt









# About

rcswitch-pi is for controlling rc remote controlled power sockets 
with the raspberry pi. Kudos to the projects [rc-switch](http://code.google.com/p/rc-switch)
and [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi).
I just adapted the rc-switch code to use the wiringpi library instead of
the library provided by the arduino.


## Usage

First you have to install the [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) library.
After that you can compile the example program *send* by executing *make*. 
It uses wiringPi pin no 2 by default. You may want to change the used GPIO pin before compilation of the codesend.cpp source file. (Good Resource for Pin Details https://pinout.xyz/pinout/wiringpi)

## Note
The 'RF\_Sniffer' code is as yet untested.  It _should_ work, but it is still being tested thoroughly.  It's provided to allow you to start playing with it now.
