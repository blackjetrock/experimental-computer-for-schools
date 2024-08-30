# IBM Experimental Computer for Schools

The schools computer project ran from 1966 to 1970 and was an experimental machine that was never sold, although several were made and were used in schools in the UK. One machine was used for ten years before it failed. It was designed to be used in schools to teach the basics of computer design and programming. The hardware was based on the same components that the IBM system 360 used. The computer was programmed in machine code, and unlike modern machines the machine code is BCD based, not binary. There are 'extracodes' in the system that are analogous to subroutines, these provided floating point arithmetic at the machine code level. The extracodes could be replaced with user code if desired, and provide a completely different set of functions.

As far as I an tell this machine was the first machine purpose built for education. there are lots of examples of computers being used to teach fundamentals of programming and computer architecture, but they are general purpose machines in educational situations, this is the earliest machine designed for the purpose I can find.

It also seems to be the first machine to use a domestic television as a display.

This is a simulation of the machine. It can run code written for the original machine.

<h1>USB</h1>
USB can be used to interact with the simulator. It just requires a Pico. You can run a simulator with just a Pico, the PCB isn't necessary. If you have a Pico W then you can access over WiFi as well.
A command line interface (CLI) is available on the USB connection, this requires a terminal emulator program such as minicom. Using the interface you can:

* Drive keys as if they came from the keyboard
* See a representation of the display.
* Run tests that check the simulation is running as required.
* Load programs from the document 'A First Course on the School's Computer'
* Run programs
* Examine the store
* Examine the machine state
* Load and store machine state from and to the SD card
  

<h1>PCB</h1>

There is a PCB that has a Pico mounted on it. This photo shows the PCB with a Pico W attached. The simulator code can be built with or without WiFi support.

![IMG_4442p1](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/ceb090f5-20b0-4658-b744-e295b8f3aa61)

This has a 0.96" OLED display that shows the same information as the original machine displayed on a domestic TV.

![IMG_4439](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/ef18ebf3-a95e-41ee-9716-92998c548807)

 There is an SD card for storage and a keyboard made from tactile switches, in the same layout as the original machines. The operation is very similar to the original, some differences have been made to key operations to enable use of the SD card. the original machines used audio cassettes for data storage.
 
![IMG_4441](https://github.com/blackjetrock/experimental-computer-for-schools/assets/31587992/c710a7aa-bbc5-48d7-a87f-0b369c42f62d)
 The Pico can be a PicoW, in which case a WiFi access point is generated.

<h1>Wifi Version</h1>
This version has a USB interface, and a Wifi interface (it generates a hot spot). The USB interface still works. The Wifi page shows the display and allows control from a web page. This version of the code runs on a Pico W and generates its own access point.


![Screenshot from 2024-08-30 09-09-44](https://github.com/user-attachments/assets/dc671aa7-ae32-4101-8cae-64d0d611e354)


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

<h1>Half Scale Replica</h1>

![IMG_20240827_103823604_HDR](https://github.com/user-attachments/assets/04b15898-e2ba-4993-99cf-3b7841f57b29)

The half scale replica is constructed of similar materials to the original, and runs the same code as the small PCB version, with some modiufications for the different display and keyboard. The display is a large LCD instead of a small OLED and a touch keyboard is used instead of tactile switches. The case is aluminium which has been powder coated, the keyboard surround is a 3D printed resin part. The original machine also used a plastic surround, this is probably because a metal surround would interfere with the touch keyboard.

<h1>Assembler</h1>
I have created an assembler that assembles instructions expressed in the verbose comments seen in the documents available for this computer. 

<h1>Documents</h1>

<h2>A First Course on the Schools's Computer</h2>
This is a document that provided a course on programming the computer, there is only one known paper copy of this document, but it has been
copied and put together into a PDF document. It has several programs written for the machine, which are the only ones I know of from the time
when the machine was trialled. These programs almost exclusively use the extracodes for the programs, not the half-word basic instructions which must have been used to implement
the extracodes themselves in the store from address 100 to 199. 

<h2>Experimental Computer for Schools</h2>
Describes the hardware and code design of the machine. This is a technical description of the machine, and contains no program listings at all.

<h2>Flowcharts</h2>
This is a four page document that has flowcharts and programs for log<sub>b</sub>N and b<sup>L</sup>.
The programs use an extracode that isn't in the standard set, which is a square root instruction. The instruction also has a jump and the jump part is used in the programs without use of the square root operation. Together with the first course on the school's computer these are the only known programs for the computer from the time whenit was being develped and trialled.

<h1>References</h1>
A photo of the schools computer in a classroom in the 60s:
https://www.stedwardsarchives.org/PDFViewer/web/viewer.html?file=%2fFilename.ashx%3ftableName%3dta_Publications%26columnName%3dfilename%26recordId%3d685%26zoom%3dpage-fit&searchText=schools%27%20computer

https://www.stedwardsarchives.org/PDFViewer/web/viewer.html?file=%2fFilename.ashx%3ftableName%3dta_Publications%26columnName%3dfilename%26recordId%3d685%26zoom%3dpage-fit&searchText=schools%27%20computer

https://ibmhursleymuseum.info/hardware-data-detail.asp?id1=140

https://news.ycombinator.com/item?id=36112633

https://www.stedwardsarchives.org/PDFViewer/web/viewer.html?file=%2fFilename.ashx%3ftableName%3dta_Publications%26columnName%3dfilename%26recordId%3d607%26zoom%3dpage-fit&searchText=ibm

https://www.stedwardsarchives.org/PDFViewer/web/viewer.html?file=%2fFilename.ashx%3ftableName%3dta_Publications%26columnName%3dfilename%26recordId%3d602%26zoom%3dpage-fit&searchText=ibm

https://www.stedwardsarchives.org/PDFViewer/web/viewer.html?file=%2fFilename.ashx%3ftableName%3dta_Publications%26columnName%3dfilename%26recordId%3d606%26zoom%3dpage-fit&searchText=ibm

https://www.stedwardsarchives.org/PDFViewer/web/viewer.html?file=%2fFilename.ashx%3ftableName%3dta_Publications%26columnName%3dfilename%26recordId%3d674%26zoom%3dpage-fit&searchText=schools%27%20computer

https://www.stedwardsoxford.org/obituary/david-tinsley/
