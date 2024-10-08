EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 3425 4150 0    50   Input ~ 0
RX_3V3
$Comp
L power:+5V #PWR0110
U 1 1 66554613
P 3975 1350
F 0 "#PWR0110" H 3975 1200 50  0001 C CNN
F 1 "+5V" H 3990 1523 50  0000 C CNN
F 2 "" H 3975 1350 50  0001 C CNN
F 3 "" H 3975 1350 50  0001 C CNN
	1    3975 1350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 6655461F
P 3825 2300
F 0 "#PWR0111" H 3825 2050 50  0001 C CNN
F 1 "GND" H 3830 2127 50  0000 C CNN
F 2 "" H 3825 2300 50  0001 C CNN
F 3 "" H 3825 2300 50  0001 C CNN
	1    3825 2300
	1    0    0    -1  
$EndComp
Text GLabel 4525 4150 2    50   Input ~ 0
RX_5V
Text GLabel 7425 2850 0    50   Input ~ 0
TX_5V
Text GLabel 8550 2850 2    50   Input ~ 0
TX_3V3
$Comp
L ajm:cd40109 U2
U 1 1 664A0294
P 3925 4150
F 0 "U2" H 3925 3925 50  0000 C CNN
F 1 "cd40109" H 3925 3834 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 3725 4450 50  0001 C CNN
F 3 "" H 3725 4450 50  0001 C CNN
	1    3925 4150
	1    0    0    -1  
$EndComp
$Comp
L ajm:cd40109 U2
U 5 1 664A09EF
P 3725 1825
F 0 "U2" H 4103 1846 50  0000 L CNN
F 1 "cd40109" H 4103 1755 50  0000 L CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 3525 2125 50  0001 C CNN
F 3 "" H 3525 2125 50  0001 C CNN
	5    3725 1825
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR01
U 1 1 664A1BF7
P 3625 1350
F 0 "#PWR01" H 3625 1200 50  0001 C CNN
F 1 "+3V3" H 3640 1523 50  0000 C CNN
F 2 "" H 3625 1350 50  0001 C CNN
F 3 "" H 3625 1350 50  0001 C CNN
	1    3625 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3625 1350 3625 1525
Wire Wire Line
	3975 1350 3975 1525
Wire Wire Line
	3825 2300 3825 2175
Wire Wire Line
	3425 4150 3625 4150
Wire Wire Line
	4225 4150 4525 4150
$Comp
L ajm:cd40109 U3
U 1 1 664A6D48
P 8025 2850
F 0 "U3" H 7575 3075 50  0000 C CNN
F 1 "cd40109" H 7650 3000 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 3150 50  0001 C CNN
F 3 "" H 7825 3150 50  0001 C CNN
	1    8025 2850
	1    0    0    -1  
$EndComp
$Comp
L ajm:cd40109 U3
U 5 1 664A7610
P 7450 1775
F 0 "U3" H 7828 1796 50  0000 L CNN
F 1 "cd40109" H 7828 1705 50  0000 L CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7250 2075 50  0001 C CNN
F 3 "" H 7250 2075 50  0001 C CNN
	5    7450 1775
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR06
U 1 1 664ADE9A
P 7350 1350
F 0 "#PWR06" H 7350 1200 50  0001 C CNN
F 1 "+5V" H 7365 1523 50  0000 C CNN
F 2 "" H 7350 1350 50  0001 C CNN
F 3 "" H 7350 1350 50  0001 C CNN
	1    7350 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR08
U 1 1 664B0ACF
P 7700 1350
F 0 "#PWR08" H 7700 1200 50  0001 C CNN
F 1 "+3V3" H 7715 1523 50  0000 C CNN
F 2 "" H 7700 1350 50  0001 C CNN
F 3 "" H 7700 1350 50  0001 C CNN
	1    7700 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 1350 7350 1475
Wire Wire Line
	7700 1350 7700 1475
$Comp
L power:GND #PWR07
U 1 1 664B2BB7
P 7550 2275
F 0 "#PWR07" H 7550 2025 50  0001 C CNN
F 1 "GND" H 7555 2102 50  0000 C CNN
F 2 "" H 7550 2275 50  0001 C CNN
F 3 "" H 7550 2275 50  0001 C CNN
	1    7550 2275
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 2275 7550 2125
Text Notes 3625 850  0    50   ~ 0
Shift UP\n3V3 to 5V
Text Notes 7350 800  0    50   ~ 0
Shift DOWN\n5V to 3V3
Wire Notes Line
	6825 6300 6825 725 
Wire Notes Line
	2750 700  2875 700 
Wire Notes Line
	2875 700  2875 7425
Wire Wire Line
	7425 2850 7725 2850
Wire Wire Line
	8325 2850 8550 2850
$Comp
L Device:C_Small C14
U 1 1 664C1DCD
P 1700 1200
F 0 "C14" H 1792 1246 50  0000 L CNN
F 1 "100nF" H 1792 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1700 1200 50  0001 C CNN
F 3 "~" H 1700 1200 50  0001 C CNN
	1    1700 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C15
U 1 1 664C1DD3
P 2100 1200
F 0 "C15" H 2192 1246 50  0000 L CNN
F 1 "100nF" H 2192 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2100 1200 50  0001 C CNN
F 3 "~" H 2100 1200 50  0001 C CNN
	1    2100 1200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 664C23AD
P 1900 900
F 0 "#PWR04" H 1900 750 50  0001 C CNN
F 1 "+5V" H 1915 1073 50  0000 C CNN
F 2 "" H 1900 900 50  0001 C CNN
F 3 "" H 1900 900 50  0001 C CNN
	1    1900 900 
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR02
U 1 1 664C23B3
P 975 925
F 0 "#PWR02" H 975 775 50  0001 C CNN
F 1 "+3V3" H 990 1098 50  0000 C CNN
F 2 "" H 975 925 50  0001 C CNN
F 3 "" H 975 925 50  0001 C CNN
	1    975  925 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 664C2A3F
P 1925 1650
F 0 "#PWR05" H 1925 1400 50  0001 C CNN
F 1 "GND" H 1930 1477 50  0000 C CNN
F 2 "" H 1925 1650 50  0001 C CNN
F 3 "" H 1925 1650 50  0001 C CNN
	1    1925 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 1100 1700 1025
Wire Wire Line
	1700 1025 1900 1025
Wire Wire Line
	1900 1025 1900 900 
Wire Wire Line
	1900 1025 2100 1025
Wire Wire Line
	2100 1025 2100 1100
Connection ~ 1900 1025
Wire Wire Line
	1700 1300 1700 1425
Wire Wire Line
	1700 1425 1925 1425
Wire Wire Line
	1925 1425 1925 1650
Wire Wire Line
	2100 1425 2100 1300
Wire Wire Line
	1925 1425 2100 1425
Connection ~ 1925 1425
$Comp
L Device:C_Small C12
U 1 1 664C71F0
P 775 1225
F 0 "C12" H 867 1271 50  0000 L CNN
F 1 "100nF" H 867 1180 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 775 1225 50  0001 C CNN
F 3 "~" H 775 1225 50  0001 C CNN
	1    775  1225
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C13
U 1 1 664C71F6
P 1175 1225
F 0 "C13" H 1267 1271 50  0000 L CNN
F 1 "100nF" H 1267 1180 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1175 1225 50  0001 C CNN
F 3 "~" H 1175 1225 50  0001 C CNN
	1    1175 1225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 664C7202
P 1000 1675
F 0 "#PWR03" H 1000 1425 50  0001 C CNN
F 1 "GND" H 1005 1502 50  0000 C CNN
F 2 "" H 1000 1675 50  0001 C CNN
F 3 "" H 1000 1675 50  0001 C CNN
	1    1000 1675
	1    0    0    -1  
$EndComp
Wire Wire Line
	775  1125 775  1050
Wire Wire Line
	775  1050 975  1050
Wire Wire Line
	975  1050 975  925 
Wire Wire Line
	975  1050 1175 1050
Wire Wire Line
	1175 1050 1175 1125
Connection ~ 975  1050
Wire Wire Line
	775  1325 775  1450
Wire Wire Line
	775  1450 1000 1450
Wire Wire Line
	1000 1450 1000 1675
Wire Wire Line
	1175 1450 1175 1325
Wire Wire Line
	1000 1450 1175 1450
Connection ~ 1000 1450
$Comp
L ajm:cd40109 U3
U 2 1 664C8E0A
P 8025 3375
F 0 "U3" H 7550 3575 50  0000 C CNN
F 1 "cd40109" H 7625 3500 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 3675 50  0001 C CNN
F 3 "" H 7825 3675 50  0001 C CNN
	2    8025 3375
	1    0    0    -1  
$EndComp
Text GLabel 7475 3375 0    50   Input ~ 0
DRDY_5V
Text GLabel 8625 3375 2    50   Input ~ 0
DRDY_3V3
Wire Wire Line
	8625 3375 8325 3375
Wire Wire Line
	7725 3375 7475 3375
Text GLabel 4575 4800 2    50   Input ~ 0
MOSI_5V
Text GLabel 7475 3925 0    50   Input ~ 0
MISO_5V
Text GLabel 4600 5475 2    50   Input ~ 0
SCK_5V
$Comp
L ajm:cd40109 U3
U 3 1 664E659D
P 8025 3925
F 0 "U3" H 7525 4150 50  0000 C CNN
F 1 "cd40109" H 7600 4075 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 4225 50  0001 C CNN
F 3 "" H 7825 4225 50  0001 C CNN
	3    8025 3925
	1    0    0    -1  
$EndComp
Wire Wire Line
	7475 3925 7725 3925
Text GLabel 8675 3925 2    50   Input ~ 0
MISO_3V3
Wire Wire Line
	8325 3925 8675 3925
$Comp
L ajm:cd40109 U2
U 3 1 664E9FC5
P 3950 5475
F 0 "U2" H 3950 5250 50  0000 C CNN
F 1 "cd40109" H 3950 5159 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 3750 5775 50  0001 C CNN
F 3 "" H 3750 5775 50  0001 C CNN
	3    3950 5475
	1    0    0    -1  
$EndComp
$Comp
L ajm:cd40109 U2
U 2 1 664EAA8B
P 3950 4800
F 0 "U2" H 3950 4575 50  0000 C CNN
F 1 "cd40109" H 3950 4484 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 3750 5100 50  0001 C CNN
F 3 "" H 3750 5100 50  0001 C CNN
	2    3950 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 4800 4575 4800
Wire Wire Line
	4250 5475 4600 5475
Text GLabel 3425 4800 0    50   Input ~ 0
MOSI_3V3
Text GLabel 3400 5475 0    50   Input ~ 0
SCK_3V3
Wire Wire Line
	3425 4800 3650 4800
Wire Wire Line
	3400 5475 3650 5475
Text GLabel 7475 4500 0    50   Input ~ 0
DBG_DATA_5V
$Comp
L ajm:cd40109 U3
U 4 1 6659CB01
P 8025 4500
F 0 "U3" H 7500 4675 50  0000 C CNN
F 1 "cd40109" H 7600 4600 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 4800 50  0001 C CNN
F 3 "" H 7825 4800 50  0001 C CNN
	4    8025 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7475 4500 7725 4500
Text GLabel 8675 4500 2    50   Input ~ 0
DBG_DATA_3V3
Wire Wire Line
	8325 4500 8675 4500
$Comp
L power:+5V #PWR012
U 1 1 6659DFB3
P 9425 2450
F 0 "#PWR012" H 9425 2300 50  0001 C CNN
F 1 "+5V" H 9440 2623 50  0000 C CNN
F 2 "" H 9425 2450 50  0001 C CNN
F 3 "" H 9425 2450 50  0001 C CNN
	1    9425 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8025 2650 8025 2775
Wire Wire Line
	9425 2450 9425 2650
Wire Wire Line
	9425 2650 8025 2650
Connection ~ 8025 2650
Wire Wire Line
	9425 2650 9425 3175
Wire Wire Line
	9425 3175 8025 3175
Connection ~ 9425 2650
Wire Wire Line
	9425 3175 9425 3725
Wire Wire Line
	9425 3725 8025 3725
Connection ~ 9425 3175
Wire Wire Line
	9425 3725 9425 4300
Wire Wire Line
	9425 4300 8025 4300
Connection ~ 9425 3725
$Comp
L ajm:cd40109 U4
U 5 1 665A925A
P 8600 1775
F 0 "U4" H 8978 1796 50  0000 L CNN
F 1 "cd40109" H 8978 1705 50  0000 L CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 8400 2075 50  0001 C CNN
F 3 "" H 8400 2075 50  0001 C CNN
	5    8600 1775
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR09
U 1 1 665A9260
P 8500 1350
F 0 "#PWR09" H 8500 1200 50  0001 C CNN
F 1 "+5V" H 8515 1523 50  0000 C CNN
F 2 "" H 8500 1350 50  0001 C CNN
F 3 "" H 8500 1350 50  0001 C CNN
	1    8500 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR011
U 1 1 665A9266
P 8850 1350
F 0 "#PWR011" H 8850 1200 50  0001 C CNN
F 1 "+3V3" H 8865 1523 50  0000 C CNN
F 2 "" H 8850 1350 50  0001 C CNN
F 3 "" H 8850 1350 50  0001 C CNN
	1    8850 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 1350 8500 1475
Wire Wire Line
	8850 1350 8850 1475
$Comp
L power:GND #PWR010
U 1 1 665A926E
P 8700 2275
F 0 "#PWR010" H 8700 2025 50  0001 C CNN
F 1 "GND" H 8705 2102 50  0000 C CNN
F 2 "" H 8700 2275 50  0001 C CNN
F 3 "" H 8700 2275 50  0001 C CNN
	1    8700 2275
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 2275 8700 2125
Text GLabel 7475 5075 0    50   Input ~ 0
DBG_CLK_5V
$Comp
L ajm:cd40109 U4
U 2 1 665ABB8B
P 8025 5075
F 0 "U4" H 7500 5250 50  0000 C CNN
F 1 "cd40109" H 7600 5175 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 5375 50  0001 C CNN
F 3 "" H 7825 5375 50  0001 C CNN
	2    8025 5075
	1    0    0    -1  
$EndComp
Wire Wire Line
	7475 5075 7725 5075
Text GLabel 8675 5075 2    50   Input ~ 0
DBG_CLK3V3
Wire Wire Line
	8325 5075 8675 5075
Wire Wire Line
	9425 4875 8025 4875
Wire Wire Line
	9425 4875 9425 4300
Connection ~ 9425 4300
$Comp
L power:+3V3 #PWR0112
U 1 1 665E6306
P 5125 3800
F 0 "#PWR0112" H 5125 3650 50  0001 C CNN
F 1 "+3V3" H 5140 3973 50  0000 C CNN
F 2 "" H 5125 3800 50  0001 C CNN
F 3 "" H 5125 3800 50  0001 C CNN
	1    5125 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5125 3800 3925 3800
Wire Wire Line
	3925 3800 3925 3950
Wire Wire Line
	5125 3800 5125 4600
Wire Wire Line
	5125 4600 3950 4600
Connection ~ 5125 3800
Wire Wire Line
	3950 5275 5125 5275
Wire Wire Line
	5125 5275 5125 4600
Connection ~ 5125 4600
Text GLabel 4600 6100 2    50   Input ~ 0
WS_5V
$Comp
L ajm:cd40109 U2
U 4 1 665EE394
P 3950 6100
F 0 "U2" H 3950 5875 50  0000 C CNN
F 1 "cd40109" H 3950 5784 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 3750 6400 50  0001 C CNN
F 3 "" H 3750 6400 50  0001 C CNN
	4    3950 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 6100 4600 6100
Text GLabel 3400 6100 0    50   Input ~ 0
WS_3V3
Wire Wire Line
	3400 6100 3650 6100
Wire Wire Line
	3950 5900 5125 5900
Wire Wire Line
	5125 5900 5125 5275
Connection ~ 5125 5275
$Comp
L ajm:cd40109 U4
U 1 1 6667B2DF
P 8025 5525
F 0 "U4" H 7500 5700 50  0000 C CNN
F 1 "cd40109" H 7600 5625 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 5825 50  0001 C CNN
F 3 "" H 7825 5825 50  0001 C CNN
	1    8025 5525
	1    0    0    -1  
$EndComp
$Comp
L ajm:cd40109 U4
U 3 1 6667CD2D
P 8025 5900
F 0 "U4" H 7500 6075 50  0000 C CNN
F 1 "cd40109" H 7600 6000 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 6200 50  0001 C CNN
F 3 "" H 7825 6200 50  0001 C CNN
	3    8025 5900
	1    0    0    -1  
$EndComp
$Comp
L ajm:cd40109 U4
U 4 1 6667EBFA
P 8025 6275
F 0 "U4" H 7500 6450 50  0000 C CNN
F 1 "cd40109" H 7600 6375 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7825 6575 50  0001 C CNN
F 3 "" H 7825 6575 50  0001 C CNN
	4    8025 6275
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 66684469
P 7050 6125
F 0 "#PWR013" H 7050 5875 50  0001 C CNN
F 1 "GND" H 7055 5952 50  0000 C CNN
F 2 "" H 7050 6125 50  0001 C CNN
F 3 "" H 7050 6125 50  0001 C CNN
	1    7050 6125
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 6125 7050 5900
Wire Wire Line
	7050 5900 7325 5900
Wire Wire Line
	7325 5525 7325 5900
Connection ~ 7325 5900
Wire Wire Line
	7325 5900 7725 5900
Wire Wire Line
	7325 5900 7325 6275
Wire Wire Line
	7325 6275 7725 6275
Wire Wire Line
	8025 5325 9425 5325
Wire Wire Line
	9425 5325 9425 4875
Connection ~ 9425 4875
Wire Wire Line
	8025 5700 9425 5700
Wire Wire Line
	9425 5700 9425 5325
Connection ~ 9425 5325
Wire Wire Line
	8025 6075 9425 6075
Wire Wire Line
	9425 6075 9425 5700
Connection ~ 9425 5700
Wire Wire Line
	7325 5525 7725 5525
$EndSCHEMATC
