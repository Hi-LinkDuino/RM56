/**
 ****************************************************************************************
 * @addtogroup DPPS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_DATAPATH_SERVER)
#include <string.h>
#include "gap.h"
#include "datapathps.h"
#include "datapathps_task.h"
#include "prf_utils.h"
#include "gatt_msg.h"
#include "ke_mem.h"
#include "co_utils.h"
#include "gatt.h"
#include "prf.h"

/*
 * DATAPATH SRVER PROFILE ATTRIBUTES
 ****************************************************************************************
 */

#define USE_128BIT_UUID 1

#if USE_128BIT_UUID

#ifdef IS_USE_CUSTOM_BLE_DATAPATH_PROFILE_UUID_ENABLED
// Service UUID
#define datapath_service_uuid_128_content       TW_BLE_DATAPATH_SERVICE_UUID
// Characteristic UUID(Notify)
#define datapath_tx_char_val_uuid_128_content   TW_BLE_DATAPATH_TX_CHAR_VAL_UUID
// Characteristic UUID(RW)
#define datapath_rx_char_val_uuid_128_content   TW_BLE_DATAPATH_RX_CHAR_VAL_UUID
#else
#define datapath_service_uuid_128_content       {0x12, 0x34, 0x56, 0x78, 0x90, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01 }
#define datapath_tx_char_val_uuid_128_content   {0x12, 0x34, 0x56, 0x78, 0x91, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02 }
#define datapath_rx_char_val_uuid_128_content   {0x12, 0x34, 0x56, 0x78, 0x92, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03 }
#endif

#define GATT_DECL_PRIMARY_SERVICE       { 0x00, 0x28 }
#define GATT_DECL_CHARACTERISTIC_UUID        { 0x03, 0x28 }
#define GATT_DESC_CLIENT_CHAR_CFG_UUID       { 0x02, 0x29 }
#define GATT_DESC_CHAR_USER_DESCRIPTION_UUID { 0x01, 0x29 }


static const uint8_t DATAPATH_SERVICE_UUID_128[GATT_UUID_128_LEN] = datapath_service_uuid_128_content;

/// Full DATAPATH SERVER Database Description - Used to add attributes into the database
const struct gatt_att_desc datapathps_att_db[DATAPATHPS_IDX_NB] =
{
    // Service Declaration
    [DATAPATHPS_IDX_SVC]        =   {GATT_DECL_PRIMARY_SERVICE, PROP(RD), 0},

    // TX Characteristic Declaration
    [DATAPATHPS_IDX_TX_CHAR]    =   {GATT_DECL_CHARACTERISTIC_UUID, PROP(RD), 0},
    // TX Characteristic Value
    [DATAPATHPS_IDX_TX_VAL]     =   {datapath_tx_char_val_uuid_128_content, PROP(N) | PROP(RD) | ATT_UUID(128), DATAPATHPS_MAX_LEN},

    // TX Characteristic - Client Characteristic Configuration Descriptor
#if defined(CTKD_ENABLE) && !defined(BLE_AUDIO_STARLORD_COMPATIBLE_SUPPORT)
    // Let app write ccc to trigger secure connection pairing
    [DATAPATHPS_IDX_TX_NTF_CFG] =   {GATT_DESC_CLIENT_CHAR_CFG_UUID, PROP(RD) | PROP(WR) | SEC_LVL(WP, AUTH), PRF_SVC_DESC_CLI_CFG_LEN},
#else
    [DATAPATHPS_IDX_TX_NTF_CFG] =   {GATT_DESC_CLIENT_CHAR_CFG_UUID, PROP(RD) | PROP(WR), PRF_SVC_DESC_CLI_CFG_LEN},
#endif
    // TX Characteristic - Characteristic User Description Descriptor
    [DATAPATHPS_IDX_TX_DESC]    =   {GATT_DESC_CHAR_USER_DESCRIPTION_UUID, PROP(RD), 32},

    // RX Characteristic Declaration
    [DATAPATHPS_IDX_RX_CHAR]    =   {GATT_DECL_CHARACTERISTIC_UUID, PROP(RD), 0},
    // RX Characteristic Value
    [DATAPATHPS_IDX_RX_VAL]     =   {datapath_rx_char_val_uuid_128_content, PROP(RD) | PROP(WR) | PROP(WC) | ATT_UUID(128), DATAPATHPS_MAX_LEN},
    // RX Characteristic - Characteristic User Description Descriptor
    [DATAPATHPS_IDX_RX_DESC]    =   {GATT_DESC_CHAR_USER_DESCRIPTION_UUID, PROP(RD), 32},
};
#else

#define DATAPATHPS_SERVICE_UUID_16BIT       0xFEF8
#define DATAPATHPS_TX_CHAR_VAL_UUID_16BIT   0xFEF9
#define DATAPATHPS_RX_CHAR_VAL_UUID_16BIT   0xFEFA

const struct gatt_att16_desc datapathps_att_db[DATAPATHPS_IDX_NB] =
{
    // Service Declaration
    [DATAPATHPS_IDX_SVC]        =   {GATT_DECL_PRIMARY_SERVICE, PROP(RD), 0},

    // TX Characteristic Declaration
    [DATAPATHPS_IDX_TX_CHAR]    =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
    // TX Characteristic Value
    [DATAPATHPS_IDX_TX_VAL]     =   {DATAPATHPS_TX_CHAR_VAL_UUID_16BIT, PROP(N), DATAPATHPS_MAX_LEN},
    // TX Characteristic - Client Characteristic Configuration Descriptor
#if defined CTKD_ENABLE && !defined(BLE_AUDIO_STARLORD_COMPATIBLE_SUPPORT)
    // Let app write ccc to trigger secure connection pairing
    [DATAPATHPS_IDX_TX_NTF_CFG] =   {GATT_DESC_CLIENT_CHAR_CFG, PROP(RD) | PROP(WR) | SEC_LVL(WP, AUTH), PRF_SVC_DESC_CLI_CFG_LEN},
#else
    [DATAPATHPS_IDX_TX_NTF_CFG] =   {GATT_DESC_CLIENT_CHAR_CFG, PROP(RD) | PROP(WR), PRF_SVC_DESC_CLI_CFG_LEN},
#endif
    // TX Characteristic - Characteristic User Description Descriptor
    [DATAPATHPS_IDX_TX_DESC]    =   {GATT_DESC_CHAR_USER_DESCRIPTION, PROP(RD), 32},

    // RX Characteristic Declaration
    [DATAPATHPS_IDX_RX_CHAR]    =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
    // RX Characteristic Value
    [DATAPATHPS_IDX_RX_VAL]     =   {DATAPATHPS_RX_CHAR_VAL_UUID_16BIT, PROP(WR) | PROP(WC), DATAPATHPS_MAX_LEN},
    // RX Characteristic - Characteristic User Description Descriptor
    [DATAPATHPS_IDX_RX_DESC]    =   {GATT_DESC_CHAR_USER_DESCRIPTION, PROP(RD), 32},
};

#endif

__STATIC void datapath_gatt_cb_event_sent(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status)
{
    // notification has been sent out
    struct ble_datapath_tx_sent_ind_t *ind = KE_MSG_ALLOC(DATAPATHPS_TX_DATA_SENT,
                                                          PRF_DST_TASK(DATAPATHPS),
                                                          PRF_SRC_TASK(DATAPATHPS),
                                                          ble_datapath_tx_sent_ind_t);

    ind->status = status;

    ke_msg_send(ind);
}

__STATIC void datapath_gatt_cb_att_read_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl, uint16_t offset,
                             uint16_t max_length)
{
    co_buf_t* p_data = NULL;
    uint16_t dataLen = 0;
    uint16_t status = GAP_ERR_NO_ERROR;

    TRACE(1, "read hdl %d", hdl);

    // Get the address of the environment
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);

    if (hdl == (datapathps_env->shdl + DATAPATHPS_IDX_TX_DESC)) {
        dataLen = sizeof(custom_tx_desc);
        prf_buf_alloc(&p_data, dataLen);
        memcpy(co_buf_data(p_data), custom_tx_desc, dataLen);
    } else if (hdl == (datapathps_env->shdl + DATAPATHPS_IDX_RX_DESC)) {
        dataLen = sizeof(custom_rx_desc);
        prf_buf_alloc(&p_data, dataLen);
        memcpy(co_buf_data(p_data), custom_rx_desc, dataLen);
    } else if (hdl == (datapathps_env->shdl + DATAPATHPS_IDX_TX_NTF_CFG)) {
        uint16_t notify_ccc;
        if (datapathps_env->isNotificationEnabled[conidx]) {
            notify_ccc = 1;
        } else {
            notify_ccc = 0;
        }
        dataLen = sizeof(notify_ccc);
        prf_buf_alloc(&p_data, dataLen);
        memcpy(co_buf_data(p_data), (uint8_t *)&notify_ccc, dataLen);
    }
    else {
        dataLen = 0;
        status = ATT_ERR_REQUEST_NOT_SUPPORTED;
    }    

    gatt_srv_att_read_get_cfm(conidx, user_lid, token, status, dataLen, p_data);

    // Release the buffer
    co_buf_release(p_data);
}

__STATIC void datapath_gatt_cb_att_event_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t dummy, uint16_t hdl,
                              uint16_t max_length)
{

}

__STATIC void datapath_gatt_cb_att_info_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl)
{
    uint16_t length = 0;
    uint16_t status = GAP_ERR_NO_ERROR;
    
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);

    // check if it's a client configuration char
    if (hdl == datapathps_env->shdl + DATAPATHPS_IDX_TX_NTF_CFG)
    {
        // CCC attribute length = 2
        length = 2;
        status = GAP_ERR_NO_ERROR;
    }
    else if (hdl == datapathps_env->shdl + DATAPATHPS_IDX_RX_VAL)
    {
        // force length to zero to reject any write starting from something != 0
        length = 0;
        status = GAP_ERR_NO_ERROR;
    }
    // not expected request
    else
    {
        length = 0;
        status = ATT_ERR_WRITE_NOT_PERMITTED;
    }

    TRACE(1, "func %s status %d hdl %d", __FUNCTION__, status, hdl);

    // Send the confirmation
    gatt_srv_att_info_get_cfm(conidx, user_lid, token, status, length);
}

__STATIC void datapath_gatt_cb_att_set(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl,
                                          uint16_t offset, co_buf_t* p_buf)
{
    // Get the address of the environment
    datapathps_env_t *datapathps_env = PRF_ENV_GET(DATAPATHPS, datapathps);

    uint8_t status = GAP_ERR_NO_ERROR;

    TRACE(4, "%s datapathps_env 0x%x write handle %d shdl %d", 
        __FUNCTION__, (uint32_t)datapathps_env, hdl, datapathps_env->shdl);

    uint8_t* pData = co_buf_data(p_buf);
    uint16_t dataLen = p_buf->data_len;

    if (datapathps_env != NULL)
    {
        // TX ccc
        if (hdl == (datapathps_env->shdl + DATAPATHPS_IDX_TX_NTF_CFG))
        {
            uint16_t value = 0x0000;

            //Extract value before check
            memcpy(&value, pData, sizeof(uint16_t));

            if (value == PRF_CLI_STOP_NTFIND) {
                datapathps_env->isNotificationEnabled[conidx] = false;
            } else if (value == PRF_CLI_START_NTF) {
                datapathps_env->isNotificationEnabled[conidx] = true;
            } else {
                status = PRF_APP_ERROR;
            }

            if (status == GAP_ERR_NO_ERROR) {
                //Inform APP of TX ccc change
                struct ble_datapath_tx_notif_config_t *ind =
                    KE_MSG_ALLOC(DATAPATHPS_TX_CCC_CHANGED,
                                 PRF_DST_TASK(DATAPATHPS),
                                 PRF_SRC_TASK(DATAPATHPS),
                                 ble_datapath_tx_notif_config_t);

                ind->isNotificationEnabled = datapathps_env->isNotificationEnabled[conidx];

                ke_msg_send(ind);
            }
        }
        // RX data
        else if (hdl == (datapathps_env->shdl + DATAPATHPS_IDX_RX_VAL))
        {
            //inform APP of data
            struct ble_datapath_rx_data_ind_t *ind = KE_MSG_ALLOC_DYN(DATAPATHPS_RX_DATA_RECEIVED,
                                                                      PRF_DST_TASK(DATAPATHPS),
                                                                      PRF_SRC_TASK(DATAPATHPS),
                                                                      ble_datapath_rx_data_ind_t,
                                                                      dataLen);

            ind->length = dataLen;
            memcpy((uint8_t *)(ind->data), pData, dataLen);

            ke_msg_send(ind);
        }
        else
        {
            status = PRF_APP_ERROR;
        }
    }

    // Inform GATT about handling
    gatt_srv_att_val_set_cfm(conidx, user_lid, token, status);
}


/// Set of callbacks functions for communication with GATT as a GATT User Client
__STATIC const gatt_srv_cb_t datapath_gatt_cb = {
    .cb_event_sent = datapath_gatt_cb_event_sent,
    .cb_att_read_get = datapath_gatt_cb_att_read_get,
    .cb_att_event_get = datapath_gatt_cb_att_event_get,
    .cb_att_info_get = datapath_gatt_cb_att_info_get,
    .cb_att_val_set = datapath_gatt_cb_att_set,
};


/**
 ****************************************************************************************
 * @brief Initialization of the DATAPATHPS module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Collector or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @see enum attm_value_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint16_t _init_cb(prf_data_t* env, uint16_t* start_hdl, 
    uint8_t sec_lvl, uint8_t user_prio, const void* params, const void* p_cb)
{
    uint16_t status = GAP_ERR_NO_ERROR;

    BLE_GATT_DBG("attm_svc_create_db_128 returns %d start handle is %d", status, *start_hdl);

    // Allocate DATAPATHPS required environment variable
    datapathps_env_t* datapathps_env =
            (datapathps_env_t* ) ke_malloc(sizeof(datapathps_env_t), KE_MEM_ATT_DB);

    memset((uint8_t *)datapathps_env, 0, sizeof(datapathps_env_t));
    // Initialize DATAPATHPS environment
    env->p_env           = (prf_hdr_t*) datapathps_env;

    // Register as GATT User Client
    status = gatt_user_srv_register(PREFERRED_BLE_MTU, 0, &datapath_gatt_cb,
                                    &(datapathps_env->user_lid));
    
    if (GAP_ERR_NO_ERROR == status)
    {

        datapathps_env->shdl = *start_hdl;
        
#if USE_128BIT_UUID
        status = gatt_db_svc_add(datapathps_env->user_lid, SVC_SEC_LVL(NOT_ENC) | SVC_UUID(128), DATAPATH_SERVICE_UUID_128,
                                   DATAPATHPS_IDX_NB, NULL, datapathps_att_db, DATAPATHPS_IDX_NB,
                                   &datapathps_env->shdl);
#else
        status = gatt_db_svc16_add(datapathps_env->user_lid, SVC_SEC_LVL(NOT_ENC), DATAPATHPS_SERVICE_UUID_16BIT,
                                   DATAPATHPS_IDX_NB, NULL, datapathps_att_db, DATAPATHPS_IDX_NB,
                                   &datapathps_env->shdl);
#endif
        if (GAP_ERR_NO_ERROR == status)
        {
            // initialize environment variable
            env->api_id                     = TASK_ID_DATAPATHPS;
            datapathps_task_init(&(env->desc), (void *)datapathps_env);

            /* Put HRS in Idle state */
            // ke_state_set(env->prf_task, DATAPATHPS_IDLE);
        }
    }

    TRACE(1, "DATAPATH uid %d shdl %d", datapathps_env->user_lid, datapathps_env->shdl);
    
    return (status);
}

/**
 ****************************************************************************************
 * @brief Destruction of the DATAPATHPS module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static uint16_t _destroy_cb(prf_data_t* p_env, uint8_t reason)
{
    datapathps_env_t* datapathps_env = (datapathps_env_t*) p_env->p_env;

    // free profile environment variables
    p_env->p_env = NULL;
    ke_free(datapathps_env);

    return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void _con_create_cb(prf_data_t* p_env, uint8_t conidx, bool is_le_con)
{
    //datapathps_env_t* datapathps_env = (datapathps_env_t*) env->env;
    //struct prf_svc datapathps_svc = {datapathps_env->shdl, datapathps_env->shdl + DATAPATHPS_IDX_NB};
    //prf_register_atthdl2gatt(env->p_env, conidx, &datapathps_svc);
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void _con_cleanup_cb(prf_data_t* env, uint8_t conidx, uint16_t reason)
{
    /* Nothing to do */
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// DATAPATHPS Task interface required by profile manager
const struct prf_task_cbs datapathps_itf =
{
    _init_cb,
    _destroy_cb,
    _con_create_cb,
    _con_cleanup_cb,
};

/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* datapathps_prf_itf_get(void)
{
   return &datapathps_itf;
}


#endif /* BLE_DATAPATH_SERVER */

/// @} DATAPATHPS
