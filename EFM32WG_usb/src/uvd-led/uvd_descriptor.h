#ifndef __UVD_DESCRIPTOR_H__
#define __UVD_DESCRIPTOR_H__


#include "em_usb.h"


#ifdef __cplusplus
extern "C" {
#endif

extern const USB_DeviceDescriptor_TypeDef   USBDESC_uvd_device;
extern const uint8_t                        USBDESC_uvd_config[];
extern const void * const                   USBDESC_uvd_strings[4];
extern const uint8_t                        USBDESC_uvd_bufferingMultiplier[];

#ifdef __cplusplus
}
#endif

#endif
