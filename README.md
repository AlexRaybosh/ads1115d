#  ADS1115 ads1115d daemon.  Probably will work for other ADCs ADS111* or should be easily tunable
No need for funcky libraries to do ADC.

## Features
- Microseconds resolution timer
- No insane busy loops
- Maintains either text or binary files for configure pins. Can be easily used from any other language

## Setup
Add systemctl service. 
Precompled armhf binary https://github.com/AlexRaybosh/ads1115d/tree/master/build/armhf
On first run a default configuration file /etc/ads1115d.json will bre created (see https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.conf.default.json)


Tune it, e.g. enable pin https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.conf.default.json#L25 
Thenand restart the service

## Notes
- Fastest way is to use mmap writeMethod, On raaspberry pi 2, the cost is about ~ 10 microseconds (Cost of formatting ~ 40 microseconds)
- "write" method cost is about 80-90 microseconds
- "rename" method cost is about 150 microseconds

All above, given tempfs location



