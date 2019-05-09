v 20070626 1
C 69800 42600 1 0 0 header8-2.sym
{
T 69800 44200 5 10 0 1 0 0 1
device=HEADER8
T 70400 44300 5 10 1 1 0 0 1
refdes=J1
T 69800 42600 5 10 0 0 0 0 1
footprint=LETTUCE_RA_SPI_SOCKET
}
T 69200 44600 9 10 1 0 0 0 1
MAINBOARD USART CONNECTOR
T 70525 43535 9 10 1 0 0 0 1
TX2
T 70122 43537 9 10 1 0 0 0 1
TX1
T 70520 43933 9 10 1 0 0 0 1
RX2
T 70155 43937 9 10 1 0 0 0 1
RX1
T 70142 42737 9 10 1 0 0 0 1
Vdd
T 70134 43139 9 10 1 0 0 0 1
Gnd
C 71200 42900 1 0 0 gnd-1.sym
C 69100 42800 1 0 0 3.3V-plus-1.sym
T 75000 38600 9 10 1 0 0 0 1
1
T 76500 38600 9 10 1 0 0 0 1
1
C 74300 38500 1 0 0 cvstitleblock-1.sym
{
T 75000 38900 5 10 1 1 0 0 1
date=2007-08-27
T 78800 38900 5 10 1 1 0 0 1
rev=1
T 78900 38600 5 10 1 1 0 0 1
auth=Andrew Wedgbury
T 74900 39200 5 10 1 1 0 0 1
fname=$Id: rs232lin.sch 56 2008-04-16 07:48:57Z  $
T 78100 39600 5 14 1 1 0 4 1
title=Lettuce-v1 plugin - !TTL / RS-232 / LINBus USART
}
T 70542 42737 9 10 1 0 0 0 1
Vdd
T 70534 43139 9 10 1 0 0 0 1
Gnd
C 69600 42900 1 0 0 gnd-1.sym
C 71500 42800 1 0 0 3.3V-plus-1.sym
N 69300 42800 69800 42800 4
N 71200 42800 71700 42800 4
C 63500 40300 1 0 0 max232-2.sym
{
T 63800 43950 5 10 0 0 0 0 1
device=MAX3232CSE+
T 65200 43800 5 10 1 1 0 6 1
refdes=U1
T 63800 44150 5 10 0 0 0 0 1
footprint=SO16
}
N 69800 43600 68800 43600 4
N 68800 41800 67900 41800 4
N 67900 41400 69000 41400 4
N 69000 44000 69800 44000 4
C 63300 43600 1 180 0 capacitor-2.sym
{
T 63100 42900 5 10 0 0 180 0 1
device=POLARIZED_CAPACITOR
T 63200 43600 5 10 1 1 180 0 1
refdes=C2
T 63100 42700 5 10 0 0 180 0 1
symversion=0.1
T 63300 43600 5 10 0 0 180 0 1
footprint=SMD_SIMPLE 80 50
T 62300 43500 5 10 1 1 0 0 1
value=0.1uF
}
C 63300 42800 1 180 0 capacitor-2.sym
{
T 63100 42100 5 10 0 0 180 0 1
device=POLARIZED_CAPACITOR
T 63200 42800 5 10 1 1 180 0 1
refdes=C3
T 63100 41900 5 10 0 0 180 0 1
symversion=0.1
T 63300 42800 5 10 0 0 180 0 1
footprint=SMD_SIMPLE 80 50
T 62300 42700 5 10 1 1 0 0 1
value=0.1uF
}
N 63500 43400 63300 43400 4
N 63500 42600 63300 42600 4
N 63500 43000 62300 43000 4
N 62300 43000 62300 43400 4
N 62300 43400 62400 43400 4
N 62400 42600 62300 42600 4
N 62300 42600 62300 42200 4
N 62300 42200 63500 42200 4
C 65700 43200 1 0 0 capacitor-2.sym
{
T 65900 43900 5 10 0 0 0 0 1
device=POLARIZED_CAPACITOR
T 65800 43200 5 10 1 1 0 0 1
refdes=C4
T 65900 44100 5 10 0 0 0 0 1
symversion=0.1
T 65700 43200 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
T 66300 43200 5 10 1 1 0 0 1
value=0.1uF
}
C 66900 43100 1 0 0 gnd-1.sym
N 65500 43400 65700 43400 4
N 66600 43400 67000 43400 4
C 66600 42800 1 180 0 capacitor-2.sym
{
T 66400 42100 5 10 0 0 180 0 1
device=POLARIZED_CAPACITOR
T 66000 42800 5 10 1 1 180 0 1
refdes=C5
T 66400 41900 5 10 0 0 180 0 1
symversion=0.1
T 66600 42800 5 10 0 0 180 0 1
footprint=SMD_SIMPLE 80 50
T 66300 42700 5 10 1 1 0 0 1
value=0.1uF
}
C 66900 42300 1 0 0 gnd-1.sym
N 65500 42600 65700 42600 4
N 66600 42600 67000 42600 4
N 71200 43600 73400 43600 4
N 72200 41000 67900 41000 4
N 71200 44000 73400 44000 4
N 72400 40600 67900 40600 4
N 63500 40600 61600 40600 4
N 71300 43200 71200 43200 4
N 69800 43200 69700 43200 4
C 66200 44700 1 180 0 capacitor-2.sym
{
T 66000 44000 5 10 0 0 180 0 1
device=POLARIZED_CAPACITOR
T 66100 44700 5 10 1 1 180 0 1
refdes=C1
T 66000 43800 5 10 0 0 180 0 1
symversion=0.1
T 66200 44700 5 10 0 0 180 0 1
footprint=SMD_SIMPLE 80 50
T 65200 44600 5 10 1 1 0 0 1
value=0.1uF
}
C 66300 44500 1 0 0 3.3V-plus-1.sym
C 64900 44200 1 0 0 gnd-1.sym
N 65000 44500 65300 44500 4
C 72600 38600 1 0 0 USART_Single_header-1.sym
{
T 72650 36750 5 10 0 1 0 0 1
device=HEADER18
T 73000 40300 5 10 1 1 0 0 1
refdes=J3
T 72600 38600 5 10 0 0 0 0 1
footprint=JUMPER4
}
C 69200 38600 1 0 0 USART_Single_header-1.sym
{
T 69250 36750 5 10 0 1 0 0 1
device=HEADER18
T 69600 40300 5 10 1 1 0 0 1
refdes=J2
T 69200 38600 5 10 0 0 0 0 1
footprint=JUMPER4
}
N 72400 40000 72400 44000 4
N 72400 40000 72600 40000 4
N 72200 39600 72200 43600 4
N 72200 39600 72600 39600 4
N 69000 40000 69000 44000 4
N 69000 40000 69200 40000 4
N 69200 39600 68800 39600 4
N 68800 39600 68800 43600 4
C 68900 38900 1 0 0 gnd-1.sym
C 72300 38900 1 0 0 gnd-1.sym
C 68200 38800 1 0 0 3.3V-plus-1.sym
C 71600 38800 1 0 0 3.3V-plus-1.sym
N 68400 38800 69200 38800 4
N 69000 39200 69200 39200 4
N 71800 38800 72600 38800 4
N 72400 39200 72600 39200 4
N 66900 41400 65500 41400 4
N 66900 41000 65500 41000 4
N 66900 40600 65500 40600 4
N 65500 41800 66900 41800 4
C 66800 44500 1 0 0 vcc-1.sym
N 66200 44500 67000 44500 4
C 58600 42000 1 0 0 RS232_Header-1.sym
{
T 58600 44000 5 10 0 1 0 0 1
device=HEADER10
T 59200 44100 5 10 1 1 0 0 1
refdes=J4
T 58600 42000 5 10 0 0 0 0 1
footprint=HEADER10_2
}
N 60000 43400 60400 43400 4
N 60400 43400 60400 44600 4
N 60400 44600 58200 44600 4
N 58200 43800 58600 43800 4
N 58600 43400 58200 43400 4
N 58200 43400 58200 44600 4
N 58600 42600 58400 42600 4
N 58400 42600 58400 44400 4
N 58400 44400 60200 44400 4
N 60200 44400 60200 43800 4
N 60200 43800 60000 43800 4
C 60100 42300 1 0 0 gnd-1.sym
N 60200 42600 60000 42600 4
N 58200 41400 63500 41400 4
N 58200 41400 58200 43000 4
N 58200 43000 58600 43000 4
N 58400 41800 63500 41800 4
N 58400 41800 58400 42200 4
N 58400 42200 58600 42200 4
C 58600 38400 1 0 0 RS232_Header-1.sym
{
T 59200 40500 5 10 1 1 0 0 1
refdes=J5
T 58600 40400 5 10 0 1 0 0 1
device=HEADER10
T 58600 38400 5 10 0 0 0 0 1
footprint=HEADER10_2
}
N 60000 39800 60400 39800 4
N 60400 39800 60400 41000 4
N 60400 41000 58200 41000 4
N 58200 40200 58600 40200 4
N 58600 39800 58200 39800 4
N 58200 39800 58200 41000 4
N 58600 39000 58400 39000 4
N 58400 39000 58400 40800 4
N 58400 40800 60200 40800 4
N 60200 40800 60200 40200 4
N 60200 40200 60000 40200 4
C 60100 38700 1 0 0 gnd-1.sym
N 60200 39000 60000 39000 4
N 58200 37800 58200 39400 4
N 58200 39400 58600 39400 4
N 58400 38200 58400 38600 4
N 58400 38600 58600 38600 4
N 61200 41000 63500 41000 4
N 61600 40600 61600 37800 4
N 61600 37800 58200 37800 4
N 58400 38200 61200 38200 4
N 61200 38200 61200 41000 4
C 67900 41700 1 90 0 jumper-1.sym
{
T 67400 42000 5 8 0 0 90 0 1
device=JUMPER
T 67500 42100 5 10 1 1 180 0 1
refdes=J7
T 67900 41700 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
C 67900 41300 1 90 0 jumper-1.sym
{
T 67400 41600 5 8 0 0 90 0 1
device=JUMPER
T 67500 41700 5 10 1 1 180 0 1
refdes=J8
T 67900 41300 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
C 67900 40900 1 90 0 jumper-1.sym
{
T 67400 41200 5 8 0 0 90 0 1
device=JUMPER
T 67500 41300 5 10 1 1 180 0 1
refdes=J9
T 67900 40900 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
C 67900 40500 1 90 0 jumper-1.sym
{
T 67400 40800 5 8 0 0 90 0 1
device=JUMPER
T 67500 40900 5 10 1 1 180 0 1
refdes=J10
T 67900 40500 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
N 73400 42200 68800 42200 4
C 74400 43900 1 90 0 jumper-1.sym
{
T 74000 44300 5 10 1 1 180 0 1
refdes=J11
T 73900 44200 5 8 0 0 90 0 1
device=JUMPER
T 74400 43900 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
C 74400 43500 1 90 0 jumper-1.sym
{
T 73900 43800 5 8 0 0 90 0 1
device=JUMPER
T 74000 43900 5 10 1 1 180 0 1
refdes=J12
T 74400 43500 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
C 74400 42100 1 90 0 jumper-1.sym
{
T 73900 42400 5 8 0 0 90 0 1
device=JUMPER
T 74000 42500 5 10 1 1 180 0 1
refdes=J13
T 74400 42100 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
B 66800 40400 1200 1800 6 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
B 73300 41600 1200 2800 6 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
C 75900 41400 1 0 0 MAX13021-1.sym
{
T 78500 40475 5 10 0 0 0 0 1
device=MSP430x44x
T 78550 43450 5 10 1 1 0 6 1
refdes=U2
T 78500 40050 5 10 0 0 0 0 1
footprint=SO8
}
N 74400 42200 75900 42200 4
N 75900 43000 75700 43000 4
N 75700 43000 75700 43600 4
N 75700 43600 74400 43600 4
N 74400 44000 75500 44000 4
N 75500 42600 75900 42600 4
C 75300 44900 1 0 0 3.3V-plus-1.sym
C 75400 44900 1 270 0 resistor-1.sym
{
T 75800 44600 5 10 0 0 270 0 1
device=RESISTOR
T 75700 44700 5 10 1 1 270 0 1
refdes=R2
T 75400 44900 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
}
N 75500 42600 75500 44000 4
C 79000 42300 1 0 0 gnd-1.sym
N 79100 42600 78800 42600 4
C 74400 41700 1 90 0 jumper-1.sym
{
T 73900 42000 5 8 0 0 90 0 1
device=JUMPER
T 74000 42100 5 10 1 1 180 0 1
refdes=J14
T 74400 41700 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 60 30
}
N 74400 41800 75900 41800 4
N 73400 41800 69000 41800 4
C 76500 40600 1 0 0 diode-1.sym
{
T 76900 41200 5 10 0 0 0 0 1
device=DIODE
T 76800 41100 5 10 1 1 0 0 1
refdes=D1
T 76500 40600 5 10 0 0 0 0 1
footprint=DIODE300
}
C 77400 40700 1 0 0 resistor-1.sym
{
T 77700 41100 5 10 0 0 0 0 1
device=RESISTOR
T 77600 41000 5 10 1 1 0 0 1
refdes=R1
T 78000 41000 5 10 1 1 0 0 1
value=1K
T 77400 40700 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
}
N 75700 41800 75700 40800 4
N 75700 40800 76500 40800 4
N 78300 40800 79000 40800 4
N 79000 40800 79000 41800 4
N 78800 41800 82800 41800 4
C 79300 41800 1 270 0 capacitor-1.sym
{
T 80000 41600 5 10 0 0 270 0 1
device=CAPACITOR
T 79600 41700 5 10 1 1 270 0 1
refdes=C6
T 80200 41600 5 10 0 0 270 0 1
symversion=0.1
T 79300 41800 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
T 79600 41200 5 10 1 1 270 0 1
value=0.1uF
}
C 79400 40600 1 0 0 gnd-1.sym
C 79700 42100 1 0 0 resistor-1.sym
{
T 80000 42500 5 10 0 0 0 0 1
device=RESISTOR
T 79900 42400 5 10 1 1 0 0 1
refdes=R3
T 80300 42400 5 10 1 1 0 0 1
value=5K
T 79700 42100 5 10 0 0 90 0 1
footprint=SMD_SIMPLE 80 50
}
N 79700 42200 78800 42200 4
T 66900 40200 6 10 1 0 0 0 1
232 Enable
T 73300 41400 6 10 1 0 0 0 1
LINBus Enable
C 81200 40800 1 0 0 header3-1.sym
{
T 82200 41450 5 10 0 0 0 0 1
device=HEADER3
T 81600 42100 5 10 1 1 0 0 1
refdes=J15
T 81200 40800 5 10 0 0 0 0 1
footprint=JUMPER3
}
N 80900 41400 82800 41400 4
C 80800 40700 1 0 0 gnd-1.sym
N 80900 41000 82800 41000 4
T 70500 38400 9 10 1 0 0 0 1
!TTL (Pass through)
T 60700 44000 9 10 1 2 270 0 1
RS-232 channel 1
T 83600 41700 9 10 1 0 270 0 1
LINBus
T 60700 40400 9 10 1 2 270 0 1
RS-232 channel 2
C 82000 40800 1 0 0 header3-1.sym
{
T 83000 41450 5 10 0 0 0 0 1
device=HEADER3
T 82400 42100 5 10 1 1 0 0 1
refdes=J16
T 82000 40800 5 10 0 0 0 0 1
footprint=JUMPER3
}
C 82800 40800 1 0 0 header3-1.sym
{
T 83800 41450 5 10 0 0 0 0 1
device=HEADER3
T 83200 42100 5 10 1 1 0 0 1
refdes=J17
T 82800 40800 5 10 0 0 0 0 1
footprint=JUMPER3
}
C 83500 42800 1 0 1 connector2-1.sym
{
T 83300 43800 5 10 0 0 0 6 1
device=CONNECTOR_2
T 83500 43600 5 10 1 1 0 6 1
refdes=J6
T 83500 42800 5 10 0 0 0 6 1
footprint=JUMPER2
}
C 81300 43300 1 0 0 gnd-1.sym
N 81800 43600 81800 43300 4
N 80900 41400 80900 43000 4
N 80600 42200 80900 42200 4
N 78800 43000 81800 43000 4
N 81800 43600 81400 43600 4
T 83600 43600 9 10 1 0 270 0 1
LIN Power