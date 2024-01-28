# IBM Experimental Computer for Schools

The schools computer dates from around 1969 and was an experimental machine that was never sold. It was designed to be used in schools to teach the basics of computer design and programming. The hardware was based on the same components that the IBM system 360 used. The computer was programmed in machine code, and unlike modern machines the machine code is BCD based, not binary. There are 'extracodes' in the system that are analogous to subroutines, these provided floating point arithmetic at the machine code level. The extracodes could be replaced with user code if desired, and provide a completely different set of functions.

This is a simulation of the machine. It can run code written for the original machine, well it
could if any programs for the machine still exist.

<h1>USB</h1>
Interaction with the simulator is over USB. It just requires a Pico. You can run a simulator with just a Pico, the PCB isn't necessary. If you have a Pico W then you can access over WiFi.

<h1>PCB</h1>

There is a PCB that has a Pico mounted on it. This photo shows the PCB with a Pico W attached. The simulator code can be built with or without WiFi support.

![IMG_4442p1](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/ceb090f5-20b0-4658-b744-e295b8f3aa61)

This has a 0.96" OLED display that shows the same information as the original machine displayed on a domestic TV.

![IMG_4439](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/ef18ebf3-a95e-41ee-9716-92998c548807)

 There is an SD card for storage and a keyboard made from tactile switches, in the same layout as the original machines. The operation is very similar to the original, some differenes have been made to key operations to enable use of the SD card. the original machines used audio cassettes for data storage.
 
![IMG_4441](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/c710a7aa-bbc5-48d7-a87f-0b369c42f62d)
 The Pico can be a PicoW, in which case a WiFi access point is generated.

<h1>Wifi Version</h1>
This version has a USB interface, and a Wifi interface (it generates a hot spot). The USB interface still works. The Wifi page shows the display and allows control from a web page. This version of the code runs on a Pico W and generates it's own access point.

If building for a Pico W then use this command line:

<code>cmake -DPICO_BOARD=pico_w -DWIFI_SSID="Schools" -DWIFI_PASSWORD="password" ..</code>

to initialise the build directory in the 

<code>firmware/escw</code>

directory. Then 

<code>make</code>

as usual.
To build the non-Wifi version, 

<code>cmake ..</code>

in the 

<code>experimental_schools_computer/build</code>

directory and then make as usual.


