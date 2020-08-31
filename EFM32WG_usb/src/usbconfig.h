#ifndef __USBCONFIG_H
#define __USBCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//#define BUSPOWERED      /* Uncomment to build buspowered device */

#if defined(BUSPOWERED)
  #define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ENTEREM2)
#else
  #define USB_PWRSAVE_MODE  (USB_PWRSAVE_MODE_ONVBUSOFF | USB_PWRSAVE_MODE_ONSUSPEND)
#endif

#define USB_USBC_32kHz_CLK   USB_USBC_32kHz_CLK_LFRCO

#define USB_DEVICE        /* Compile stack for device mode. */


/****************************************************************************
**                                                                         **
** USB HID keyboard class device driver definitions.                       **
**                                                                         **
*****************************************************************************/
#define HIDKBD_INTERFACE_NO     0
#define HIDKBD_INTR_IN_EP_ADDR  0x81    /* Bit 7 must be set.                 */
#define HIDKBD_POLL_RATE        24      /* The bInterval reported with the    */
                                        /* interrupt IN endpoint descriptor.  */
#define HIDKBD_IDLE_TIMER       0       /* Timer used to implement the idle-  */
                                        /* rate defined in the HID class spec.*/


#define USBDEMO_HID_KBD                                           (1)
#define USBDEMO_CDC_USB2SERIAL                                    (2)
#define USBDEMO_MSC_UDISK                                         (3)
#define USBDEMO_UVD_LED                                           (4)
#define USBDEMO_USE                                               USBDEMO_UVD_LED

#if   (USBDEMO_USE == USBDEMO_HID)
	#include "hid_config.h"
#elif (USBDEMO_USE == USBDEMO_CDC_USB2SERIAL)
	#include "cdc_config.h"
#elif (USBDEMO_USE == USBDEMO_MSC_UDISK)
	#include "msc_config.h"
#elif (USBDEMO_USE == USBDEMO_UVD_LED)
	#include "uvd_config.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USBCONFIG_H */
