#ifndef __CDC_DESCRIPTOR_H__
#define __CDC_DESCRIPTOR_H__

#include "em_usb.h"

#ifdef __cplusplus
extern "C" {
#endif


extern const USB_DeviceDescriptor_TypeDef   USBDESC_cdc_device;
extern const uint8_t                        USBDESC_cdc_config[];
extern const void * const                   USBDESC_cdc_strings[4];
extern const uint8_t                        USBDESC_cdc_bufferingMultiplier[];


#ifdef __cplusplus
}
#endif

#endif
