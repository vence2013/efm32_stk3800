#include "em_usb.h"

#include "bsp.h"

#include "uvd_descriptor.h"
#include "uvd_led.h"



#define LED0            0
#define LED1            1

#define VND_GET_LEDS    0x10
#define VND_SET_LED     0x11

static int SetupCmd(const USB_Setup_TypeDef *setup);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = NULL,
  .setupCmd        = SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_uvd_device,
  .configDescriptor    = USBDESC_uvd_config,
  .stringDescriptors   = USBDESC_uvd_strings,
  .numberOfStrings     = sizeof(USBDESC_uvd_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_uvd_bufferingMultiplier,
  .reserved            = 0
};


void uvd_setup( void )
{
	/* Initialize LED driver */
	BSP_LedsInit();
	BSP_LedClear(LED0);
	BSP_LedClear(LED1);

	/* Initialize and start USB device stack. */
	USBD_Init(&usbInitStruct);
}


/***************************************************************************//**
 * @brief
 *   Handle USB setup commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 ******************************************************************************/
static int SetupCmd(const USB_Setup_TypeDef *setup)
{
  int             retVal;
  static uint32_t buffer;
  uint8_t         *pBuffer = (uint8_t*) &buffer;

  retVal = USB_STATUS_REQ_UNHANDLED;

  if (setup->Type == USB_SETUP_TYPE_VENDOR) {
    switch (setup->bRequest) {
      case VND_GET_LEDS:
        /********************/
        *pBuffer = (uint8_t)BSP_LedsGet();
        retVal   = USBD_Write(0, pBuffer, setup->wLength, NULL);
        break;

      case VND_SET_LED:
        /********************/
        if (setup->wValue) {
          if ( setup->wIndex == LED0 ) {
            BSP_LedSet(LED0);
          } else if ( setup->wIndex == LED1 ) {
            BSP_LedSet(LED1);
          }
        } else {
          if ( setup->wIndex == LED0 ) {
            BSP_LedClear(LED0);
          } else if ( setup->wIndex == LED1 ) {
            BSP_LedClear(LED1);
          }
        }
        retVal = USB_STATUS_OK;
        break;
    }
  }

  return retVal;
}
