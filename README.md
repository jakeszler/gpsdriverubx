### GPS Drivers ###


This repository contains a driver to interface the PIKSI multi GPS and the Pixhawk mini (or any FMU-v2 board) running the PX4 firmware together, currently the interface is overwritten in the UBX driver files, I might make it a separate module later.
To get this firmware working simply make sure you can build PX4 firmware following this guide.
https://dev.px4.io/en/setup/building_px4.html


The wiring beetween the 2 devices is simply connecting 3 wires and rx, tx and gnd (rx to tx, tx to rx and gnd to gnd) from the gps&i2c pins on the pixhawk mini to the piksi.
Link to pixhawk pinouts: https://docs.px4.io/en/flight_controller/pixhawk_mini.html
and for the piksi: https://support.swiftnav.com/customer/en/portal/articles/2803651-piksi-multi-ardupilot-integration-guide


There are 2 separate repo's you need to clone the main being
[PX4 main driver](https://github.com/jakeszler/PixhawkFirmware) 
and this one, this one needs to be cloned into the src/drivers/gps/devices folder.
After that it should just build.


