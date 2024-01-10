# experimental-computer-for-schools
IBM Experimental computer for schools

The schools computer dates from around 1969 and ws an experimental machine that was never sold.

This is a simulation of the machine. It can run code written for the original machine, well it
could if any programs for the machine still exist.

<h1>USB</h1>
Interaction with the simulator is over USB. It just requires a Pico. You can run a simulator with just a Pico, the PCB isn't necessary. If you have a Pico W then you can access over WiFi.

<h1>PCB</h1>
![IMG_4442p1](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/ceb090f5-20b0-4658-b744-e295b8f3aa61.jpg)
![IMG_4439](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/ef18ebf3-a95e-41ee-9716-92998c548807)
![IMG_4441](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/c710a7aa-bbc5-48d7-a87f-0b369c42f62d)

There is a PCB that has a Pico mounted on it. This has a 0.96" OLED display that shows the same information 
as the original machine displayed on a domestic TV. There is an SD ard for storage and a keyboard made from tactile 
switches. The Pico can be a PicoW, in which case a WiFi access point is generated.

<h1>Wifi Version</h1>
This version has a USB interface, and a Wifi interface (it generates a hot spot). The USB interface still works. The Wifi page shows the display and allows control from a web page. This version of the code runs on a Pico W and generates it's own access point.

If building for a Pico W then use this command line:

<code>cmake -DPICO_BOARD=pico_w -DWIFI_SSID="Schools" -DWIFI_PASSWORD="password" ..</code>

to initialise the build directory.

