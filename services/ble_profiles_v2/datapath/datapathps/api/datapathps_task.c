/**
 ****************************************************************************************
 * @addtogroup DATAPATHPSTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#include "gapc_msg.h"

#if (BLE_DATAPATH_SERVER)
#include <string.h>
#include "gap.h"
#include "datapathps.h"
#include "datapathps_task.h"

#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
static int gapc_disconnect_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);
    uint8_t conidx = KE_IDX_GET(src_id);
    datapathps_env->isNotificationEnabled[conidx] = false;
    return KE_MSG_CONSUMED;
}

static void send_notifiction(uint8_t conidx, const uint8_t* ptrData, uint32_t length)
{
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);

    if (datapathps_env->isNotificationEnabled[conidx])
    {
        co_buf_t* p_buf = NULL;
        prf_buf_alloc(&p_buf, length);

        uint8_t* p_data = co_buf_data(p_buf);
        memcpy(p_data, ptrData, length);

        // Dummy parameter provided to GATT
        uint16_t dummy = 0;

        // Inform the GATT that notification must be sent
        gatt_srv_event_send(conidx, datapathps_env->user_lid, dummy, GATT_NOTIFY,
                            datapathps_env->shdl + DATAPATHPS_IDX_TX_VAL, p_buf);

        // Release the buffer
        co_buf_release(p_buf);
    }
}

__STATIC int send_data_via_notification_handler(ke_msg_id_t const msgid,
        struct ble_datapath_send_data_req_t const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    send_notifiction(param->connecionIndex, param->value, param->length);
    return (KE_MSG_CONSUMED);
}

static void send_indication(uint8_t conidx, const uint8_t* ptrData, uint32_t length)
{
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);

    if (datapathps_env->isNotificationEnabled[conidx]) {

        co_buf_t* p_buf = NULL;
        prf_buf_alloc(&p_buf, length);

        uint8_t* p_data = co_buf_data(p_buf);
        memcpy(p_data, ptrData, length);

        // Dummy parameter provided to GATT
        uint16_t dummy = 0;

        // Inform the GATT that notification must be sent
        gatt_srv_event_send(conidx, datapathps_env->user_lid, dummy, GATT_INDICATE,
                            datapathps_env->shdl + DATAPATHPS_IDX_TX_VAL, p_buf);

        // Release the buffer
        co_buf_release(p_buf);        
    }
}

__STATIC int send_data_via_indication_handler(ke_msg_id_t const msgid,
                                      struct ble_datapath_send_data_req_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    send_indication(param->connecionIndex, param->value, param->length);
    return (KE_MSG_CONSUMED);
}

__STATIC int send_data_via_write_command_handler(ke_msg_id_t const msgid,
                                      struct ble_datapath_send_data_req_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // TODO: update to use new API
    /*
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);

    prf_gatt_write(&(datapathps_env->prf_env), KE_IDX_GET(dest_id), datapathps_env->shdl + DATAPATHPS_IDX_RX_VAL,
                                   (uint8_t *)&param->value, param->length, GATTC_WRITE_NO_RESPONSE);

    // ke_state_set(dest_id, DATAPATHPS_BUSY);
    */
    return (KE_MSG_CONSUMED);
}

__STATIC int send_data_via_write_request_handler(ke_msg_id_t const msgid,
                                      struct ble_datapath_send_data_req_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // TODO: update to use new API
    /*
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);

    prf_gatt_write(&(datapathps_env->prf_env), KE_IDX_GET(dest_id), datapathps_env->shdl + DATAPATHPS_IDX_RX_VAL,
                                   (uint8_t *)&param->value, param->length, GATTC_WRITE);

    // ke_state_set(dest_id, DATAPATHPS_BUSY);
    */
    return (KE_MSG_CONSUMED);
}

__STATIC int control_notification_handler(ke_msg_id_t const msgid,
                                      struct ble_datapath_control_notification_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // TODO: update to use new API
    /*
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);
    uint16_t ccc_write_val = 0;
    if (param->isEnable)
    {
        ccc_write_val = 1;
    }

    prf_gatt_write(&(datapathps_env->prf_env), param->connecionIndex, datapathps_env->shdl + DATAPATHPS_IDX_TX_NTF_CFG,
                                   (uint8_t *)&ccc_write_val, sizeof(ccc_write_val), GATTC_WRITE);

    // ke_state_set(dest_id, DATAPATHPS_BUSY);
    */
    return (KE_MSG_CONSUMED);
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(datapathps)
{
    {GAPC_DISCONNECT_IND,                       (ke_msg_func_t) gapc_disconnect_ind_handler},
    {DATAPATHPS_SEND_DATA_VIA_NOTIFICATION,     (ke_msg_func_t) send_data_via_notification_handler},
    {DATAPATHPS_SEND_DATA_VIA_INDICATION,       (ke_msg_func_t) send_data_via_indication_handler},
    {DATAPATHPS_SEND_DATA_VIA_WRITE_COMMAND,    (ke_msg_func_t) send_data_via_write_command_handler},
    {DATAPATHPS_SEND_DATA_VIA_WRITE_REQUEST,    (ke_msg_func_t) send_data_via_write_request_handler},
    {DATAPATHPS_CONTROL_NOTIFICATION,           (ke_msg_func_t) control_notification_handler},

};

void datapathps_task_init(struct ke_task_desc *task_desc, void *p_env)
{
    // Get the address of the environment
    datapathps_env_t *datapathps_env = (datapathps_env_t *)p_env;

    task_desc->msg_handler_tab = datapathps_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(datapathps_msg_handler_tab);
    task_desc->state           = &(datapathps_env->state);
    task_desc->idx_max         = BLE_CONNECTION_MAX;
}

#endif /* #if (BLE_DATAPATH_SERVER) */

/// @} DATAPATHPSTASK
