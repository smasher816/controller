###| CMake Kiibohd Controller Output Module |###
#
# Written by Jacob Alexander in 2011-2018 for the Kiibohd Controller
#
# Released into the Public Domain
#
###


###
# Required Sub-modules
#

AddModule ( Output HID-IO )
AddModule ( Output Interface )


###
# Module C files
#

#| AVR Compiler
if ( ${COMPILER_FAMILY} MATCHES "avr" )

	set ( Module_SRCS
		output_com.c
		output_usb.c
		avr/usb_keyboard_serial.c
	)

#| ARM Compiler
elseif ( ${COMPILER_FAMILY} MATCHES "arm" )

	set ( Module_SRCS
		output_com.c
		output_usb.c
		arm/usb_desc.c
		arm/usb_dev.c
		arm/usb_joystick.c
		arm/usb_keyboard.c
		arm/usb_mem.c
		arm/usb_mouse.c
		arm/usb_rawio.c
		arm/usb_serial.c
	)


	if ( "${CHIP}" MATCHES "^sam.*$" )
	list(APPEND Module_SRCS
		../../Lib/ASF/sam/utils/cmsis/sam4s/source/templates/system_sam4s.c
		../../Lib/ASF/sam/drivers/udp/udp_device.c
		../../Lib/ASF/sam/drivers/pmc/pmc.c
		../../Lib/ASF/common/utils/interrupt/interrupt_sam_nvic.c
		../../Lib/ASF/common/services/clock/sam4s/sysclk.c

		../../Lib/ASF/common/services/usb/udc/udc.c

		#../../Lib/ASF/common/services/usb/class/hid/device/udi_hid.c
		#../../Lib/ASF/common/services/usb/class/hid/device/kbd/udi_hid_kbd.c
		#../../Lib/ASF/common/services/usb/class/hid/device/kbd/udi_hid_kbd_desc.c
	)
	endif ( )

#| Host Mode
elseif ( ${COMPILER_FAMILY} MATCHES "host" )

	set ( Module_SRCS
		output_com.c
		output_usb.c
	)

endif ()


###
# Compiler Family Compatibility
#

# XXX (HaaTa) USB module only supports host family used as a TestOut submodule
set ( ModuleCompatibility
	arm
	avr
	host
)

