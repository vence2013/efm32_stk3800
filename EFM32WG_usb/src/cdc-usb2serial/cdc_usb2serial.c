#include "em_usb.h"
#include "cdc.h"
#include "cdc_descriptor.h"


int SetupCmd(const USB_Setup_TypeDef *setup);
void StateChangeEvent(USBD_State_TypeDef oldState,
                      USBD_State_TypeDef newState);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChangeEvent,
  .setupCmd        = SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_cdc_device,
  .configDescriptor    = USBDESC_cdc_config,
  .stringDescriptors   = USBDESC_cdc_strings,
  .numberOfStrings     = sizeof(USBDESC_cdc_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_cdc_bufferingMultiplier,
  .reserved            = 0
};

void cdc_setup( void )
{
	CDC_Init();                   /* Initialize the communication class device. */

	/* Initialize and start USB device stack. */
	USBD_Init(&usbInitStruct);
}

/***************************************************************************//**
 * @brief
 *   Called whenever a USB setup command is received.
 *
 * @param[in] setup
 *   Pointer to an USB setup packet.
 *
 * @return
 *   An appropriate status/error code. See USB_Status_TypeDef.
 ******************************************************************************/
int SetupCmd(const USB_Setup_TypeDef *setup)
{
	int retVal;

	/* Call SetupCmd handlers for all functions within the composite device. */
	retVal = CDC_SetupCmd(setup);

	return retVal;
}

/***************************************************************************//**
 * @brief
 *   Called whenever the USB device has changed its device state.
 *
 * @param[in] oldState
 *   The device USB state just leaved. See USBD_State_TypeDef.
 *
 * @param[in] newState
 *   New (the current) USB device state. See USBD_State_TypeDef.
 ******************************************************************************/
void StateChangeEvent(USBD_State_TypeDef oldState,
                      USBD_State_TypeDef newState)
{
	/* Call device StateChange event handlers for all relevant functions within
	   the composite device. */

	CDC_StateChangeEvent(oldState, newState);
}
