/*
  RFSniffer connected to RPI GPIO 2 / BCM27 / Pin 13
  Usage: ./RFSniffer [<pulseLength>]
  [] = optional
  Hacked from http://code.google.com/p/rc-switch/
  by @justy to provide a handy RF code sniffer
  by @josar to forward the massages to MQTT
*/

#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>

/* usleep */
#include <unistd.h>

/* nanosleep */
#include <time.h>

/* for options */
#include <getopt.h>
#include <strings.h>

/* inlcude MQTT */
#include <mosquitto.h>

/* parameter
 * -g GPIO Pin connected to pi
 * -h host
 * -p port
 * -u mqtt username
 * -x pasword
 * -t topic
 * -w pulsewith
 * -n help
 */

RCSwitch mySwitch;


int main(int argc, char *argv[]) {

    // This pin is not the first pin on the RPi GPIO header!
    // Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
    // for more information.
    // This pin is BCM27 / Pin 13
    int pin = 0;
    int pulseLength = 0;

    /* default values */
    char  MQTT_HOSTNAME[] = "127.0.0.1\0";
    char  MQTT_USERTNAME[]= "mqttbroker\0";
    char  MQTT_PASSWORD[] = "m\0";
    char  MQTT_TOPIC[]    = "433MHz\0";


    char* host = MQTT_HOSTNAME;
    int port   = 1883, waitClocks=127000;
    char* usr  = MQTT_USERTNAME;
    char* pswd = MQTT_PASSWORD;
    char* topic= MQTT_TOPIC;
   /* parse input parameter */
   int opt;
   opterr = 0;
    while ((opt = getopt(argc, argv, "g:h:p:u:x:t:c:n")) != -1) {
            switch (opt) {
            case 'h':
                 host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'u':
                usr = optarg;
                break;
            case 'x':
                pswd = optarg;
                break;
            case 't':
                topic= optarg;
                break;
            case 'g':
                pin = atoi(optarg);
                break;
            case 'c':
                waitClocks = atoi(optarg);
                break;
            case 'w':
                pulseLength = atoi(optarg);
                break;
            case 'n':
                fprintf (stdout,"Usage: %s [-h Host] [-p Port] [-u username] [-x password] [-t topic] [-g WiringPI GPIO] [-c Wait Clocks] [-w pulsewith]\n",
                        argv[0]);
                exit(0);
                break;
            }
        }

     fprintf (stdout,"Running with -h %s -p %i -u %s -x %s -t %s -g %i -c %i -w %i\n",
                     host, port, usr ,pswd , topic, pin , waitClocks , pulseLength );


    struct mosquitto *mosq = NULL;

    // Initialize the Mosquitto library
    mosquitto_lib_init();

    // Create a new Mosquitto runtime instance with a random client ID,
    //  and no application-specific callback data.
    mosq = mosquitto_new (NULL, true, NULL);
    if (!mosq)
    {
      fprintf (stderr, "Can't initialize Mosquitto library\n");
      exit (-1);
    }

    mosquitto_username_pw_set (mosq, usr, pswd);

    // Establish a connection to the MQTT server. Do not use a keep-alive ping
    int ret = mosquitto_connect (mosq, host, port, 1000);
    if (ret)
    {
      fprintf (stderr, "Can't connect to Mosquitto server\n");
      exit (-1);
    }

    if(wiringPiSetup() == -1) {
      fprintf (stderr,"wiringPiSetup failed, exiting...");
      return 0;
    }

    mySwitch = RCSwitch();
    if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
    mySwitch.enableReceive(pin);  // Receiver on interrupt 0 => that is pin #2

    /* Setup timespec */
    struct timespec req;
    req.tv_sec = 0L;
    req.tv_nsec = 500000L;
    
    char valueStr[35];



int v1=0,v2=0,v3=0;
clock_t btv1,btv2,btv3;
char cv1='0',cv2='0',cv3='0';
    
    while (1) 
    {
if(cv1=='1'){
clock_t now = clock();
if((now-btv1)>=waitClocks){
v1=0;
btv1=0;
cv1='0';
}
}
if(cv2=='1'){
clock_t now = clock();
if((now-btv2)>=waitClocks){ 
v2=0;
btv2=0;
cv2='0';
}
}
if(cv3=='1'){
clock_t now = clock();
if((now-btv3)>=waitClocks){ 
v3=0;
btv3=0;
cv3='0';
}
}
      if (mySwitch.available())
      {
        int value = mySwitch.getReceivedValue();
        mySwitch.resetAvailable();
        
        if (value > 0) 
        {
if(v1==value || v2==value || v3==value){
continue;
}
else if(v1==0){
v1=value;
btv1 = clock();
cv1='1';
}
else if(v2==0){ 
v2=value;
btv2 = clock();
cv2='1';
}
else if(v3==0){ 
v3=value;
btv3 = clock();
cv3='1';
}
          sprintf(valueStr,"%i", value );
          ret = mosquitto_publish (mosq, NULL, topic, strlen (valueStr), valueStr, 0, false);
        }
        
        /* Include for debugging */
        /*
        if (value == 0)
        {
          fprintf (stderr,"Unknown encoding\n");
        }else
        {
          if (ret)
          {
            fprintf (stderr,"Can't publish to Mosquitto server\n");
            exit (-1);
          }else
          {
            fprintf (stdout,"Received %s\n", valueStr );
          }
          
        }
        fflush(stdout);
        */
      }
      else
      {
         //usleep(500U);
        nanosleep( &req, (struct timespec *)NULL);
      }
    }

  // Tidy up
  mosquitto_disconnect (mosq);
  mosquitto_destroy (mosq);
  mosquitto_lib_cleanup();

  exit(0);
}
