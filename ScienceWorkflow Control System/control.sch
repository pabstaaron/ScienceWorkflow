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
LIBS:control
LIBS:adafruitpwm
LIBS:5v_ubec
LIBS:cont_rot_servo
LIBS:micro_serv
LIBS:control-cache
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
L RPI3 R?
U 1 1 591A352B
P 2450 1850
F 0 "R?" H 2850 -200 60  0000 C CNN
F 1 "RPI3" H 2850 1900 60  0000 C CNN
F 2 "" H 2850 1900 60  0001 C CNN
F 3 "" H 2850 1900 60  0001 C CNN
	1    2450 1850
	1    0    0    -1  
$EndComp
$Comp
L AdafruitPWM A?
U 1 1 591B6C82
P 6450 3400
F 0 "A?" H 6650 350 60  0000 C CNN
F 1 "AdafruitPWM" H 6600 6150 60  0000 C CNN
F 2 "" H 11400 1900 60  0001 C CNN
F 3 "" H 11400 1900 60  0001 C CNN
	1    6450 3400
	1    0    0    -1  
$EndComp
$Comp
L 5V_UBEC U?
U 1 1 591B714E
P 2450 5900
F 0 "U?" H 2400 5650 60  0000 C CNN
F 1 "5V_UBEC" H 2400 6250 60  0000 C CNN
F 2 "" H 4100 7250 60  0001 C CNN
F 3 "" H 4100 7250 60  0001 C CNN
	1    2450 5900
	1    0    0    -1  
$EndComp
$Comp
L 5V_UBEC U?
U 1 1 591B71CF
P 2450 6700
F 0 "U?" H 2400 6450 60  0000 C CNN
F 1 "5V_UBEC" H 2400 7050 60  0000 C CNN
F 2 "" H 4100 8050 60  0001 C CNN
F 3 "" H 4100 8050 60  0001 C CNN
	1    2450 6700
	1    0    0    -1  
$EndComp
$Comp
L CP1 C?
U 1 1 591B7206
P 3000 5850
F 0 "C?" H 3025 5950 50  0000 L CNN
F 1 "100u" H 3025 5750 50  0000 L CNN
F 2 "" H 3000 5850 50  0001 C CNN
F 3 "" H 3000 5850 50  0001 C CNN
	1    3000 5850
	1    0    0    -1  
$EndComp
$Comp
L CP1 C?
U 1 1 591B7246
P 3000 6650
F 0 "C?" H 3025 6750 50  0000 L CNN
F 1 "100u" H 3025 6550 50  0000 L CNN
F 2 "" H 3000 6650 50  0001 C CNN
F 3 "" H 3000 6650 50  0001 C CNN
	1    3000 6650
	1    0    0    -1  
$EndComp
$Comp
L Cont_Rot_Servo C?
U 1 1 591B759D
P 7950 850
F 0 "C?" H 8200 600 60  0000 C CNN
F 1 "Cont_Rot_Servo" H 8200 1000 60  0000 C CNN
F 2 "" H 9200 1800 60  0001 C CNN
F 3 "" H 9200 1800 60  0001 C CNN
	1    7950 850 
	1    0    0    -1  
$EndComp
$Comp
L Micro_Serv M?
U 1 1 591B768A
P 7950 2950
F 0 "M?" H 8200 2800 60  0000 C CNN
F 1 "Micro_Serv" H 8200 3200 60  0000 C CNN
F 2 "" H 7950 2950 60  0001 C CNN
F 3 "" H 7950 2950 60  0001 C CNN
	1    7950 2950
	1    0    0    -1  
$EndComp
$Comp
L Micro_Serv M?
U 1 1 591B7719
P 7950 3450
F 0 "M?" H 8200 3300 60  0000 C CNN
F 1 "Micro_Serv" H 8200 3700 60  0000 C CNN
F 2 "" H 7950 3450 60  0001 C CNN
F 3 "" H 7950 3450 60  0001 C CNN
	1    7950 3450
	1    0    0    -1  
$EndComp
$Comp
L Cont_Rot_Servo C?
U 1 1 591B7758
P 7950 1350
F 0 "C?" H 8200 1100 60  0000 C CNN
F 1 "Cont_Rot_Servo" H 8200 1500 60  0000 C CNN
F 2 "" H 9200 2300 60  0001 C CNN
F 3 "" H 9200 2300 60  0001 C CNN
	1    7950 1350
	1    0    0    -1  
$EndComp
$Comp
L Cont_Rot_Servo C?
U 1 1 591B77AE
P 7950 1850
F 0 "C?" H 8200 1600 60  0000 C CNN
F 1 "Cont_Rot_Servo" H 8200 2000 60  0000 C CNN
F 2 "" H 9200 2800 60  0001 C CNN
F 3 "" H 9200 2800 60  0001 C CNN
	1    7950 1850
	1    0    0    -1  
$EndComp
$Comp
L Cont_Rot_Servo C?
U 1 1 591B77ED
P 7950 2350
F 0 "C?" H 8200 2100 60  0000 C CNN
F 1 "Cont_Rot_Servo" H 8200 2500 60  0000 C CNN
F 2 "" H 9200 3300 60  0001 C CNN
F 3 "" H 9200 3300 60  0001 C CNN
	1    7950 2350
	1    0    0    -1  
$EndComp
$Comp
L +9V #PWR?
U 1 1 591B7C05
P 1450 5700
F 0 "#PWR?" H 1450 5550 50  0001 C CNN
F 1 "+9V" H 1450 5840 50  0000 C CNN
F 2 "" H 1450 5700 50  0001 C CNN
F 3 "" H 1450 5700 50  0001 C CNN
	1    1450 5700
	0    -1   -1   0   
$EndComp
$Comp
L +9V #PWR?
U 1 1 591B7C53
P 1500 6500
F 0 "#PWR?" H 1500 6350 50  0001 C CNN
F 1 "+9V" H 1500 6640 50  0000 C CNN
F 2 "" H 1500 6500 50  0001 C CNN
F 3 "" H 1500 6500 50  0001 C CNN
	1    1500 6500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1450 5700 1800 5700
Wire Wire Line
	1500 6500 1800 6500
$Comp
L GNDA #PWR?
U 1 1 591B7CDC
P 1400 6000
F 0 "#PWR?" H 1400 5750 50  0001 C CNN
F 1 "GNDA" H 1400 5850 50  0000 C CNN
F 2 "" H 1400 6000 50  0001 C CNN
F 3 "" H 1400 6000 50  0001 C CNN
	1    1400 6000
	0    1    1    0   
$EndComp
$Comp
L GNDA #PWR?
U 1 1 591B7D12
P 1450 6800
F 0 "#PWR?" H 1450 6550 50  0001 C CNN
F 1 "GNDA" H 1450 6650 50  0000 C CNN
F 2 "" H 1450 6800 50  0001 C CNN
F 3 "" H 1450 6800 50  0001 C CNN
	1    1450 6800
	0    1    1    0   
$EndComp
Wire Wire Line
	1400 6000 1800 6000
Wire Wire Line
	1450 6800 1800 6800
Wire Wire Line
	2950 6500 3250 6500
Wire Wire Line
	3250 6500 3250 5700
Wire Wire Line
	2950 5700 5200 5700
Wire Wire Line
	2950 6000 3150 6000
Wire Wire Line
	3150 6000 3150 6800
Wire Wire Line
	3150 6800 2950 6800
Text Notes 1450 7150 0    60   ~ 0
*Keep wires jumping UBECS long and equal in length to avoid eddy current (i.e. explosions)
Connection ~ 3250 5700
Text Notes 700  7750 1    60   ~ 0
*Note: Powering the RPI directly through the 5V rails is dangerous. These pins are not protected from overcurrent or over/under voltage.
Text Notes 800  7550 1    60   ~ 0
 If a wire is shorted during device operation, the whole system will blow.
Wire Wire Line
	3150 6100 4850 6100
Connection ~ 3150 6100
Text Notes 6150 3750 2    60   ~ 0
*Connect V+ via terminal block
Wire Wire Line
	7850 800  7450 800 
Wire Wire Line
	7450 800  7450 850 
Wire Wire Line
	7450 850  7250 850 
Wire Wire Line
	7450 900  7850 900 
Wire Wire Line
	7450 900  7450 950 
Wire Wire Line
	7450 950  7250 950 
Wire Wire Line
	7850 1000 7500 1000
Wire Wire Line
	7500 1000 7500 1050
Wire Wire Line
	7500 1050 7250 1050
Wire Wire Line
	7850 1300 7700 1300
Wire Wire Line
	7700 1300 7700 1200
Wire Wire Line
	7700 1200 7250 1200
Wire Wire Line
	7850 1400 7650 1400
Wire Wire Line
	7650 1400 7650 1300
Wire Wire Line
	7650 1300 7250 1300
Wire Wire Line
	7850 1500 7600 1500
Wire Wire Line
	7600 1500 7600 1400
Wire Wire Line
	7600 1400 7250 1400
Wire Wire Line
	7850 1800 7850 1600
Wire Wire Line
	7850 1600 7550 1600
Wire Wire Line
	7550 1600 7550 1550
Wire Wire Line
	7550 1550 7250 1550
Wire Wire Line
	7850 1900 7800 1900
Wire Wire Line
	7800 1900 7800 1700
Wire Wire Line
	7800 1700 7500 1700
Wire Wire Line
	7500 1700 7500 1650
Wire Wire Line
	7500 1650 7250 1650
Wire Wire Line
	7850 2000 7700 2000
Wire Wire Line
	7700 2000 7700 1750
Wire Wire Line
	7700 1750 7250 1750
Wire Wire Line
	7650 2300 7850 2300
Wire Wire Line
	7650 2300 7650 1900
Wire Wire Line
	7650 1900 7250 1900
Wire Wire Line
	7850 2400 7600 2400
Wire Wire Line
	7600 2400 7600 2000
Wire Wire Line
	7600 2000 7250 2000
Wire Wire Line
	7850 2500 7550 2500
Wire Wire Line
	7550 2500 7550 2100
Wire Wire Line
	7550 2100 7250 2100
Wire Wire Line
	7850 2800 7750 2800
Wire Wire Line
	7750 2800 7750 2600
Wire Wire Line
	7750 2600 7250 2600
Wire Wire Line
	7850 2900 7700 2900
Wire Wire Line
	7700 2900 7700 2700
Wire Wire Line
	7700 2700 7250 2700
Wire Wire Line
	7850 3000 7600 3000
Wire Wire Line
	7600 3000 7600 2800
Wire Wire Line
	7600 2800 7250 2800
Wire Wire Line
	7850 3300 7700 3300
Wire Wire Line
	7700 3300 7700 3100
Wire Wire Line
	7700 3100 7550 3100
Wire Wire Line
	7550 3100 7550 2950
Wire Wire Line
	7550 2950 7250 2950
Wire Wire Line
	7850 3400 7650 3400
Wire Wire Line
	7650 3400 7650 3150
Wire Wire Line
	7650 3150 7500 3150
Wire Wire Line
	7500 3150 7500 3050
Wire Wire Line
	7500 3050 7250 3050
Wire Wire Line
	7600 3500 7850 3500
Wire Wire Line
	7600 3200 7600 3500
Wire Wire Line
	7600 3200 7450 3200
Wire Wire Line
	7450 3200 7450 3150
Wire Wire Line
	7450 3150 7250 3150
$Comp
L USB_OTG J?
U 1 1 591B9E29
P 5500 5900
F 0 "J?" H 5300 6350 50  0000 L CNN
F 1 "USB_OTG" H 5300 6250 50  0000 L CNN
F 2 "" H 5650 5850 50  0001 C CNN
F 3 "" H 5650 5850 50  0001 C CNN
	1    5500 5900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5500 6300 5500 6400
Wire Wire Line
	5500 6400 4850 6400
Wire Wire Line
	4850 6400 4850 6100
NoConn ~ 5200 6100
NoConn ~ 5200 6000
NoConn ~ 5200 5900
NoConn ~ 5600 6300
Wire Wire Line
	6000 3650 4550 3650
Wire Wire Line
	4550 3650 4550 5700
Connection ~ 4550 5700
Wire Wire Line
	2100 1900 1900 1900
Wire Wire Line
	1900 1900 1900 1300
Wire Wire Line
	1900 1300 5500 1300
Wire Wire Line
	5500 1300 5500 3550
Wire Wire Line
	5500 3550 6000 3550
Wire Wire Line
	3600 2100 5050 2100
Wire Wire Line
	5050 2100 5050 3150
Wire Wire Line
	5050 3150 6000 3150
Wire Wire Line
	2100 2000 1800 2000
Wire Wire Line
	1800 2000 1800 1500
Wire Wire Line
	1800 1500 5600 1500
Wire Wire Line
	5600 1500 5600 3450
Wire Wire Line
	5600 3450 6000 3450
Wire Wire Line
	2100 2100 1750 2100
Wire Wire Line
	1750 2100 1750 1450
Wire Wire Line
	1750 1450 5650 1450
Wire Wire Line
	5650 1450 5650 3350
Wire Wire Line
	5650 3350 6000 3350
Wire Wire Line
	3600 2600 4900 2600
Wire Wire Line
	4900 2600 4900 3250
Wire Wire Line
	4900 3250 6000 3250
Text Notes 4750 3900 0    60   ~ 0
Ground should be connected to power terminal in addition to control.\n
$EndSCHEMATC
