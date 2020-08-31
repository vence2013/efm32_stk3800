#include "hid_descriptor.h"

SL_ALIGN(4)
const USB_DeviceDescriptor_TypeDef USBDESC_hid_device SL_ATTRIBUTE_ALIGN(4) =
{
  .bLength            = USB_DEVICE_DESCSIZE,
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = 0,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = USB_FS_CTRL_EP_MAXSIZE,
  .idVendor           = 0x10C4,
  .idProduct          = 0x0002,
  .bcdDevice          = 0x0000,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1
};


SL_ALIGN(4)
const uint8_t USBDESC_hid_config[] SL_ATTRIBUTE_ALIGN(4) =
{
  /*** Configuration descriptor ***/
  USB_CONFIG_DESCSIZE,    /* bLength                                   */
  USB_CONFIG_DESCRIPTOR,  /* bDescriptorType                           */

  USB_CONFIG_DESCSIZE     /* wTotalLength (LSB)                        */
  + USB_INTERFACE_DESCSIZE
  + USB_HID_DESCSIZE
  + (USB_ENDPOINT_DESCSIZE * NUM_EP_USED),

  (USB_CONFIG_DESCSIZE    /* wTotalLength (MSB)                        */
   + USB_INTERFACE_DESCSIZE
   + USB_HID_DESCSIZE
   + (USB_ENDPOINT_DESCSIZE * NUM_EP_USED)) >> 8,

  1,                      /* bNumInterfaces                            */
  1,                      /* bConfigurationValue                       */
  0,                      /* iConfiguration                            */
#if defined(BUSPOWERED)
  CONFIG_DESC_BM_RESERVED_D7,    /* bmAttrib: Bus powered              */
#else
  CONFIG_DESC_BM_RESERVED_D7     /* bmAttrib: Self powered             */
  | CONFIG_DESC_BM_SELFPOWERED,
#endif
  CONFIG_DESC_MAXPOWER_mA(100),   /* bMaxPower: 100 mA                 */

  /*** Interface descriptor ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  HIDKBD_INTERFACE_NO,    /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  NUM_EP_USED,            /* bNumEndpoints         */
  0x03,                   /* bInterfaceClass (HID) */
  0,                      /* bInterfaceSubClass    */
  1,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */

  /*** HID descriptor ***/
  USB_HID_DESCSIZE,       /* bLength               */
  USB_HID_DESCRIPTOR,     /* bDescriptorType       */
  0x11,                   /* bcdHID (LSB)          */
  0x01,                   /* bcdHID (MSB)          */
  0,                      /* bCountryCode          */
  1,                      /* bNumDescriptors       */
  USB_HID_REPORT_DESCRIPTOR,            /* bDecriptorType        */
  sizeof(HIDKBD_ReportDescriptor),      /* wDescriptorLength(LSB)*/
  sizeof(HIDKBD_ReportDescriptor) >> 8, /* wDescriptorLength(MSB)*/

  /*** Endpoint descriptor ***/
  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  HIDKBD_INTR_IN_EP_ADDR, /* bEndpointAddress (IN) */
  USB_EPTYPE_INTR,        /* bmAttributes          */
  USB_FS_INTR_EP_MAXSIZE, /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  HIDKBD_POLL_RATE,       /* bInterval             */
};

const void *USBDESC_HidDescriptor = (void*)
                                    &USBDESC_hid_config[USB_CONFIG_DESCSIZE + USB_INTERFACE_DESCSIZE];

STATIC_CONST_STRING_DESC_LANGID(langID, 0x04, 0x09);
STATIC_CONST_STRING_DESC(iManufacturer, 'S', 'i', 'l', 'i', 'c', 'o', 'n', ' ', 'L', \
                         'a', 'b', 'o', 'r', 'a', 't', 'o', 'r', 'i',                \
                         'e', 's', ' ', 'I', 'n', 'c', '.');
STATIC_CONST_STRING_DESC(iProduct, 'E', 'F', 'M', '3', '2', ' ', 'U', 'S', 'B', \
                         ' ', 'H', 'I', 'D', ' ', 'K', 'e', 'y', 'b',           \
                         'o', 'a', 'r', 'd');
STATIC_CONST_STRING_DESC(iSerialNumber, '0', '0', '0', '0', '0', '0', \
                         '0', '0', '1', '2', '3', '4');

const void * const USBDESC_hid_strings[] =
{
  &langID,
  &iManufacturer,
  &iProduct,
  &iSerialNumber
};

/* Endpoint buffer sizes */
/* 1 = single buffer, 2 = double buffering, 3 = triple buffering ... */
const uint8_t USBDESC_hid_bufferingMultiplier[NUM_EP_USED + 1] = { 1, 1 };

const HIDKBD_KeyReport_t USBDESC_noKeyReport =
{ 0x00, 0x00, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

const HIDKBD_KeyReport_t USBDESC_saveReport =
{ 0x01, 0x00, { 0x16, 0x00, 0x00, 0x00, 0x00, 0x00 } };   /* LeftContrl + s */

/* A sequence of keystroke input reports. */
SL_ALIGN(4)
const HIDKBD_KeyReport_t USBDESC_titleReportTable[11] SL_ATTRIBUTE_ALIGN(4) =
{
  { 0x02, 0x00, { 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'C'   */
  { 0x00, 0x00, { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'o'   */
  { 0x00, 0x00, { 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'u'   */
  { 0x00, 0x00, { 0x11, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'n'   */
  { 0x00, 0x00, { 0x17, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 't'   */
  { 0x00, 0x00, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* space */
  { 0x00, 0x00, { 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'b'   */
  { 0x00, 0x00, { 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'y'   */
  { 0x00, 0x00, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* space */
  { 0x00, 0x00, { 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'w'   */
  { 0x02, 0x00, { 0x33, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* ':'   */
};

SL_ALIGN(4)
const HIDKBD_KeyReport_t USBDESC_numberReportTable[10] SL_ATTRIBUTE_ALIGN(4) =
{
  { 0x00, 0x00, { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '0'   */
  { 0x00, 0x00, { 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '1'   */
  { 0x00, 0x00, { 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '2'   */
  { 0x00, 0x00, { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '3'   */
  { 0x00, 0x00, { 0x21, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '4'   */
  { 0x00, 0x00, { 0x22, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '5'   */
  { 0x00, 0x00, { 0x23, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '6'   */
  { 0x00, 0x00, { 0x24, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '7'   */
  { 0x00, 0x00, { 0x25, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '8'   */
  { 0x00, 0x00, { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '9'   */
};

