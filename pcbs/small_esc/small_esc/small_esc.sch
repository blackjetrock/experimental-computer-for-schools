EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Experimental Schools Computer Small Re-creation"
Date "2023-12-23"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L rp2040:Pico U1
U 1 1 65874095
P 5925 3175
F 0 "U1" H 5925 4390 50  0000 C CNN
F 1 "Pico" H 5925 4299 50  0000 C CNN
F 2 "rp2040:RPi_Pico_SMD_TH" V 5925 3175 50  0001 C CNN
F 3 "" H 5925 3175 50  0001 C CNN
	1    5925 3175
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW8
U 1 1 65876494
P 2150 6100
F 0 "SW8" H 2575 6200 50  0000 C CNN
F 1 "minus" H 2650 6100 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2150 6300 50  0001 C CNN
F 3 "~" H 2150 6300 50  0001 C CNN
	1    2150 6100
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW9
U 1 1 65878AC6
P 2150 6325
F 0 "SW9" H 2575 6425 50  0000 C CNN
F 1 "point" H 2650 6325 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2150 6525 50  0001 C CNN
F 3 "~" H 2150 6525 50  0001 C CNN
	1    2150 6325
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW10
U 1 1 658795BA
P 2150 6550
F 0 "SW10" H 2575 6650 50  0000 C CNN
F 1 "1" H 2650 6550 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2150 6750 50  0001 C CNN
F 3 "~" H 2150 6750 50  0001 C CNN
	1    2150 6550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW11
U 1 1 658795C0
P 2150 6775
F 0 "SW11" H 2575 6875 50  0000 C CNN
F 1 "2" H 2650 6775 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2150 6975 50  0001 C CNN
F 3 "~" H 2150 6975 50  0001 C CNN
	1    2150 6775
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW12
U 1 1 6587B0D0
P 2150 7000
F 0 "SW12" H 2575 7100 50  0000 C CNN
F 1 "3" H 2650 7000 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2150 7200 50  0001 C CNN
F 3 "~" H 2150 7200 50  0001 C CNN
	1    2150 7000
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW13
U 1 1 6587B0D6
P 2150 7225
F 0 "SW13" H 2575 7325 50  0000 C CNN
F 1 "4" H 2650 7225 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2150 7425 50  0001 C CNN
F 3 "~" H 2150 7425 50  0001 C CNN
	1    2150 7225
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW14
U 1 1 6587B0DC
P 2150 7450
F 0 "SW14" H 2575 7550 50  0000 C CNN
F 1 "5" H 2650 7450 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2150 7650 50  0001 C CNN
F 3 "~" H 2150 7650 50  0001 C CNN
	1    2150 7450
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 6587DCB2
P 1125 6100
F 0 "SW1" H 1550 6200 50  0000 C CNN
F 1 "load iar" H 1625 6100 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1125 6300 50  0001 C CNN
F 3 "~" H 1125 6300 50  0001 C CNN
	1    1125 6100
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 6587DCB8
P 1125 6325
F 0 "SW2" H 1550 6425 50  0000 C CNN
F 1 "load addr" H 1625 6325 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1125 6525 50  0001 C CNN
F 3 "~" H 1125 6525 50  0001 C CNN
	1    1125 6325
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 6587DCBE
P 1125 6550
F 0 "SW3" H 1550 6650 50  0000 C CNN
F 1 "load store" H 1625 6550 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1125 6750 50  0001 C CNN
F 3 "~" H 1125 6750 50  0001 C CNN
	1    1125 6550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 6587DCC4
P 1125 6775
F 0 "SW4" H 1550 6875 50  0000 C CNN
F 1 "decr addr" H 1625 6775 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1125 6975 50  0001 C CNN
F 3 "~" H 1125 6975 50  0001 C CNN
	1    1125 6775
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW5
U 1 1 6587DCCA
P 1125 7000
F 0 "SW5" H 1550 7100 50  0000 C CNN
F 1 "incr addr" H 1625 7000 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1125 7200 50  0001 C CNN
F 3 "~" H 1125 7200 50  0001 C CNN
	1    1125 7000
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW6
U 1 1 6587DCD0
P 1125 7225
F 0 "SW6" H 1550 7325 50  0000 C CNN
F 1 "normal reset" H 1625 7225 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1125 7425 50  0001 C CNN
F 3 "~" H 1125 7425 50  0001 C CNN
	1    1125 7225
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW7
U 1 1 6587DCD6
P 1125 7450
F 0 "SW7" H 1550 7550 50  0000 C CNN
F 1 "clear" H 1625 7450 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1125 7650 50  0001 C CNN
F 3 "~" H 1125 7650 50  0001 C CNN
	1    1125 7450
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW15
U 1 1 65880026
P 3175 6100
F 0 "SW15" H 3600 6200 50  0000 C CNN
F 1 "6" H 3675 6100 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3175 6300 50  0001 C CNN
F 3 "~" H 3175 6300 50  0001 C CNN
	1    3175 6100
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW16
U 1 1 6588002C
P 3175 6325
F 0 "SW16" H 3600 6425 50  0000 C CNN
F 1 "7" H 3675 6325 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3175 6525 50  0001 C CNN
F 3 "~" H 3175 6525 50  0001 C CNN
	1    3175 6325
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW17
U 1 1 65880032
P 3175 6550
F 0 "SW17" H 3600 6650 50  0000 C CNN
F 1 "8" H 3675 6550 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3175 6750 50  0001 C CNN
F 3 "~" H 3175 6750 50  0001 C CNN
	1    3175 6550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW18
U 1 1 65880038
P 3175 6775
F 0 "SW18" H 3600 6875 50  0000 C CNN
F 1 "9" H 3675 6775 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3175 6975 50  0001 C CNN
F 3 "~" H 3175 6975 50  0001 C CNN
	1    3175 6775
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW19
U 1 1 6588003E
P 3175 7000
F 0 "SW19" H 3600 7100 50  0000 C CNN
F 1 "0" H 3675 7000 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3175 7200 50  0001 C CNN
F 3 "~" H 3175 7200 50  0001 C CNN
	1    3175 7000
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW20
U 1 1 65880044
P 3175 7225
F 0 "SW20" H 3600 7325 50  0000 C CNN
F 1 "A" H 3675 7225 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3175 7425 50  0001 C CNN
F 3 "~" H 3175 7425 50  0001 C CNN
	1    3175 7225
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW21
U 1 1 6588004A
P 3175 7450
F 0 "SW21" H 3600 7550 50  0000 C CNN
F 1 "B" H 3675 7450 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3175 7650 50  0001 C CNN
F 3 "~" H 3175 7650 50  0001 C CNN
	1    3175 7450
	1    0    0    -1  
$EndComp
$Comp
L Device:D D3
U 1 1 65881C10
P 4875 2225
F 0 "D3" H 4750 2275 50  0000 C CNN
F 1 "SS36" H 5050 2275 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123" H 4875 2225 50  0001 C CNN
F 3 "~" H 4875 2225 50  0001 C CNN
	1    4875 2225
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW22
U 1 1 6589D0DB
P 4175 6100
F 0 "SW22" H 4600 6200 50  0000 C CNN
F 1 "C" H 4675 6100 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4175 6300 50  0001 C CNN
F 3 "~" H 4175 6300 50  0001 C CNN
	1    4175 6100
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW23
U 1 1 6589D0E1
P 4175 6325
F 0 "SW23" H 4600 6425 50  0000 C CNN
F 1 "ki reset" H 4675 6325 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4175 6525 50  0001 C CNN
F 3 "~" H 4175 6525 50  0001 C CNN
	1    4175 6325
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW24
U 1 1 6589D0E7
P 4175 6550
F 0 "SW24" H 4600 6650 50  0000 C CNN
F 1 "run" H 4675 6550 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4175 6750 50  0001 C CNN
F 3 "~" H 4175 6750 50  0001 C CNN
	1    4175 6550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW25
U 1 1 6589D0ED
P 4175 6775
F 0 "SW25" H 4600 6875 50  0000 C CNN
F 1 "stop" H 4675 6775 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4175 6975 50  0001 C CNN
F 3 "~" H 4175 6975 50  0001 C CNN
	1    4175 6775
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW26
U 1 1 6589D0F3
P 4175 7000
F 0 "SW26" H 4600 7100 50  0000 C CNN
F 1 "dump" H 4675 7000 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4175 7200 50  0001 C CNN
F 3 "~" H 4175 7200 50  0001 C CNN
	1    4175 7000
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW27
U 1 1 6589D0F9
P 4175 7225
F 0 "SW27" H 4600 7325 50  0000 C CNN
F 1 "check" H 4675 7225 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4175 7425 50  0001 C CNN
F 3 "~" H 4175 7425 50  0001 C CNN
	1    4175 7225
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW28
U 1 1 6589D0FF
P 4175 7450
F 0 "SW28" H 4600 7550 50  0000 C CNN
F 1 "reload" H 4675 7450 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4175 7650 50  0001 C CNN
F 3 "~" H 4175 7650 50  0001 C CNN
	1    4175 7450
	1    0    0    -1  
$EndComp
Text GLabel 8650 1950 0    50   Input ~ 0
SD_DAT2
Text GLabel 8650 1800 0    50   Input ~ 0
SD_SS
Text GLabel 8650 1650 0    50   Input ~ 0
MOSI_DI
Text GLabel 8650 1500 0    50   Input ~ 0
SD_CLK
Text GLabel 8650 2250 0    50   Input ~ 0
MISO_D0
Text GLabel 8650 2100 0    50   Input ~ 0
SD_DAT1
Wire Wire Line
	9800 2650 9800 2575
Wire Wire Line
	9275 1650 8650 1650
Wire Wire Line
	8650 1800 9275 1800
Wire Wire Line
	9275 2250 8650 2250
$Comp
L power:GND #PWR?
U 1 1 658A44CD
P 9800 2650
AR Path="/60C45767/658A44CD" Ref="#PWR?"  Part="1" 
AR Path="/658A44CD" Ref="#PWR07"  Part="1" 
AR Path="/6104FF77/658A44CD" Ref="#PWR?"  Part="1" 
F 0 "#PWR07" H 9800 2400 50  0001 C CNN
F 1 "GND" H 9805 2477 50  0000 C CNN
F 2 "" H 9800 2650 50  0001 C CNN
F 3 "" H 9800 2650 50  0001 C CNN
	1    9800 2650
	-1   0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 658A44D3
P 9800 900
AR Path="/60C45767/658A44D3" Ref="#PWR?"  Part="1" 
AR Path="/658A44D3" Ref="#PWR06"  Part="1" 
AR Path="/6104FF77/658A44D3" Ref="#PWR?"  Part="1" 
F 0 "#PWR06" H 9800 750 50  0001 C CNN
F 1 "+3V3" H 9815 1073 50  0000 C CNN
F 2 "" H 9800 900 50  0001 C CNN
F 3 "" H 9800 900 50  0001 C CNN
	1    9800 900 
	-1   0    0    -1  
$EndComp
$Comp
L 502p:SD_ebay-uSD-push_push_SMD XS?
U 1 1 658A44DB
P 9650 1950
AR Path="/60C45767/658A44DB" Ref="XS?"  Part="1" 
AR Path="/658A44DB" Ref="XS1"  Part="1" 
AR Path="/6104FF77/658A44DB" Ref="XS?"  Part="1" 
F 0 "XS1" H 9346 1996 50  0000 R CNN
F 1 "SD_ebay-uSD-push_push_SMD" H 9346 1905 50  0000 R CNN
F 2 "ajm_kicad:Conn_uSDcard" H 9650 1950 10  0001 C CNN
F 3 "_" H 9650 1950 10  0001 C CNN
F 4 "_" H 9650 1950 10  0001 C CNN "Manf#"
F 5 "ebay" H 9650 1950 10  0001 C CNN "Manf"
	1    9650 1950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9275 2100 8650 2100
Wire Wire Line
	9275 1950 8650 1950
Wire Wire Line
	9275 1500 8650 1500
Wire Wire Line
	9800 900  9800 1325
NoConn ~ 9275 2400
Text GLabel 6625 3525 2    50   Input ~ 0
SD_DAT2
Text GLabel 6625 3325 2    50   Input ~ 0
SD_SS
Text GLabel 6625 3625 2    50   Input ~ 0
SD_DAT1
Text GLabel 6625 4025 2    50   Input ~ 0
I2C_SCLK
Text GLabel 6625 4125 2    50   Input ~ 0
MISO_D0
Text GLabel 6625 3725 2    50   Input ~ 0
MOSI_DI
Text GLabel 6625 3825 2    50   Input ~ 0
SD_CLK
$Comp
L power:GND #PWR05
U 1 1 658AC81C
P 7200 4575
F 0 "#PWR05" H 7200 4325 50  0001 C CNN
F 1 "GND" H 7205 4402 50  0000 C CNN
F 2 "" H 7200 4575 50  0001 C CNN
F 3 "" H 7200 4575 50  0001 C CNN
	1    7200 4575
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 658AD2D9
P 5075 4550
F 0 "#PWR03" H 5075 4300 50  0001 C CNN
F 1 "GND" H 5080 4377 50  0000 C CNN
F 2 "" H 5075 4550 50  0001 C CNN
F 3 "" H 5075 4550 50  0001 C CNN
	1    5075 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5225 2325 4700 2325
Wire Wire Line
	4700 2625 5225 2625
Wire Wire Line
	5225 2225 5025 2225
Wire Wire Line
	5225 2525 5025 2525
Text GLabel 4125 2225 0    50   Input ~ 0
KBD_DRV_0
Text GLabel 4125 2325 0    50   Input ~ 0
KBD_DRV_1
Text GLabel 4125 2525 0    50   Input ~ 0
KBD_DRV_2
Text GLabel 4125 2625 0    50   Input ~ 0
KBD_DRV_3
Wire Wire Line
	4725 2225 4125 2225
Wire Wire Line
	4400 2325 4125 2325
Wire Wire Line
	4725 2525 4125 2525
Wire Wire Line
	4400 2625 4125 2625
$Comp
L Device:D D1
U 1 1 658B4F4C
P 4550 2325
F 0 "D1" H 4425 2375 50  0000 C CNN
F 1 "SS36" H 4725 2375 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123" H 4550 2325 50  0001 C CNN
F 3 "~" H 4550 2325 50  0001 C CNN
	1    4550 2325
	1    0    0    -1  
$EndComp
$Comp
L Device:D D4
U 1 1 658B567A
P 4875 2525
F 0 "D4" H 4750 2575 50  0000 C CNN
F 1 "SS36" H 5050 2575 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123" H 4875 2525 50  0001 C CNN
F 3 "~" H 4875 2525 50  0001 C CNN
	1    4875 2525
	1    0    0    -1  
$EndComp
$Comp
L Device:D D2
U 1 1 658B5DBC
P 4550 2625
F 0 "D2" H 4425 2675 50  0000 C CNN
F 1 "SS36" H 4725 2675 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123" H 4550 2625 50  0001 C CNN
F 3 "~" H 4550 2625 50  0001 C CNN
	1    4550 2625
	1    0    0    -1  
$EndComp
Text GLabel 4125 2725 0    50   Input ~ 0
KBD_SENSE_0
Text GLabel 4125 2825 0    50   Input ~ 0
KBD_SENSE_1
Text GLabel 4125 3025 0    50   Input ~ 0
KBD_SENSE_2
Text GLabel 4125 3125 0    50   Input ~ 0
KBD_SENSE_3
Text GLabel 4125 3225 0    50   Input ~ 0
KBD_SENSE_4
Text GLabel 4125 3325 0    50   Input ~ 0
KBD_SENSE_5
Text GLabel 4125 3525 0    50   Input ~ 0
KBD_SENSE_6
Wire Wire Line
	5225 2725 4125 2725
Wire Wire Line
	4125 2825 5225 2825
Wire Wire Line
	5225 3025 4125 3025
Wire Wire Line
	4125 3125 5225 3125
Wire Wire Line
	5225 3225 4125 3225
Wire Wire Line
	4125 3325 5225 3325
Wire Wire Line
	5225 3525 4125 3525
Text GLabel 1025 4925 0    50   Input ~ 0
KBD_DRV_0
Text GLabel 1025 5025 0    50   Input ~ 0
KBD_DRV_1
Text GLabel 1025 5125 0    50   Input ~ 0
KBD_DRV_2
Text GLabel 1025 5225 0    50   Input ~ 0
KBD_DRV_3
Text GLabel 5575 6150 2    50   Input ~ 0
KBD_SENSE_0
Text GLabel 5600 6375 2    50   Input ~ 0
KBD_SENSE_1
Text GLabel 5600 6625 2    50   Input ~ 0
KBD_SENSE_2
Text GLabel 5600 6850 2    50   Input ~ 0
KBD_SENSE_3
Text GLabel 5600 7075 2    50   Input ~ 0
KBD_SENSE_4
Text GLabel 5600 7300 2    50   Input ~ 0
KBD_SENSE_5
Text GLabel 5600 7500 2    50   Input ~ 0
KBD_SENSE_6
Wire Wire Line
	925  6100 925  6150
Wire Wire Line
	925  6150 1950 6150
Wire Wire Line
	1950 6150 1950 6100
Wire Wire Line
	1950 6150 2975 6150
Wire Wire Line
	2975 6150 2975 6100
Connection ~ 1950 6150
Wire Wire Line
	2975 6150 3975 6150
Wire Wire Line
	3975 6150 3975 6100
Connection ~ 2975 6150
Wire Wire Line
	3975 6150 5575 6150
Connection ~ 3975 6150
Wire Wire Line
	925  6325 925  6375
Wire Wire Line
	925  6375 1950 6375
Wire Wire Line
	1950 6325 1950 6375
Connection ~ 1950 6375
Wire Wire Line
	1950 6375 2975 6375
Wire Wire Line
	2975 6325 2975 6375
Connection ~ 2975 6375
Wire Wire Line
	2975 6375 3975 6375
Wire Wire Line
	3975 6325 3975 6375
Connection ~ 3975 6375
Wire Wire Line
	3975 6375 5600 6375
Wire Wire Line
	925  6550 925  6625
Wire Wire Line
	925  6625 1950 6625
Wire Wire Line
	925  6850 1950 6850
Wire Wire Line
	925  6775 925  6850
Wire Wire Line
	925  7075 1950 7075
Wire Wire Line
	925  7000 925  7075
Wire Wire Line
	925  7225 925  7300
Wire Wire Line
	925  7300 1950 7300
Wire Wire Line
	925  7450 925  7500
Wire Wire Line
	925  7500 1950 7500
Wire Wire Line
	1025 5225 1325 5225
Wire Wire Line
	1325 5225 1325 6100
Wire Wire Line
	1325 6100 1325 6325
Connection ~ 1325 6100
Connection ~ 1325 6325
Wire Wire Line
	1325 6325 1325 6550
Wire Wire Line
	1325 6550 1325 6775
Connection ~ 1325 6550
Connection ~ 1325 6775
Wire Wire Line
	1325 6775 1325 7000
Connection ~ 1325 7000
Wire Wire Line
	1325 7000 1325 7225
Connection ~ 1325 7225
Wire Wire Line
	1325 7225 1325 7450
Wire Wire Line
	1025 5125 2350 5125
Wire Wire Line
	2350 5125 2350 6100
Connection ~ 2350 6100
Wire Wire Line
	2350 6100 2350 6325
Connection ~ 2350 6325
Wire Wire Line
	2350 6325 2350 6550
Wire Wire Line
	2350 6550 2350 6775
Connection ~ 2350 6550
Connection ~ 2350 6775
Wire Wire Line
	2350 6775 2350 7000
Connection ~ 2350 7000
Wire Wire Line
	2350 7000 2350 7225
Connection ~ 2350 7225
Wire Wire Line
	2350 7225 2350 7450
Wire Wire Line
	1025 5025 3375 5025
Wire Wire Line
	3375 5025 3375 6100
Connection ~ 3375 6100
Wire Wire Line
	3375 6100 3375 6325
Wire Wire Line
	3375 6325 3375 6550
Connection ~ 3375 6325
Connection ~ 3375 6550
Wire Wire Line
	3375 6550 3375 6775
Connection ~ 3375 6775
Wire Wire Line
	3375 6775 3375 7000
Connection ~ 3375 7000
Wire Wire Line
	3375 7000 3375 7225
Connection ~ 3375 7225
Wire Wire Line
	3375 7225 3375 7450
Wire Wire Line
	1025 4925 4375 4925
Wire Wire Line
	4375 4925 4375 6100
Connection ~ 4375 6100
Wire Wire Line
	4375 6100 4375 6325
Wire Wire Line
	4375 6325 4375 6550
Connection ~ 4375 6325
Connection ~ 4375 6550
Wire Wire Line
	4375 6550 4375 6775
Connection ~ 4375 6775
Wire Wire Line
	4375 6775 4375 7000
Connection ~ 4375 7000
Wire Wire Line
	4375 7000 4375 7225
Connection ~ 4375 7225
Wire Wire Line
	4375 7225 4375 7450
Wire Wire Line
	2975 6550 2975 6625
Connection ~ 2975 6625
Wire Wire Line
	2975 6625 3975 6625
Wire Wire Line
	2975 6775 2975 6850
Connection ~ 2975 6850
Wire Wire Line
	2975 6850 3975 6850
Wire Wire Line
	2975 7000 2975 7075
Connection ~ 2975 7075
Wire Wire Line
	2975 7075 3975 7075
Wire Wire Line
	2975 7225 2975 7300
Connection ~ 2975 7300
Wire Wire Line
	2975 7300 3975 7300
Wire Wire Line
	2975 7450 2975 7500
Connection ~ 2975 7500
Wire Wire Line
	2975 7500 3975 7500
Wire Wire Line
	3975 7450 3975 7500
Connection ~ 3975 7500
Wire Wire Line
	3975 7500 5600 7500
Wire Wire Line
	3975 7225 3975 7300
Connection ~ 3975 7300
Wire Wire Line
	3975 7300 5600 7300
Wire Wire Line
	3975 7000 3975 7075
Connection ~ 3975 7075
Wire Wire Line
	3975 7075 5600 7075
Wire Wire Line
	3975 6775 3975 6850
Connection ~ 3975 6850
Wire Wire Line
	3975 6850 5600 6850
Wire Wire Line
	3975 6550 3975 6625
Connection ~ 3975 6625
Wire Wire Line
	3975 6625 5600 6625
Wire Wire Line
	1950 6550 1950 6625
Connection ~ 1950 6625
Wire Wire Line
	1950 6625 2975 6625
Wire Wire Line
	1950 6775 1950 6850
Connection ~ 1950 6850
Wire Wire Line
	1950 6850 2975 6850
Wire Wire Line
	1950 7000 1950 7075
Connection ~ 1950 7075
Wire Wire Line
	1950 7075 2975 7075
Wire Wire Line
	1950 7225 1950 7300
Connection ~ 1950 7300
Wire Wire Line
	1950 7300 2975 7300
Wire Wire Line
	1950 7450 1950 7500
Connection ~ 1950 7500
Wire Wire Line
	1950 7500 2975 7500
Wire Wire Line
	5225 2425 5075 2425
Wire Wire Line
	5075 2425 5075 2925
Wire Wire Line
	5225 2925 5075 2925
Connection ~ 5075 2925
Wire Wire Line
	5075 2925 5075 3425
Wire Wire Line
	5225 3425 5075 3425
Connection ~ 5075 3425
Wire Wire Line
	5075 3425 5075 3925
Wire Wire Line
	5225 3925 5075 3925
Connection ~ 5075 3925
Wire Wire Line
	5075 3925 5075 4550
Wire Wire Line
	5925 4575 5925 4325
Wire Wire Line
	7200 3925 7200 4575
Wire Wire Line
	6625 3925 7200 3925
Wire Wire Line
	6625 3425 7200 3425
Wire Wire Line
	7200 3425 7200 3925
Connection ~ 7200 3925
Wire Wire Line
	6625 2925 7200 2925
Wire Wire Line
	7200 2925 7200 3425
Connection ~ 7200 3425
Wire Wire Line
	6625 2425 7200 2425
Wire Wire Line
	7200 2425 7200 2925
Connection ~ 7200 2925
Text GLabel 4900 1375 2    50   Input ~ 0
I2C_SCLK
Text GLabel 4900 1275 2    50   Input ~ 0
I2C_SDA
$Comp
L power:GND #PWR?
U 1 1 65946D61
P 3550 1850
AR Path="/63690FA3/65946D61" Ref="#PWR?"  Part="1" 
AR Path="/65946D61" Ref="#PWR01"  Part="1" 
F 0 "#PWR01" H 3550 1600 50  0001 C CNN
F 1 "GND" H 3555 1677 50  0000 C CNN
F 2 "" H 3550 1850 50  0001 C CNN
F 3 "" H 3550 1850 50  0001 C CNN
	1    3550 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 1850 3550 1600
$Comp
L Device:R_Small R?
U 1 1 65946D68
P 4050 1050
AR Path="/63690FA3/65946D68" Ref="R?"  Part="1" 
AR Path="/65946D68" Ref="R1"  Part="1" 
F 0 "R1" H 4109 1096 50  0000 L CNN
F 1 "4k7" H 4109 1005 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 4050 1050 50  0001 C CNN
F 3 "~" H 4050 1050 50  0001 C CNN
	1    4050 1050
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 65946D6E
P 4550 1050
AR Path="/63690FA3/65946D6E" Ref="R?"  Part="1" 
AR Path="/65946D6E" Ref="R2"  Part="1" 
F 0 "R2" H 4609 1096 50  0000 L CNN
F 1 "4k7" H 4609 1005 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 4550 1050 50  0001 C CNN
F 3 "~" H 4550 1050 50  0001 C CNN
	1    4550 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 1150 4050 1275
Connection ~ 4050 1275
Wire Wire Line
	3600 725  3600 850 
Wire Wire Line
	4050 850  4050 950 
Wire Wire Line
	4050 850  4550 850 
Wire Wire Line
	4550 850  4550 950 
Connection ~ 4050 850 
Wire Wire Line
	3600 850  4050 850 
Connection ~ 3600 850 
Wire Wire Line
	3600 850  3600 1475
Wire Wire Line
	4050 1275 4900 1275
Wire Wire Line
	4550 1150 4550 1375
Connection ~ 4550 1375
Wire Wire Line
	4550 1375 4900 1375
$Comp
L power:+3V3 #PWR02
U 1 1 65946D82
P 3600 725
F 0 "#PWR02" H 3600 575 50  0001 C CNN
F 1 "+3V3" H 3615 898 50  0000 C CNN
F 2 "" H 3600 725 50  0001 C CNN
F 3 "" H 3600 725 50  0001 C CNN
	1    3600 725 
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP1
U 1 1 65946D88
P 2825 1475
F 0 "JP1" H 3750 1475 50  0000 C CNN
F 1 "Pin3 3V3" H 4025 1475 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 2825 1475 50  0001 C CNN
F 3 "~" H 2825 1475 50  0001 C CNN
	1    2825 1475
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP2
U 1 1 65946D8E
P 2825 1600
F 0 "JP2" H 3750 1600 50  0000 C CNN
F 1 "Pin3 GND" H 4025 1600 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 2825 1600 50  0001 C CNN
F 3 "~" H 2825 1600 50  0001 C CNN
	1    2825 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP3
U 1 1 65946D94
P 2825 1725
F 0 "JP3" H 2025 1725 50  0000 C CNN
F 1 "Pin4 3V3" H 2300 1725 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 2825 1725 50  0001 C CNN
F 3 "~" H 2825 1725 50  0001 C CNN
	1    2825 1725
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP4
U 1 1 65946D9A
P 2825 1850
F 0 "JP4" H 2025 1850 50  0000 C CNN
F 1 "Pin4 GND" H 2300 1850 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 2825 1850 50  0001 C CNN
F 3 "~" H 2825 1850 50  0001 C CNN
	1    2825 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2725 1600 2725 1475
Connection ~ 2725 1475
Wire Wire Line
	2525 1850 2725 1850
Wire Wire Line
	2725 1725 2525 1725
Connection ~ 2525 1725
Wire Wire Line
	2525 1725 2525 1850
Wire Wire Line
	2925 1475 3225 1475
Wire Wire Line
	2925 1600 3550 1600
Wire Wire Line
	2925 1725 3225 1725
Wire Wire Line
	3225 1725 3225 1475
Connection ~ 3225 1475
Wire Wire Line
	3225 1475 3600 1475
Wire Wire Line
	2925 1850 3550 1850
Connection ~ 3550 1850
Text GLabel 4975 4125 0    50   Input ~ 0
I2C_SCLK
Text GLabel 4975 4025 0    50   Input ~ 0
I2C_SDA
Wire Wire Line
	5225 4025 4975 4025
Wire Wire Line
	4975 4125 5225 4125
$Comp
L Connector:Conn_01x13_Female J2
U 1 1 659D721D
P 900 2975
F 0 "J2" H 792 3760 50  0000 C CNN
F 1 "Conn_01x13_Female" H 792 3669 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 900 2975 50  0001 C CNN
F 3 "~" H 900 2975 50  0001 C CNN
	1    900  2975
	-1   0    0    -1  
$EndComp
Text GLabel 1100 2375 2    50   Input ~ 0
KBD_DRV_0
Text GLabel 1100 2475 2    50   Input ~ 0
KBD_DRV_1
Text GLabel 1100 2575 2    50   Input ~ 0
KBD_DRV_2
Text GLabel 1100 2675 2    50   Input ~ 0
KBD_DRV_3
Text GLabel 1100 2775 2    50   Input ~ 0
KBD_SENSE_0
Text GLabel 1100 2875 2    50   Input ~ 0
KBD_SENSE_1
Text GLabel 1100 2975 2    50   Input ~ 0
KBD_SENSE_2
Text GLabel 1100 3075 2    50   Input ~ 0
KBD_SENSE_3
Text GLabel 1100 3175 2    50   Input ~ 0
KBD_SENSE_4
Text GLabel 1100 3275 2    50   Input ~ 0
KBD_SENSE_5
Text GLabel 1100 3375 2    50   Input ~ 0
KBD_SENSE_6
$Comp
L power:GND #PWR?
U 1 1 659F3F67
P 1350 3650
AR Path="/63690FA3/659F3F67" Ref="#PWR?"  Part="1" 
AR Path="/659F3F67" Ref="#PWR0101"  Part="1" 
F 0 "#PWR0101" H 1350 3400 50  0001 C CNN
F 1 "GND" H 1355 3477 50  0000 C CNN
F 2 "" H 1350 3650 50  0001 C CNN
F 3 "" H 1350 3650 50  0001 C CNN
	1    1350 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 3575 1350 3575
Wire Wire Line
	1350 3575 1350 3650
$Comp
L power:+3V3 #PWR0102
U 1 1 65A022E9
P 2000 3425
F 0 "#PWR0102" H 2000 3275 50  0001 C CNN
F 1 "+3V3" H 2015 3598 50  0000 C CNN
F 2 "" H 2000 3425 50  0001 C CNN
F 3 "" H 2000 3425 50  0001 C CNN
	1    2000 3425
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 3475 2000 3475
Wire Wire Line
	2000 3475 2000 3425
$Comp
L power:+3V3 #PWR0103
U 1 1 65A10FFF
P 7425 2100
F 0 "#PWR0103" H 7425 1950 50  0001 C CNN
F 1 "+3V3" H 7440 2273 50  0000 C CNN
F 2 "" H 7425 2100 50  0001 C CNN
F 3 "" H 7425 2100 50  0001 C CNN
	1    7425 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6625 2625 7425 2625
$Comp
L Connector:Conn_01x03_Female J4
U 1 1 65A1FC20
P 5925 4775
F 0 "J4" V 5771 4923 50  0000 L CNN
F 1 "Conn_01x03_Female" V 5862 4923 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Vertical" H 5925 4775 50  0001 C CNN
F 3 "~" H 5925 4775 50  0001 C CNN
	1    5925 4775
	0    1    1    0   
$EndComp
Wire Wire Line
	5825 4325 5825 4575
Wire Wire Line
	6025 4575 6025 4325
$Comp
L pak_breakout:AP1117-33 U2
U 1 1 65A3160D
P 9100 3500
F 0 "U2" H 9100 3742 50  0000 C CNN
F 1 "AP1117-33" H 9100 3651 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 9100 3700 50  0001 C CNN
F 3 "" H 9200 3250 50  0001 C CNN
	1    9100 3500
	1    0    0    -1  
$EndComp
Text GLabel 6875 2325 2    50   Input ~ 0
VSYS
Wire Wire Line
	6625 2325 6875 2325
Text GLabel 8500 3500 0    50   Input ~ 0
VSYS
Wire Wire Line
	8500 3500 8625 3500
$Comp
L power:GND #PWR?
U 1 1 65A49210
P 9100 3925
AR Path="/60C45767/65A49210" Ref="#PWR?"  Part="1" 
AR Path="/65A49210" Ref="#PWR0104"  Part="1" 
AR Path="/6104FF77/65A49210" Ref="#PWR?"  Part="1" 
F 0 "#PWR0104" H 9100 3675 50  0001 C CNN
F 1 "GND" H 9105 3752 50  0000 C CNN
F 2 "" H 9100 3925 50  0001 C CNN
F 3 "" H 9100 3925 50  0001 C CNN
	1    9100 3925
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9100 3800 9100 3875
$Comp
L Device:Jumper_NO_Small JP5
U 1 1 65A59DE7
P 9700 3500
F 0 "JP5" H 9675 3600 50  0000 C CNN
F 1 "Jumper_NO_Small" H 9700 3594 50  0001 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 9700 3500 50  0001 C CNN
F 3 "~" H 9700 3500 50  0001 C CNN
	1    9700 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 3500 9525 3500
$Comp
L power:+3V3 #PWR0105
U 1 1 65A69A9F
P 9925 3175
F 0 "#PWR0105" H 9925 3025 50  0001 C CNN
F 1 "+3V3" H 9940 3348 50  0000 C CNN
F 2 "" H 9925 3175 50  0001 C CNN
F 3 "" H 9925 3175 50  0001 C CNN
	1    9925 3175
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 3500 9925 3500
Wire Wire Line
	9925 3500 9925 3175
$Comp
L Device:C_Small C2
U 1 1 65A79AD6
P 9525 3775
F 0 "C2" H 9617 3821 50  0000 L CNN
F 1 "100nF" H 9617 3730 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 9525 3775 50  0001 C CNN
F 3 "~" H 9525 3775 50  0001 C CNN
	1    9525 3775
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 65A7A14A
P 8625 3750
F 0 "C1" H 8717 3796 50  0000 L CNN
F 1 "100nF" H 8717 3705 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 8625 3750 50  0001 C CNN
F 3 "~" H 8625 3750 50  0001 C CNN
	1    8625 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9525 3675 9525 3500
Connection ~ 9525 3500
Wire Wire Line
	9525 3500 9600 3500
Wire Wire Line
	9525 3875 9100 3875
Connection ~ 9100 3875
Wire Wire Line
	9100 3875 9100 3925
Wire Wire Line
	9100 3875 8625 3875
Wire Wire Line
	8625 3875 8625 3850
Wire Wire Line
	8625 3650 8625 3500
Connection ~ 8625 3500
Wire Wire Line
	8625 3500 8800 3500
Text Notes 8575 4400 0    50   ~ 0
Power from USB for larger display
$Comp
L Connector:Conn_01x10_Female J3
U 1 1 65A9CDFE
P 2325 2800
F 0 "J3" H 2217 3385 50  0000 C CNN
F 1 "Conn_01x10_Female" H 2217 3294 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x10_P2.54mm_Vertical" H 2325 2800 50  0001 C CNN
F 3 "~" H 2325 2800 50  0001 C CNN
	1    2325 2800
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 65A9FA3B
P 2775 3375
AR Path="/63690FA3/65A9FA3B" Ref="#PWR?"  Part="1" 
AR Path="/65A9FA3B" Ref="#PWR0106"  Part="1" 
F 0 "#PWR0106" H 2775 3125 50  0001 C CNN
F 1 "GND" H 2780 3202 50  0000 C CNN
F 2 "" H 2775 3375 50  0001 C CNN
F 3 "" H 2775 3375 50  0001 C CNN
	1    2775 3375
	1    0    0    -1  
$EndComp
Wire Wire Line
	2525 3300 2775 3300
Wire Wire Line
	2775 3300 2775 3375
$Comp
L power:+3V3 #PWR0107
U 1 1 65A9FA43
P 2950 3150
F 0 "#PWR0107" H 2950 3000 50  0001 C CNN
F 1 "+3V3" H 2965 3323 50  0000 C CNN
F 2 "" H 2950 3150 50  0001 C CNN
F 3 "" H 2950 3150 50  0001 C CNN
	1    2950 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2525 3200 2950 3200
Wire Wire Line
	2950 3200 2950 3150
Text GLabel 2525 2500 2    50   Input ~ 0
I2C_SCLK
Text GLabel 2525 2400 2    50   Input ~ 0
I2C_SDA
Text GLabel 4875 3825 0    50   Input ~ 0
GPIO13
Text GLabel 4875 3625 0    50   Input ~ 0
GPIO11
Text GLabel 4875 3725 0    50   Input ~ 0
GPIO12
Text GLabel 2525 2900 2    50   Input ~ 0
GPIO26
Text GLabel 2525 3000 2    50   Input ~ 0
GPIO27
Text GLabel 2525 3100 2    50   Input ~ 0
GPIO28
Text GLabel 6625 3125 2    50   Input ~ 0
GPIO26
Text GLabel 6625 3025 2    50   Input ~ 0
GPIO27
Text GLabel 6625 2825 2    50   Input ~ 0
GPIO28
Wire Wire Line
	5225 3625 4875 3625
Wire Wire Line
	4875 3725 5225 3725
Wire Wire Line
	5225 3825 4875 3825
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 65B57FDA
P 825 1475
AR Path="/6104FF77/65B57FDA" Ref="J?"  Part="1" 
AR Path="/65B57FDA" Ref="J5"  Part="1" 
AR Path="/63690FA3/65B57FDA" Ref="J?"  Part="1" 
F 0 "J5" H 717 1760 50  0000 C CNN
F 1 "Conn_01x04_Female" H 717 1669 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 825 1475 50  0001 C CNN
F 3 "~" H 825 1475 50  0001 C CNN
	1    825  1475
	-1   0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP6
U 1 1 65BB290E
P 7425 2350
F 0 "JP6" V 7425 2500 50  0000 C CNN
F 1 "Jumper_NO_Small" H 7425 2444 50  0001 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 7425 2350 50  0001 C CNN
F 3 "~" H 7425 2350 50  0001 C CNN
	1    7425 2350
	0    1    1    0   
$EndComp
Wire Wire Line
	7425 2450 7425 2625
Wire Wire Line
	7425 2100 7425 2250
$Comp
L Connector:Conn_01x02_Female J7
U 1 1 65BD6A44
P 6425 1050
F 0 "J7" H 6317 725 50  0000 C CNN
F 1 "Conn_01x02_Female" H 6317 816 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 6425 1050 50  0001 C CNN
F 3 "~" H 6425 1050 50  0001 C CNN
	1    6425 1050
	-1   0    0    1   
$EndComp
$Comp
L Device:D D5
U 1 1 65BD8ABF
P 7000 950
F 0 "D5" H 6875 1000 50  0000 C CNN
F 1 "SS36" H 7175 1000 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123" H 7000 950 50  0001 C CNN
F 3 "~" H 7000 950 50  0001 C CNN
	1    7000 950 
	-1   0    0    1   
$EndComp
Text GLabel 7325 950  2    50   Input ~ 0
VSYS
Wire Wire Line
	7150 950  7325 950 
Wire Wire Line
	6625 950  6850 950 
$Comp
L power:GND #PWR?
U 1 1 65C087CC
P 6950 1200
AR Path="/60C45767/65C087CC" Ref="#PWR?"  Part="1" 
AR Path="/65C087CC" Ref="#PWR0108"  Part="1" 
AR Path="/6104FF77/65C087CC" Ref="#PWR?"  Part="1" 
F 0 "#PWR0108" H 6950 950 50  0001 C CNN
F 1 "GND" H 6955 1027 50  0000 C CNN
F 2 "" H 6950 1200 50  0001 C CNN
F 3 "" H 6950 1200 50  0001 C CNN
	1    6950 1200
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6625 1050 6950 1050
Wire Wire Line
	6950 1050 6950 1200
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 65C356E2
P 825 875
AR Path="/6104FF77/65C356E2" Ref="J?"  Part="1" 
AR Path="/65C356E2" Ref="J1"  Part="1" 
AR Path="/63690FA3/65C356E2" Ref="J?"  Part="1" 
F 0 "J1" H 717 1160 50  0000 C CNN
F 1 "Conn_01x04_Female" H 717 1069 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 825 875 50  0001 C CNN
F 3 "~" H 825 875 50  0001 C CNN
	1    825  875 
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 65C4CF1A
P 1500 875
AR Path="/6104FF77/65C4CF1A" Ref="J?"  Part="1" 
AR Path="/65C4CF1A" Ref="J6"  Part="1" 
AR Path="/63690FA3/65C4CF1A" Ref="J?"  Part="1" 
F 0 "J6" H 1392 1160 50  0000 C CNN
F 1 "Conn_01x04_Female" H 1392 1069 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 1500 875 50  0001 C CNN
F 3 "~" H 1500 875 50  0001 C CNN
	1    1500 875 
	-1   0    0    -1  
$EndComp
Text GLabel 1025 775  2    50   Input ~ 0
OLED_P1
Text GLabel 1025 875  2    50   Input ~ 0
OLED_P2
Text GLabel 1025 975  2    50   Input ~ 0
OLED_P3
Text GLabel 1025 1075 2    50   Input ~ 0
OLED_P4
Text GLabel 1025 1375 2    50   Input ~ 0
OLED_P1
Text GLabel 1025 1475 2    50   Input ~ 0
OLED_P2
Text GLabel 1025 1575 2    50   Input ~ 0
OLED_P3
Text GLabel 1025 1675 2    50   Input ~ 0
OLED_P4
Text GLabel 1700 775  2    50   Input ~ 0
OLED_P1
Text GLabel 1700 875  2    50   Input ~ 0
OLED_P2
Text GLabel 1700 975  2    50   Input ~ 0
OLED_P3
Text GLabel 1700 1075 2    50   Input ~ 0
OLED_P4
Wire Wire Line
	2525 1575 2525 1725
Text GLabel 2400 1275 0    50   Input ~ 0
OLED_P1
Text GLabel 2400 1375 0    50   Input ~ 0
OLED_P2
Text GLabel 2400 1475 0    50   Input ~ 0
OLED_P3
Text GLabel 2400 1575 0    50   Input ~ 0
OLED_P4
Wire Wire Line
	2400 1275 4050 1275
Wire Wire Line
	2400 1375 4550 1375
Wire Wire Line
	2400 1475 2725 1475
Wire Wire Line
	2525 1575 2400 1575
Text GLabel 2525 2800 2    50   Input ~ 0
GPIO13
Text GLabel 2525 2600 2    50   Input ~ 0
GPIO11
Text GLabel 2525 2700 2    50   Input ~ 0
GPIO12
Text Notes 7775 825  0    39   ~ 0
VSYS can be powered here from a 5V supply. For \nsending 5V out of the PCB, either short the diode\n and use this connector or remove the diode\nand connect the connector to VBUS
$EndSCHEMATC
