/**
 * \file
 *
 * \brief USB configuration file
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include <buildvars.h>
#include "compiler.h"

/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             VENDOR_ID
#define  USB_DEVICE_PRODUCT_ID            PRODUCT_ID
//#define  USB_DEVICE_MAJOR_VERSION         1
//#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_VERSION               BCD_VERSION
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  (USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
//#define  USB_DEVICE_MANUFACTURE_NAME      "Kiibohd"
//#define  USB_DEVICE_PRODUCT_NAME          "Boot Keyboard"
// #define  USB_DEVICE_SERIAL_NAME           "12...EF"

/**
 * Device speeds support
 * @{
 */
//! To define a Low speed device
//#define  USB_DEVICE_LOW_SPEED

//! To authorize the High speed
#if (UC3A3||UC3A4)
//#define  USB_DEVICE_HS_SUPPORT
#elif (SAM3XA||SAM3U)
//#define  USB_DEVICE_HS_SUPPORT
#endif
//@}

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
//#define  UDC_VBUS_EVENT(b_vbus_high)
//#define  UDC_SOF_EVENT()                  main_sof_action()
//#define  UDC_SUSPEND_EVENT()              main_suspend_action()
//#define  UDC_RESUME_EVENT()               main_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
//#define  UDC_REMOTEWAKEUP_ENABLE()        main_remotewakeup_enable()
//#define  UDC_REMOTEWAKEUP_DISABLE()       main_remotewakeup_disable()
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
extern bool main_extra_string();
#define  UDC_GET_EXTRA_STRING() main_extra_string()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of HID Keyboard interface
 * @{
 */
//! Interface callback definition
#define  UDI_HID_KBD_ENABLE_EXT()       main_kbd_enable()
#define  UDI_HID_KBD_DISABLE_EXT()
#define  UDI_HID_KBD_CHANGE_LED(value)
//@}
//@}

/**
 * Configuration of DFU interface
 * @{
 */
//! Interface callback definition
#define  UDI_DFU_ENABLE_EXT()             true
#define  UDI_DFU_DISABLE_EXT()

//! FLIP protocol version to use
#define  FLIP_PROTOCOL_VERSION   FLIP_PROTOCOL_VERSION_2
// Split erase is available since batchisp 1.2.5 to avoid USB protocol 2 error
#define  UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
// Reduce the RAM used (1KB instead of 2KB), but the CODE increase of 80B
#define  UDI_DFU_SMALL_RAM
//@}
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#if defined(_bootloader_)
bool udi_dfu_atmel_setup(void);

#undef USB_DEVICE_MAX_EP
#define USB_DEVICE_SPECIFIC_REQUEST udi_dfu_atmel_setup
#define  USB_DEVICE_MAX_EP 1
#include "udi_dfu_atmel_conf.h"

#else
#define USB_DEVICE_SPECIFIC_REQUEST usb_setup
#include "udi_hid_kbd_conf.h"
#endif

#include "main.h"

#endif // _CONF_USB_H_
