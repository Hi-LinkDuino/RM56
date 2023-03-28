#ifndef __BT_ADAPTER_SERVICE_BLE_H__
#define __BT_ADAPTER_SERVICE_BLE_H__
#include "rwip_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "ke_task.h"
#include "prf_utils.h"

/// Possible states of the btadater service task
enum
{
    /// Idle state
    BTADAPTER_SERVICE_IDLE,
    /// Connected state
    BTADAPTER_SERVICE_BUSY,

    /// Number of defined states.
    BTADAPTER_SERVICE_STATE_MAX,
};

typedef struct btadaper_service_client_env_tag
{
    uint8_t     con_index;
    uint8_t     isReady;
    uint16_t    token;
    /// flag to mark whether notification is enabled
    uint8_t     isNotificationEnabled;
}btadaper_service_client_env_t;

/// Profile Server environment variable
typedef struct btadapter_service_env_tag
{
    /// profile environment
    prf_hdr_t 	prf_env;
    /// Service Start Handle
    uint16_t    shdl;
    int32_t      server_if;
    uint8_t      nattb;
    /// State of different task instances
    ke_state_t  state;
    /// GATT User local index
    uint8_t user_lid;
    btadaper_service_client_env_t client_env[BLE_CONNECTION_MAX];
}btadapter_service_env_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve HRP service profile interface
 *
 * @return HRP service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* btadapter_ble_service_prf_itf_get(void);

const gatt_srv_cb_t * btadapter_ble_server_get_cbs(void);

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * Initialize task handler
 *
 * @param task_desc Task descriptor to fill
 ****************************************************************************************
 */
void btadapter_ble_service_task_init(struct ke_task_desc *task_desc, btadapter_service_env_t *service_env);
#endif
