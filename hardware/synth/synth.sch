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
Text Label 10400 2650 0    50   ~ 0
LED
Wire Wire Line
	10050 2950 10650 2950
Text Label 8300 2750 0    50   ~ 0
MIDI_OUT
Wire Wire Line
	8850 2750 8200 2750
Text Label 8300 2850 0    50   ~ 0
MIDI_IN
Wire Wire Line
	8850 2850 8200 2850
Text Notes 2650 7150 0    50   ~ 0
Volume
Text Notes 6500 4800 0    50   ~ 0
Wet / Dry
Text Notes 5300 3550 0    50   ~ 0
Filter
Text Notes 3450 4000 0    50   ~ 0
Gain
Text Notes 1700 6300 0    50   ~ 0
Output Stage
Text Notes 6100 3400 0    50   ~ 0
Wet/Dry Mix Stage
Text Notes 4000 3150 0    50   ~ 0
Filter Stage
Text Notes 2700 3150 0    50   ~ 0
Input Stage
Wire Notes Line
	1700 7650 1700 6300
Wire Notes Line
	4200 7650 1700 7650
Wire Notes Line
	4200 6300 4200 7650
Wire Notes Line
	1700 6300 4200 6300
Wire Notes Line
	6100 3400 6100 5800
Wire Notes Line
	7350 3400 6100 3400
Wire Notes Line
	7350 5800 7350 3400
Wire Notes Line
	6100 5800 7350 5800
Connection ~ 5700 3800
Wire Wire Line
	6200 5700 6200 5500
Wire Notes Line
	5950 3150 4000 3150
Wire Notes Line
	5950 4450 5950 3150
Wire Notes Line
	4000 4450 5950 4450
Wire Notes Line
	4000 3150 4000 4450
Wire Notes Line
	3850 3150 2700 3150
Wire Notes Line
	3850 5100 3850 3150
Wire Notes Line
	2700 5100 3850 5100
Wire Notes Line
	2700 3150 2700 5100
Wire Wire Line
	4900 7050 5200 7050
Connection ~ 5200 7050
Wire Wire Line
	5200 7050 5200 7200
Wire Wire Line
	5200 6900 5200 7050
Wire Wire Line
	5600 7200 5500 7200
Wire Wire Line
	5600 7100 5600 7200
Wire Wire Line
	5700 7100 5600 7100
Wire Wire Line
	5600 6900 5500 6900
Wire Wire Line
	5600 7000 5600 6900
Wire Wire Line
	5700 7000 5600 7000
$Comp
L Device:R R14
U 1 1 60FCCADC
P 5350 7200
F 0 "R14" V 5250 7050 50  0000 C CNN
F 1 "0R" V 5234 7200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 5280 7200 50  0001 C CNN
F 3 "~" H 5350 7200 50  0001 C CNN
	1    5350 7200
	0    -1   1    0   
$EndComp
$Comp
L Device:R R12
U 1 1 60FCCAD2
P 5350 6900
F 0 "R12" V 5250 6750 50  0000 C CNN
F 1 "0R" V 5234 6900 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 5280 6900 50  0001 C CNN
F 3 "~" H 5350 6900 50  0001 C CNN
	1    5350 6900
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR040
U 1 1 60FCCAC8
P 5700 7200
F 0 "#PWR040" H 5700 6950 50  0001 C CNN
F 1 "GND" H 5705 7027 50  0000 C CNN
F 2 "" H 5700 7200 50  0001 C CNN
F 3 "" H 5700 7200 50  0001 C CNN
	1    5700 7200
	-1   0    0    -1  
$EndComp
$Comp
L Connector:AudioJack3 J5
U 1 1 60FCCA9A
P 5900 7100
F 0 "J5" H 5800 6700 50  0000 R CNN
F 1 "Audio Out" H 6000 6800 50  0000 R CNN
F 2 "synth:Tayda_3.5mm_stereo_TRS_jack_A-853" H 5900 7100 50  0001 C CNN
F 3 "~" H 5900 7100 50  0001 C CNN
	1    5900 7100
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR042
U 1 1 60FBEBE8
P 4900 7350
F 0 "#PWR042" H 4900 7100 50  0001 C CNN
F 1 "GND" H 4905 7177 50  0000 C CNN
F 2 "" H 4900 7350 50  0001 C CNN
F 3 "" H 4900 7350 50  0001 C CNN
	1    4900 7350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 7050 4800 7050
Connection ~ 4900 7050
$Comp
L power:VDD #PWR038
U 1 1 60FB7244
P 1300 6600
F 0 "#PWR038" H 1300 6450 50  0001 C CNN
F 1 "VDD" H 1315 6773 50  0000 C CNN
F 2 "" H 1300 6600 50  0001 C CNN
F 3 "" H 1300 6600 50  0001 C CNN
	1    1300 6600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR039
U 1 1 60FB6D96
P 1300 7200
F 0 "#PWR039" H 1300 6950 50  0001 C CNN
F 1 "GND" H 1305 7027 50  0000 C CNN
F 2 "" H 1300 7200 50  0001 C CNN
F 3 "" H 1300 7200 50  0001 C CNN
	1    1300 7200
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:MCP602 U4
U 3 1 60FB5A71
P 1400 6900
F 0 "U4" H 1358 6946 50  0000 L CNN
F 1 "MCP6042" V 1100 6700 50  0000 L CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 1400 6900 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 1400 6900 50  0001 C CNN
	3    1400 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 6000 1600 6700
Wire Wire Line
	7500 6000 1600 6000
Wire Wire Line
	7500 4750 7500 6000
Wire Wire Line
	7150 4750 7500 4750
Wire Wire Line
	4300 7050 4500 7050
Connection ~ 4300 7050
Connection ~ 4100 7050
Wire Wire Line
	4100 7050 4300 7050
$Comp
L Device:R R13
U 1 1 60F3EFC1
P 4900 7200
F 0 "R13" H 4970 7246 50  0000 L CNN
F 1 "1M" H 4970 7155 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 4830 7200 50  0001 C CNN
F 3 "~" H 4900 7200 50  0001 C CNN
	1    4900 7200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C10
U 1 1 60F3E826
P 4650 7050
F 0 "C10" V 4398 7050 50  0000 C CNN
F 1 "1u" V 4489 7050 50  0000 C CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4688 6900 50  0001 C CNN
F 3 "~" H 4650 7050 50  0001 C CNN
	1    4650 7050
	0    1    1    0   
$EndComp
Connection ~ 1550 3600
Wire Wire Line
	1550 3600 1550 3750
Wire Wire Line
	1550 3600 1650 3600
Wire Wire Line
	1550 3450 1550 3600
Wire Wire Line
	2050 3600 1950 3600
$Comp
L Device:C C6
U 1 1 60F33E0C
P 1800 3600
F 0 "C6" V 1548 3600 50  0000 C CNN
F 1 "1u" V 1639 3600 50  0000 C CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1838 3450 50  0001 C CNN
F 3 "~" H 1800 3600 50  0001 C CNN
	1    1800 3600
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR041
U 1 1 60F302BB
P 4300 7350
F 0 "#PWR041" H 4300 7100 50  0001 C CNN
F 1 "GND" H 4305 7177 50  0000 C CNN
F 2 "" H 4300 7350 50  0001 C CNN
F 3 "" H 4300 7350 50  0001 C CNN
	1    4300 7350
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Zener D8
U 1 1 60F2FC51
P 4300 7200
F 0 "D8" V 4254 7280 50  0000 L CNN
F 1 "TVS" V 4345 7280 50  0000 L CNN
F 2 "synth:SOD-323-handsoldering" H 4300 7200 50  0001 C CNN
F 3 "~" H 4300 7200 50  0001 C CNN
	1    4300 7200
	0    1    1    0   
$EndComp
Wire Wire Line
	3500 6950 3250 6950
Wire Wire Line
	4100 7350 4100 7050
Wire Wire Line
	3500 7350 4100 7350
Wire Wire Line
	3500 7150 3500 7350
Connection ~ 2550 6800
Wire Wire Line
	2550 6800 3100 6800
$Comp
L Device:C C11
U 1 1 60F27C75
P 3100 7250
F 0 "C11" H 3250 7350 50  0000 L CNN
F 1 "1u" H 3250 7250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3138 7100 50  0001 C CNN
F 3 "~" H 3100 7250 50  0001 C CNN
	1    3100 7250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR043
U 1 1 60F27407
P 3100 7400
F 0 "#PWR043" H 3100 7150 50  0001 C CNN
F 1 "GND" H 3105 7227 50  0000 C CNN
F 2 "" H 3100 7400 50  0001 C CNN
F 3 "" H 3100 7400 50  0001 C CNN
	1    3100 7400
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT RV5
U 1 1 60F268CF
P 3100 6950
F 0 "RV5" H 2850 6900 50  0000 R CNN
F 1 "50K LOG" H 2950 7000 50  0000 R CNN
F 2 "synth:Alpha_16mm_Potentiometer_RA" H 3100 6950 50  0001 C CNN
F 3 "~" H 3100 6950 50  0001 C CNN
	1    3100 6950
	1    0    0    1   
$EndComp
Wire Wire Line
	2550 7100 2550 6800
Wire Wire Line
	1950 7100 2550 7100
Wire Wire Line
	1950 6900 1950 7100
Wire Wire Line
	1950 6700 1600 6700
$Comp
L Amplifier_Operational:MCP602 U4
U 2 1 60F1D551
P 3800 7050
F 0 "U4" H 3800 7417 50  0000 C CNN
F 1 "MCP6042" H 3800 7326 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 3800 7050 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 3800 7050 50  0001 C CNN
	2    3800 7050
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:MCP602 U4
U 1 1 60F1B1DA
P 2250 6800
F 0 "U4" H 2250 7167 50  0000 C CNN
F 1 "MCP6042" H 2250 7076 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 2250 6800 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 2250 6800 50  0001 C CNN
	1    2250 6800
	1    0    0    -1  
$EndComp
Connection ~ 6800 5400
Wire Wire Line
	7000 5400 7000 4900
Wire Wire Line
	6800 5400 7000 5400
Connection ~ 6800 3900
Wire Wire Line
	7000 3900 6800 3900
Wire Wire Line
	7000 4600 7000 3900
$Comp
L Device:R_POT RV4
U 1 1 60F15BA6
P 7000 4750
F 0 "RV4" H 6700 4550 50  0000 R CNN
F 1 "50K LIN" H 6850 4650 50  0000 R CNN
F 2 "synth:Alpha_16mm_Potentiometer_RA" H 7000 4750 50  0001 C CNN
F 3 "~" H 7000 4750 50  0001 C CNN
	1    7000 4750
	1    0    0    1   
$EndComp
Wire Wire Line
	2600 3600 2750 3600
Connection ~ 2600 3600
Wire Wire Line
	2600 5300 6200 5300
Wire Wire Line
	2600 3600 2600 5300
Wire Wire Line
	2350 3600 2600 3600
$Comp
L power:VDD #PWR034
U 1 1 60F0C8E8
P 2100 4600
F 0 "#PWR034" H 2100 4450 50  0001 C CNN
F 1 "VDD" H 2115 4773 50  0000 C CNN
F 2 "" H 2100 4600 50  0001 C CNN
F 3 "" H 2100 4600 50  0001 C CNN
	1    2100 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR036
U 1 1 60F0C56D
P 2100 5200
F 0 "#PWR036" H 2100 4950 50  0001 C CNN
F 1 "GND" H 2105 5027 50  0000 C CNN
F 2 "" H 2100 5200 50  0001 C CNN
F 3 "" H 2100 5200 50  0001 C CNN
	1    2100 5200
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:MCP604 U3
U 5 1 60F097D6
P 2200 4900
F 0 "U3" H 2250 4900 50  0000 C CNN
F 1 "MCP6044" V 1950 4900 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 2150 5000 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 2250 5100 50  0001 C CNN
	5    2200 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 5700 6800 5400
Wire Wire Line
	6200 5700 6800 5700
$Comp
L Amplifier_Operational:MCP604 U3
U 4 1 60F0663F
P 6500 5400
F 0 "U3" H 6500 5750 50  0000 C CNN
F 1 "MCP6044" H 6500 5650 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 6450 5500 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 6550 5600 50  0001 C CNN
	4    6500 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 3800 6200 3800
Wire Wire Line
	6800 4200 6800 3900
Wire Wire Line
	6200 4200 6800 4200
Wire Wire Line
	6200 4000 6200 4200
$Comp
L Amplifier_Operational:MCP604 U3
U 3 1 60F0046B
P 6500 3900
F 0 "U3" H 6500 4267 50  0000 C CNN
F 1 "MCP6044" H 6500 4176 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 6450 4000 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 6550 4100 50  0001 C CNN
	3    6500 3900
	1    0    0    -1  
$EndComp
Connection ~ 5500 3800
Wire Wire Line
	5500 3800 5700 3800
$Comp
L Device:C C7
U 1 1 60EF8A8F
P 5700 3950
F 0 "C7" H 5750 3850 50  0000 L CNN
F 1 "10n" H 5750 3750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 5738 3800 50  0001 C CNN
F 3 "~" H 5700 3950 50  0001 C CNN
	1    5700 3950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR032
U 1 1 60EFA5DB
P 5700 4100
F 0 "#PWR032" H 5700 3850 50  0001 C CNN
F 1 "GND" H 5705 3927 50  0000 C CNN
F 2 "" H 5700 4100 50  0001 C CNN
F 3 "" H 5700 4100 50  0001 C CNN
	1    5700 4100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT RV2
U 1 1 60EF6AA1
P 5350 3800
F 0 "RV2" V 5150 3850 50  0000 R CNN
F 1 "50K LOG" V 5250 3850 50  0000 R CNN
F 2 "synth:Alpha_16mm_Potentiometer_RA" H 5350 3800 50  0001 C CNN
F 3 "~" H 5350 3800 50  0001 C CNN
	1    5350 3800
	0    -1   1    0   
$EndComp
Wire Wire Line
	5500 3800 5500 3950
Wire Wire Line
	5500 3950 5350 3950
Connection ~ 5050 3800
Wire Wire Line
	5200 3800 5050 3800
Wire Wire Line
	5050 4100 5050 3800
Wire Wire Line
	4450 4100 5050 4100
Wire Wire Line
	4450 3900 4450 4100
$Comp
L Amplifier_Operational:MCP604 U3
U 2 1 60EF453A
P 4750 3800
F 0 "U3" H 4750 4167 50  0000 C CNN
F 1 "MCP6044" H 4750 4076 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 4700 3900 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 4800 4000 50  0001 C CNN
	2    4750 3800
	1    0    0    -1  
$EndComp
Text Label 4200 3700 0    50   ~ 0
DAC
Wire Wire Line
	4150 3700 4450 3700
Text Label 10400 2750 0    50   ~ 0
KNOB
Text Label 10400 3150 0    50   ~ 0
DAC
Wire Wire Line
	10050 3150 10650 3150
Text Label 10400 2950 0    50   ~ 0
ADC
Wire Wire Line
	10650 2750 10050 2750
Text Label 3500 3700 0    50   ~ 0
ADC
Wire Wire Line
	3100 4550 3100 4400
Wire Wire Line
	3350 4550 3100 4550
Wire Wire Line
	3100 4100 2750 4100
Connection ~ 3100 4100
$Comp
L Device:C C8
U 1 1 60EE1BE1
P 3100 4250
F 0 "C8" H 2850 4300 50  0000 L CNN
F 1 "DNL" H 2800 4200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3138 4100 50  0001 C CNN
F 3 "~" H 3100 4250 50  0001 C CNN
	1    3100 4250
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT RV3
U 1 1 60ED5AEB
P 3350 4100
F 0 "RV3" H 3280 4146 50  0000 R CNN
F 1 "50K LIN" H 3280 4055 50  0000 R CNN
F 2 "synth:Alpha_16mm_Potentiometer_RA" H 3350 4100 50  0001 C CNN
F 3 "~" H 3350 4100 50  0001 C CNN
	1    3350 4100
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R11
U 1 1 60ED625A
P 3350 4400
F 0 "R11" H 3420 4446 50  0000 L CNN
F 1 "100R" H 3420 4355 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 3280 4400 50  0001 C CNN
F 3 "~" H 3350 4400 50  0001 C CNN
	1    3350 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3700 3350 3700
Connection ~ 3350 3700
Connection ~ 3350 4550
$Comp
L Device:C C9
U 1 1 60ED6DFA
P 3350 4700
F 0 "C9" H 3465 4746 50  0000 L CNN
F 1 "1u" H 3465 4655 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3388 4550 50  0001 C CNN
F 3 "~" H 3350 4700 50  0001 C CNN
	1    3350 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR035
U 1 1 60ED712F
P 3350 4850
F 0 "#PWR035" H 3350 4600 50  0001 C CNN
F 1 "GND" H 3355 4677 50  0000 C CNN
F 2 "" H 3350 4850 50  0001 C CNN
F 3 "" H 3350 4850 50  0001 C CNN
	1    3350 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 3700 3350 3950
Wire Wire Line
	3200 4100 3100 4100
Wire Wire Line
	2750 4100 2750 3800
Connection ~ 2350 3600
Wire Wire Line
	2050 3600 2350 3600
$Comp
L power:GND #PWR031
U 1 1 60ED476B
P 2350 3900
F 0 "#PWR031" H 2350 3650 50  0001 C CNN
F 1 "GND" H 2355 3727 50  0000 C CNN
F 2 "" H 2350 3900 50  0001 C CNN
F 3 "" H 2350 3900 50  0001 C CNN
	1    2350 3900
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Zener D7
U 1 1 60ED4145
P 2350 3750
F 0 "D7" V 2304 3830 50  0000 L CNN
F 1 "TVS" V 2395 3830 50  0000 L CNN
F 2 "synth:SOD-323-handsoldering" H 2350 3750 50  0001 C CNN
F 3 "~" H 2350 3750 50  0001 C CNN
	1    2350 3750
	0    1    1    0   
$EndComp
$Comp
L power:VDD #PWR027
U 1 1 60ED25D6
P 2050 3300
F 0 "#PWR027" H 2050 3150 50  0001 C CNN
F 1 "VDD" H 2065 3473 50  0000 C CNN
F 2 "" H 2050 3300 50  0001 C CNN
F 3 "" H 2050 3300 50  0001 C CNN
	1    2050 3300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR030
U 1 1 60ED1EA3
P 2050 3900
F 0 "#PWR030" H 2050 3650 50  0001 C CNN
F 1 "GND" H 2055 3727 50  0000 C CNN
F 2 "" H 2050 3900 50  0001 C CNN
F 3 "" H 2050 3900 50  0001 C CNN
	1    2050 3900
	1    0    0    -1  
$EndComp
Connection ~ 2050 3600
$Comp
L Device:R R10
U 1 1 60ED1AC3
P 2050 3750
F 0 "R10" H 2120 3796 50  0000 L CNN
F 1 "1M" H 2120 3705 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1980 3750 50  0001 C CNN
F 3 "~" H 2050 3750 50  0001 C CNN
	1    2050 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 60ED1A2C
P 2050 3450
F 0 "R8" H 2120 3496 50  0000 L CNN
F 1 "1M" H 2120 3405 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1980 3450 50  0001 C CNN
F 3 "~" H 2050 3450 50  0001 C CNN
	1    2050 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 3750 1250 3750
Wire Wire Line
	1150 3650 1150 3750
Wire Wire Line
	1050 3650 1150 3650
Wire Wire Line
	1150 3450 1250 3450
Wire Wire Line
	1150 3550 1150 3450
Wire Wire Line
	1050 3550 1150 3550
$Comp
L Device:R R9
U 1 1 60ECE8FD
P 1400 3750
F 0 "R9" V 1300 3600 50  0000 C CNN
F 1 "10K" V 1284 3750 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1330 3750 50  0001 C CNN
F 3 "~" H 1400 3750 50  0001 C CNN
	1    1400 3750
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 60ECE70E
P 1400 3450
F 0 "R7" V 1300 3300 50  0000 C CNN
F 1 "10K" V 1284 3450 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1330 3450 50  0001 C CNN
F 3 "~" H 1400 3450 50  0001 C CNN
	1    1400 3450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR029
U 1 1 60ECE3CF
P 1050 3750
F 0 "#PWR029" H 1050 3500 50  0001 C CNN
F 1 "GND" H 1055 3577 50  0000 C CNN
F 2 "" H 1050 3750 50  0001 C CNN
F 3 "" H 1050 3750 50  0001 C CNN
	1    1050 3750
	1    0    0    -1  
$EndComp
$Comp
L Connector:AudioJack3 J3
U 1 1 60ECD593
P 850 3650
F 0 "J3" H 750 3250 50  0000 R CNN
F 1 "Audio In" H 950 3350 50  0000 R CNN
F 2 "synth:Tayda_3.5mm_stereo_TRS_jack_A-853" H 850 3650 50  0001 C CNN
F 3 "~" H 850 3650 50  0001 C CNN
	1    850  3650
	1    0    0    1   
$EndComp
$Comp
L Amplifier_Operational:MCP604 U3
U 1 1 60EC8525
P 3050 3700
F 0 "U3" H 3050 4067 50  0000 C CNN
F 1 "MCP6044" H 3050 3976 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 3000 3800 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21314g.pdf" H 3100 3900 50  0001 C CNN
	1    3050 3700
	1    0    0    -1  
$EndComp
Text Label 10400 2550 0    50   ~ 0
UPDI
Wire Wire Line
	10050 2550 10650 2550
Text Label 10200 4700 0    50   ~ 0
UPDI
Wire Wire Line
	9950 4700 10400 4700
$Comp
L power:VDD #PWR033
U 1 1 60EC3CCB
P 9450 4400
F 0 "#PWR033" H 9450 4250 50  0001 C CNN
F 1 "VDD" H 9465 4573 50  0000 C CNN
F 2 "" H 9450 4400 50  0001 C CNN
F 3 "" H 9450 4400 50  0001 C CNN
	1    9450 4400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR037
U 1 1 60EC3761
P 9450 5200
F 0 "#PWR037" H 9450 4950 50  0001 C CNN
F 1 "GND" H 9455 5027 50  0000 C CNN
F 2 "" H 9450 5200 50  0001 C CNN
F 3 "" H 9450 5200 50  0001 C CNN
	1    9450 5200
	1    0    0    -1  
$EndComp
$Comp
L Connector:AVR-UPDI-6 J4
U 1 1 60EC2858
P 9550 4800
F 0 "J4" V 9100 4850 50  0000 R CNN
F 1 "AVR-UPDI-6" V 9200 5050 50  0000 R CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" V 9300 4750 50  0001 C CNN
F 3 "https://www.microchip.com/webdoc/GUID-9D10622A-5C16-4405-B092-1BDD437B4976/index.html?GUID-9B349315-2842-4189-B88C-49F4E1055D7F" H 8275 4250 50  0001 C CNN
	1    9550 4800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR028
U 1 1 60EBF3B1
P 9450 3650
F 0 "#PWR028" H 9450 3400 50  0001 C CNN
F 1 "GND" H 9455 3477 50  0000 C CNN
F 2 "" H 9450 3650 50  0001 C CNN
F 3 "" H 9450 3650 50  0001 C CNN
	1    9450 3650
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR023
U 1 1 60EBEF0A
P 9450 2250
F 0 "#PWR023" H 9450 2100 50  0001 C CNN
F 1 "VDD" H 9465 2423 50  0000 C CNN
F 2 "" H 9450 2250 50  0001 C CNN
F 3 "" H 9450 2250 50  0001 C CNN
	1    9450 2250
	1    0    0    -1  
$EndComp
$Comp
L MCU_Microchip_ATtiny:ATtiny814-SS U2
U 1 1 60EBD5FD
P 9450 2950
F 0 "U2" H 9000 3600 50  0000 C CNN
F 1 "ATtiny814-SS" H 9850 2300 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 9450 2950 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/40001912A.pdf" H 9450 2950 50  0001 C CNN
	1    9450 2950
	1    0    0    -1  
$EndComp
NoConn ~ 1550 2100
$Comp
L power:VDD #PWR021
U 1 1 60EBA9B3
P 1550 1900
F 0 "#PWR021" H 1550 1750 50  0001 C CNN
F 1 "VDD" V 1565 2028 50  0000 L CNN
F 2 "" H 1550 1900 50  0001 C CNN
F 3 "" H 1550 1900 50  0001 C CNN
	1    1550 1900
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_SPDT SW1
U 1 1 610291C4
P 1350 2000
F 0 "SW1" H 1450 2350 50  0000 C CNN
F 1 "POWER" H 1450 2250 50  0000 C CNN
F 2 "Button_Switch_THT:SW_CuK_OS102011MA1QN1_SPDT_Angled" H 1350 2000 50  0001 C CNN
F 3 "~" H 1350 2000 50  0001 C CNN
	1    1350 2000
	1    0    0    1   
$EndComp
Text Label 2400 2300 0    50   ~ 0
LED
Wire Wire Line
	2850 2300 2300 2300
$Comp
L Device:R R6
U 1 1 611520B7
P 3000 2300
F 0 "R6" V 3200 2250 50  0000 L CNN
F 1 "220R" V 3100 2200 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 2930 2300 50  0001 C CNN
F 3 "~" H 3000 2300 50  0001 C CNN
	1    3000 2300
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D6
U 1 1 611520A3
P 3300 2300
F 0 "D6" H 3300 2050 50  0000 R CNN
F 1 "LED" H 3400 2150 50  0000 R CNN
F 2 "LEDs:LED-3MM" H 3300 2300 50  0001 C CNN
F 3 "~" H 3300 2300 50  0001 C CNN
	1    3300 2300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR024
U 1 1 61151723
P 3450 2300
F 0 "#PWR024" H 3450 2050 50  0001 C CNN
F 1 "GND" H 3455 2127 50  0000 C CNN
F 2 "" H 3450 2300 50  0001 C CNN
F 3 "" H 3450 2300 50  0001 C CNN
	1    3450 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 1200 5050 1200
Connection ~ 4800 1200
Wire Wire Line
	4400 1200 4800 1200
Connection ~ 4800 900 
Wire Wire Line
	4800 900  5050 900 
Text Label 6700 1200 0    50   ~ 0
MIDI_OUT
Wire Wire Line
	7250 1200 6600 1200
Connection ~ 7650 1200
Wire Wire Line
	7650 1200 7550 1200
Wire Wire Line
	7650 1200 7650 1450
Wire Wire Line
	8350 1200 7650 1200
Wire Wire Line
	8000 1100 8000 1300
$Comp
L Device:R R5
U 1 1 61123036
P 7400 1200
F 0 "R5" V 7193 1200 50  0000 C CNN
F 1 "10R" V 7284 1200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 7330 1200 50  0001 C CNN
F 3 "~" H 7400 1200 50  0001 C CNN
	1    7400 1200
	0    1    1    0   
$EndComp
Connection ~ 8000 1300
$Comp
L Device:R R3
U 1 1 61122B39
P 8000 950
F 0 "R3" H 8070 996 50  0000 L CNN
F 1 "10R" H 8070 905 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 7930 950 50  0001 C CNN
F 3 "~" H 8000 950 50  0001 C CNN
	1    8000 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 1300 8000 1450
Wire Wire Line
	8350 1300 8000 1300
$Comp
L power:VDD #PWR07
U 1 1 61116E97
P 8000 800
F 0 "#PWR07" H 8000 650 50  0001 C CNN
F 1 "VDD" H 8015 973 50  0000 C CNN
F 2 "" H 8000 800 50  0001 C CNN
F 3 "" H 8000 800 50  0001 C CNN
	1    8000 800 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR019
U 1 1 6110FD9A
P 7650 1750
F 0 "#PWR019" H 7650 1500 50  0001 C CNN
F 1 "GND" H 7655 1577 50  0000 C CNN
F 2 "" H 7650 1750 50  0001 C CNN
F 3 "" H 7650 1750 50  0001 C CNN
	1    7650 1750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 6110FA0A
P 8000 1750
F 0 "#PWR020" H 8000 1500 50  0001 C CNN
F 1 "GND" H 8005 1577 50  0000 C CNN
F 2 "" H 8000 1750 50  0001 C CNN
F 3 "" H 8000 1750 50  0001 C CNN
	1    8000 1750
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Zener D5
U 1 1 61108189
P 8000 1600
F 0 "D5" V 7954 1680 50  0000 L CNN
F 1 "TVS" V 8045 1680 50  0000 L CNN
F 2 "synth:SOD-323-handsoldering" H 8000 1600 50  0001 C CNN
F 3 "~" H 8000 1600 50  0001 C CNN
	1    8000 1600
	0    1    1    0   
$EndComp
$Comp
L Device:D_Zener D4
U 1 1 6110967C
P 7650 1600
F 0 "D4" V 7604 1680 50  0000 L CNN
F 1 "TVS" V 7695 1680 50  0000 L CNN
F 2 "synth:SOD-323-handsoldering" H 7650 1600 50  0001 C CNN
F 3 "~" H 7650 1600 50  0001 C CNN
	1    7650 1600
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR016
U 1 1 6110732B
P 8350 1400
F 0 "#PWR016" H 8350 1150 50  0001 C CNN
F 1 "GND" H 8355 1227 50  0000 C CNN
F 2 "" H 8350 1400 50  0001 C CNN
F 3 "" H 8350 1400 50  0001 C CNN
	1    8350 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 610FFBFF
P 4200 1550
F 0 "#PWR018" H 4200 1300 50  0001 C CNN
F 1 "GND" H 4205 1377 50  0000 C CNN
F 2 "" H 4200 1550 50  0001 C CNN
F 3 "" H 4200 1550 50  0001 C CNN
	1    4200 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 610FF8BF
P 4200 1400
F 0 "C5" H 4315 1446 50  0000 L CNN
F 1 "DNL" H 4315 1355 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4238 1250 50  0001 C CNN
F 3 "~" H 4200 1400 50  0001 C CNN
	1    4200 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 900  4200 1250
Wire Wire Line
	4100 900  4200 900 
$Comp
L Connector:AudioJack3 J2
U 1 1 610F383F
P 8550 1300
F 0 "J2" H 8450 900 50  0000 R CNN
F 1 "MIDI Out" H 8650 1000 50  0000 R CNN
F 2 "synth:Tayda_3.5mm_stereo_TRS_jack_A-853" H 8550 1300 50  0001 C CNN
F 3 "~" H 8550 1300 50  0001 C CNN
	1    8550 1300
	-1   0    0    1   
$EndComp
Wire Wire Line
	5050 900  5050 950 
Wire Wire Line
	4400 900  4500 900 
Text Label 5950 1050 0    50   ~ 0
MIDI_IN
Wire Wire Line
	5650 1050 6300 1050
Text Notes 3700 1550 0    50   ~ 0
Type A
Wire Wire Line
	4400 1000 4400 900 
Wire Wire Line
	4100 1000 4400 1000
Wire Wire Line
	4400 1100 4100 1100
Wire Wire Line
	4400 1200 4400 1100
$Comp
L power:VDD #PWR01
U 1 1 610598B5
P 5350 750
F 0 "#PWR01" H 5350 600 50  0001 C CNN
F 1 "VDD" H 5365 923 50  0000 C CNN
F 2 "" H 5350 750 50  0001 C CNN
F 3 "" H 5350 750 50  0001 C CNN
	1    5350 750 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 61059CFA
P 5350 1350
F 0 "#PWR014" H 5350 1100 50  0001 C CNN
F 1 "GND" H 5355 1177 50  0000 C CNN
F 2 "" H 5350 1350 50  0001 C CNN
F 3 "" H 5350 1350 50  0001 C CNN
	1    5350 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Zener D2
U 1 1 6105A125
P 4800 1050
F 0 "D2" V 4754 1130 50  0000 L CNN
F 1 "TVS" V 4845 1130 50  0000 L CNN
F 2 "synth:SOD-323-handsoldering" H 4800 1050 50  0001 C CNN
F 3 "~" H 4800 1050 50  0001 C CNN
	1    4800 1050
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 6105B8EE
P 4650 900
F 0 "R1" V 4443 900 50  0000 C CNN
F 1 "220R" V 4534 900 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 4580 900 50  0001 C CNN
F 3 "~" H 4650 900 50  0001 C CNN
	1    4650 900 
	0    1    1    0   
$EndComp
$Comp
L Connector:AudioJack3 J1
U 1 1 61060A9B
P 3900 1000
F 0 "J1" H 3800 600 50  0000 R CNN
F 1 "MIDI In" H 4000 700 50  0000 R CNN
F 2 "synth:Tayda_3.5mm_stereo_TRS_jack_A-853" H 3900 1000 50  0001 C CNN
F 3 "~" H 3900 1000 50  0001 C CNN
	1    3900 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 61046A22
P 2950 950
F 0 "R2" H 3020 996 50  0000 L CNN
F 1 "220R" H 3020 905 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 2880 950 50  0001 C CNN
F 3 "~" H 2950 950 50  0001 C CNN
	1    2950 950 
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR06
U 1 1 610454E8
P 2950 800
F 0 "#PWR06" H 2950 650 50  0001 C CNN
F 1 "VDD" H 2965 973 50  0000 C CNN
F 2 "" H 2950 800 50  0001 C CNN
F 3 "" H 2950 800 50  0001 C CNN
	1    2950 800 
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 61044BE7
P 2950 1250
F 0 "D3" V 2989 1132 50  0000 R CNN
F 1 "POWER" V 2898 1132 50  0000 R CNN
F 2 "LEDs:LED-3MM" H 2950 1250 50  0001 C CNN
F 3 "~" H 2950 1250 50  0001 C CNN
	1    2950 1250
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR015
U 1 1 61043D98
P 2950 1400
F 0 "#PWR015" H 2950 1150 50  0001 C CNN
F 1 "GND" H 2955 1227 50  0000 C CNN
F 2 "" H 2950 1400 50  0001 C CNN
F 3 "" H 2950 1400 50  0001 C CNN
	1    2950 1400
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR02
U 1 1 61031070
P 1300 800
F 0 "#PWR02" H 1300 650 50  0001 C CNN
F 1 "VDD" H 1315 973 50  0000 C CNN
F 2 "" H 1300 800 50  0001 C CNN
F 3 "" H 1300 800 50  0001 C CNN
	1    1300 800 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 61030D2F
P 1300 1100
F 0 "#PWR09" H 1300 850 50  0001 C CNN
F 1 "GND" H 1305 927 50  0000 C CNN
F 2 "" H 1300 1100 50  0001 C CNN
F 3 "" H 1300 1100 50  0001 C CNN
	1    1300 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 2000 900  2000
$Comp
L Device:D_Zener D1
U 1 1 6101CFD1
P 1300 950
F 0 "D1" V 1254 1030 50  0000 L CNN
F 1 "TVS" V 1345 1030 50  0000 L CNN
F 2 "synth:SOD-323-handsoldering" H 1300 950 50  0001 C CNN
F 3 "~" H 1300 950 50  0001 C CNN
	1    1300 950 
	0    1    1    0   
$EndComp
Text Label 10300 1150 0    50   ~ 0
KNOB
Connection ~ 10050 1150
Wire Wire Line
	10050 1150 10550 1150
$Comp
L Device:R R4
U 1 1 61012E10
P 9900 1150
F 0 "R4" V 9693 1150 50  0000 C CNN
F 1 "100K" V 9784 1150 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 9830 1150 50  0001 C CNN
F 3 "~" H 9900 1150 50  0001 C CNN
	1    9900 1150
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR017
U 1 1 6100FAA9
P 10050 1450
F 0 "#PWR017" H 10050 1200 50  0001 C CNN
F 1 "GND" H 10055 1277 50  0000 C CNN
F 2 "" H 10050 1450 50  0001 C CNN
F 3 "" H 10050 1450 50  0001 C CNN
	1    10050 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 6100D65A
P 10050 1300
F 0 "C4" H 10165 1346 50  0000 L CNN
F 1 "100n" H 10165 1255 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10088 1150 50  0001 C CNN
F 3 "~" H 10050 1300 50  0001 C CNN
	1    10050 1300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 6100CADB
P 9600 1300
F 0 "#PWR013" H 9600 1050 50  0001 C CNN
F 1 "GND" H 9605 1127 50  0000 C CNN
F 2 "" H 9600 1300 50  0001 C CNN
F 3 "" H 9600 1300 50  0001 C CNN
	1    9600 1300
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR08
U 1 1 6100C78D
P 9600 1000
F 0 "#PWR08" H 9600 850 50  0001 C CNN
F 1 "VDD" H 9615 1173 50  0000 C CNN
F 2 "" H 9600 1000 50  0001 C CNN
F 3 "" H 9600 1000 50  0001 C CNN
	1    9600 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT RV1
U 1 1 6100BE24
P 9600 1150
F 0 "RV1" H 9300 950 50  0000 R CNN
F 1 "50K LIN" H 9450 1050 50  0000 R CNN
F 2 "synth:Alpha_16mm_Potentiometer_RA" H 9600 1150 50  0001 C CNN
F 3 "~" H 9600 1150 50  0001 C CNN
	1    9600 1150
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 60EC6D35
P 2100 1100
F 0 "#PWR011" H 2100 850 50  0001 C CNN
F 1 "GND" H 2105 927 50  0000 C CNN
F 2 "" H 2100 1100 50  0001 C CNN
F 3 "" H 2100 1100 50  0001 C CNN
	1    2100 1100
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR04
U 1 1 60EC6D2B
P 2100 800
F 0 "#PWR04" H 2100 650 50  0001 C CNN
F 1 "VDD" H 2115 973 50  0000 C CNN
F 2 "" H 2100 800 50  0001 C CNN
F 3 "" H 2100 800 50  0001 C CNN
	1    2100 800 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 60EC6BE9
P 2100 950
F 0 "C2" H 2215 996 50  0000 L CNN
F 1 "100n" H 2215 905 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 2138 800 50  0001 C CNN
F 3 "~" H 2100 950 50  0001 C CNN
	1    2100 950 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 60EBC85E
P 2550 1100
F 0 "#PWR012" H 2550 850 50  0001 C CNN
F 1 "GND" H 2555 927 50  0000 C CNN
F 2 "" H 2550 1100 50  0001 C CNN
F 3 "" H 2550 1100 50  0001 C CNN
	1    2550 1100
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR05
U 1 1 60EBC45E
P 2550 800
F 0 "#PWR05" H 2550 650 50  0001 C CNN
F 1 "VDD" H 2565 973 50  0000 C CNN
F 2 "" H 2550 800 50  0001 C CNN
F 3 "" H 2550 800 50  0001 C CNN
	1    2550 800 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 60EBC140
P 2550 950
F 0 "C3" H 2665 996 50  0000 L CNN
F 1 "1n" H 2665 905 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 2588 800 50  0001 C CNN
F 3 "~" H 2550 950 50  0001 C CNN
	1    2550 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 60EBBAE7
P 1700 950
F 0 "C1" H 1815 996 50  0000 L CNN
F 1 "10u" H 1815 905 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1738 800 50  0001 C CNN
F 3 "~" H 1700 950 50  0001 C CNN
	1    1700 950 
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR03
U 1 1 60EBB39A
P 1700 800
F 0 "#PWR03" H 1700 650 50  0001 C CNN
F 1 "VDD" H 1715 973 50  0000 C CNN
F 2 "" H 1700 800 50  0001 C CNN
F 3 "" H 1700 800 50  0001 C CNN
	1    1700 800 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 60EBAED3
P 1700 1100
F 0 "#PWR010" H 1700 850 50  0001 C CNN
F 1 "GND" H 1705 927 50  0000 C CNN
F 2 "" H 1700 1100 50  0001 C CNN
F 3 "" H 1700 1100 50  0001 C CNN
	1    1700 1100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR025
U 1 1 60EB981C
P 900 2400
F 0 "#PWR025" H 900 2150 50  0001 C CNN
F 1 "GND" H 905 2227 50  0000 C CNN
F 2 "" H 900 2400 50  0001 C CNN
F 3 "" H 900 2400 50  0001 C CNN
	1    900  2400
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery BT1
U 1 1 60EB8AAB
P 900 2200
F 0 "BT1" H 1008 2246 50  0000 L CNN
F 1 "Battery" H 1008 2155 50  0000 L CNN
F 2 "Battery:BatteryHolder_Keystone_2462_2xAA" V 900 2260 50  0001 C CNN
F 3 "~" V 900 2260 50  0001 C CNN
	1    900  2200
	1    0    0    -1  
$EndComp
$Comp
L Oscillator:ASE-xxxMHz X1
U 1 1 61260730
P 4850 2250
F 0 "X1" H 5194 2296 50  0000 L CNN
F 1 "DSC1001" H 5194 2205 50  0000 L CNN
F 2 "synth:DSC_1001_Oscillator_SMD_5032_4Pads" H 5550 1900 50  0001 C CNN
F 3 "http://www.abracon.com/Oscillators/ASV.pdf" H 4750 2250 50  0001 C CNN
	1    4850 2250
	1    0    0    -1  
$EndComp
NoConn ~ 4550 2250
$Comp
L power:VDD #PWR022
U 1 1 61264781
P 4850 1950
F 0 "#PWR022" H 4850 1800 50  0001 C CNN
F 1 "VDD" H 4865 2123 50  0000 C CNN
F 2 "" H 4850 1950 50  0001 C CNN
F 3 "" H 4850 1950 50  0001 C CNN
	1    4850 1950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR026
U 1 1 61264D09
P 4850 2550
F 0 "#PWR026" H 4850 2300 50  0001 C CNN
F 1 "GND" H 4855 2377 50  0000 C CNN
F 2 "" H 4850 2550 50  0001 C CNN
F 3 "" H 4850 2550 50  0001 C CNN
	1    4850 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2250 5650 2250
Text Label 5450 2250 0    50   ~ 0
CLK
Wire Wire Line
	10050 2850 10650 2850
Text Label 10400 2850 0    50   ~ 0
CLK
NoConn ~ 8850 2550
NoConn ~ 8850 2650
NoConn ~ 10050 3050
Text Notes 8750 5850 0    50   ~ 0
All TVS diodes: DESD5V0U1BA-7
$Comp
L synth:TLP2312 U1
U 1 1 6149FD0F
P 5350 1050
F 0 "U1" H 5694 1096 50  0000 L CNN
F 1 "TLP2312" H 5694 1005 50  0000 L CNN
F 2 "Package_SO:SO-5_4.4x3.6mm_P1.27mm" H 5260 1050 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/H11L3M-D.PDF" H 5260 1050 50  0001 C CNN
	1    5350 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 1150 5050 1200
Wire Wire Line
	10050 2650 10650 2650
NoConn ~ 10050 3250
$EndSCHEMATC
