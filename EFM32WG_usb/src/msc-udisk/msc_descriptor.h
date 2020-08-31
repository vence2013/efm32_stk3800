#ifndef __MSC_DESCRIPTOR_H__
#define __MSC_DESCRIPTOR_H__

#include "em_usb.h"

extern const USB_DeviceDescriptor_TypeDef   USBDESC_msc_device;
extern const uint8_t                        USBDESC_msc_config[];
extern const void * const                   USBDESC_msc_strings[4];
extern const uint8_t                        USBDESC_msc_bufferingMultiplier[];

#endif
