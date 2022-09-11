#  ADS1115 ads1115d daemon.  Probably will work for other ADCs ADS11xx or should be easily tunable
No need for funcky libraries to do ADC. Just read a text or binary file, e.g. /run/ads1115/AIN1.txt

## Features
- Microseconds resolution timer
- No insane busy loops. Sleep times between ADC request and ADC fetch are calculated based on rate frequences (https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.conf.default.json#L47). 
Add more sleep, after reading, if needed (https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.conf.default.json#L38).
- Maintains either text or binary files for configured pins. Can be easily used from any other language (as long as you can read a file there)



## Setup
Add systemctl service. 
Precompled armhf binary https://github.com/AlexRaybosh/ads1115d/tree/master/build/armhf
Crosscompiled & Linked with static libgcc and libstdc++ - https://github.com/AlexRaybosh/ads1115d/blob/master/src/arch.mk#L42

On first run a default configuration file /etc/ads1115d.json will be created 
(see https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.conf.default.json)


Tune it, e.g. enable/disable pin https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.conf.default.json#L25 
Add more pins.
Then restart the service

## Notes
- Fastest way is to use mmap writeMethod, On raspberry pi 2, the cost is about ~ 6-8 microseconds (Cost of text formatting ~ 40 microseconds)
- "write" method cost is about 80-90 microseconds (including text formatting)
- "rename" method cost is about 150 microseconds (including text formatting)

All the above, given tempfs output file location (e.g. /run on raspberian )



## More notes
- Did't put any specific arch flags. Should work on Pi 2+
- armel is commented out
- To cross build, need to get /usr/bin/arm-linux-gnueabihf* stuff, and qemu-arm

## systemctl
- Example https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.service
- Copy ads1115.service to /etc/systemd/system/
- Copy ads1115d from https://github.com/AlexRaybosh/ads1115d/tree/master/build/armhf to /usr/bin
- enable with
systemctl enable ads1115.service
- start with 
systemctl start ads1115.service
- edit /etc/ads1115d.json
- restart 
systemctl restart ads1115.service
- check status
systemctl status ads1115

[code]
root@r2b:/home/pi# systemctl status ads1115
● ads1115.service - ads1115d
   Loaded: loaded (/etc/systemd/system/ads1115.service; enabled; vendor preset: enabled)
   Active: active (running) since Sun 2022-09-11 16:01:32 CDT; 1s ago
 Main PID: 30370 (ads1115d)
    Tasks: 1 (limit: 2164)
   CGroup: /system.slice/ads1115.service
           └─30370 /usr/bin/ads1115d

Sep 11 16:01:32 r2b systemd[1]: Started ads1115d.




