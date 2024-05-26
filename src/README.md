


# Initialize / Setup 


1. Check if Wifi is setup correctly. 
    a) found ssid and pw login. 
    b) If not setup open Hotspot for setup
2. If (1a) read EPROM from sensor 
    a) Found sensor id **and** sleep behavior id. 
       --> Use registry to create sensor and behavior object.
    b) Found neither or just one of them 
       --> Open Webserver to allow to flash EPROM with sensor id and behavior  id
3. If (2a) Start normal operation

# Sensors

## Temperature DS18B20

... 


# Sleep Behavior 

* Id of behavior is written to EPROM
* Each implementation of the behavior can have configuration written to the EPROM 
* Using the config mode of the board a webserver is opened that allows to adjust 
  the configuration of each sensor board. 

## Interval Behavior 

...

## Day/Night  Behavior 

...

## At home  Behavior 

...