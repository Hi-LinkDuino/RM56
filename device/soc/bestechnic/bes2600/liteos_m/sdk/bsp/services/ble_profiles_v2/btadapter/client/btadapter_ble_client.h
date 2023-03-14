#ifndef __BT_ADAPTER_CLIENT_H__
#define __BT_ADAPTER_CLIENT_H__
#include "rwip_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "ke_task.h"
#include "prf_utils.h"
#include "bleadapter_def.h"
#include "app_ble_mode_switch.h"


/// Possible states of the btadater service task
enum
{
    /// Idle state
    BTADAPTER_CLIENT_IDLE,
    /// Connected state
    BTADAPTER_CLIENT_BUSY,

    /// Number of defined states.
    BTADAPTER_CLIENT_STATE_MAX,
};

struct btadapter_client_cnx_env
{
    // Current Operation
    void *operation;

    // Provide an indication about the last operation
    uint16_t last_req;

    // Last characteristic code discovered
    uint8_t last_char_code;

    // Counter used to check service uniqueness
    uint8_t nb_svc;
};

// Profile client environment variable
typedef struct btadapter_client_env_tag
{
    // profile environment
    prf_hdr_t 	prf_env;
    // Service Start Handle
    uint16_t    shdl;

    // Environment variable pointer for each connections
    struct btadapter_client_cnx_env* env[BLE_CONNECTION_MAX];

    // State of different task instances
    ke_state_t state[BLE_CONNECTION_MAX];

    /// GATT User local index
    uint8_t user_lid;
}btadapter_client_env_t;


const gatt_cli_cb_t * btadapter_ble_client_get_cbs(void);
void btadapter_ble_client_task_init(struct ke_task_desc *task_desc, void* p_env);


#endif