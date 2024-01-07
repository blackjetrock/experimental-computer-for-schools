# experimental-computer-for-schools
IBM Experimental computer for schools

The schools computer dates from around 1969 and ws an experimental machine that was never sold.

This is a simulation of the machine. It can run code written for the original machine, well it
could if any programs for the machine still exist.

<h1>USB Version</h1>
Interaction with this version of the simulator is over USB. It just requires a Pico.

<h1>PCB</h1>
There is a PCB that has a Pico mounted on it. This has a 0.96" OLED display that shows the same information 
as the original machine displayed on a domestic TV. There is an SD ard for storage and a keyboard made from tactile 
switches. The Pico can be a PicoW, in which case a WiFi access point is generated.

<h1>Wifi Version</h1>
This version has a USB interface, and a Wifi interface (it generates a hot spot). The USB interface still works. The Wifi page shows the display and allows control from a web page. This version of the code runs on a Pico W and generates it's own access point.
If building for a Pico W then use this command line:

<code>cmake -DPICO_BOARD=pico_w -DWIFI_SSID="Schools" -DWIFI_PASSWORD="password" ..</code>

to initialise the build directory.

