#  main
This contains the main entry point to the firmware, initialisation routines and the main execution loop. It also contains the interrupt vector code.

# Configuration headers

## TCPIPConfig
Standard header required by the Microchip TCP/IP stack. This defines which of the stack modules should be used, as well as configuration settings such as the default IP address to use.

## HardwareProfile
Another standard header required by the Microchip TCP/IP stack. This maps PIC IO port pins to their uses on the Lettuce-v1 core board.

# Modules
Each of these corresponds to a source (.c) or header (.h) file, or both:

## LettuceClient
This broadcasts lettuce events out to the network using UDP.

## LettuceServer
This implements the UDP and TCP lettuce servers, recieving requests, dispatching them to the required module or plugin and sending back responses.

## LettuceSystem
This handles requests for lettuce system devices, namely:

* Info - supports reading of firmware version, build information and settings
* Clock - supports reading and setting the system's realtime clock

## LettuceRegistry
This manages and provides access to the lettuce registry - a settings database which is stored on the core board's EEPROM.

## LettuceCore
This handles requests for lettuce core devices, namely:

* EEPROM - allows reading and writing the core board's 1024 Kbit EEPROM
* Aux - supports the auxiliary board's temperature sensor and status LED
* USART - dual channel USART
* PWM - pulse width modulation outputs
* Counter - counter inputs
* Buttons - button inputs

## PluginController
Manages plugin device detection and dispatching requests and interrupts to plugins.

# Plugins
Optional code which is only used if a corresponding device is plugged in:

## plugin_ioport
Manages ioport plugin boards.

## plugin_mem
Manages SPI memory plugin boards.

## plugin_rtc
Manages realtime clock plugin boards.

## plugin_mp3
Manages MP3 player plugin boards.

# Utilities

## lettuce
This defines the basic constants used throughout the lettuce system, and implements low-level functions to manage IO ports, the SPI bus and interrupts for plugin devices.

## Buffer
This defines and implements the lettuce buffer structure, which is used to transfer and store variant data types.

## sralloc
This implements a simple dynamic memory manager for the lettuce system.
