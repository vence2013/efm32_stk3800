#include "em_cmu.h"
#include "descriptors.h"
#include "cfg.h"
#include "PinNames.h"


#if (APP_USB_HID_KBD_ENABLE)

#define SCAN_TIMER              1       /* Timer used to scan keyboard. */
#define SCAN_RATE               50

static int      keySeqNo;           /* Current position in report table. */
static bool     keyPushed;          /* Current pushbutton status. */

/***************************************************************************//**
 * @brief
 *   Timeout function for keyboard scan timer.
 *   Scan keyboard to check for key press/release events.
 *   This function is called at a fixed rate.
 ******************************************************************************/
static void ScanTimeout(void)
{
	bool pushed;
	HIDKBD_KeyReport_t *report;

	/* Check pushbutton */
	pushed = GPIO_PinInGet(PB_Port, PB0_Pin) == 0;

	if (!keyPushed) {
		led_toggle( LED0 );
	}

	if ( pushed != keyPushed ) { /* Any change in keyboard status ? */
		if ( pushed ) {
			report = (void*)&USBDESC_reportTable[keySeqNo];
		} else {
			report = (void*)&USBDESC_noKeyReport;
		}

		/* Pass keyboard report on to the HID keyboard driver. */
		HIDKBD_KeyboardEvent(report);
	}

	/* Keep track of the new keypush event (if any) */
	if ( pushed && !keyPushed ) {
		/* Advance to next position in report table */
		keySeqNo++;
		if ( keySeqNo == (sizeof(USBDESC_reportTable) / sizeof(HIDKBD_KeyReport_t))) {
			keySeqNo = 0;
		}
		led_ctrl(LED0, LED0);
	}
	keyPushed = pushed;

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
			keySeqNo        = 0;
			keyPushed       = false;
			led_ctrl(LED0, LED0);
		}
		USBTIMER_Start(SCAN_TIMER, SCAN_RATE, ScanTimeout);
	} else if ( (oldState == USBD_STATE_CONFIGURED)
			 && (newState != USBD_STATE_SUSPENDED)    )
	{
		/* We have been de-configured, stop keyboard scanning. */
		USBTIMER_Stop(SCAN_TIMER);
		led_ctrl(LED0, 0);
	} else if ( newState == USBD_STATE_SUSPENDED ) {
		/* We have been suspended, stop keyboard scanning. */
		/* Reduce current consumption to below 2.5 mA.     */
		led_ctrl(LED0, 0);
		USBTIMER_Stop(SCAN_TIMER);
	}
}

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
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

void usb_hid_kbd_task( void )
{
	HIDKBD_Init_t hidInitStruct;

	led_setup();
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(PB_Port, PB0_Pin, gpioModeInputPull, 1);

	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	/* Initialize HID keyboard driver. */
	hidInitStruct.hidDescriptor = (void*)USBDESC_HidDescriptor;
	hidInitStruct.setReportFunc = NULL;
	HIDKBD_Init(&hidInitStruct);

	/* Initialize and start USB device stack. */
	USBD_Init(&usbInitStruct);
}

#endif
