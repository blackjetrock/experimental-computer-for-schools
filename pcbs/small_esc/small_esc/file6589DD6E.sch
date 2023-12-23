EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 9
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L 74xx:74LS245 U3
U 1 1 637625BF
P 2075 2200
F 0 "U3" H 1625 3200 50  0000 C CNN
F 1 "74LS245" H 1775 3100 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 2075 2200 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS245" H 2075 2200 50  0001 C CNN
	1    2075 2200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 6376C946
P 2075 3100
F 0 "#PWR09" H 2075 2850 50  0001 C CNN
F 1 "GND" H 2080 2927 50  0000 C CNN
F 2 "" H 2075 3100 50  0001 C CNN
F 3 "" H 2075 3100 50  0001 C CNN
	1    2075 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2075 3000 2075 3025
$Comp
L power:+3V3 #PWR08
U 1 1 6376D3AB
P 2075 1250
F 0 "#PWR08" H 2075 1100 50  0001 C CNN
F 1 "+3V3" H 2075 1425 50  0000 C CNN
F 2 "" H 2075 1250 50  0001 C CNN
F 3 "" H 2075 1250 50  0001 C CNN
	1    2075 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2075 1250 2075 1400
Text GLabel 1400 1700 0    50   Input ~ 0
DA0
Text GLabel 1400 1800 0    50   Input ~ 0
DA1
Text GLabel 1400 1900 0    50   Input ~ 0
DA2
Text GLabel 1400 2000 0    50   Input ~ 0
DA3
Text GLabel 1400 2100 0    50   Input ~ 0
DA4
Text GLabel 1400 2200 0    50   Input ~ 0
DA5
Text GLabel 1400 2300 0    50   Input ~ 0
DA6
Text GLabel 1400 2400 0    50   Input ~ 0
DA7
Wire Wire Line
	1400 1700 1575 1700
Wire Wire Line
	1575 1800 1400 1800
Wire Wire Line
	1400 1900 1575 1900
Wire Wire Line
	1575 2000 1400 2000
Wire Wire Line
	1575 2200 1400 2200
Wire Wire Line
	1400 2100 1575 2100
Wire Wire Line
	1400 2300 1575 2300
Wire Wire Line
	1575 2400 1400 2400
Text GLabel 2575 1700 2    50   Input ~ 0
P_DA0
Text GLabel 2575 1800 2    50   Input ~ 0
P_DA1
Text GLabel 2575 1900 2    50   Input ~ 0
P_DA2
Text GLabel 2575 2000 2    50   Input ~ 0
P_DA3
Text GLabel 2575 2100 2    50   Input ~ 0
P_DA4
Text GLabel 2575 2200 2    50   Input ~ 0
P_DA5
Text GLabel 2575 2300 2    50   Input ~ 0
P_DA6
Text GLabel 2575 2400 2    50   Input ~ 0
P_DA7
Text GLabel 10025 2750 2    50   Input ~ 0
P_DA0
Text GLabel 10025 2850 2    50   Input ~ 0
P_DA1
Text GLabel 10025 2950 2    50   Input ~ 0
P_DA2
Text GLabel 10025 3050 2    50   Input ~ 0
P_DA3
Text GLabel 10025 3150 2    50   Input ~ 0
P_DA4
Text GLabel 10025 3250 2    50   Input ~ 0
P_DA5
Text GLabel 10025 3350 2    50   Input ~ 0
P_DA6
Text GLabel 10025 3450 2    50   Input ~ 0
P_DA7
$Comp
L power:GND #PWR019
U 1 1 6377438C
P 8625 6250
F 0 "#PWR019" H 8625 6000 50  0001 C CNN
F 1 "GND" H 8630 6077 50  0000 C CNN
F 2 "" H 8625 6250 50  0001 C CNN
F 3 "" H 8625 6250 50  0001 C CNN
	1    8625 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8625 6050 8625 6250
Text GLabel 10050 3550 2    50   Input ~ 0
A0
Text GLabel 10050 3650 2    50   Input ~ 0
A1
Text GLabel 10050 3750 2    50   Input ~ 0
A2
Text GLabel 10050 3850 2    50   Input ~ 0
A3
Text GLabel 10050 3950 2    50   Input ~ 0
A4
Text GLabel 10050 4050 2    50   Input ~ 0
A5
Text GLabel 10050 4150 2    50   Input ~ 0
A6
Text GLabel 10050 4250 2    50   Input ~ 0
A7
Text GLabel 10050 4350 2    50   Input ~ 0
A8
Text GLabel 10050 4450 2    50   Input ~ 0
A9
Text GLabel 10050 4550 2    50   Input ~ 0
A10
Text GLabel 10050 4650 2    50   Input ~ 0
A11
Text GLabel 10050 4750 2    50   Input ~ 0
A12
Text GLabel 10050 4850 2    50   Input ~ 0
A13
Wire Wire Line
	9875 4850 10050 4850
Wire Wire Line
	10050 4750 9875 4750
Wire Wire Line
	9875 4650 10050 4650
Wire Wire Line
	10050 4550 9875 4550
Wire Wire Line
	1575 2700 1575 3025
Wire Wire Line
	1575 3025 2075 3025
Connection ~ 2075 3025
Wire Wire Line
	2075 3025 2075 3100
Text GLabel 1575 2600 0    50   Input ~ 0
DATA_DIR_HW
Text GLabel 10050 5550 2    50   Input ~ 0
P_INPUT0
Text GLabel 10050 5450 2    50   Input ~ 0
P_INPUT1
Text Notes 775  4275 0    50   Italic 10
Use USB for data transfer from host to Pico.\nSecond core handles bus interface. Overclock to 270MHz if\nnecessary.
$Comp
L Connector:Conn_01x03_Female J1
U 1 1 63987833
P 5500 5375
F 0 "J1" H 5528 5401 50  0000 L CNN
F 1 "Conn_01x03_Female" H 5528 5310 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 5500 5375 50  0001 C CNN
F 3 "~" H 5500 5375 50  0001 C CNN
	1    5500 5375
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 639ABA14
P 6575 5725
F 0 "#PWR05" H 6575 5475 50  0001 C CNN
F 1 "GND" H 6580 5552 50  0000 C CNN
F 2 "" H 6575 5725 50  0001 C CNN
F 3 "" H 6575 5725 50  0001 C CNN
	1    6575 5725
	-1   0    0    -1  
$EndComp
$Sheet
S 3600 7075 825  525 
U 639C14AC
F0 "copro" 50
F1 "/tree/projects/github/casio-fx9000p/pcbs/pico9000-pack/pico-9000-pack/copro.sch" 50
$EndSheet
Text GLabel 10050 5650 2    50   Input ~ 0
COPRO_RX
Text GLabel 10075 5750 2    50   Input ~ 0
COPRO_TX
Text Notes 725  975  0    50   ~ 0
DATA_DIR can be driven by the Pico if needed, otherwise hardware\ncan drive the data direction. Pico still needs to know whether it's a \nread or a write if emulating a RAM cartridge. ROM cartridge can be \nonly data out.
Text Notes 800  4725 0    50   ~ 0
If the bus Pico sends data out then the SD card\nPico can receive only. With no data flow from SD to \nbus pico then GPIO can be used for data direction.
Text Notes 800  3850 0    50   ~ 0
The Pico is apparently 5V tolerant on inputs. So, no level shifters on inputs\nand no level shifters on outputs from the Pico driving inputs on 5V chips.\nThe level shifters would only have driven 3V3 anyway. level shifters used \non data bus as contention could lead to 5V being placed on outputs.
$Comp
L MCU_RaspberryPi_RP2040:RP2040 U5
U 1 1 641F5980
P 8625 4050
F 0 "U5" H 9700 1875 50  0000 C CNN
F 1 "RP2040" H 9800 1800 50  0000 C CNN
F 2 "rp2040:RP2040-QFN-56-no_holes" H 7875 4050 50  0001 C CNN
F 3 "" H 7875 4050 50  0001 C CNN
	1    8625 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	9875 2750 10025 2750
Wire Wire Line
	9875 2850 10025 2850
Wire Wire Line
	9875 2950 10025 2950
Wire Wire Line
	9875 3050 10025 3050
Wire Wire Line
	9875 3150 10025 3150
Wire Wire Line
	9875 3250 10025 3250
Wire Wire Line
	9875 3350 10025 3350
Wire Wire Line
	9875 3450 10025 3450
Wire Wire Line
	9875 3550 10050 3550
Wire Wire Line
	9875 3650 10050 3650
Wire Wire Line
	9875 3750 10050 3750
Wire Wire Line
	9875 3850 10050 3850
Wire Wire Line
	9875 3950 10050 3950
Wire Wire Line
	9875 4050 10050 4050
Wire Wire Line
	9875 4150 10050 4150
Wire Wire Line
	9875 4250 10050 4250
Wire Wire Line
	9875 4350 10050 4350
Wire Wire Line
	9875 4450 10050 4450
Wire Wire Line
	10050 5450 9875 5450
Wire Wire Line
	10050 5550 9875 5550
Wire Wire Line
	10050 5650 9875 5650
$Comp
L Device:C C?
U 1 1 64322AB9
P 5200 4075
AR Path="/63E0266A/64322AB9" Ref="C?"  Part="1" 
AR Path="/64322AB9" Ref="C5"  Part="1" 
F 0 "C5" H 5315 4121 50  0000 L CNN
F 1 "27p" H 5315 4030 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5238 3925 50  0001 C CNN
F 3 "~" H 5200 4075 50  0001 C CNN
	1    5200 4075
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 64322ABF
P 5200 4475
AR Path="/63E0266A/64322ABF" Ref="C?"  Part="1" 
AR Path="/64322ABF" Ref="C6"  Part="1" 
F 0 "C6" H 5315 4521 50  0000 L CNN
F 1 "27p" H 5315 4430 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5238 4325 50  0001 C CNN
F 3 "~" H 5200 4475 50  0001 C CNN
	1    5200 4475
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 64322AC5
P 4950 4575
AR Path="/63E0266A/64322AC5" Ref="#PWR?"  Part="1" 
AR Path="/64322AC5" Ref="#PWR012"  Part="1" 
F 0 "#PWR012" H 4950 4325 50  0001 C CNN
F 1 "GND" H 4955 4402 50  0000 C CNN
F 2 "" H 4950 4575 50  0001 C CNN
F 3 "" H 4950 4575 50  0001 C CNN
	1    4950 4575
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 4075 4950 4075
Wire Wire Line
	4950 4075 4950 4475
Wire Wire Line
	5050 4475 4950 4475
Connection ~ 4950 4475
Wire Wire Line
	4950 4475 4950 4575
Wire Wire Line
	5350 4475 5800 4475
Wire Wire Line
	5350 4075 5800 4075
Wire Wire Line
	5800 4125 5800 4075
Wire Wire Line
	5800 4425 5800 4475
Connection ~ 5800 4475
Wire Wire Line
	5800 4475 6350 4475
$Comp
L Device:R R?
U 1 1 64322AD6
P 6500 4475
AR Path="/63E0266A/64322AD6" Ref="R?"  Part="1" 
AR Path="/64322AD6" Ref="R2"  Part="1" 
F 0 "R2" V 6293 4475 50  0000 C CNN
F 1 "1k" V 6384 4475 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6430 4475 50  0001 C CNN
F 3 "~" H 6500 4475 50  0001 C CNN
	1    6500 4475
	0    1    1    0   
$EndComp
$Comp
L Device:Crystal Y?
U 1 1 64322ADC
P 5800 4275
AR Path="/63E0266A/64322ADC" Ref="Y?"  Part="1" 
AR Path="/64322ADC" Ref="Y1"  Part="1" 
F 0 "Y1" V 5754 4406 50  0000 L CNN
F 1 "ABLS-12.000MHZ-B4-T" V 5500 4025 50  0000 L CNN
F 2 "ajm_kicad:ecx-32" H 5800 4275 50  0001 C CNN
F 3 "~" H 5800 4275 50  0001 C CNN
	1    5800 4275
	0    1    1    0   
$EndComp
Text Notes 5475 3750 0    100  ~ 0
Crystal
Connection ~ 5800 4075
Wire Wire Line
	4850 3050 5300 3050
Wire Wire Line
	6025 2250 6025 2300
Wire Wire Line
	5800 3350 5800 3400
Wire Wire Line
	6025 1900 5800 1900
Wire Wire Line
	6025 1950 6025 1900
$Comp
L power:GND #PWR?
U 1 1 64322B11
P 6025 2300
AR Path="/63E6E16A/64322B11" Ref="#PWR?"  Part="1" 
AR Path="/64322B11" Ref="#PWR015"  Part="1" 
F 0 "#PWR015" H 6025 2050 50  0001 C CNN
F 1 "GND" H 6175 2250 50  0000 C CNN
F 2 "" H 6025 2300 50  0001 C CNN
F 3 "" H 6025 2300 50  0001 C CNN
	1    6025 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 64322B17
P 6025 2100
AR Path="/63E6E16A/64322B17" Ref="C?"  Part="1" 
AR Path="/64322B17" Ref="C7"  Part="1" 
F 0 "C7" H 6140 2146 50  0000 L CNN
F 1 "100n" H 6140 2055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6063 1950 50  0001 C CNN
F 3 "~" H 6025 2100 50  0001 C CNN
	1    6025 2100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 64322B1D
P 5800 3400
AR Path="/63E6E16A/64322B1D" Ref="#PWR?"  Part="1" 
AR Path="/64322B1D" Ref="#PWR014"  Part="1" 
F 0 "#PWR014" H 5800 3150 50  0001 C CNN
F 1 "GND" H 5650 3350 50  0000 C CNN
F 2 "" H 5800 3400 50  0001 C CNN
F 3 "" H 5800 3400 50  0001 C CNN
	1    5800 3400
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 64322B23
P 5800 1800
AR Path="/63E6E16A/64322B23" Ref="#PWR?"  Part="1" 
AR Path="/64322B23" Ref="#PWR013"  Part="1" 
F 0 "#PWR013" H 5800 1650 50  0001 C CNN
F 1 "+3V3" H 5815 1973 50  0000 C CNN
F 2 "" H 5800 1800 50  0001 C CNN
F 3 "" H 5800 1800 50  0001 C CNN
	1    5800 1800
	1    0    0    -1  
$EndComp
$Comp
L Memory_Flash:W25Q128JVS U?
U 1 1 64322B29
P 5800 2950
AR Path="/63E6E16A/64322B29" Ref="U?"  Part="1" 
AR Path="/64322B29" Ref="U4"  Part="1" 
F 0 "U4" H 5450 3350 50  0000 C CNN
F 1 "W25Q128JVS" H 6100 2550 50  0000 C CNN
F 2 "Package_SO:SOIC-8_5.23x5.23mm_P1.27mm" H 5800 2950 50  0001 C CNN
F 3 "http://www.winbond.com/resource-files/w25q128jv_dtr%20revc%2003272018%20plus.pdf" H 5800 2950 50  0001 C CNN
	1    5800 2950
	1    0    0    -1  
$EndComp
Text Notes 4525 1800 0    100  ~ 0
Flash
Wire Wire Line
	6300 3150 6950 3150
Wire Wire Line
	5800 1900 5800 2550
$Comp
L Device:R_Small R1
U 1 1 64322B7A
P 4750 2625
F 0 "R1" H 4809 2671 50  0000 L CNN
F 1 "10k" H 4809 2580 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 4750 2625 50  0001 C CNN
F 3 "~" H 4750 2625 50  0001 C CNN
	1    4750 2625
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 64322B80
P 4750 2425
AR Path="/63E6E16A/64322B80" Ref="#PWR?"  Part="1" 
AR Path="/64322B80" Ref="#PWR011"  Part="1" 
F 0 "#PWR011" H 4750 2275 50  0001 C CNN
F 1 "+3V3" H 4765 2598 50  0000 C CNN
F 2 "" H 4750 2425 50  0001 C CNN
F 3 "" H 4750 2425 50  0001 C CNN
	1    4750 2425
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 2425 4750 2525
Wire Wire Line
	4750 2725 4750 2850
$Sheet
S 1625 7075 825  525 
U 64337B55
F0 "EdgeConnector" 50
F1 "/tree/projects/github/casio-fx9000p/pcbs/pico9000-pack/pico-9000-pack/EdgeConnector.sch" 50
$EndSheet
$Sheet
S 600  7050 900  575 
U 643821B4
F0 "Decoupling" 39
F1 "/tree/projects/github/casio-fx9000p/pcbs/pico9000-pack/pico-9000-pack/Decoupling.sch" 39
$EndSheet
Wire Wire Line
	7375 4150 7175 4150
Wire Wire Line
	7175 4150 7175 4075
Wire Wire Line
	5800 4075 7175 4075
Wire Wire Line
	7375 4350 7200 4350
Wire Wire Line
	7200 4350 7200 4475
Wire Wire Line
	6650 4475 7200 4475
Wire Wire Line
	4850 3050 4850 3550
Wire Wire Line
	4850 3550 6875 3550
Wire Wire Line
	6875 3550 6875 3400
Wire Wire Line
	6875 3400 7375 3400
Wire Wire Line
	7225 2750 7225 2950
Wire Wire Line
	7225 2950 7375 2950
Wire Wire Line
	6300 2750 7225 2750
Wire Wire Line
	7175 2850 7175 3050
Wire Wire Line
	7175 3050 7375 3050
Wire Wire Line
	6300 2850 7175 2850
Wire Wire Line
	7125 3050 7125 3150
Wire Wire Line
	7125 3150 7375 3150
Wire Wire Line
	6300 3050 7125 3050
Wire Wire Line
	6950 3150 6950 3250
Wire Wire Line
	6950 3250 7375 3250
Wire Wire Line
	5800 1900 5800 1800
Connection ~ 5800 1900
$Comp
L power:GND #PWR?
U 1 1 644103EF
P 10075 1200
AR Path="/63E0266A/644103EF" Ref="#PWR?"  Part="1" 
AR Path="/644103EF" Ref="#PWR022"  Part="1" 
AR Path="/6431CE78/644103EF" Ref="#PWR?"  Part="1" 
F 0 "#PWR022" H 10075 950 50  0001 C CNN
F 1 "GND" H 10080 1027 50  0000 C CNN
F 2 "" H 10075 1200 50  0001 C CNN
F 3 "" H 10075 1200 50  0001 C CNN
	1    10075 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	10525 1200 10075 1200
Text Notes 9925 1700 0    50   ~ 0
Make sure C12\n is close to pin 44 of RP2040
Wire Wire Line
	9275 750  9275 1350
Connection ~ 9275 750 
Wire Wire Line
	9275 700  9275 750 
Wire Wire Line
	9275 750  9625 750 
Wire Wire Line
	9275 1350 9275 1550
Connection ~ 9275 1350
Wire Wire Line
	9275 1350 9625 1350
$Comp
L power:GND #PWR?
U 1 1 644103FE
P 9625 1750
AR Path="/63E0266A/644103FE" Ref="#PWR?"  Part="1" 
AR Path="/644103FE" Ref="#PWR021"  Part="1" 
AR Path="/6431CE78/644103FE" Ref="#PWR?"  Part="1" 
F 0 "#PWR021" H 9625 1500 50  0001 C CNN
F 1 "GND" H 9630 1577 50  0000 C CNN
F 2 "" H 9625 1750 50  0001 C CNN
F 3 "" H 9625 1750 50  0001 C CNN
	1    9625 1750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 64410404
P 7475 1750
AR Path="/63E0266A/64410404" Ref="#PWR?"  Part="1" 
AR Path="/64410404" Ref="#PWR017"  Part="1" 
AR Path="/6431CE78/64410404" Ref="#PWR?"  Part="1" 
F 0 "#PWR017" H 7475 1500 50  0001 C CNN
F 1 "GND" H 7480 1577 50  0000 C CNN
F 2 "" H 7475 1750 50  0001 C CNN
F 3 "" H 7475 1750 50  0001 C CNN
	1    7475 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9625 1700 9625 1750
Wire Wire Line
	7475 1700 7475 1750
Wire Wire Line
	7475 750  7825 750 
Connection ~ 7475 750 
Wire Wire Line
	7475 800  7475 750 
Wire Wire Line
	7825 750  7825 1350
Wire Wire Line
	7025 750  7475 750 
Wire Wire Line
	7025 800  7025 750 
Wire Wire Line
	7025 1150 7025 1200
Connection ~ 7025 1150
Wire Wire Line
	7475 1150 7475 1100
Wire Wire Line
	7025 1150 7475 1150
Wire Wire Line
	7025 1100 7025 1150
$Comp
L power:GND #PWR?
U 1 1 64410418
P 7025 1200
AR Path="/63E0266A/64410418" Ref="#PWR?"  Part="1" 
AR Path="/64410418" Ref="#PWR016"  Part="1" 
AR Path="/6431CE78/64410418" Ref="#PWR?"  Part="1" 
F 0 "#PWR016" H 7025 950 50  0001 C CNN
F 1 "GND" H 7030 1027 50  0000 C CNN
F 2 "" H 7025 1200 50  0001 C CNN
F 3 "" H 7025 1200 50  0001 C CNN
	1    7025 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9625 1400 9625 1350
Wire Wire Line
	7825 1350 7825 1550
Connection ~ 7825 1350
Wire Wire Line
	7475 1350 7825 1350
Wire Wire Line
	7475 1400 7475 1350
Wire Wire Line
	10525 1100 10525 1200
Wire Wire Line
	10075 1200 9625 1200
Connection ~ 10075 1200
Wire Wire Line
	10075 1100 10075 1200
Wire Wire Line
	9625 1100 9625 1200
Wire Wire Line
	9625 750  10075 750 
Connection ~ 9625 750 
Wire Wire Line
	9625 800  9625 750 
Wire Wire Line
	10075 750  10525 750 
Connection ~ 10075 750 
Wire Wire Line
	10075 800  10075 750 
Wire Wire Line
	10525 800  10525 750 
$Comp
L Device:C C?
U 1 1 6441042F
P 9625 1550
AR Path="/63E0266A/6441042F" Ref="C?"  Part="1" 
AR Path="/6441042F" Ref="C12"  Part="1" 
AR Path="/6431CE78/6441042F" Ref="C?"  Part="1" 
F 0 "C12" H 9740 1596 50  0000 L CNN
F 1 "1u" H 9740 1505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 9663 1400 50  0001 C CNN
F 3 "~" H 9625 1550 50  0001 C CNN
	1    9625 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 64410435
P 7475 1550
AR Path="/63E0266A/64410435" Ref="C?"  Part="1" 
AR Path="/64410435" Ref="C10"  Part="1" 
AR Path="/6431CE78/64410435" Ref="C?"  Part="1" 
F 0 "C10" H 7590 1596 50  0000 L CNN
F 1 "1u" H 7590 1505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7513 1400 50  0001 C CNN
F 3 "~" H 7475 1550 50  0001 C CNN
	1    7475 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 6441043B
P 7475 950
AR Path="/63E0266A/6441043B" Ref="C?"  Part="1" 
AR Path="/6441043B" Ref="C9"  Part="1" 
AR Path="/6431CE78/6441043B" Ref="C?"  Part="1" 
F 0 "C9" H 7590 996 50  0000 L CNN
F 1 "100n" H 7590 905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7513 800 50  0001 C CNN
F 3 "~" H 7475 950 50  0001 C CNN
	1    7475 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 64410441
P 7025 950
AR Path="/63E0266A/64410441" Ref="C?"  Part="1" 
AR Path="/64410441" Ref="C8"  Part="1" 
AR Path="/6431CE78/64410441" Ref="C?"  Part="1" 
F 0 "C8" H 7140 996 50  0000 L CNN
F 1 "100n" H 7140 905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7063 800 50  0001 C CNN
F 3 "~" H 7025 950 50  0001 C CNN
	1    7025 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 64410447
P 10525 950
AR Path="/63E0266A/64410447" Ref="C?"  Part="1" 
AR Path="/64410447" Ref="C14"  Part="1" 
AR Path="/6431CE78/64410447" Ref="C?"  Part="1" 
F 0 "C14" H 10640 996 50  0000 L CNN
F 1 "100n" H 10640 905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 10563 800 50  0001 C CNN
F 3 "~" H 10525 950 50  0001 C CNN
	1    10525 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 6441044D
P 10075 950
AR Path="/63E0266A/6441044D" Ref="C?"  Part="1" 
AR Path="/6441044D" Ref="C13"  Part="1" 
AR Path="/6431CE78/6441044D" Ref="C?"  Part="1" 
F 0 "C13" H 10190 996 50  0000 L CNN
F 1 "100n" H 10190 905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 10113 800 50  0001 C CNN
F 3 "~" H 10075 950 50  0001 C CNN
	1    10075 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 64410453
P 9625 950
AR Path="/63E0266A/64410453" Ref="C?"  Part="1" 
AR Path="/64410453" Ref="C11"  Part="1" 
AR Path="/6431CE78/64410453" Ref="C?"  Part="1" 
F 0 "C11" H 9740 996 50  0000 L CNN
F 1 "100n" H 9740 905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 9663 800 50  0001 C CNN
F 3 "~" H 9625 950 50  0001 C CNN
	1    9625 950 
	1    0    0    -1  
$EndComp
Connection ~ 7825 1550
Connection ~ 9275 1550
$Comp
L power:+3V3 #PWR?
U 1 1 64410462
P 9275 700
AR Path="/63E0266A/64410462" Ref="#PWR?"  Part="1" 
AR Path="/64410462" Ref="#PWR020"  Part="1" 
AR Path="/6431CE78/64410462" Ref="#PWR?"  Part="1" 
F 0 "#PWR020" H 9275 550 50  0001 C CNN
F 1 "+3V3" H 9290 873 50  0000 C CNN
F 2 "" H 9275 700 50  0001 C CNN
F 3 "" H 9275 700 50  0001 C CNN
	1    9275 700 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7925 1550 7825 1550
Connection ~ 7925 1550
Wire Wire Line
	7925 2050 7925 1550
Wire Wire Line
	7825 1550 7825 2050
Wire Wire Line
	8125 1550 7925 1550
Wire Wire Line
	8125 2050 8125 1550
Connection ~ 8475 1550
Wire Wire Line
	8275 1550 8475 1550
Wire Wire Line
	8275 2050 8275 1550
Wire Wire Line
	9125 1550 9275 1550
Connection ~ 9125 1550
Wire Wire Line
	9125 2050 9125 1550
Connection ~ 8975 1550
Wire Wire Line
	9275 1550 9275 2050
Wire Wire Line
	8975 1550 9125 1550
Wire Wire Line
	8875 1550 8975 1550
Connection ~ 8875 1550
Wire Wire Line
	8875 2050 8875 1550
Wire Wire Line
	8775 1550 8875 1550
Connection ~ 8775 1550
Wire Wire Line
	8775 2050 8775 1550
Wire Wire Line
	8675 1550 8775 1550
Connection ~ 8675 1550
Wire Wire Line
	8675 2050 8675 1550
Wire Wire Line
	8575 1550 8675 1550
Connection ~ 8575 1550
Wire Wire Line
	8575 2050 8575 1550
Wire Wire Line
	8975 1550 8975 2050
Wire Wire Line
	8475 1550 8575 1550
Wire Wire Line
	8475 2050 8475 1550
$Comp
L Device:R_Small R3
U 1 1 645C5319
P 10175 2350
F 0 "R3" V 10000 2250 50  0000 C CNN
F 1 "R_Small" V 10070 2350 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 10175 2350 50  0001 C CNN
F 3 "~" H 10175 2350 50  0001 C CNN
	1    10175 2350
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R4
U 1 1 645C5CA0
P 10175 2450
F 0 "R4" V 10250 2375 50  0000 C CNN
F 1 "R_Small" V 10325 2475 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 10175 2450 50  0001 C CNN
F 3 "~" H 10175 2450 50  0001 C CNN
	1    10175 2450
	0    1    1    0   
$EndComp
Wire Wire Line
	9875 2350 10075 2350
Wire Wire Line
	9875 2450 10075 2450
$Comp
L cq1re1-cache:Connector_USB_B_Micro J?
U 1 1 645D8CDF
P 10825 2350
AR Path="/645D3AC7/645D8CDF" Ref="J?"  Part="1" 
AR Path="/645D8CDF" Ref="J2"  Part="1" 
F 0 "J2" H 10882 2817 50  0000 C CNN
F 1 "Connector_USB_B_Micro" H 10882 2726 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_GCT_USB3076-30-A" H 10975 2300 50  0001 C CNN
F 3 "" H 10975 2300 50  0001 C CNN
	1    10825 2350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10275 2350 10525 2350
Wire Wire Line
	10275 2450 10525 2450
$Comp
L power:GND #PWR?
U 1 1 645EDDF7
P 10825 2925
AR Path="/63E0266A/645EDDF7" Ref="#PWR?"  Part="1" 
AR Path="/645EDDF7" Ref="#PWR023"  Part="1" 
AR Path="/6431CE78/645EDDF7" Ref="#PWR?"  Part="1" 
F 0 "#PWR023" H 10825 2675 50  0001 C CNN
F 1 "GND" H 10830 2752 50  0000 C CNN
F 2 "" H 10825 2925 50  0001 C CNN
F 3 "" H 10825 2925 50  0001 C CNN
	1    10825 2925
	1    0    0    -1  
$EndComp
Wire Wire Line
	10825 2750 10825 2825
Wire Wire Line
	10825 2825 10925 2825
Wire Wire Line
	10925 2825 10925 2750
Connection ~ 10825 2825
Wire Wire Line
	10825 2825 10825 2925
Text GLabel 10525 2150 0    39   Input ~ 0
VBUS_BUS
$Sheet
S 5500 7100 725  500 
U 646543AD
F0 "Power" 39
F1 "/tree/projects/github/casio-fx9000p/pcbs/pico9000-pack/pico-9000-pack/Power.sch" 39
$EndSheet
Wire Wire Line
	5700 5275 7175 5275
Wire Wire Line
	7175 5275 7175 5300
Wire Wire Line
	7175 5300 7375 5300
Wire Wire Line
	7375 5400 6950 5400
Wire Wire Line
	6950 5400 6950 5475
Wire Wire Line
	5700 5475 6950 5475
Wire Wire Line
	6575 5725 6575 5375
Wire Wire Line
	6575 5375 5700 5375
$Sheet
S 4600 7150 625  400 
U 6469EF32
F0 "LogicProc" 39
F1 "/tree/projects/github/casio-fx9000p/pcbs/pico9000-pack/pico-9000-pack/LogicProc.sch" 39
$EndSheet
Wire Wire Line
	4750 2850 5125 2850
Wire Wire Line
	7375 2475 5125 2475
Wire Wire Line
	5125 2475 5125 2850
Wire Wire Line
	7375 2475 7375 2800
Connection ~ 5125 2850
Wire Wire Line
	5125 2850 5300 2850
Text GLabel 2850 5725 2    50   Input ~ 0
P_INPUT0
Text GLabel 2500 6000 2    50   Input ~ 0
P_INPUT1
Text GLabel 2475 6325 2    50   Input ~ 0
DATA_DIR_HW
Text Notes 3275 6350 0    50   ~ 0
Level shifter direction control
Wire Wire Line
	2500 6000 1050 6000
Text GLabel 1050 6000 0    50   Input ~ 0
RD_N
Text GLabel 1225 5725 0    50   Input ~ 0
SELECTED
Wire Wire Line
	1225 5725 2850 5725
Text Notes 3275 5825 0    50   ~ 0
INPUT0 indicates to Pico that the cartridge has been\nselected
Text Notes 3250 6050 0    50   ~ 0
INPUT1 tells Pico the data direction
Text GLabel 1525 6325 0    50   Input ~ 0
SELECTED_AND_RD
Wire Wire Line
	2475 6325 1525 6325
Wire Wire Line
	9875 5750 10075 5750
Text GLabel 10050 4950 2    50   Input ~ 0
PSEUDO_A14
Wire Wire Line
	9875 4950 10050 4950
$EndSCHEMATC
