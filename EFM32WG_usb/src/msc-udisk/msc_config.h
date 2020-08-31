#ifndef __MSC_CONFIG_H
#define __MSC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
**                                                                         **
** Specify total number of endpoints used (in addition to EP0).            **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 2

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 1

/****************************************************************************
**                                                                         **
** Misc. definitions for the interfaces of the composite device.           **
**                                                                         **
*****************************************************************************/

/* Define interface numbers */
#define MSD_INTERFACE_NO        (0)
#define NUM_INTERFACES          (1)

#define MSD_NUM_EP_USED         (2)   /* Number of EP's used by MSD function */

#define MSD_BULK_OUT      (0x01)   /* Endpoint for MSD data reception.       */
#define MSD_BULK_IN       (0x81)   /* Endpoint for MSD data transmission.    */


#ifdef __cplusplus
}
#endif

#endif /* __MSC_CONFIG_H */
