EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
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
L ajm:QT1481 U1
U 1 1 6646DCA8
P 3925 3200
F 0 "U1" H 4025 1211 50  0000 C CNN
F 1 "QT1481" H 4025 1120 50  0000 C CNN
F 2 "Package_QFP:LQFP-44_10x10mm_P0.8mm" H 3825 4100 50  0001 C CNN
F 3 "" H 3825 4100 50  0001 C CNN
	1    3925 3200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 6646EF75
P 5250 2275
F 0 "#PWR0101" H 5250 2025 50  0001 C CNN
F 1 "GND" H 5255 2102 50  0000 C CNN
F 2 "" H 5250 2275 50  0001 C CNN
F 3 "" H 5250 2275 50  0001 C CNN
	1    5250 2275
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C5
U 1 1 6646F602
P 5725 1975
F 0 "C5" H 5817 2021 50  0000 L CNN
F 1 "100nF" H 5817 1930 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5725 1975 50  0001 C CNN
F 3 "~" H 5725 1975 50  0001 C CNN
	1    5725 1975
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 664702FC
P 1325 3600
F 0 "D1" H 1318 3817 50  0000 C CNN
F 1 "LED" H 1318 3726 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1325 3600 50  0001 C CNN
F 3 "~" H 1325 3600 50  0001 C CNN
	1    1325 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R1
U 1 1 66471688
P 900 3600
F 0 "R1" V 1096 3600 50  0000 C CNN
F 1 "4k7" V 1005 3600 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 900 3600 50  0001 C CNN
F 3 "~" H 900 3600 50  0001 C CNN
	1    900  3600
	0    -1   -1   0   
$EndComp
$Comp
L power:+5V #PWR0102
U 1 1 664730B9
P 700 3325
F 0 "#PWR0102" H 700 3175 50  0001 C CNN
F 1 "+5V" H 715 3498 50  0000 C CNN
F 2 "" H 700 3325 50  0001 C CNN
F 3 "" H 700 3325 50  0001 C CNN
	1    700  3325
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C3
U 1 1 66473758
P 4825 1975
F 0 "C3" H 4917 2021 50  0000 L CNN
F 1 "100nF" H 4917 1930 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4825 1975 50  0001 C CNN
F 3 "~" H 4825 1975 50  0001 C CNN
	1    4825 1975
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C4
U 1 1 6647396D
P 5250 1975
F 0 "C4" H 5342 2021 50  0000 L CNN
F 1 "100nF" H 5342 1930 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5250 1975 50  0001 C CNN
F 3 "~" H 5250 1975 50  0001 C CNN
	1    5250 1975
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 66473C95
P 1775 4350
F 0 "Y1" V 1475 4150 50  0000 L CNN
F 1 "16MHz" V 1550 4150 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_HC49-SD_HandSoldering" H 1775 4350 50  0001 C CNN
F 3 "~" H 1775 4350 50  0001 C CNN
	1    1775 4350
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C1
U 1 1 66474E11
P 1175 4525
F 0 "C1" H 1267 4571 50  0000 L CNN
F 1 "20pF" H 1267 4480 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1175 4525 50  0001 C CNN
F 3 "~" H 1175 4525 50  0001 C CNN
	1    1175 4525
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C2
U 1 1 6647517A
P 1775 4775
F 0 "C2" H 1867 4821 50  0000 L CNN
F 1 "20pF" H 1867 4730 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1775 4775 50  0001 C CNN
F 3 "~" H 1775 4775 50  0001 C CNN
	1    1775 4775
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 664754A5
P 1175 5025
F 0 "#PWR0103" H 1175 4775 50  0001 C CNN
F 1 "GND" H 1180 4852 50  0000 C CNN
F 2 "" H 1175 5025 50  0001 C CNN
F 3 "" H 1175 5025 50  0001 C CNN
	1    1175 5025
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 664757EF
P 3100 5475
F 0 "#PWR0104" H 3100 5225 50  0001 C CNN
F 1 "GND" H 3105 5302 50  0000 C CNN
F 2 "" H 3100 5475 50  0001 C CNN
F 3 "" H 3100 5475 50  0001 C CNN
	1    3100 5475
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R2
U 1 1 66476B31
P 4925 5450
F 0 "R2" V 4925 4725 50  0000 L CNN
F 1 "R_Small" V 4925 4875 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 4925 5450 50  0001 C CNN
F 3 "~" H 4925 5450 50  0001 C CNN
	1    4925 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C11
U 1 1 66476F5F
P 6000 4900
F 0 "C11" V 5950 5050 50  0000 C CNN
F 1 "C_Small" V 5950 5275 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6000 4900 50  0001 C CNN
F 3 "~" H 6000 4900 50  0001 C CNN
	1    6000 4900
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R21
U 1 1 6647A66B
P 6825 4800
F 0 "R21" V 6875 4525 50  0000 L CNN
F 1 "R_Small" V 6875 4200 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6825 4800 50  0001 C CNN
F 3 "~" H 6825 4800 50  0001 C CNN
	1    6825 4800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R3
U 1 1 6647F4F4
P 5075 5450
F 0 "R3" V 5075 4725 50  0000 L CNN
F 1 "R_Small" V 5075 4875 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5075 5450 50  0001 C CNN
F 3 "~" H 5075 5450 50  0001 C CNN
	1    5075 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R4
U 1 1 6647F68D
P 5225 5450
F 0 "R4" V 5225 4725 50  0000 L CNN
F 1 "R_Small" V 5225 4875 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5225 5450 50  0001 C CNN
F 3 "~" H 5225 5450 50  0001 C CNN
	1    5225 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R5
U 1 1 6647F9CF
P 5375 5450
F 0 "R5" V 5375 4725 50  0000 L CNN
F 1 "R_Small" V 5375 4875 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5375 5450 50  0001 C CNN
F 3 "~" H 5375 5450 50  0001 C CNN
	1    5375 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R6
U 1 1 6647FC5A
P 5525 5450
F 0 "R6" V 5525 4725 50  0000 L CNN
F 1 "R_Small" V 5525 4875 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5525 5450 50  0001 C CNN
F 3 "~" H 5525 5450 50  0001 C CNN
	1    5525 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R9
U 1 1 6647FE5D
P 5675 5450
F 0 "R9" V 5675 4725 50  0000 L CNN
F 1 "R_Small" V 5675 4875 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5675 5450 50  0001 C CNN
F 3 "~" H 5675 5450 50  0001 C CNN
	1    5675 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C10
U 1 1 66480B15
P 6000 4700
F 0 "C10" V 5950 4850 50  0000 C CNN
F 1 "C_Small" V 5950 5075 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6000 4700 50  0001 C CNN
F 3 "~" H 6000 4700 50  0001 C CNN
	1    6000 4700
	0    1    1    0   
$EndComp
Wire Wire Line
	4675 4700 5075 4700
Wire Wire Line
	5900 4900 4925 4900
$Comp
L Device:C_Small C9
U 1 1 664829D8
P 6000 4500
F 0 "C9" V 5950 4650 50  0000 C CNN
F 1 "C_Small" V 5950 4875 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6000 4500 50  0001 C CNN
F 3 "~" H 6000 4500 50  0001 C CNN
	1    6000 4500
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C8
U 1 1 664829DE
P 6000 4300
F 0 "C8" V 5950 4450 50  0000 C CNN
F 1 "C_Small" V 5950 4675 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6000 4300 50  0001 C CNN
F 3 "~" H 6000 4300 50  0001 C CNN
	1    6000 4300
	0    1    1    0   
$EndComp
Wire Wire Line
	4675 4300 5375 4300
Wire Wire Line
	5900 4500 5225 4500
$Comp
L Device:C_Small C7
U 1 1 66483946
P 6000 4100
F 0 "C7" V 5950 4250 50  0000 C CNN
F 1 "C_Small" V 5950 4475 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6000 4100 50  0001 C CNN
F 3 "~" H 6000 4100 50  0001 C CNN
	1    6000 4100
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C6
U 1 1 6648394C
P 6000 3900
F 0 "C6" V 5950 4050 50  0000 C CNN
F 1 "C_Small" V 5950 4275 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6000 3900 50  0001 C CNN
F 3 "~" H 6000 3900 50  0001 C CNN
	1    6000 3900
	0    1    1    0   
$EndComp
Wire Wire Line
	4675 3900 5675 3900
Wire Wire Line
	5900 4100 5525 4100
Wire Wire Line
	4675 4800 6250 4800
Wire Wire Line
	6250 4800 6250 4900
Wire Wire Line
	6250 4900 6100 4900
Wire Wire Line
	4675 4600 6250 4600
Wire Wire Line
	6250 4600 6250 4700
Wire Wire Line
	6250 4700 6100 4700
Wire Wire Line
	4675 4400 6250 4400
Wire Wire Line
	6250 4400 6250 4500
Wire Wire Line
	6250 4500 6100 4500
Wire Wire Line
	4675 4200 6250 4200
Wire Wire Line
	6250 4200 6250 4300
Wire Wire Line
	6250 4300 6100 4300
Wire Wire Line
	4675 4000 6250 4000
Wire Wire Line
	6250 4000 6250 4100
Wire Wire Line
	6250 4100 6100 4100
Wire Wire Line
	4675 3800 6250 3800
Wire Wire Line
	6250 3800 6250 3900
Wire Wire Line
	6250 3900 6100 3900
Wire Wire Line
	3975 5100 3975 5675
Wire Wire Line
	3975 5675 4925 5675
Wire Wire Line
	5675 5675 5675 5550
Wire Wire Line
	5525 5550 5525 5675
Connection ~ 5525 5675
Wire Wire Line
	5525 5675 5675 5675
Wire Wire Line
	5375 5550 5375 5675
Connection ~ 5375 5675
Wire Wire Line
	5375 5675 5525 5675
Wire Wire Line
	5225 5550 5225 5675
Connection ~ 5225 5675
Wire Wire Line
	5225 5675 5375 5675
Wire Wire Line
	5075 5550 5075 5675
Connection ~ 5075 5675
Wire Wire Line
	5075 5675 5225 5675
Wire Wire Line
	4925 5550 4925 5675
Connection ~ 4925 5675
Wire Wire Line
	4925 5675 5075 5675
Connection ~ 4925 4900
Wire Wire Line
	4925 4900 4675 4900
Wire Wire Line
	4925 4900 4925 5350
Wire Wire Line
	5075 5350 5075 4700
Connection ~ 5075 4700
Wire Wire Line
	5075 4700 5900 4700
Wire Wire Line
	5225 5350 5225 4500
Connection ~ 5225 4500
Wire Wire Line
	5225 4500 4675 4500
Wire Wire Line
	5375 5350 5375 4300
Connection ~ 5375 4300
Wire Wire Line
	5375 4300 5900 4300
Wire Wire Line
	5525 5350 5525 4100
Connection ~ 5525 4100
Wire Wire Line
	5525 4100 4675 4100
Wire Wire Line
	5675 5350 5675 3900
Connection ~ 5675 3900
Wire Wire Line
	5675 3900 5900 3900
Wire Wire Line
	6725 4800 6250 4800
Connection ~ 6250 4800
$Comp
L Device:R_Small R20
U 1 1 66494604
P 6825 4600
F 0 "R20" V 6875 4325 50  0000 L CNN
F 1 "R_Small" V 6875 4000 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6825 4600 50  0001 C CNN
F 3 "~" H 6825 4600 50  0001 C CNN
	1    6825 4600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R19
U 1 1 664957FB
P 6825 4400
F 0 "R19" V 6875 4125 50  0000 L CNN
F 1 "R_Small" V 6875 3800 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6825 4400 50  0001 C CNN
F 3 "~" H 6825 4400 50  0001 C CNN
	1    6825 4400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R18
U 1 1 664966B1
P 6825 4200
F 0 "R18" V 6875 3925 50  0000 L CNN
F 1 "R_Small" V 6875 3600 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6825 4200 50  0001 C CNN
F 3 "~" H 6825 4200 50  0001 C CNN
	1    6825 4200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R17
U 1 1 66497BE4
P 6825 4000
F 0 "R17" V 6875 3725 50  0000 L CNN
F 1 "R_Small" V 6875 3400 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6825 4000 50  0001 C CNN
F 3 "~" H 6825 4000 50  0001 C CNN
	1    6825 4000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R16
U 1 1 66498B3D
P 6825 3800
F 0 "R16" V 6875 3525 50  0000 L CNN
F 1 "R_Small" V 6875 3200 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6825 3800 50  0001 C CNN
F 3 "~" H 6825 3800 50  0001 C CNN
	1    6825 3800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 4600 6725 4600
Connection ~ 6250 4600
Wire Wire Line
	6725 4400 6250 4400
Connection ~ 6250 4400
Wire Wire Line
	6725 4200 6250 4200
Connection ~ 6250 4200
Wire Wire Line
	6725 4000 6250 4000
Connection ~ 6250 4000
Wire Wire Line
	6725 3800 6250 3800
Connection ~ 6250 3800
$Comp
L Device:R_Small R15
U 1 1 664A06B6
P 6450 3600
F 0 "R15" V 6500 3325 50  0000 L CNN
F 1 "R_Small" V 6500 3000 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6450 3600 50  0001 C CNN
F 3 "~" H 6450 3600 50  0001 C CNN
	1    6450 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6350 3600 4675 3600
$Comp
L Device:R_Small R13
U 1 1 664A4157
P 6150 3500
F 0 "R13" V 6200 3225 50  0000 L CNN
F 1 "R_Small" V 6200 2900 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6150 3500 50  0001 C CNN
F 3 "~" H 6150 3500 50  0001 C CNN
	1    6150 3500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R11
U 1 1 664A5785
P 5850 3400
F 0 "R11" V 5900 3125 50  0000 L CNN
F 1 "R_Small" V 5900 2800 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5850 3400 50  0001 C CNN
F 3 "~" H 5850 3400 50  0001 C CNN
	1    5850 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R8
U 1 1 664A6DC6
P 5550 3300
F 0 "R8" V 5600 3025 50  0000 L CNN
F 1 "R_Small" V 5600 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5550 3300 50  0001 C CNN
F 3 "~" H 5550 3300 50  0001 C CNN
	1    5550 3300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4675 3500 6050 3500
Wire Wire Line
	4675 3300 5450 3300
Wire Wire Line
	5750 3400 4675 3400
$Comp
L Device:R_Small R14
U 1 1 664AEDCF
P 6450 3200
F 0 "R14" V 6500 2925 50  0000 L CNN
F 1 "R_Small" V 6500 2600 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6450 3200 50  0001 C CNN
F 3 "~" H 6450 3200 50  0001 C CNN
	1    6450 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6350 3200 4675 3200
$Comp
L Device:R_Small R12
U 1 1 664AEDD6
P 6150 3100
F 0 "R12" V 6200 2825 50  0000 L CNN
F 1 "R_Small" V 6200 2500 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 6150 3100 50  0001 C CNN
F 3 "~" H 6150 3100 50  0001 C CNN
	1    6150 3100
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R10
U 1 1 664AEDDC
P 5850 3000
F 0 "R10" V 5900 2725 50  0000 L CNN
F 1 "R_Small" V 5900 2400 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5850 3000 50  0001 C CNN
F 3 "~" H 5850 3000 50  0001 C CNN
	1    5850 3000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R7
U 1 1 664AEDE2
P 5550 2900
F 0 "R7" V 5600 2625 50  0000 L CNN
F 1 "R_Small" V 5600 2300 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 5550 2900 50  0001 C CNN
F 3 "~" H 5550 2900 50  0001 C CNN
	1    5550 2900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4675 3100 6050 3100
Wire Wire Line
	4675 2900 5450 2900
Wire Wire Line
	5750 3000 4675 3000
Wire Wire Line
	4275 2400 4275 1525
Wire Wire Line
	4275 1525 4825 1525
Wire Wire Line
	4825 1525 4825 1875
Wire Wire Line
	4175 1525 4275 1525
Connection ~ 4275 1525
Wire Wire Line
	4075 2400 4075 1525
Wire Wire Line
	4075 1525 4175 1525
Connection ~ 4175 1525
Wire Wire Line
	3975 2400 3975 1525
Wire Wire Line
	3975 1525 4075 1525
Connection ~ 4075 1525
Wire Wire Line
	3875 1525 3975 1525
Connection ~ 3975 1525
Wire Wire Line
	4825 1525 5250 1525
Wire Wire Line
	5250 1525 5250 1875
Connection ~ 4825 1525
Wire Wire Line
	5250 1525 5725 1525
Wire Wire Line
	5725 1525 5725 1875
Connection ~ 5250 1525
Wire Wire Line
	5725 2075 5725 2275
Wire Wire Line
	5725 2275 5250 2275
Wire Wire Line
	5250 2075 5250 2275
Connection ~ 5250 2275
Wire Wire Line
	4825 2075 4825 2275
Wire Wire Line
	4825 2275 5250 2275
$Comp
L power:+5V #PWR0105
U 1 1 664C8917
P 2275 700
F 0 "#PWR0105" H 2275 550 50  0001 C CNN
F 1 "+5V" H 2290 873 50  0000 C CNN
F 2 "" H 2275 700 50  0001 C CNN
F 3 "" H 2275 700 50  0001 C CNN
	1    2275 700 
	1    0    0    -1  
$EndComp
Connection ~ 3875 1525
Wire Wire Line
	1950 3600 1475 3600
Wire Wire Line
	1175 3600 1000 3600
Wire Wire Line
	3375 4300 2125 4300
Wire Wire Line
	2125 4300 2125 4200
Wire Wire Line
	2125 4200 1775 4200
Wire Wire Line
	3375 4400 2125 4400
Wire Wire Line
	2125 4400 2125 4500
Wire Wire Line
	2125 4500 1775 4500
Wire Wire Line
	1175 5025 1175 4625
Wire Wire Line
	1175 4425 1175 4200
Wire Wire Line
	1175 4200 1775 4200
Connection ~ 1775 4200
Wire Wire Line
	1775 4500 1775 4675
Connection ~ 1775 4500
$Comp
L power:GND #PWR0106
U 1 1 664E50DF
P 1775 5025
F 0 "#PWR0106" H 1775 4775 50  0001 C CNN
F 1 "GND" H 1780 4852 50  0000 C CNN
F 2 "" H 1775 5025 50  0001 C CNN
F 3 "" H 1775 5025 50  0001 C CNN
	1    1775 5025
	1    0    0    -1  
$EndComp
Wire Wire Line
	1775 5025 1775 4875
Wire Wire Line
	3100 5475 3100 5325
Wire Wire Line
	3100 4600 3375 4600
Connection ~ 3100 4700
Wire Wire Line
	3100 4700 3100 4600
Wire Wire Line
	3375 4800 3100 4800
Connection ~ 3100 4800
Wire Wire Line
	3100 4800 3100 4700
Wire Wire Line
	3375 4900 3100 4900
Connection ~ 3100 4900
Wire Wire Line
	3100 4900 3100 4800
Wire Wire Line
	3100 4700 3375 4700
Wire Wire Line
	3375 2800 2875 2800
Wire Wire Line
	2875 2800 2875 1525
Wire Wire Line
	2875 1525 3875 1525
Wire Wire Line
	3875 1525 3875 2400
Wire Wire Line
	4175 1525 4175 2400
Wire Wire Line
	3775 5100 3775 5325
Wire Wire Line
	3775 5325 3100 5325
Connection ~ 3100 5325
Wire Wire Line
	3100 5325 3100 4900
$Comp
L power:GND #PWR0107
U 1 1 6651856C
P 2400 1550
F 0 "#PWR0107" H 2400 1300 50  0001 C CNN
F 1 "GND" H 2405 1377 50  0000 C CNN
F 2 "" H 2400 1550 50  0001 C CNN
F 3 "" H 2400 1550 50  0001 C CNN
	1    2400 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2275 1350 2275 900 
Wire Wire Line
	1175 1350 2275 1350
$Comp
L Connector:Conn_01x12_Female J1
U 1 1 66540103
P 975 1850
F 0 "J1" H 867 2535 50  0000 C CNN
F 1 "Conn_01x12_Female" H 867 2444 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x12_P2.54mm_Vertical_SMD_Pin1Left" H 975 1850 50  0001 C CNN
F 3 "~" H 975 1850 50  0001 C CNN
	1    975  1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2400 1450 2400 1550
Wire Wire Line
	1175 1450 2400 1450
$Sheet
S 925  6650 1075 575 
U 6654F4AC
F0 "Level Shift" 50
F1 "levelshift.sch" 50
$EndSheet
$Sheet
S 2650 6650 1025 575 
U 6655ADAF
F0 "Keys" 50
F1 "keys.sch" 50
$EndSheet
Text GLabel 3375 4000 0    50   Input ~ 0
DRDY_5V
Text GLabel 3375 3700 0    50   Input ~ 0
TX_5V
Text GLabel 3375 3600 0    50   Input ~ 0
RX_5V
Text GLabel 1175 1650 2    50   Input ~ 0
RX_3V3
Text GLabel 1175 1750 2    50   Input ~ 0
TX_3V3
Text GLabel 1175 1850 2    50   Input ~ 0
DRDY_3V3
Text GLabel 3375 3200 0    50   Input ~ 0
MOSI_5V
Text GLabel 3375 3300 0    50   Input ~ 0
MISO_5V
Text GLabel 3375 3400 0    50   Input ~ 0
SCK_5V
Text GLabel 1175 1950 2    50   Input ~ 0
MOSI_3V3
Text GLabel 1175 2050 2    50   Input ~ 0
SCK_3v3
Text GLabel 1175 2150 2    50   Input ~ 0
MISO_3V3
Text GLabel 7475 3600 2    50   Input ~ 0
X0
Text GLabel 7475 3500 2    50   Input ~ 0
X1
Text GLabel 7475 3400 2    50   Input ~ 0
X2
Text GLabel 7475 3300 2    50   Input ~ 0
X3
Text GLabel 7475 3200 2    50   Input ~ 0
X4
Text GLabel 7475 3100 2    50   Input ~ 0
X5
Text GLabel 7475 3000 2    50   Input ~ 0
X6
Text GLabel 7475 2900 2    50   Input ~ 0
X7
Wire Wire Line
	6550 3600 7475 3600
Wire Wire Line
	7475 3500 6250 3500
Wire Wire Line
	7475 3300 5650 3300
Wire Wire Line
	6550 3200 7475 3200
Wire Wire Line
	7475 3100 6250 3100
Wire Wire Line
	5950 3000 7475 3000
Wire Wire Line
	7475 2900 5650 2900
Text GLabel 8000 4400 2    50   Input ~ 0
Y0
Text GLabel 8000 4200 2    50   Input ~ 0
Y1
Text GLabel 8000 4000 2    50   Input ~ 0
Y2
Text GLabel 8000 3800 2    50   Input ~ 0
Y3
Text GLabel 8000 4600 2    50   Input ~ 0
Y4
Text GLabel 8000 4800 2    50   Input ~ 0
Y5
Wire Wire Line
	8000 3800 6925 3800
Wire Wire Line
	6925 4000 8000 4000
Wire Wire Line
	8000 4200 6925 4200
Wire Wire Line
	8000 4400 6925 4400
Wire Wire Line
	8000 4600 6925 4600
Wire Wire Line
	6925 4800 8000 4800
$Comp
L Device:D D2
U 1 1 664B96E2
P 2650 900
F 0 "D2" H 2650 683 50  0000 C CNN
F 1 "D" H 2650 774 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2650 900 50  0001 C CNN
F 3 "~" H 2650 900 50  0001 C CNN
	1    2650 900 
	-1   0    0    1   
$EndComp
Wire Wire Line
	2500 900  2275 900 
Connection ~ 2275 900 
Wire Wire Line
	2275 900  2275 700 
Wire Wire Line
	3875 900  3875 1525
$Comp
L Device:C_Small C16
U 1 1 664D2675
P 4175 1075
F 0 "C16" H 4267 1121 50  0000 L CNN
F 1 "100uF" H 4267 1030 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4175 1075 50  0001 C CNN
F 3 "~" H 4175 1075 50  0001 C CNN
	1    4175 1075
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C17
U 1 1 664D8C28
P 4675 1075
F 0 "C17" H 4767 1121 50  0000 L CNN
F 1 "100nF" H 4767 1030 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4675 1075 50  0001 C CNN
F 3 "~" H 4675 1075 50  0001 C CNN
	1    4675 1075
	1    0    0    -1  
$EndComp
Wire Wire Line
	4175 975  4175 900 
Wire Wire Line
	4175 900  3875 900 
Wire Wire Line
	4175 900  4675 900 
Wire Wire Line
	4675 900  4675 975 
Connection ~ 4175 900 
$Comp
L power:GND #PWR0108
U 1 1 664EBBC5
P 4675 1225
F 0 "#PWR0108" H 4675 975 50  0001 C CNN
F 1 "GND" H 4680 1052 50  0000 C CNN
F 2 "" H 4675 1225 50  0001 C CNN
F 3 "" H 4675 1225 50  0001 C CNN
	1    4675 1225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 664F1FDF
P 4175 1225
F 0 "#PWR0109" H 4175 975 50  0001 C CNN
F 1 "GND" H 4180 1052 50  0000 C CNN
F 2 "" H 4175 1225 50  0001 C CNN
F 3 "" H 4175 1225 50  0001 C CNN
	1    4175 1225
	1    0    0    -1  
$EndComp
Wire Wire Line
	4175 1225 4175 1175
Wire Wire Line
	4675 1225 4675 1175
$Comp
L Device:R_Small R22
U 1 1 6650BD73
P 3275 900
F 0 "R22" V 3079 900 50  0000 C CNN
F 1 "R_Small" V 3170 900 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 3275 900 50  0001 C CNN
F 3 "~" H 3275 900 50  0001 C CNN
	1    3275 900 
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 900  3175 900 
Wire Wire Line
	3375 900  3875 900 
Connection ~ 3875 900 
$Comp
L Device:Jumper JP1
U 1 1 665BE849
P 2250 3600
F 0 "JP1" H 1750 3775 50  0000 C CNN
F 1 "DBG_LED" H 1825 3700 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 2250 3600 50  0001 C CNN
F 3 "~" H 2250 3600 50  0001 C CNN
	1    2250 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper JP2
U 1 1 665C04AA
P 2250 3875
F 0 "JP2" H 1750 4050 50  0000 C CNN
F 1 "DBG_DATA" H 1825 3975 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 2250 3875 50  0001 C CNN
F 3 "~" H 2250 3875 50  0001 C CNN
	1    2250 3875
	1    0    0    -1  
$EndComp
Wire Wire Line
	3375 4100 2550 4100
Wire Wire Line
	2550 4100 2550 3875
Wire Wire Line
	2550 3875 2550 3600
Connection ~ 2550 3875
Text GLabel 1950 3875 0    50   Input ~ 0
DBG_DATA_5V
Text GLabel 3375 3000 0    50   Input ~ 0
DBG_CLK_5V
Text GLabel 1175 2250 2    50   Input ~ 0
DBG_DATA_3V3
Text GLabel 1175 2350 2    50   Input ~ 0
DBG_CLK_3V3
Text GLabel 3375 3800 0    50   Input ~ 0
WS_5V
Text GLabel 1175 2450 2    50   Input ~ 0
WS_3V3
Wire Wire Line
	700  3325 700  3600
Wire Wire Line
	700  3600 800  3600
$Comp
L power:+3V3 #PWR0113
U 1 1 6660825A
P 2075 1725
F 0 "#PWR0113" H 2075 1575 50  0001 C CNN
F 1 "+3V3" H 2090 1898 50  0000 C CNN
F 2 "" H 2075 1725 50  0001 C CNN
F 3 "" H 2075 1725 50  0001 C CNN
	1    2075 1725
	1    0    0    -1  
$EndComp
Wire Wire Line
	2075 1725 1750 1725
Wire Wire Line
	1750 1725 1750 1550
Wire Wire Line
	1750 1550 1175 1550
Wire Wire Line
	5950 3400 7475 3400
$EndSCHEMATC
