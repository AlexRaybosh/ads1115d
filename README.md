#  ADS1115 ads1115d daemon.  Probably will work for other ADCs ADS111* or should be easily tunable
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
