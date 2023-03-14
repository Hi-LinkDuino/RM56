#ifndef _DATACOMM_TASK_H_
#define _DATACOMM_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup DATACOMMTASK Task
 * @ingroup DATACOMM
 * @brief Heart Rate Profile Task.
 *
 * The DATACOMMTASK is responsible for handling the messages coming in and out of the
 * @ref DATACOMM collector block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include "rwip_task.h" // Task definitions

/*
 * DEFINES
 ****************************************************************************************
 */
/// Messages for Data Path Server Profile 
enum datacomm_msg_id
{
    DATACOMM_TX_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_DATACOMM),

    DATACOMM_TX_DATA_SENT,
    
    DATACOMM_RX_DATA_RECEIVED,

    DATACOMM_NOTIFICATION_RECEIVED,

    DATACOMM_SEND_DATA_VIA_NOTIFICATION,

    DATACOMM_SEND_DATA_VIA_INDICATION,

    DATACOMM_SEND_DATA_VIA_WRITE_COMMAND,

    DATACOMM_SEND_DATA_VIA_WRITE_REQUEST,

    DATACOMM_CONTROL_NOTIFICATION,

    DATACOMM_PROBE_CCC_CHANGED,
    DATACOMM_PROBE_VALUE_RX,
    DATACOMM_PROBE_VALUE_TX,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct ble_datacomm_tx_notif_config_t
{
    bool         isNotificationEnabled;
};

struct ble_datacomm_rx_data_ind_t
{
    uint16_t    length;
    uint8_t        data[0];
};

struct ble_datacomm_tx_sent_ind_t
{
    uint8_t     status;
};

struct ble_datacomm_send_data_req_t
{
    uint8_t     connecionIndex;
    uint32_t     length;
    uint8_t      value[__ARRAY_EMPTY];
};

struct ble_datacomm_control_notification_t
{
    bool         isEnable;
    uint8_t     connecionIndex;
};

struct ble_datacomm_probe_notif_config_t
{
    bool         isProbeNotificateEnabled;
};

struct ble_datacomm_probe_value_ind_t
{
    uint8_t         connecionIndex;
    uint8_t         probe_enable;
};

/// @} DATACOMMTASK

#endif /* _DATACOMM_TASK_H_ */
