#ifndef __HID_DESCRIPTOR_H__
#define __HID_DESCRIPTOR_H__

#include "em_usb.h"
#include "hidkbd.h"

#ifdef __cplusplus
extern "C" {
#endif


extern const HIDKBD_KeyReport_t             USBDESC_noKeyReport;
extern const HIDKBD_KeyReport_t             USBDESC_saveReport;
extern const HIDKBD_KeyReport_t             USBDESC_titleReportTable[11];
extern const HIDKBD_KeyReport_t             USBDESC_numberReportTable[10];
extern const void *                         USBDESC_HidDescriptor;

extern const USB_DeviceDescriptor_TypeDef   USBDESC_hid_device;
extern const uint8_t                        USBDESC_hid_config[];
extern const void * const                   USBDESC_hid_strings[4];
extern const uint8_t                        USBDESC_hid_bufferingMultiplier[];

#ifdef __cplusplus
}
#endif

#endif
