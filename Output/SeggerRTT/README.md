# Kiibohd Controller - SeggerRTT Output Module

With RTT it is possible to output information from the target microcontroller as well as sending input to the application at a very high speed without affecting the target's real time behavior.
SEGGER RTT can be used with any J-Link model and any supported target processor which allows background memory access, which are Cortex-M and RX targets.

Tools are available for Linux, Mac, and Windows by Downloading the "J-Link Software and Documentation Pack"
from Seggers website at https://www.segger.com/products/debug-probes/j-link/technology/real-time-transfer/about-real-time-transfer/

**NOTE:** This requires a seperate JLink device or segger compatible dev kit (such as the Nordic Preview Kits or the Atmel Xplained series).
This output module **is not** usable with standalone keyboard PCBS.

# Usage

If the `JLINK` variable is set to `1` in your CMake config then the `./load` script will automatically start a JLink server after flashing.

A GDB capable debug server can also be manually started with `./debug -s`.

Once a server is running then you can use the `./rtt` script to open a console connection.
This console is bidirectional and will allow you to see debug prints and interract with the cli.

## Files

* [capabilities.kll](capabilities.kll) - KLL capabilities file for the SeggerRTT Scan Module.
* [output_com.c](output_com.c) - Stub functions for Output module.
* output_rtt[.h](output_rtt.h)/[.c](output_rtt.c) - SeggerRTT module implementation and overrides.
* [setup.cmake](setup.cmake) - CMake configuration for SeggerRTT module.

