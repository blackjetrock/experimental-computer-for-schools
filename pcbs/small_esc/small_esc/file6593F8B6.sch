EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
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
L rp2040:Pico U7
U 1 1 6366AE32
P 2025 1800
F 0 "U7" H 2025 3015 50  0000 C CNN
F 1 "Pico" H 2025 2924 50  0000 C CNN
F 2 "rp2040:RPi_Pico_SMD_TH" V 2025 1800 50  0001 C CNN
F 3 "" H 2025 1800 50  0001 C CNN
	1    2025 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x08_Odd_Even J?
U 1 1 636773E8
P 10075 2075
AR Path="/6366C142/636773E8" Ref="J?"  Part="1" 
AR Path="/636773E8" Ref="J?"  Part="1" 
AR Path="/6593F8B7/636773E8" Ref="J8"  Part="1" 
F 0 "J8" H 10125 2592 50  0000 C CNN
F 1 "Conn_02x08_Odd_Even" H 10125 2501 50  0000 C CNN
F 2 "ajm_kicad:PinHeader_2x08_P2.54mm_Horizontal_psionSide" H 10075 2075 50  0001 C CNN
F 3 "~" H 10075 2075 50  0001 C CNN
	1    10075 2075
	1    0    0    -1  
$EndComp
Text GLabel 10375 1775 2    50   Input ~ 0
P_SD0
Text GLabel 9875 1775 0    50   Input ~ 0
P_SD1
Text GLabel 10375 1875 2    50   Input ~ 0
P_SD2
Text GLabel 9875 1875 0    50   Input ~ 0
P_SD3
Text GLabel 10375 1975 2    50   Input ~ 0
P_SD4
Text GLabel 9875 1975 0    50   Input ~ 0
P_SD5
Text GLabel 10375 2075 2    50   Input ~ 0
P_SD6
Text GLabel 9875 2075 0    50   Input ~ 0
P_SD7
Text GLabel 9875 2175 0    50   Input ~ 0
P_SMR
Text GLabel 10375 2175 2    50   Input ~ 0
P_SCLK
Text GLabel 9875 2275 0    50   Input ~ 0
P_SOE
Text GLabel 10375 2275 2    50   Input ~ 0
P_SS1
$Comp
L power:GND #PWR?
U 1 1 636773FA
P 9650 2700
AR Path="/6366C142/636773FA" Ref="#PWR?"  Part="1" 
AR Path="/636773FA" Ref="#PWR?"  Part="1" 
AR Path="/6593F8B7/636773FA" Ref="#PWR065"  Part="1" 
F 0 "#PWR065" H 9650 2450 50  0001 C CNN
F 1 "GND" H 9655 2527 50  0000 C CNN
F 2 "" H 9650 2700 50  0001 C CNN
F 3 "" H 9650 2700 50  0001 C CNN
	1    9650 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 2375 9875 2375
Wire Wire Line
	9650 2375 9650 2700
Text GLabel 9350 2475 0    50   Input ~ 0
VCC_SLOT
Wire Wire Line
	9875 2475 9350 2475
Text GLabel 10375 2375 2    50   Input ~ 0
P_SPGM
Text GLabel 10375 2475 2    50   Input ~ 0
VPP_DRIVE
$Comp
L 74xx:74LS245 U8
U 1 1 63677927
P 4850 2325
F 0 "U8" H 5100 3325 50  0000 C CNN
F 1 "74LS245" H 5225 3250 50  0000 C CNN
F 2 "Package_SO:SOP-20_7.5x12.8mm_P1.27mm" H 4850 2325 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS245" H 4850 2325 50  0001 C CNN
	1    4850 2325
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS245 U9
U 1 1 63678AE8
P 4900 4675
F 0 "U9" H 5100 5600 50  0000 C CNN
F 1 "74LS245" H 5225 5475 50  0000 C CNN
F 2 "Package_SO:SOP-20_7.5x12.8mm_P1.27mm" H 4900 4675 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS245" H 4900 4675 50  0001 C CNN
	1    4900 4675
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR046
U 1 1 6367C304
P 2025 3225
F 0 "#PWR046" H 2025 2975 50  0001 C CNN
F 1 "GND" H 2030 3052 50  0000 C CNN
F 2 "" H 2025 3225 50  0001 C CNN
F 3 "" H 2025 3225 50  0001 C CNN
	1    2025 3225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR060
U 1 1 6367C6EB
P 6575 1300
F 0 "#PWR060" H 6575 1050 50  0001 C CNN
F 1 "GND" H 6580 1127 50  0000 C CNN
F 2 "" H 6575 1300 50  0001 C CNN
F 3 "" H 6575 1300 50  0001 C CNN
	1    6575 1300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR057
U 1 1 6367CA98
P 4900 5625
F 0 "#PWR057" H 4900 5375 50  0001 C CNN
F 1 "GND" H 4905 5452 50  0000 C CNN
F 2 "" H 4900 5625 50  0001 C CNN
F 3 "" H 4900 5625 50  0001 C CNN
	1    4900 5625
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR051
U 1 1 6367D4B6
P 4250 850
F 0 "#PWR051" H 4250 700 50  0001 C CNN
F 1 "+3V3" H 4265 1023 50  0000 C CNN
F 2 "" H 4250 850 50  0001 C CNN
F 3 "" H 4250 850 50  0001 C CNN
	1    4250 850 
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR056
U 1 1 6367D50D
P 4900 3725
F 0 "#PWR056" H 4900 3575 50  0001 C CNN
F 1 "+3V3" H 4915 3898 50  0000 C CNN
F 2 "" H 4900 3725 50  0001 C CNN
F 3 "" H 4900 3725 50  0001 C CNN
	1    4900 3725
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR054
U 1 1 6367D984
P 4850 1275
F 0 "#PWR054" H 4850 1125 50  0001 C CNN
F 1 "+3V3" H 4865 1448 50  0000 C CNN
F 2 "" H 4850 1275 50  0001 C CNN
F 3 "" H 4850 1275 50  0001 C CNN
	1    4850 1275
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 1275 4850 1525
Wire Wire Line
	4250 1250 2725 1250
Wire Wire Line
	4850 3125 4850 3250
Wire Wire Line
	4900 5475 4900 5525
Wire Wire Line
	2025 2950 2025 3225
$Comp
L power:GND #PWR044
U 1 1 63680DDA
P 1025 3225
F 0 "#PWR044" H 1025 2975 50  0001 C CNN
F 1 "GND" H 1030 3052 50  0000 C CNN
F 2 "" H 1025 3225 50  0001 C CNN
F 3 "" H 1025 3225 50  0001 C CNN
	1    1025 3225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR048
U 1 1 6368129A
P 3175 3225
F 0 "#PWR048" H 3175 2975 50  0001 C CNN
F 1 "GND" H 3180 3052 50  0000 C CNN
F 2 "" H 3175 3225 50  0001 C CNN
F 3 "" H 3175 3225 50  0001 C CNN
	1    3175 3225
	1    0    0    -1  
$EndComp
Wire Wire Line
	1025 3225 1025 2550
Wire Wire Line
	1025 2550 1325 2550
Wire Wire Line
	1325 2050 1025 2050
Wire Wire Line
	1025 2050 1025 2550
Connection ~ 1025 2550
Wire Wire Line
	3175 2550 2725 2550
Wire Wire Line
	3175 2550 3175 2050
Wire Wire Line
	3175 2050 2725 2050
Connection ~ 3175 2550
Wire Wire Line
	3175 1550 2725 1550
Wire Wire Line
	1025 2050 1025 1550
Wire Wire Line
	1025 1550 1325 1550
Connection ~ 1025 2050
Wire Wire Line
	1325 1050 1025 1050
Wire Wire Line
	1025 1050 1025 1550
Connection ~ 1025 1550
Wire Wire Line
	2725 1050 3175 1050
Wire Wire Line
	3175 1050 3175 1550
Connection ~ 3175 1550
Text GLabel 5350 1825 2    50   Input ~ 0
P_SD0
Text GLabel 5350 1925 2    50   Input ~ 0
P_SD1
Text GLabel 5350 2025 2    50   Input ~ 0
P_SD2
Text GLabel 5350 2125 2    50   Input ~ 0
P_SD3
Text GLabel 5350 2225 2    50   Input ~ 0
P_SD4
Text GLabel 5350 2325 2    50   Input ~ 0
P_SD5
Text GLabel 5350 2425 2    50   Input ~ 0
P_SD6
Text GLabel 5350 2525 2    50   Input ~ 0
P_SD7
Text GLabel 4350 1825 0    50   Input ~ 0
SD0
Text GLabel 4350 1925 0    50   Input ~ 0
SD1
Text GLabel 4350 2025 0    50   Input ~ 0
SD2
Text GLabel 4350 2125 0    50   Input ~ 0
SD3
Text GLabel 4350 2225 0    50   Input ~ 0
SD4
Text GLabel 4350 2325 0    50   Input ~ 0
SD5
Text GLabel 4350 2425 0    50   Input ~ 0
SD6
Text GLabel 4350 2525 0    50   Input ~ 0
SD7
Text GLabel 1325 850  0    50   Input ~ 0
SD0
Text GLabel 1325 950  0    50   Input ~ 0
SD1
Text GLabel 1325 1150 0    50   Input ~ 0
SD2
Text GLabel 1325 1250 0    50   Input ~ 0
SD3
Text GLabel 1325 1350 0    50   Input ~ 0
SD4
Text GLabel 1325 1450 0    50   Input ~ 0
SD5
Text GLabel 1325 1650 0    50   Input ~ 0
SD6
Text GLabel 1325 1750 0    50   Input ~ 0
SD7
Text GLabel 5400 4175 2    50   Input ~ 0
P_SMR
Text GLabel 5400 4275 2    50   Input ~ 0
P_SOE
Text GLabel 5400 4575 2    50   Input ~ 0
P_SCLK
Text GLabel 5400 4475 2    50   Input ~ 0
P_SS1
Text GLabel 5400 4375 2    50   Input ~ 0
P_SPGM
Text GLabel 4400 4175 0    50   Input ~ 0
SMR
Text GLabel 4400 4275 0    50   Input ~ 0
SOE
Text GLabel 4400 4375 0    50   Input ~ 0
SPGM
Text GLabel 4400 4475 0    50   Input ~ 0
SS
Text GLabel 4400 4575 0    50   Input ~ 0
SCLK
Text GLabel 1325 1850 0    50   Input ~ 0
SMR
Text GLabel 1325 1950 0    50   Input ~ 0
SOE
Text GLabel 1325 2150 0    50   Input ~ 0
SPGM
Text GLabel 1325 2250 0    50   Input ~ 0
SS
Text GLabel 1325 2350 0    50   Input ~ 0
SCLK
$Comp
L New_Library:Si2302CDS-n-channel TR?
U 1 1 6369051D
P 9075 5400
AR Path="/6366C142/6369051D" Ref="TR?"  Part="1" 
AR Path="/6369051D" Ref="TR?"  Part="1" 
AR Path="/6593F8B7/6369051D" Ref="TR2"  Part="1" 
F 0 "TR2" H 9353 5446 50  0000 L CNN
F 1 "Si2302CDS-n-channel" H 9353 5355 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9105 5550 20  0001 C CNN
F 3 "" H 9075 5400 50  0001 C CNN
	1    9075 5400
	1    0    0    -1  
$EndComp
$Comp
L GeekAmmo:P-CHANNEL-MOSFET M?
U 1 1 63690523
P 9800 4550
AR Path="/6366C142/63690523" Ref="M?"  Part="1" 
AR Path="/63690523" Ref="M?"  Part="1" 
AR Path="/6593F8B7/63690523" Ref="M2"  Part="1" 
F 0 "M2" H 10078 4596 50  0000 L CNN
F 1 "P-CHANNEL-MOSFET" H 10078 4505 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9830 4700 20  0001 C CNN
F 3 "" H 9800 4550 50  0001 C CNN
	1    9800 4550
	1    0    0    -1  
$EndComp
Text GLabel 8700 3925 0    50   Input ~ 0
VPP_SUPPLY
$Comp
L Device:R_Small R?
U 1 1 6369052A
P 9175 4150
AR Path="/6366C142/6369052A" Ref="R?"  Part="1" 
AR Path="/6369052A" Ref="R?"  Part="1" 
AR Path="/6593F8B7/6369052A" Ref="R21"  Part="1" 
F 0 "R21" H 9234 4196 50  0000 L CNN
F 1 "10k" H 9234 4105 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 9175 4150 50  0001 C CNN
F 3 "~" H 9175 4150 50  0001 C CNN
	1    9175 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 3925 9175 3925
Wire Wire Line
	9175 3925 9175 4050
Wire Wire Line
	9175 4250 9175 4450
Wire Wire Line
	9600 4450 9175 4450
Wire Wire Line
	9175 3925 9900 3925
Wire Wire Line
	9900 3925 9900 4250
Connection ~ 9175 3925
$Comp
L power:GND #PWR?
U 1 1 63690539
P 9175 6025
AR Path="/6366C142/63690539" Ref="#PWR?"  Part="1" 
AR Path="/63690539" Ref="#PWR?"  Part="1" 
AR Path="/6593F8B7/63690539" Ref="#PWR063"  Part="1" 
F 0 "#PWR063" H 9175 5775 50  0001 C CNN
F 1 "GND" H 9180 5852 50  0000 C CNN
F 2 "" H 9175 6025 50  0001 C CNN
F 3 "" H 9175 6025 50  0001 C CNN
	1    9175 6025
	1    0    0    -1  
$EndComp
Text GLabel 10225 5200 2    50   Input ~ 0
VPP_DRIVE
Wire Wire Line
	9900 4850 9900 5200
Wire Wire Line
	9900 5200 10225 5200
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 636A2829
P 775 6975
AR Path="/6104FF77/636A2829" Ref="J?"  Part="1" 
AR Path="/636A2829" Ref="J?"  Part="1" 
AR Path="/63690FA3/636A2829" Ref="J?"  Part="1" 
AR Path="/6593F8B7/636A2829" Ref="J7"  Part="1" 
F 0 "J7" H 667 7260 50  0000 C CNN
F 1 "Conn_01x04_Female" H 667 7169 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 775 6975 50  0001 C CNN
F 3 "~" H 775 6975 50  0001 C CNN
	1    775  6975
	-1   0    0    -1  
$EndComp
Text GLabel 3350 6975 2    50   Input ~ 0
I2C_SCLK
Text GLabel 3350 6875 2    50   Input ~ 0
I2C_SDA
$Comp
L power:GND #PWR?
U 1 1 636A2838
P 2000 7450
AR Path="/63690FA3/636A2838" Ref="#PWR?"  Part="1" 
AR Path="/636A2838" Ref="#PWR?"  Part="1" 
AR Path="/6593F8B7/636A2838" Ref="#PWR045"  Part="1" 
F 0 "#PWR045" H 2000 7200 50  0001 C CNN
F 1 "GND" H 2005 7277 50  0000 C CNN
F 2 "" H 2000 7450 50  0001 C CNN
F 3 "" H 2000 7450 50  0001 C CNN
	1    2000 7450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 7450 2000 7200
$Comp
L Device:R_Small R?
U 1 1 636A2843
P 2500 6650
AR Path="/63690FA3/636A2843" Ref="R?"  Part="1" 
AR Path="/636A2843" Ref="R?"  Part="1" 
AR Path="/6593F8B7/636A2843" Ref="R15"  Part="1" 
F 0 "R15" H 2559 6696 50  0000 L CNN
F 1 "4k7" H 2559 6605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2500 6650 50  0001 C CNN
F 3 "~" H 2500 6650 50  0001 C CNN
	1    2500 6650
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 636A2849
P 3000 6650
AR Path="/63690FA3/636A2849" Ref="R?"  Part="1" 
AR Path="/636A2849" Ref="R?"  Part="1" 
AR Path="/6593F8B7/636A2849" Ref="R16"  Part="1" 
F 0 "R16" H 3059 6696 50  0000 L CNN
F 1 "4k7" H 3059 6605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3000 6650 50  0001 C CNN
F 3 "~" H 3000 6650 50  0001 C CNN
	1    3000 6650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 6750 2500 6875
Connection ~ 2500 6875
Wire Wire Line
	2050 6325 2050 6450
Wire Wire Line
	2500 6450 2500 6550
Wire Wire Line
	2500 6450 3000 6450
Wire Wire Line
	3000 6450 3000 6550
Connection ~ 2500 6450
Wire Wire Line
	2050 6450 2500 6450
Connection ~ 2050 6450
Wire Wire Line
	2050 6450 2050 7075
Wire Wire Line
	2500 6875 3350 6875
Wire Wire Line
	3000 6750 3000 6975
Connection ~ 3000 6975
Wire Wire Line
	3000 6975 3350 6975
Wire Wire Line
	5275 7075 5275 7225
Wire Wire Line
	5675 6675 5275 6675
Wire Wire Line
	4600 7075 4600 7225
Text GLabel 5000 6675 2    50   Input ~ 0
SW1
Wire Wire Line
	5000 6675 4600 6675
Text GLabel 4325 6675 2    50   Input ~ 0
SW0
$Comp
L mirotan-due-cassette-shield-cache:Switch_SW_SPST SW?
U 1 1 636A9B7B
P 3925 6875
AR Path="/6104FF77/636A9B7B" Ref="SW?"  Part="1" 
AR Path="/636A9B7B" Ref="SW?"  Part="1" 
AR Path="/63690FA3/636A9B7B" Ref="SW?"  Part="1" 
AR Path="/6593F8B7/636A9B7B" Ref="SW33"  Part="1" 
F 0 "SW33" V 3879 6973 50  0000 L CNN
F 1 "sw" V 3970 6973 50  0000 L CNN
F 2 "Button_Switch_SMD:SW_Push_1P1T_NO_Vertical_Wuerth_434133025816" H 3925 6875 50  0001 C CNN
F 3 "" H 3925 6875 50  0001 C CNN
	1    3925 6875
	0    1    1    0   
$EndComp
$Comp
L mirotan-due-cassette-shield-cache:Switch_SW_SPST SW?
U 1 1 636A9B81
P 4600 6875
AR Path="/6104FF77/636A9B81" Ref="SW?"  Part="1" 
AR Path="/636A9B81" Ref="SW?"  Part="1" 
AR Path="/63690FA3/636A9B81" Ref="SW?"  Part="1" 
AR Path="/6593F8B7/636A9B81" Ref="SW34"  Part="1" 
F 0 "SW34" V 4554 6973 50  0000 L CNN
F 1 "sw" V 4645 6973 50  0000 L CNN
F 2 "Button_Switch_SMD:SW_Push_1P1T_NO_Vertical_Wuerth_434133025816" H 4600 6875 50  0001 C CNN
F 3 "" H 4600 6875 50  0001 C CNN
	1    4600 6875
	0    1    1    0   
$EndComp
$Comp
L mirotan-due-cassette-shield-cache:Switch_SW_SPST SW?
U 1 1 636A9B87
P 5275 6875
AR Path="/6104FF77/636A9B87" Ref="SW?"  Part="1" 
AR Path="/636A9B87" Ref="SW?"  Part="1" 
AR Path="/63690FA3/636A9B87" Ref="SW?"  Part="1" 
AR Path="/6593F8B7/636A9B87" Ref="SW35"  Part="1" 
F 0 "SW35" V 5229 6973 50  0000 L CNN
F 1 "sw" V 5320 6973 50  0000 L CNN
F 2 "Button_Switch_SMD:SW_Push_1P1T_NO_Vertical_Wuerth_434133025816" H 5275 6875 50  0001 C CNN
F 3 "" H 5275 6875 50  0001 C CNN
	1    5275 6875
	0    1    1    0   
$EndComp
Wire Wire Line
	3925 7075 3925 7225
Text GLabel 5675 6675 2    50   Input ~ 0
SW2
$Comp
L power:GND #PWR?
U 1 1 636A9B98
P 3925 7225
AR Path="/63690FA3/636A9B98" Ref="#PWR?"  Part="1" 
AR Path="/636A9B98" Ref="#PWR?"  Part="1" 
AR Path="/6593F8B7/636A9B98" Ref="#PWR050"  Part="1" 
F 0 "#PWR050" H 3925 6975 50  0001 C CNN
F 1 "GND" H 3930 7052 50  0000 C CNN
F 2 "" H 3925 7225 50  0001 C CNN
F 3 "" H 3925 7225 50  0001 C CNN
	1    3925 7225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 636A9B9E
P 4600 7225
AR Path="/63690FA3/636A9B9E" Ref="#PWR?"  Part="1" 
AR Path="/636A9B9E" Ref="#PWR?"  Part="1" 
AR Path="/6593F8B7/636A9B9E" Ref="#PWR053"  Part="1" 
F 0 "#PWR053" H 4600 6975 50  0001 C CNN
F 1 "GND" H 4605 7052 50  0000 C CNN
F 2 "" H 4600 7225 50  0001 C CNN
F 3 "" H 4600 7225 50  0001 C CNN
	1    4600 7225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 636A9BA4
P 5275 7225
AR Path="/63690FA3/636A9BA4" Ref="#PWR?"  Part="1" 
AR Path="/636A9BA4" Ref="#PWR?"  Part="1" 
AR Path="/6593F8B7/636A9BA4" Ref="#PWR059"  Part="1" 
F 0 "#PWR059" H 5275 6975 50  0001 C CNN
F 1 "GND" H 5280 7052 50  0000 C CNN
F 2 "" H 5275 7225 50  0001 C CNN
F 3 "" H 5275 7225 50  0001 C CNN
	1    5275 7225
	1    0    0    -1  
$EndComp
$Comp
L pak_breakout:502p_SD_ebay-uSD-push_push_SMD XS3
U 1 1 636ABAA4
P 1025 4525
F 0 "XS3" H 721 4571 50  0000 R CNN
F 1 "502p_SD_ebay-uSD-push_push_SMD" H 1950 3675 50  0000 R CNN
F 2 "ajm_kicad:Conn_uSDcard" H 1025 4525 10  0001 C CNN
F 3 "_" H 1025 4525 10  0001 C CNN
F 4 "_" H 1025 4525 10  0001 C CNN "Manf#"
F 5 "ebay" H 1025 4525 10  0001 C CNN "Manf"
	1    1025 4525
	1    0    0    -1  
$EndComp
Text GLabel 2725 2450 2    50   Input ~ 0
SD_CLK
Text GLabel 2725 2350 2    50   Input ~ 0
MOSI_DI
Text GLabel 2725 1950 2    50   Input ~ 0
SD_SS
Text GLabel 2725 2150 2    50   Input ~ 0
SD_DAT2
Text GLabel 2725 2250 2    50   Input ~ 0
SD_DAT1
Text GLabel 2725 2750 2    50   Input ~ 0
MISO_D0
$Comp
L power:GND #PWR043
U 1 1 636B2703
P 875 5250
F 0 "#PWR043" H 875 5000 50  0001 C CNN
F 1 "GND" H 880 5077 50  0000 C CNN
F 2 "" H 875 5250 50  0001 C CNN
F 3 "" H 875 5250 50  0001 C CNN
	1    875  5250
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR042
U 1 1 636B2E4E
P 875 3800
F 0 "#PWR042" H 875 3650 50  0001 C CNN
F 1 "+3V3" H 890 3973 50  0000 C CNN
F 2 "" H 875 3800 50  0001 C CNN
F 3 "" H 875 3800 50  0001 C CNN
	1    875  3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	875  3800 875  3900
Wire Wire Line
	875  5150 875  5250
Wire Wire Line
	1400 4825 1525 4825
Wire Wire Line
	1525 4675 1400 4675
Wire Wire Line
	1400 4525 1525 4525
Wire Wire Line
	1400 4375 1525 4375
Wire Wire Line
	1525 4225 1400 4225
Wire Wire Line
	1400 4075 1525 4075
Text GLabel 950  2650 0    50   Input ~ 0
I2C_SCLK
Text GLabel 950  2750 0    50   Input ~ 0
I2C_SDA
Wire Wire Line
	950  2650 1325 2650
Wire Wire Line
	950  2750 1325 2750
Wire Wire Line
	4400 5175 4225 5175
Wire Wire Line
	4225 5525 4900 5525
Connection ~ 4900 5525
Wire Wire Line
	4900 5525 4900 5625
Wire Wire Line
	4850 3075 4175 3075
Wire Wire Line
	4175 3075 4175 2825
Wire Wire Line
	4175 2825 4350 2825
Text GLabel 4050 2725 0    50   Input ~ 0
DATA_DIR
Wire Wire Line
	4050 2725 4350 2725
Text GLabel 950  2450 0    50   Input ~ 0
DATA_DIR
Wire Wire Line
	950  2450 1325 2450
Wire Wire Line
	3175 2550 3175 3225
Text GLabel 2125 2950 3    50   Input ~ 0
SWDIO
Text GLabel 1925 2950 3    50   Input ~ 0
SWCLK
$Comp
L Connector:Conn_01x03_Female J9
U 1 1 636D9694
P 10225 850
F 0 "J9" H 10253 876 50  0000 L CNN
F 1 "Conn_01x03_Female" H 10253 785 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 10225 850 50  0001 C CNN
F 3 "~" H 10225 850 50  0001 C CNN
	1    10225 850 
	1    0    0    -1  
$EndComp
Text GLabel 9925 950  0    50   Input ~ 0
SWDIO
Text GLabel 9925 750  0    50   Input ~ 0
SWCLK
Wire Wire Line
	9925 750  10025 750 
Wire Wire Line
	9925 950  10025 950 
$Comp
L power:GND #PWR064
U 1 1 636E0898
P 9450 925
F 0 "#PWR064" H 9450 675 50  0001 C CNN
F 1 "GND" H 9455 752 50  0000 C CNN
F 2 "" H 9450 925 50  0001 C CNN
F 3 "" H 9450 925 50  0001 C CNN
	1    9450 925 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 925  9450 850 
Wire Wire Line
	9450 850  10025 850 
Text GLabel 1525 4075 2    50   Input ~ 0
SD_CLK
Text GLabel 1525 4225 2    50   Input ~ 0
MOSI_DI
Text GLabel 1525 4375 2    50   Input ~ 0
SD_SS
Text GLabel 1525 4525 2    50   Input ~ 0
SD_DAT2
Text GLabel 1525 4675 2    50   Input ~ 0
SD_DAT1
Text GLabel 1525 4825 2    50   Input ~ 0
MISO_D0
Wire Wire Line
	3175 1550 3175 2050
Connection ~ 3175 2050
Text GLabel 8100 5500 0    50   Input ~ 0
VPP_ON
$Comp
L Device:R_Small R20
U 1 1 636F0992
P 8525 5875
F 0 "R20" H 8584 5921 50  0000 L CNN
F 1 "10k" H 8584 5830 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 8525 5875 50  0001 C CNN
F 3 "~" H 8525 5875 50  0001 C CNN
	1    8525 5875
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 636F1ABE
P 8525 6050
AR Path="/6366C142/636F1ABE" Ref="#PWR?"  Part="1" 
AR Path="/636F1ABE" Ref="#PWR?"  Part="1" 
AR Path="/6593F8B7/636F1ABE" Ref="#PWR062"  Part="1" 
F 0 "#PWR062" H 8525 5800 50  0001 C CNN
F 1 "GND" H 8530 5877 50  0000 C CNN
F 2 "" H 8525 6050 50  0001 C CNN
F 3 "" H 8525 6050 50  0001 C CNN
	1    8525 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	9175 5700 9175 6025
Wire Wire Line
	8525 6050 8525 5975
Wire Wire Line
	8100 5500 8525 5500
Wire Wire Line
	8525 5775 8525 5500
Connection ~ 8525 5500
Wire Wire Line
	8525 5500 8875 5500
Text GLabel 2725 2650 2    50   Input ~ 0
VPP_ON
$Sheet
S 6875 1675 1000 575 
U 63706713
F0 "Sheet63706712" 50
F1 "/tree/projects/github/psion-org-datapack-tool/pcb/datapack-tool1/usb.sch" 50
$EndSheet
Text GLabel 3375 850  2    50   Input ~ 0
VPP_IN
Wire Wire Line
	2725 850  3075 850 
Text GLabel 3375 1450 2    50   Input ~ 0
SW0
Wire Wire Line
	3375 1450 2725 1450
Text GLabel 3375 1650 2    50   Input ~ 0
SW1
Wire Wire Line
	3375 1650 2725 1650
Text GLabel 3375 1750 2    50   Input ~ 0
SW2
Wire Wire Line
	3375 1750 2725 1750
$Comp
L Device:R_Small R22
U 1 1 636A4E66
P 9175 4775
F 0 "R22" H 9234 4821 50  0000 L CNN
F 1 "47k" H 9234 4730 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 9175 4775 50  0001 C CNN
F 3 "~" H 9175 4775 50  0001 C CNN
	1    9175 4775
	1    0    0    -1  
$EndComp
Wire Wire Line
	9175 4875 9175 5100
Wire Wire Line
	9175 4450 9175 4675
Connection ~ 9175 4450
$Comp
L Device:R_Small R17
U 1 1 636AE2E9
P 3925 6375
F 0 "R17" H 3984 6421 50  0000 L CNN
F 1 "10k" H 3984 6330 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 3925 6375 50  0001 C CNN
F 3 "~" H 3925 6375 50  0001 C CNN
	1    3925 6375
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R18
U 1 1 636AE707
P 4600 6375
F 0 "R18" H 4659 6421 50  0000 L CNN
F 1 "10k" H 4659 6330 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 4600 6375 50  0001 C CNN
F 3 "~" H 4600 6375 50  0001 C CNN
	1    4600 6375
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R19
U 1 1 636AE9C5
P 5275 6375
F 0 "R19" H 5334 6421 50  0000 L CNN
F 1 "10k" H 5334 6330 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5275 6375 50  0001 C CNN
F 3 "~" H 5275 6375 50  0001 C CNN
	1    5275 6375
	1    0    0    -1  
$EndComp
Wire Wire Line
	5275 6475 5275 6675
Connection ~ 5275 6675
Wire Wire Line
	4600 6475 4600 6675
Connection ~ 4600 6675
Wire Wire Line
	4325 6675 3925 6675
$Comp
L power:+3V3 #PWR047
U 1 1 636BA625
P 2050 6325
F 0 "#PWR047" H 2050 6175 50  0001 C CNN
F 1 "+3V3" H 2065 6498 50  0000 C CNN
F 2 "" H 2050 6325 50  0001 C CNN
F 3 "" H 2050 6325 50  0001 C CNN
	1    2050 6325
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR049
U 1 1 636BAD78
P 3925 6125
F 0 "#PWR049" H 3925 5975 50  0001 C CNN
F 1 "+3V3" H 3940 6298 50  0000 C CNN
F 2 "" H 3925 6125 50  0001 C CNN
F 3 "" H 3925 6125 50  0001 C CNN
	1    3925 6125
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR052
U 1 1 636BB077
P 4600 6150
F 0 "#PWR052" H 4600 6000 50  0001 C CNN
F 1 "+3V3" H 4615 6323 50  0000 C CNN
F 2 "" H 4600 6150 50  0001 C CNN
F 3 "" H 4600 6150 50  0001 C CNN
	1    4600 6150
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR058
U 1 1 636BB402
P 5275 6125
F 0 "#PWR058" H 5275 5975 50  0001 C CNN
F 1 "+3V3" H 5290 6298 50  0000 C CNN
F 2 "" H 5275 6125 50  0001 C CNN
F 3 "" H 5275 6125 50  0001 C CNN
	1    5275 6125
	1    0    0    -1  
$EndComp
Wire Wire Line
	3925 6125 3925 6275
Wire Wire Line
	3925 6475 3925 6675
Connection ~ 3925 6675
Wire Wire Line
	4600 6150 4600 6275
Wire Wire Line
	5275 6125 5275 6275
Wire Wire Line
	4900 3725 4900 3875
Wire Wire Line
	4400 5075 4225 5075
Wire Wire Line
	4225 5075 4225 5175
Connection ~ 4225 5175
Wire Wire Line
	4225 5175 4225 5525
$Comp
L Device:Jumper_NO_Small JP6
U 1 1 636E1D83
P 1275 7075
F 0 "JP6" H 1275 7260 50  0000 C CNN
F 1 "Jumper_NO_Small" H 1275 7169 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 1275 7075 50  0001 C CNN
F 3 "~" H 1275 7075 50  0001 C CNN
	1    1275 7075
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP7
U 1 1 636E2089
P 1275 7200
F 0 "JP7" H 1275 7385 50  0000 C CNN
F 1 "Jumper_NO_Small" H 1275 7294 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 1275 7200 50  0001 C CNN
F 3 "~" H 1275 7200 50  0001 C CNN
	1    1275 7200
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP8
U 1 1 636E27DD
P 1275 7325
F 0 "JP8" H 1275 7510 50  0000 C CNN
F 1 "Jumper_NO_Small" H 1275 7419 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 1275 7325 50  0001 C CNN
F 3 "~" H 1275 7325 50  0001 C CNN
	1    1275 7325
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP9
U 1 1 636E2889
P 1275 7450
F 0 "JP9" H 1275 7635 50  0000 C CNN
F 1 "Jumper_NO_Small" H 1275 7544 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 1275 7450 50  0001 C CNN
F 3 "~" H 1275 7450 50  0001 C CNN
	1    1275 7450
	1    0    0    -1  
$EndComp
Wire Wire Line
	975  7075 1175 7075
Wire Wire Line
	1175 7200 1175 7075
Connection ~ 1175 7075
Wire Wire Line
	975  7175 975  7325
Wire Wire Line
	975  7450 1175 7450
Wire Wire Line
	1175 7325 975  7325
Connection ~ 975  7325
Wire Wire Line
	975  7325 975  7450
Wire Wire Line
	1375 7075 1675 7075
Wire Wire Line
	1375 7200 2000 7200
Wire Wire Line
	1375 7325 1675 7325
Wire Wire Line
	1675 7325 1675 7075
Connection ~ 1675 7075
Wire Wire Line
	1675 7075 2050 7075
Wire Wire Line
	1375 7450 2000 7450
Connection ~ 2000 7450
Wire Wire Line
	975  6875 2500 6875
Wire Wire Line
	975  6975 3000 6975
$Comp
L Device:Jumper_NO_Small JP10
U 1 1 63745F7B
P 4250 1150
F 0 "JP10" V 4204 1198 50  0000 L CNN
F 1 "Jumper_NO_Small" V 4295 1198 50  0000 L CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 4250 1150 50  0001 C CNN
F 3 "~" H 4250 1150 50  0001 C CNN
	1    4250 1150
	0    1    1    0   
$EndComp
$Comp
L Device:Jumper_NO_Small JP11
U 1 1 63746CAC
P 7150 875
F 0 "JP11" H 7150 1060 50  0000 C CNN
F 1 "Jumper_NO_Small" H 7150 969 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 7150 875 50  0001 C CNN
F 3 "~" H 7150 875 50  0001 C CNN
	1    7150 875 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 850  4250 1050
$Comp
L pak_breakout:AMS1117-3.3 U10
U 1 1 6374C131
P 6575 875
F 0 "U10" H 6575 1117 50  0000 C CNN
F 1 "AMS1117-3.3" H 6575 1026 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 6575 1075 50  0001 C CNN
F 3 "" H 6675 625 50  0001 C CNN
	1    6575 875 
	1    0    0    -1  
$EndComp
Wire Wire Line
	6575 1300 6575 1225
Wire Wire Line
	6875 875  6975 875 
Wire Wire Line
	3075 850  3075 575 
Wire Wire Line
	3075 575  5850 575 
Wire Wire Line
	5850 575  5850 875 
Wire Wire Line
	5850 875  5975 875 
Connection ~ 3075 850 
Wire Wire Line
	3075 850  3375 850 
$Comp
L power:+3V3 #PWR061
U 1 1 6375A2DF
P 7700 750
F 0 "#PWR061" H 7700 600 50  0001 C CNN
F 1 "+3V3" H 7715 923 50  0000 C CNN
F 2 "" H 7700 750 50  0001 C CNN
F 3 "" H 7700 750 50  0001 C CNN
	1    7700 750 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 875  7700 875 
Wire Wire Line
	7700 875  7700 750 
$Comp
L Device:C_Small C13
U 1 1 63762D32
P 5975 1075
F 0 "C13" H 6067 1121 50  0000 L CNN
F 1 "100nF" H 6067 1030 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5975 1075 50  0001 C CNN
F 3 "~" H 5975 1075 50  0001 C CNN
	1    5975 1075
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C14
U 1 1 6376314C
P 6975 1075
F 0 "C14" H 7067 1121 50  0000 L CNN
F 1 "100nF" H 7067 1030 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6975 1075 50  0001 C CNN
F 3 "~" H 6975 1075 50  0001 C CNN
	1    6975 1075
	1    0    0    -1  
$EndComp
Wire Wire Line
	6975 975  6975 875 
Connection ~ 6975 875 
Wire Wire Line
	6975 875  7050 875 
Wire Wire Line
	5975 875  5975 975 
Connection ~ 5975 875 
Wire Wire Line
	5975 875  6275 875 
Wire Wire Line
	5975 1175 5975 1225
Wire Wire Line
	5975 1225 6575 1225
Connection ~ 6575 1225
Wire Wire Line
	6575 1225 6575 1175
Wire Wire Line
	6575 1225 6975 1225
Wire Wire Line
	6975 1225 6975 1175
$Comp
L power:GND #PWR055
U 1 1 637823D8
P 4850 3250
F 0 "#PWR055" H 4850 3000 50  0001 C CNN
F 1 "GND" H 4855 3077 50  0000 C CNN
F 2 "" H 4850 3250 50  0001 C CNN
F 3 "" H 4850 3250 50  0001 C CNN
	1    4850 3250
	1    0    0    -1  
$EndComp
Text GLabel 3375 975  2    50   Input ~ 0
VCC_SLOT
Wire Wire Line
	3375 975  3075 975 
Wire Wire Line
	3075 975  3075 850 
Text Notes 2775 550  0    50   ~ 0
+5V for datapack and DC-DC converter
$EndSCHEMATC
