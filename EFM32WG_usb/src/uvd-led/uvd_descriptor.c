#include "uvd_descriptor.h"

SL_ALIGN(4)
const USB_DeviceDescriptor_TypeDef USBDESC_uvd_device SL_ATTRIBUTE_ALIGN(4) =
{
  .bLength            = USB_DEVICE_DESCSIZE,
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = 0xFF,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = USB_FS_CTRL_EP_MAXSIZE,
  .idVendor           = 0x10C4,
  .idProduct          = 0x0001,
  .bcdDevice          = 0x0000,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1
};


SL_ALIGN(4)
const uint8_t USBDESC_uvd_config[] SL_ATTRIBUTE_ALIGN(4) =
{
  /*** Configuration descriptor ***/
  USB_CONFIG_DESCSIZE,    /* bLength                                   */
  USB_CONFIG_DESCRIPTOR,  /* bDescriptorType                           */

  USB_CONFIG_DESCSIZE     /* wTotalLength (LSB)                        */
  + USB_INTERFACE_DESCSIZE
  + (USB_ENDPOINT_DESCSIZE * NUM_EP_USED),

  (USB_CONFIG_DESCSIZE    /* wTotalLength (MSB)                        */
   + USB_INTERFACE_DESCSIZE
   + (USB_ENDPOINT_DESCSIZE * NUM_EP_USED)) >> 8,

  1,                      /* bNumInterfaces                            */
  1,                      /* bConfigurationValue                       */
  0,                      /* iConfiguration                            */
  CONFIG_DESC_BM_RESERVED_D7     /* bmAttrib: Self powered             */
  | CONFIG_DESC_BM_SELFPOWERED,
  CONFIG_DESC_MAXPOWER_mA(100),  /* bMaxPower: 100 mA                  */

  /*** Interface descriptor ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  0,                      /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  NUM_EP_USED,            /* bNumEndpoints         */
  0xFF,                   /* bInterfaceClass       */
  0,                      /* bInterfaceSubClass    */
  0,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */
};

STATIC_CONST_STRING_DESC_LANGID(langID, 0x04, 0x09);
STATIC_CONST_STRING_DESC(iManufacturer, 'S', 'i', 'l', 'i', 'c', 'o', 'n', ' ', 'L', \
                         'a', 'b', 'o', 'r', 'a', 't', 'o', 'r', 'i',                \
                         'e', 's', ' ', 'I', 'n', 'c', '.');
STATIC_CONST_STRING_DESC(iProduct, 'E', 'F', 'M', '3', '2', ' ', 'U', 'S', 'B', \
                         ' ', 'V', 'e', 'n', 'd', 'o', 'r', ' ',                \
                         'U', 'n', 'i', 'q', 'u', 'e', ' ',                     \
                         'L', 'E', 'D', ' ',                                    \
                         'D', 'e', 'v', 'i', 'c', 'e');
STATIC_CONST_STRING_DESC(iSerialNumber, '0', '0', '0', '0', '0', '0', \
                         '0', '0', '1', '2', '3', '4');

const void * const USBDESC_uvd_strings[] =
{
  &langID,
  &iManufacturer,
  &iProduct,
  &iSerialNumber
};

/* Endpoint buffer sizes */
/* 1 = single buffer, 2 = double buffering, 3 = tripple buffering ... */
const uint8_t USBDESC_uvd_bufferingMultiplier[NUM_EP_USED + 1] = { 1 };
