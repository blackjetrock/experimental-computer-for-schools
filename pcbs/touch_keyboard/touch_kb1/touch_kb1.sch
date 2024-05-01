EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L New_Library:Si2302CDS-n-channel TR?
U 1 1 66327D0E
P 5050 2700
F 0 "TR?" H 5328 2746 50  0000 L CNN
F 1 "Si2302CDS-n-channel" H 5328 2655 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5080 2850 20  0001 C CNN
F 3 "" H 5050 2700 50  0001 C CNN
	1    5050 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 66328EFF
P 2525 2550
F 0 "R?" V 2329 2550 50  0000 C CNN
F 1 "R_Small" V 2420 2550 50  0000 C CNN
F 2 "" H 2525 2550 50  0001 C CNN
F 3 "~" H 2525 2550 50  0001 C CNN
	1    2525 2550
	0    1    1    0   
$EndComp
$Comp
L Device:D D?
U 1 1 6632921F
P 3050 2550
F 0 "D?" H 3050 2767 50  0000 C CNN
F 1 "D" H 3050 2676 50  0000 C CNN
F 2 "" H 3050 2550 50  0001 C CNN
F 3 "~" H 3050 2550 50  0001 C CNN
	1    3050 2550
	-1   0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 66329764
P 4275 3250
F 0 "C?" H 4367 3296 50  0000 L CNN
F 1 "C_Small" H 4367 3205 50  0000 L CNN
F 2 "" H 4275 3250 50  0001 C CNN
F 3 "~" H 4275 3250 50  0001 C CNN
	1    4275 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 66329C81
P 2525 3000
F 0 "R?" V 2329 3000 50  0000 C CNN
F 1 "R_Small" V 2420 3000 50  0000 C CNN
F 2 "" H 2525 3000 50  0001 C CNN
F 3 "~" H 2525 3000 50  0001 C CNN
	1    2525 3000
	0    1    1    0   
$EndComp
$Comp
L Device:D D?
U 1 1 6632A879
P 3075 3000
F 0 "D?" H 3075 3217 50  0000 C CNN
F 1 "D" H 3075 3126 50  0000 C CNN
F 2 "" H 3075 3000 50  0001 C CNN
F 3 "~" H 3075 3000 50  0001 C CNN
	1    3075 3000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6632AF4E
P 5150 3525
F 0 "#PWR?" H 5150 3275 50  0001 C CNN
F 1 "GND" H 5155 3352 50  0000 C CNN
F 2 "" H 5150 3525 50  0001 C CNN
F 3 "" H 5150 3525 50  0001 C CNN
	1    5150 3525
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6632B3A1
P 4275 3525
F 0 "#PWR?" H 4275 3275 50  0001 C CNN
F 1 "GND" H 4280 3352 50  0000 C CNN
F 2 "" H 4275 3525 50  0001 C CNN
F 3 "" H 4275 3525 50  0001 C CNN
	1    4275 3525
	1    0    0    -1  
$EndComp
Wire Wire Line
	3225 3000 4275 3000
Wire Wire Line
	4275 3000 4275 3150
Wire Wire Line
	3200 2550 4275 2550
Wire Wire Line
	4275 2550 4275 2800
Connection ~ 4275 3000
Wire Wire Line
	4850 2800 4275 2800
Connection ~ 4275 2800
Wire Wire Line
	4275 2800 4275 3000
Wire Wire Line
	4275 3525 4275 3350
Wire Wire Line
	5150 3525 5150 3000
$Comp
L Device:R_Small R?
U 1 1 6632EDBC
P 5150 2100
F 0 "R?" H 5209 2146 50  0000 L CNN
F 1 "2k2" H 5209 2055 50  0000 L CNN
F 2 "" H 5150 2100 50  0001 C CNN
F 3 "~" H 5150 2100 50  0001 C CNN
	1    5150 2100
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 6632FA3B
P 5150 1650
F 0 "#PWR?" H 5150 1500 50  0001 C CNN
F 1 "+3V3" H 5165 1823 50  0000 C CNN
F 2 "" H 5150 1650 50  0001 C CNN
F 3 "" H 5150 1650 50  0001 C CNN
	1    5150 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 1650 5150 2000
Wire Wire Line
	5150 2200 5150 2300
$Comp
L 74xx:74HC165 U?
U 1 1 66330903
P 9700 2600
F 0 "U?" H 9700 3681 50  0000 C CNN
F 1 "74HC165" H 9700 3590 50  0000 C CNN
F 2 "" H 9700 2600 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74HC_HCT165.pdf" H 9700 2600 50  0001 C CNN
	1    9700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 2550 1800 2775
$Comp
L Connector:TestPoint TP?
U 1 1 66331F4D
P 1275 2650
F 0 "TP?" H 1333 2768 50  0000 L CNN
F 1 "TestPoint" H 1333 2677 50  0000 L CNN
F 2 "" H 1475 2650 50  0001 C CNN
F 3 "~" H 1475 2650 50  0001 C CNN
	1    1275 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1275 2650 1275 2775
Wire Wire Line
	1275 2775 1800 2775
Connection ~ 1800 2775
Wire Wire Line
	1800 2775 1800 3000
$Comp
L Connector:TestPoint TP?
U 1 1 66332761
P 6950 2300
F 0 "TP?" V 6904 2488 50  0000 L CNN
F 1 "TestPoint" V 6995 2488 50  0000 L CNN
F 2 "" H 7150 2300 50  0001 C CNN
F 3 "~" H 7150 2300 50  0001 C CNN
	1    6950 2300
	0    1    1    0   
$EndComp
Wire Wire Line
	6950 2300 5150 2300
Connection ~ 5150 2300
Wire Wire Line
	5150 2300 5150 2400
Wire Wire Line
	2425 3000 1800 3000
Wire Wire Line
	2425 2550 1800 2550
Wire Wire Line
	2625 2550 2900 2550
Wire Wire Line
	2925 3000 2850 3000
$Comp
L Device:D D?
U 1 1 66336DC3
P 2475 3350
F 0 "D?" H 2475 3133 50  0000 C CNN
F 1 "D" H 2475 3224 50  0000 C CNN
F 2 "" H 2475 3350 50  0001 C CNN
F 3 "~" H 2475 3350 50  0001 C CNN
	1    2475 3350
	-1   0    0    1   
$EndComp
Wire Wire Line
	2325 3350 1800 3350
Wire Wire Line
	1800 3350 1800 3000
Connection ~ 1800 3000
Wire Wire Line
	2625 3350 2850 3350
Wire Wire Line
	2850 3350 2850 3000
Connection ~ 2850 3000
Wire Wire Line
	2850 3000 2625 3000
$Comp
L Connector:TestPoint TP?
U 1 1 663384A9
P 2850 3675
F 0 "TP?" H 2792 3701 50  0000 R CNN
F 1 "TestPoint" H 2792 3792 50  0000 R CNN
F 2 "" H 3050 3675 50  0001 C CNN
F 3 "~" H 3050 3675 50  0001 C CNN
	1    2850 3675
	-1   0    0    1   
$EndComp
Wire Wire Line
	2850 3675 2850 3350
Connection ~ 2850 3350
Text Notes 2900 4025 0    50   ~ 0
Touch\nContact
Text Notes 975  2675 0    50   ~ 0
Drive\nSignal
$EndSCHEMATC
