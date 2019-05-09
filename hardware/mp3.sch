v 20070626 1
C 38000 47700 1 0 0 crystal-1.sym
{
T 38200 48200 5 10 0 0 0 0 1
device=CRYSTAL
T 38000 48100 5 10 1 1 0 0 1
refdes=X1
T 38200 48400 5 10 0 0 0 0 1
symversion=0.1
T 38000 47700 5 10 0 0 0 0 1
footprint=CRYSTAL 300
T 38400 48100 5 10 1 1 0 0 1
value=12.288 MHz
}
C 37700 47000 1 270 0 capacitor-1.sym
{
T 38400 46800 5 10 0 0 270 0 1
device=CAPACITOR
T 38000 46900 5 10 1 1 270 0 1
refdes=C1
T 38600 46800 5 10 0 0 270 0 1
symversion=0.1
T 37700 47000 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
T 38000 46400 5 10 1 1 270 0 1
value=33pF
}
C 38600 47000 1 270 0 capacitor-1.sym
{
T 39300 46800 5 10 0 0 270 0 1
device=CAPACITOR
T 38900 46900 5 10 1 1 270 0 1
refdes=C2
T 39500 46800 5 10 0 0 270 0 1
symversion=0.1
T 38600 47000 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
T 38900 46400 5 10 1 1 270 0 1
value=33pF
}
N 38800 48000 38800 47000 4
N 38700 47800 38800 47800 4
N 38000 47800 37900 47800 4
C 46600 46800 1 0 0 capacitor-1.sym
{
T 46800 47500 5 10 0 0 0 0 1
device=CAPACITOR
T 46700 47100 5 10 1 1 0 0 1
refdes=C5
T 46800 47700 5 10 0 0 0 0 1
symversion=0.1
T 46600 46800 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
T 47200 47100 5 10 1 1 0 0 1
value=100nF
}
C 48400 49400 1 0 0 3.3V-plus-1.sym
{
T 49000 49400 5 10 1 1 0 0 1
netname=AVDD
}
C 38800 47500 1 180 0 resistor-1.sym
{
T 38500 47100 5 10 0 0 180 0 1
device=RESISTOR
T 38300 47200 5 10 1 1 180 0 1
refdes=R3
T 38800 47500 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
T 38400 47100 5 10 1 1 0 0 1
value=1M
}
T 43700 53500 9 10 1 0 0 0 1
1
T 45200 53500 9 10 1 0 0 0 1
1
C 43000 53400 1 0 0 cvstitleblock-1.sym
{
T 43700 53800 5 10 1 1 0 0 1
date=2007-08-31
T 47500 53800 5 10 1 1 0 0 1
rev=1
T 47600 53500 5 10 1 1 0 0 1
auth=Andrew Wedgbury
T 43600 54100 5 10 1 1 0 0 1
fname=$Id: mp3.sch 39 2007-11-28 09:15:33Z  $
T 46800 54500 5 14 1 1 0 4 1
title=Lettuce v1.0 mp3 audio plugin (VS1011)
}
C 42200 45400 1 0 0 VS1011-28-1.sym
{
T 44800 49275 5 10 0 0 0 0 1
device=MSP430x44x
T 46250 52050 5 10 1 1 0 6 1
refdes=U1
T 42200 45400 5 10 0 0 0 0 1
footprint=SO28
}
N 42200 48400 37900 48400 4
N 37900 47000 37900 48400 4
N 38800 48000 42200 48000 4
C 39300 46000 1 270 0 resistor-1.sym
{
T 39700 45700 5 10 0 0 270 0 1
device=RESISTOR
T 39600 46000 5 10 1 1 270 0 1
refdes=R4
T 39300 46000 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 80 50
T 39600 45600 5 10 1 1 270 0 1
value=100K
}
N 42200 47400 39400 47400 4
N 39400 47400 39400 46000 4
C 39900 46000 1 270 0 resistor-1.sym
{
T 40300 45700 5 10 0 0 270 0 1
device=RESISTOR
T 40200 46000 5 10 1 1 270 0 1
refdes=R5
T 39900 46000 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 80 50
T 40200 45600 5 10 1 1 270 0 1
value=100K
}
C 40500 46000 1 270 0 resistor-1.sym
{
T 40900 45700 5 10 0 0 270 0 1
device=RESISTOR
T 40800 46000 5 10 1 1 270 0 1
refdes=R6
T 40500 46000 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 80 50
T 40800 45600 5 10 1 1 270 0 1
value=100K
}
C 41100 46000 1 270 0 resistor-1.sym
{
T 41500 45700 5 10 0 0 270 0 1
device=RESISTOR
T 41400 46000 5 10 1 1 270 0 1
refdes=R7
T 41100 46000 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 80 50
T 41400 45600 5 10 1 1 270 0 1
value=100K
}
N 42200 47000 40000 47000 4
N 40000 47000 40000 46000 4
N 42200 46600 40600 46600 4
N 40600 46600 40600 46000 4
N 42200 46200 41200 46200 4
N 41200 46200 41200 46000 4
C 41800 45600 1 0 0 3.3V-plus-1.sym
N 42000 45600 42200 45600 4
C 37100 49200 1 270 0 resistor-1.sym
{
T 37500 48900 5 10 0 0 270 0 1
device=RESISTOR
T 37400 49200 5 10 1 1 270 0 1
refdes=R2
T 37100 49200 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 80 50
T 37400 48800 5 10 1 1 270 0 1
value=100K
}
C 39500 44300 1 0 0 gnd-1.sym
N 47500 47000 47600 47000 4
N 47600 45000 47600 47000 4
N 46600 46400 47600 46400 4
N 46600 46000 47600 46000 4
N 46600 45600 47600 45600 4
N 46600 49200 48600 49200 4
N 48600 48800 48600 49400 4
N 48600 48800 46600 48800 4
N 46600 50400 47200 50400 4
N 47200 50400 47200 49600 4
N 47200 49600 46600 49600 4
N 46600 50000 47200 50000 4
C 47100 49300 1 0 0 gnd-1.sym
C 47000 52000 1 0 0 3.3V-plus-1.sym
N 47200 51000 47200 52000 4
N 47200 51000 46600 51000 4
N 46600 51400 47200 51400 4
N 46600 51800 47200 51800 4
N 46600 47800 48300 47800 4
N 46600 48200 47700 48200 4
C 47700 48100 1 0 0 resistor-1.sym
{
T 48000 48500 5 10 0 0 0 0 1
device=RESISTOR
T 47700 48400 5 10 1 1 0 0 1
refdes=R8
T 47700 48100 5 10 0 0 180 0 1
footprint=SMD_SIMPLE 80 50
T 48100 48400 5 10 1 1 0 0 1
value=100R
}
C 48300 47700 1 0 0 resistor-1.sym
{
T 48600 48100 5 10 0 0 0 0 1
device=RESISTOR
T 48300 47500 5 10 1 1 0 0 1
refdes=R9
T 48300 47700 5 10 0 0 180 0 1
footprint=SMD_SIMPLE 80 50
T 48700 47500 5 10 1 1 0 0 1
value=100R
}
N 49700 48200 51200 48200 4
N 50300 47800 50400 47800 4
N 47600 45600 50800 45600 4
N 50800 45600 50800 47900 4
N 50800 47900 51200 47900 4
N 36000 54000 35400 54000 4
N 35400 51200 35400 54000 4
N 35400 51200 42200 51200 4
N 37400 54000 38000 54000 4
N 38000 54000 38000 51600 4
N 38000 51600 42200 51600 4
N 42200 50800 35600 50800 4
N 35600 50800 35600 53600 4
N 35600 53600 36000 53600 4
C 36200 51900 1 0 0 resistor-1.sym
{
T 36500 52300 5 10 0 0 0 0 1
device=RESISTOR
T 36200 52200 5 10 1 1 0 0 1
refdes=R1
T 36200 51900 5 10 0 0 180 0 1
footprint=SMD_SIMPLE 80 50
T 36600 52200 5 10 1 1 0 0 1
value=100R
}
N 36000 53200 35800 53200 4
N 35800 53200 35800 52000 4
N 35800 52000 36200 52000 4
N 37100 52000 37600 52000 4
N 37600 52000 37600 53200 4
N 37400 53200 37600 53200 4
C 37300 52500 1 0 0 gnd-1.sym
N 36000 52800 35000 52800 4
C 34800 52800 1 0 0 3.3V-plus-1.sym
N 42200 49200 37800 49200 4
N 37800 49200 37800 53600 4
N 37800 53600 37400 53600 4
N 42200 50400 38600 50400 4
N 38600 50400 38600 54000 4
N 38600 54000 39200 54000 4
N 42200 48800 41000 48800 4
N 41000 48800 41000 54000 4
N 41000 54000 40600 54000 4
N 38800 50000 38800 53600 4
N 38800 53600 39200 53600 4
T 37000 54800 9 10 1 0 0 0 1
MAINBOARD CONNECTORS
N 37200 50000 42200 50000 4
N 37200 50000 37200 49200 4
T 53000 48300 9 10 1 0 270 0 1
AUDIO OUT
N 37200 45000 37200 48300 4
N 41200 45000 41200 45100 4
N 40600 45000 40600 45100 4
N 40000 45000 40000 45100 4
N 39400 45000 39400 45100 4
N 38800 46100 38800 45000 4
N 37900 46100 37900 45000 4
N 39600 44600 39600 45000 4
T 39800 44400 9 10 1 0 0 0 1
(DIGITAL)
C 47500 44700 1 0 0 gnd-1.sym
{
T 47800 44600 5 10 1 1 0 0 1
netname=AGND
}
T 47800 44800 9 10 1 0 0 0 1
(ANALOG)
N 37200 45000 41200 45000 4
T 49000 49600 9 10 1 0 0 0 1
(ANALOG)
T 39800 44400 9 10 1 0 0 0 1
(DIGITAL)
T 47400 49400 9 10 1 0 0 0 1
(DIGITAL)
T 47600 52200 9 10 1 0 0 0 1
(DIGITAL)
C 48800 48000 1 0 0 capacitor-2.sym
{
T 49000 48700 5 10 0 0 0 0 1
device=POLARIZED_CAPACITOR
T 48800 48500 5 10 1 1 0 0 1
refdes=C3
T 49000 48900 5 10 0 0 0 0 1
symversion=0.1
T 48800 48000 5 10 0 0 0 0 1
footprint=RCY100
T 49200 48500 5 10 1 1 0 0 1
value=10uF
}
C 49400 47600 1 0 0 capacitor-2.sym
{
T 49600 48300 5 10 0 0 0 0 1
device=POLARIZED_CAPACITOR
T 49400 47400 5 10 1 1 0 0 1
refdes=C4
T 49600 48500 5 10 0 0 0 0 1
symversion=0.1
T 49400 47600 5 10 0 0 0 0 1
footprint=RCY100
T 49800 47400 5 10 1 1 0 0 1
value=10uF
}
N 49200 47800 49400 47800 4
N 48600 48200 48800 48200 4
C 52900 47100 1 0 1 connector4-1.sym
{
T 51100 48000 5 10 0 0 0 6 1
device=CONNECTOR_4
T 52900 48500 5 10 1 1 0 6 1
refdes=J3
T 52900 47100 5 10 0 0 0 0 1
footprint=JUMPER4
}
N 50400 47800 50400 47300 4
N 50400 47300 51200 47300 4
N 51200 47600 50800 47600 4
C 36000 52600 1 0 0 SPI_Header-1.sym
{
T 36000 54600 5 10 0 1 0 0 1
device=HEADER10
T 36600 54300 5 10 1 1 0 0 1
refdes=J1
T 36000 52600 5 10 0 0 0 0 1
footprint=LETTUCE_RA_SPI_SOCKET
}
C 39200 52600 1 0 0 DIG_Header-1.sym
{
T 39200 54600 5 10 0 1 0 0 1
device=HEADER10
T 39800 54300 5 10 1 1 0 0 1
refdes=J2
T 39200 52600 5 10 0 0 0 0 1
footprint=LETTUCE_RA_SPI_SOCKET
}
