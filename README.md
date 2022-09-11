# ads1115d daemon. 
No need for funcky libraries to do ADC.

## Features
- Microseconds resolution timer
- No insane busy loops
- Maintains either text or binary files for configure pins

## Setup
Add systemctl service. On first run a default configuration file /etc/ads1115d.json will bre created (see https://github.com/AlexRaybosh/ads1115d/blob/master/ads1115.conf.default.json)
Tune it, and restart the service


