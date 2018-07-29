EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:arduino
LIBS:MSEShadowUno-cache
EELAYER 25 0
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
L Conn_01x03_Male J5
U 1 1 5AFD552F
P 9200 2350
F 0 "J5" H 9200 2550 50  0000 C CNN
F 1 "Motor" H 9200 2150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 9200 2350 50  0001 C CNN
F 3 "" H 9200 2350 50  0001 C CNN
	1    9200 2350
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x03_Male J6
U 1 1 5AFD5578
P 9200 3000
F 0 "J6" H 9200 3200 50  0000 C CNN
F 1 "Servo" H 9200 2800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 9200 3000 50  0001 C CNN
F 3 "" H 9200 3000 50  0001 C CNN
	1    9200 3000
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x03_Male J3
U 1 1 5AFD55D9
P 9150 3750
F 0 "J3" H 9150 3950 50  0000 C CNN
F 1 "Sparkfun MP3 Trigger" H 9150 3550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 9150 3750 50  0001 C CNN
F 3 "" H 9150 3750 50  0001 C CNN
	1    9150 3750
	1    0    0    -1  
$EndComp
$Comp
L Audio-Jack-3_2Switches J7
U 1 1 5AFD567D
P 9200 5250
F 0 "J7" H 9200 5540 50  0000 C CNN
F 1 "Audio Out" H 9050 5050 50  0000 L CNN
F 2 "Connectors:Stereo_Jack_3.5mm_Switch_Ledino_KB3SPRS" H 9450 5350 50  0001 C CNN
F 3 "" H 9450 5350 50  0001 C CNN
	1    9200 5250
	1    0    0    -1  
$EndComp
$Comp
L Screw_Terminal_01x02 J2
U 1 1 5AFD579D
P 7850 5300
F 0 "J2" H 7850 5400 50  0000 C CNN
F 1 "Power In" H 7850 5100 50  0000 C CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS1.5-2pol" H 7850 5300 50  0001 C CNN
F 3 "" H 7850 5300 50  0001 C CNN
	1    7850 5300
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x03_Male J4
U 1 1 5AFD5A36
P 9150 4400
F 0 "J4" H 9150 4500 50  0000 C CNN
F 1 "Power Jumper" H 9150 4200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 9150 4400 50  0001 C CNN
F 3 "" H 9150 4400 50  0001 C CNN
	1    9150 4400
	1    0    0    -1  
$EndComp
NoConn ~ 10450 2350
NoConn ~ 10450 2450
NoConn ~ 10450 2550
NoConn ~ 10450 2650
NoConn ~ 10450 2750
NoConn ~ 10450 2850
NoConn ~ 10450 2950
NoConn ~ 10450 3050
NoConn ~ 10450 3150
NoConn ~ 10450 3250
NoConn ~ 10450 3550
NoConn ~ 10450 3650
NoConn ~ 10450 3350
NoConn ~ 10450 3450
NoConn ~ 9400 5050
NoConn ~ 9400 5350
Wire Wire Line
	7650 5300 7650 4500
Wire Wire Line
	7650 4500 9350 4500
Wire Wire Line
	9700 2350 9400 2350
Wire Wire Line
	9400 3000 10000 3000
Connection ~ 9700 3000
Wire Wire Line
	9350 3750 10100 3750
Connection ~ 9700 3750
Connection ~ 9700 4400
Wire Wire Line
	9800 4200 7550 4200
Wire Wire Line
	9800 1850 9800 4200
Wire Wire Line
	9800 2250 9400 2250
Wire Wire Line
	9400 2900 9800 2900
Connection ~ 9800 2900
Wire Wire Line
	9350 3650 9800 3650
Connection ~ 9800 3650
Wire Wire Line
	10450 4150 9800 4150
Connection ~ 9800 4150
Wire Wire Line
	9000 4800 9800 4800
Wire Wire Line
	9800 4800 9800 4150
Wire Wire Line
	9400 5250 10300 5250
Wire Wire Line
	10300 5250 10300 4050
Wire Wire Line
	10300 4050 10450 4050
Wire Wire Line
	9400 5150 10200 5150
Wire Wire Line
	10200 5150 10200 3950
Wire Wire Line
	10200 3950 10450 3950
Wire Wire Line
	9350 4400 10150 4400
Wire Wire Line
	10150 4400 10150 4250
Wire Wire Line
	10150 4250 10450 4250
Wire Wire Line
	7650 5400 7550 5400
Wire Wire Line
	7550 5400 7550 4200
Wire Wire Line
	9800 1850 3750 1850
Connection ~ 9800 2250
Wire Wire Line
	3900 1950 10100 1950
Connection ~ 9700 2350
Wire Wire Line
	10450 3750 10300 3750
Wire Wire Line
	10300 3750 10300 3850
Wire Wire Line
	10300 3850 7450 3850
Connection ~ 9350 3850
Wire Wire Line
	7400 2450 9400 2450
Wire Wire Line
	7300 3100 9400 3100
Wire Wire Line
	9000 5450 9000 4800
Wire Wire Line
	10100 1950 10100 4400
Wire Wire Line
	10100 4400 9700 4400
Wire Wire Line
	9700 3000 9700 2350
Connection ~ 10100 3750
Wire Wire Line
	10000 3000 10000 4300
Wire Wire Line
	10000 4300 9350 4300
Wire Wire Line
	3750 1850 3750 2600
$Comp
L Arduino_Uno_Shield XA1
U 1 1 5B5CF1ED
P 5350 3350
F 0 "XA1" V 5450 3350 60  0000 C CNN
F 1 "Arduino_Uno_Shield" V 5250 3350 60  0000 C CNN
F 2 "Arduino:Arduino_Uno_Shield_No_ICSP" H 7150 7100 60  0001 C CNN
F 3 "" H 7150 7100 60  0001 C CNN
	1    5350 3350
	1    0    0    1   
$EndComp
Wire Wire Line
	7300 3100 7300 4200
Wire Wire Line
	7300 4200 6650 4200
Wire Wire Line
	7400 2450 7400 4300
Wire Wire Line
	7400 4300 6650 4300
Wire Wire Line
	7450 3850 7450 4400
Wire Wire Line
	7450 4400 6650 4400
Wire Wire Line
	3900 1950 3900 2300
Wire Wire Line
	3900 2300 4050 2300
Wire Wire Line
	3750 2600 4050 2600
$Comp
L Conn_01x20_Female J1
U 1 1 5AFD54DB
P 10650 3250
F 0 "J1" H 10650 4250 50  0000 C CNN
F 1 "MDFly MP3 Trigger" H 10650 2150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x20_Pitch2.54mm" H 10650 3250 50  0001 C CNN
F 3 "" H 10650 3250 50  0001 C CNN
	1    10650 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 3800 10450 3800
Wire Wire Line
	10450 3800 10450 3850
$EndSCHEMATC
