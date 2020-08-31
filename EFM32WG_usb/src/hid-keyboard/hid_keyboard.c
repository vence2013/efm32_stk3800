#include "em_usb.h"
#include "hid_descriptor.h"

#include "hid_keyboard.h"


#define SCAN_TIMER              1       /* Timer used to scan keyboard. */
#define SCAN_RATE               50


static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState);


static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChange,
  .setupCmd        = HIDKBD_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_hid_device,
  .configDescriptor    = USBDESC_hid_config,
  .stringDescriptors   = USBDESC_hid_strings,
  .numberOfStrings     = sizeof(USBDESC_hid_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_hid_bufferingMultiplier,
  .reserved            = 0
};

void hid_setup( void )
{
	HIDKBD_Init_t hidInitStruct;

	/* Initialize HID keyboard driver. */
	hidInitStruct.hidDescriptor = (void*)USBDESC_HidDescriptor;
	hidInitStruct.setReportFunc = NULL;
	HIDKBD_Init(&hidInitStruct);

	/* Initialize and start USB device stack. */
	USBD_Init(&usbInitStruct);
}


/***************************************************************************//**
 * @brief
 *   Timeout function for keyboard scan timer.
 *   Scan keyboard to check for key press/release events.
 *   This function is called at a fixed rate.
 ******************************************************************************/
static void ScanTimeout(void)
{
	static int tCnt = 0;
	static int Counter = 0;

	// 50ms * 20 = 1s
	if (tCnt < 11)
	{
		HIDKBD_KeyboardEvent((HIDKBD_KeyReport_t *)&USBDESC_titleReportTable[tCnt]);
	}
	else if (tCnt == 11)
	{
		HIDKBD_KeyboardEvent((HIDKBD_KeyReport_t *)&USBDESC_numberReportTable[Counter]);
	}
	else if (tCnt == 12)
	{
		HIDKBD_KeyboardEvent((HIDKBD_KeyReport_t *)&USBDESC_saveReport);
	}
	else if (tCnt == 13)
	{
		Counter = (Counter + 1) % 10;
		HIDKBD_KeyboardEvent((HIDKBD_KeyReport_t *)&USBDESC_noKeyReport);
	}

	tCnt = (tCnt+1) % 100;  // 5s

	/* Restart keyboard scan timer */
	USBTIMER_Start(SCAN_TIMER, SCAN_RATE, ScanTimeout);
}

/***************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *   Starts keyboard scanning when device has been configured by USB host.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 ******************************************************************************/
static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState)
{
	/* Call HIDKBD drivers state change event handler. */
	HIDKBD_StateChangeEvent(oldState, newState);

	if ( newState == USBD_STATE_CONFIGURED ) {

		/* We have been configured, start scanning the keyboard ! */
		if ( oldState != USBD_STATE_SUSPENDED ) { /* Resume ?   */

		}

		USBTIMER_Start(SCAN_TIMER, SCAN_RATE, ScanTimeout);
	} else if ( (oldState == USBD_STATE_CONFIGURED)
				&& (newState != USBD_STATE_SUSPENDED)    ) {

		/* We have been de-configured, stop keyboard scanning. */
		USBTIMER_Stop(SCAN_TIMER);
	} else if ( newState == USBD_STATE_SUSPENDED ) {

		/* We have been suspended, stop keyboard scanning. */
		/* Reduce current consumption to below 2.5 mA.     */
		USBTIMER_Stop(SCAN_TIMER);
	}
}
