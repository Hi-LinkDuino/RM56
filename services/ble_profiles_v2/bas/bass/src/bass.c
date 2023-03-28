/**
 ****************************************************************************************
 *
 * @file bass.c
 *
 * @brief Battery Server Implementation.
 *
 * Copyright (C) RivieraWaves 2009-2019
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup BASS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BATT_SERVER)
#include "bass.h"
#include "gatt.h"

#include "prf_utils.h"
#include "prf.h"

#include "co_utils.h"
#include "co_endian.h"
#include "co_math.h"

#include "ke_mem.h"
#include <string.h>

/*
 * DEFINES
 ****************************************************************************************
 */
///Maximum number of Battery Server task instances

#define BAS_CFG_FLAG_MANDATORY_MASK       (0x07)
#define BAS_CFG_FLAG_NTF_SUP_MASK         (0x08)
#define BAS_CFG_FLAG_MTP_BAS_MASK         (0x10)

#define BASS_FLAG_NTF_CFG_BIT             (0x02)

/// Battery Service Attributes Indexes
enum
{
    BAS_IDX_SVC,

    BAS_IDX_BATT_LVL_CHAR,
    BAS_IDX_BATT_LVL_VAL,
    BAS_IDX_BATT_LVL_NTF_CFG,
    BAS_IDX_BATT_LVL_PRES_FMT,

    BAS_IDX_NB,
};

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

/// Value element
struct bass_val_elmt
{
    /// list element header
    struct co_list_hdr hdr;
    /// value identifier
    uint8_t value;
    /// value length
    uint8_t length;
    /// value data
    uint8_t data[__ARRAY_EMPTY];
};

/// Battery service server environment variable
typedef struct bass_env
{
    /// profile environment
    prf_hdr_t           prf_env;
    /// Operation Event TX wait queue
    co_list_t           wait_queue;
    /// Battery Level Characteristic Presentation Format - Should not change during connection
    prf_char_pres_fmt_t batt_level_pres_format[BASS_NB_BAS_INSTANCES_MAX];
    /// Notification configuration of peer devices.
    uint8_t             ntf_cfg[BLE_CONNECTION_MAX];
    /// Service Attribute Start Handle
    uint16_t            start_hdl;
    /// Level of the battery
    uint8_t             batt_lvl[BASS_NB_BAS_INSTANCES_MAX];
    /// Number of BAS
    uint8_t             nb_svc;
    /// Services features
    uint16_t            features;
    /// GATT user local identifier
    uint8_t             user_lid;
    /// Operation On-going
    bool                op_ongoing;
    /// Prevent recursion in execute_operation function
    bool                in_exe_op;

} bass_env_t;

/// ongoing operation information
typedef struct bass_buf_meta
{
     /// Handle of the attribute to indicate/notify
     uint16_t handle;
     /// used to know on which device interval update has been requested, and to prevent
     /// indication to be triggered on this connection index
     uint8_t  conidx;
     /// Service index
     uint8_t  svc_idx;
} bass_buf_meta_t;



/*
 * ATTRIBUTES DATABASE
 ****************************************************************************************
 */

/// Full Database Description - Used to add attributes into the database
const gatt_att16_desc_t bass_att_db[BAS_IDX_NB] =
{
    // Battery Service Declaration
    [BAS_IDX_SVC]               = { GATT_DECL_PRIMARY_SERVICE,  PROP(RD),          0              },
    // Battery Level Characteristic Declaration
    [BAS_IDX_BATT_LVL_CHAR]     = { GATT_DECL_CHARACTERISTIC,   PROP(RD),          0              },
    // Battery Level Characteristic Value
    [BAS_IDX_BATT_LVL_VAL]      = { GATT_CHAR_BATTERY_LEVEL,    PROP(RD),          OPT(NO_OFFSET) },
    // Battery Level Characteristic - Client Characteristic Configuration Descriptor
    [BAS_IDX_BATT_LVL_NTF_CFG]  = { GATT_DESC_CLIENT_CHAR_CFG,  PROP(RD)|PROP(WR), OPT(NO_OFFSET) },
    // Battery Level Characteristic - Characteristic Presentation Format Descriptor
    [BAS_IDX_BATT_LVL_PRES_FMT] = { GATT_DESC_CHAR_PRES_FORMAT, PROP(RD),          OPT(NO_OFFSET) },
};

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve Attribute handle from service and attribute index
 *
 * @param[in] svc_idx BAS Service index
 * @param[in] att_idx Attribute index
 *
 * @return BAS attribute handle or INVALID HANDLE if nothing found
 ****************************************************************************************
 */
__STATIC uint16_t bass_get_att_handle(uint8_t svc_idx, uint8_t att_idx)
{
    bass_env_t *p_bass_env = PRF_ENV_GET(BASS, bass);
    uint16_t    handle = GATT_INVALID_HDL;

    if (svc_idx < p_bass_env ->nb_svc)
    {
        // full service size is reserved indatabase
        handle = p_bass_env->start_hdl + (BAS_IDX_NB * svc_idx) + att_idx;

        // sanity check
        if(   ((att_idx == BAS_IDX_BATT_LVL_NTF_CFG) && (((p_bass_env->features >> svc_idx) & 0x01) != BAS_BATT_LVL_NTF_SUP))
           || ((att_idx == BAS_IDX_BATT_LVL_PRES_FMT) && (p_bass_env->nb_svc < 2)))
        {
            handle = GATT_INVALID_HDL;
        }
        // update handle if battery level notification not present
        else if ((att_idx > BAS_IDX_BATT_LVL_NTF_CFG) && (((p_bass_env->features >> svc_idx) & 0x01) != BAS_BATT_LVL_NTF_SUP))
        {
            handle -= 1;
        }
    }

    return handle;
}

/**
 ****************************************************************************************
 * @brief Retrieve Service and attribute index form attribute handle
 *
 * @param[out] handle     Attribute handle
 * @param[out] p_svc_idx  BAS Service index
 * @param[out] p_att_idx  Attribute index
 *
 * @return Success if attribute and service index found, else Application error
 ****************************************************************************************
 */
__STATIC uint16_t bass_get_att_idx(uint16_t handle, uint8_t *p_svc_idx, uint8_t *p_att_idx)
{
    bass_env_t *p_bass_env = PRF_ENV_GET(BASS, bass);
    uint16_t hdl_cursor = p_bass_env->start_hdl;
    uint16_t status = PRF_APP_ERROR;

    // Browse list of services
    // handle must be greater than current index
    for (*p_svc_idx = 0; (*p_svc_idx < p_bass_env->nb_svc) && (handle >= hdl_cursor); (*p_svc_idx)++)
    {
        // check if handle is within service range
        if (handle <= (hdl_cursor + BAS_IDX_NB))
        {
            *p_att_idx = handle - hdl_cursor;

            // check if notification are present
            if ((*p_att_idx >= BAS_IDX_BATT_LVL_NTF_CFG) && ((p_bass_env->features >> *p_svc_idx) & 0x01) != BAS_BATT_LVL_NTF_SUP)
            {
                *p_att_idx += 1;
            }

            // If Battery level presentation format but should be not present, there is an error
            if((*p_att_idx == BAS_IDX_BATT_LVL_PRES_FMT) && (p_bass_env->nb_svc == 1))
            {
                break;
            }

            // search succeed
            status = GAP_ERR_NO_ERROR;
            break;
        }

        hdl_cursor += BAS_IDX_NB;
    }

    return (status);
}

/**
 ****************************************************************************************
 * @brief  This function fully manage notification Battery service
 *         to peer(s) device(s) according to on-going operation requested by application:
 ****************************************************************************************
 */
__STATIC void bass_exe_operation(void)
{
    bass_env_t* p_bass_env = PRF_ENV_GET(BASS, bass);

    if((p_bass_env != NULL) && (!p_bass_env->in_exe_op))
    {
        p_bass_env->in_exe_op = true;

        while(!co_list_is_empty(&(p_bass_env->wait_queue)) && !(p_bass_env->op_ongoing))
        {
            uint16_t status = GAP_ERR_NO_ERROR;
            co_buf_t* p_buf = (co_buf_t*) co_list_pop_front(&(p_bass_env->wait_queue));
            bass_buf_meta_t* p_buf_meta = (bass_buf_meta_t*) co_buf_metadata(p_buf);
            uint32_t conidx_bf = 0;


            // check if a connection index must be ignored
            if(p_buf_meta->conidx != GAP_INVALID_CONIDX)
            {
                conidx_bf = CO_BIT(p_buf_meta->conidx);
            }
            else
            {
                uint8_t  conidx;
                // prepare bit field of connection where event must be triggered
                for(conidx = 0 ; conidx < BLE_CONNECTION_MAX ; conidx++)
                {
                    if((p_bass_env->ntf_cfg[conidx] & (1 << p_buf_meta->svc_idx)) != 0)
                    {
                        conidx_bf |= CO_BIT(conidx);
                    }
                }
            }

            // Send Notification / Indication
            if(conidx_bf != 0)
            {
                // Send Multi point event
                status = gatt_srv_event_mtp_send(conidx_bf, p_bass_env->user_lid,
                                                 (p_buf_meta->conidx == GAP_INVALID_CONIDX), GATT_NOTIFY,
                                                 p_buf_meta->handle, p_buf, true);
                if(status == GAP_ERR_NO_ERROR)
                {
                    p_bass_env->op_ongoing = true;
                }
            }

            // Consider job done
            if ((!p_bass_env->op_ongoing) && (p_buf_meta->conidx != 0))
            {
                const bass_cb_t* p_cb = (const bass_cb_t*) p_bass_env->prf_env.p_cb;
                // Inform application that event has been sent
                p_cb->cb_batt_level_upd_cmp(status);
            }

            // release buffer
            co_buf_release(p_buf);
        }

        p_bass_env->in_exe_op = false;
    }
}


/**
 ****************************************************************************************
 * @brief  Trigger battery level notification
 *
 * @param p_bass_env profile environment
 * @param conidx     peer destination connection index
 * @param svc_idx    Service index
 ****************************************************************************************
 */
__STATIC uint16_t bass_notify_batt_lvl(bass_env_t *p_bass_env, uint8_t conidx, uint8_t svc_idx)
{
    co_buf_t* p_buf = NULL;
    uint16_t status = co_buf_alloc(&p_buf, GATT_BUFFER_HEADER_LEN, 1, GATT_BUFFER_TAIL_LEN);

    if(status == CO_BUF_ERR_NO_ERROR)
    {
        bass_buf_meta_t* p_buf_meta = (bass_buf_meta_t*) co_buf_metadata(p_buf);
        p_buf_meta->conidx          = GAP_INVALID_CONIDX;
        p_buf_meta->svc_idx         = svc_idx;
        p_buf_meta->handle          = bass_get_att_handle(svc_idx, BAS_IDX_BATT_LVL_VAL);
        co_buf_data(p_buf)[0]       = p_bass_env->batt_lvl[svc_idx];

        // put event on wait queue
        co_list_push_back(&(p_bass_env->wait_queue), &(p_buf->hdr));
        // execute operation
        bass_exe_operation();
    }
    else
    {
        status = GAP_ERR_INSUFF_RESOURCES;
    }
    return (status);
}

/*
 * GATT USER SERVICE HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief This function is called when peer want to read local attribute database value.
 *
 *        #gatt_srv_att_read_get_cfm shall be called to provide attribute value
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] token         Procedure token that must be returned in confirmation function
 * @param[in] hdl           Attribute handle
 * @param[in] offset        Data offset
 * @param[in] max_length    Maximum data length to return
 ****************************************************************************************
 */
__STATIC void bass_cb_att_read_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl, uint16_t offset,
                                   uint16_t max_length)
{
    bass_env_t* p_bass_env = PRF_ENV_GET(BASS, bass);
    co_buf_t*   p_buf      = NULL;
    uint16_t    status     = PRF_APP_ERROR;

    if(co_buf_alloc(&p_buf, GATT_BUFFER_HEADER_LEN, 0, PRF_CHAR_PRES_FMT_SIZE + GATT_BUFFER_TAIL_LEN) != GAP_ERR_NO_ERROR)
    {
        status = ATT_ERR_INSUFF_RESOURCE;
    }
    else
    {
        if(p_bass_env != NULL)
        {
            uint8_t svc_idx = 0, att_idx = 0;
            status = bass_get_att_idx(hdl, &svc_idx, &att_idx);
            if(status == GAP_ERR_NO_ERROR)
            {
                switch(att_idx)
                {
                    case BAS_IDX_BATT_LVL_VAL:
                    {
                        co_buf_data(p_buf)[0] = p_bass_env->batt_lvl[svc_idx];
                        co_buf_tail_reserve(p_buf, 1);
                    } break;
                    case BAS_IDX_BATT_LVL_NTF_CFG:
                    {
                        uint16_t ntf_cfg = (p_bass_env->ntf_cfg[conidx] >> svc_idx & BAS_BATT_LVL_NTF_SUP)
                                         ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND;
                        co_write16p(co_buf_data(p_buf), co_htobs(ntf_cfg));
                        co_buf_tail_reserve(p_buf, 2);
                    } break;
                    case BAS_IDX_BATT_LVL_PRES_FMT:
                    {
                        prf_pack_char_pres_fmt(p_buf, &(p_bass_env->batt_level_pres_format[svc_idx]));
                    } break;
                    default: { status = PRF_APP_ERROR; } break;
                }
            }
        }
    }

    // Send result to peer device
    gatt_srv_att_read_get_cfm(conidx, user_lid, token, status, co_buf_data_len(p_buf), p_buf);
    if(p_buf != NULL)
    {
        co_buf_release(p_buf);
    }
}

/**
 ****************************************************************************************
 * @brief This function is called during a write procedure to modify attribute handle.
 *
 *        #gatt_srv_att_val_set_cfm shall be called to accept or reject attribute
 *        update.
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] token         Procedure token that must be returned in confirmation function
 * @param[in] hdl           Attribute handle
 * @param[in] offset        Value offset
 * @param[in] p_data        Pointer to buffer that contains data to write starting from offset
 ****************************************************************************************
 */
__STATIC void bass_cb_att_val_set(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl, uint16_t offset,
                                  co_buf_t* p_data)
{
    bass_env_t *p_bass_env = PRF_ENV_GET(BASS, bass);
    uint16_t status = PRF_APP_ERROR;

    if(p_bass_env != NULL)
    {
        if (co_buf_data_len(p_data) != sizeof(uint16_t))
        {
            status = PRF_ERR_UNEXPECTED_LEN;
        }
        else
        {
            uint8_t svc_idx = 0, att_idx = 0;
            status = bass_get_att_idx(hdl, &svc_idx, &att_idx);

            if(status == GAP_ERR_NO_ERROR)
            {
                // Extract value
                uint16_t ntf_cfg = co_btohs(co_read16p(co_buf_data(p_data)));

                 // Only update configuration if value for stop or notification enable
                if (   (att_idx == BAS_IDX_BATT_LVL_NTF_CFG)
                    && ((ntf_cfg == PRF_CLI_STOP_NTFIND) || (ntf_cfg == PRF_CLI_START_NTF)))
                {
                    const bass_cb_t* p_cb  = (const bass_cb_t*) p_bass_env->prf_env.p_cb;

                    // Conserve information in environment
                    if (ntf_cfg == PRF_CLI_START_NTF)
                    {
                        // Ntf cfg bit set to 1
                        p_bass_env->ntf_cfg[conidx] |= (BAS_BATT_LVL_NTF_SUP << svc_idx);
                    }
                    else
                    {
                        // Ntf cfg bit set to 0
                        p_bass_env->ntf_cfg[conidx] &= ~(BAS_BATT_LVL_NTF_SUP << svc_idx);
                    }

                    // Inform application about bond data update
                    p_cb->cb_bond_data_upd(conidx, p_bass_env->ntf_cfg[conidx]);
                }
                else
                {
                    status = PRF_APP_ERROR;
                }
            }
        }
    }

    gatt_srv_att_val_set_cfm(conidx, user_lid, token, status);
}

/**
 ****************************************************************************************
 * @brief This function is called when GATT server user has initiated event send to peer
 *        device or if an error occurs.
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] dummy         Dummy parameter provided by upper layer for command execution
 * @param[in] status        Status of the procedure (see enum #hl_err)
 ****************************************************************************************
 */
__STATIC void bass_cb_event_sent(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status)
{
    if(conidx == GAP_INVALID_CONIDX)
    {
        // Consider job done
        bass_env_t *p_bass_env = PRF_ENV_GET(BASS, bass);
        if(p_bass_env != NULL)
        {
            const bass_cb_t* p_cb  = (const bass_cb_t*) p_bass_env->prf_env.p_cb;
            p_bass_env->op_ongoing = false;

            // Inform application that event has been sent
            p_cb->cb_batt_level_upd_cmp(status);

            // continue operation execution
            bass_exe_operation();
        }
    }
}


/// Service callback hander
__STATIC const gatt_srv_cb_t bass_cb =
{
        .cb_event_sent    = bass_cb_event_sent,
        .cb_att_read_get  = bass_cb_att_read_get,
        .cb_att_event_get = NULL,
        .cb_att_info_get  = NULL,
        .cb_att_val_set   = bass_cb_att_val_set,
};

/*
 * PROFILE NATIVE HANDLERS
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief Restore bond data of a known peer device (at connection establishment)
 *
 * @param[in] conidx          Connection index
 * @param[in] ntf_cfg         Notification Configuration
 * @param[in] p_old_batt_lvl  Old Battery Level used to decide if notification should be triggered
 *                            Array of BASS_NB_BAS_INSTANCES_MAX size.
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t bass_enable(uint8_t conidx, uint8_t ntf_cfg, const uint8_t* p_old_batt_lvl)
{
    bass_env_t *p_bass_env = PRF_ENV_GET(BASS, bass);
    uint16_t status = PRF_ERR_REQ_DISALLOWED;

    if(p_old_batt_lvl == NULL)
    {
        status = PRF_ERR_INVALID_PARAM;
    }
    else if(p_bass_env != NULL)
    {
        // check state of the task
        if (gapc_get_conhdl(conidx) != GAP_INVALID_CONHDL)
        {
            uint8_t svc_cursor;
            p_bass_env->ntf_cfg[conidx] = ntf_cfg;

            // loop on all services to check if notification should be triggered
            for(svc_cursor = 0 ; svc_cursor < p_bass_env->nb_svc; svc_cursor++)
            {
                if (   ((p_bass_env->ntf_cfg[conidx] & (1 << svc_cursor)) != 0)
                    && (p_old_batt_lvl[svc_cursor] != p_bass_env->batt_lvl[svc_cursor]))
                {
                    // trigger notification
                    bass_notify_batt_lvl(p_bass_env, conidx, svc_cursor);
                }
            }

            status = GAP_ERR_NO_ERROR;
        }
    }

    return (status);
}

/**
 ****************************************************************************************
 * @brief Update a battery level
 *
 * Wait for #cb_batt_level_upd_cmp execution before starting a new procedure
 *
 * @param[in] p_temp_meas   Pointer to Temperature Measurement information
 * @param[in] batt_level   Stable or intermediary type of temperature (True stable meas, else false)
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t bass_batt_level_upd(uint8_t bas_instance, uint8_t batt_level)
{
    bass_env_t *p_bass_env = PRF_ENV_GET(BASS, bass);
    uint16_t status = PRF_ERR_REQ_DISALLOWED;

    if(p_bass_env != NULL)
    {
        // Parameter sanity check
        if ((bas_instance < p_bass_env->nb_svc) && (batt_level <= BAS_BATTERY_LVL_MAX))
        {
            // update the battery level value
            p_bass_env->batt_lvl[bas_instance] = batt_level;
            status = bass_notify_batt_lvl(p_bass_env, GAP_INVALID_CONIDX, bas_instance);
        }
        else
        {
            status = PRF_ERR_INVALID_PARAM;
        }
    }

    return (status);
}

#if (HOST_MSG_API)
/*
 * PROFILE MSG HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BASS_ENABLE_REQ message.
 * The handler enables the Battery 'Profile' Server Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] p_param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int bass_enable_req_handler(ke_msg_id_t const msgid, struct bass_enable_req const *p_param,
                                     ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

    struct bass_enable_rsp *p_rsp;
    // Request status
    uint16_t status = bass_enable(p_param->conidx, p_param->ntf_cfg, p_param->old_batt_lvl);

    // send response to application
    p_rsp         = KE_MSG_ALLOC(BASS_ENABLE_RSP, src_id, dest_id, bass_enable_rsp);
    if(p_rsp)
    {
        p_rsp->conidx = p_param->conidx;
        p_rsp->status = status;

        ke_msg_send(p_rsp);
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BASS_BATT_LEVEL_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] p_param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int bass_batt_level_upd_req_handler(ke_msg_id_t const msgid,
                                            struct bass_batt_level_upd_req const *p_param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    uint16_t status = bass_batt_level_upd(p_param->bas_instance, p_param->batt_level);

    if(status != GAP_ERR_NO_ERROR)
    {
        // an error occurs, trigger it.
        struct bass_batt_level_upd_rsp *p_rsp = KE_MSG_ALLOC(BASS_BATT_LEVEL_UPD_RSP, src_id, dest_id,
                                                             bass_batt_level_upd_rsp);
        if(p_rsp != NULL)
        {
            p_rsp->status = status;
            ke_msg_send(p_rsp);
        }
    }

    return (KE_MSG_CONSUMED);
}


/// Default State handlers definition
KE_MSG_HANDLER_TAB(bass)
{
    // Note: all messages must be sorted in ID ascending order

    {BASS_ENABLE_REQ,               (ke_msg_func_t) bass_enable_req_handler},
    {BASS_BATT_LEVEL_UPD_REQ,       (ke_msg_func_t) bass_batt_level_upd_req_handler},
};

/**
 ****************************************************************************************
 * @brief Completion of battery level update
 *
 * @param[in] status Status of the procedure execution (see enum #hl_err)
 ****************************************************************************************
 */
void bass_cb_batt_level_upd_cmp(uint16_t status)
{
    struct bass_batt_level_upd_rsp *p_rsp =
            KE_MSG_ALLOC(BASS_BATT_LEVEL_UPD_RSP, PRF_DST_TASK(BASS), PRF_SRC_TASK(BASS), bass_batt_level_upd_rsp);

    if(p_rsp)
    {
        p_rsp->status = status;
        ke_msg_send(p_rsp);
    }
}

/**
 ****************************************************************************************
 * @brief Inform that Bond data updated for the connection.
 *
 * @param[in] conidx        Connection index
 * @param[in] ntf_cfg       Notification Configuration
 ****************************************************************************************
 */
void bass_cb_bond_data_upd(uint8_t conidx, uint8_t ntf_ind_cfg)
{
    bass_env_t *p_bass_env = PRF_ENV_GET(BASS, bass);
    struct bass_batt_level_ntf_cfg_ind* p_ind =
            KE_MSG_ALLOC(BASS_BATT_LEVEL_NTF_CFG_IND, PRF_DST_TASK(BASS), PRF_SRC_TASK(BASS), bass_batt_level_ntf_cfg_ind);

    if(p_ind != NULL)
    {
        p_ind->conidx = conidx;
        p_ind->ntf_cfg = p_bass_env->ntf_cfg[conidx];

        ke_msg_send(p_ind);
    }
}

/// Default Message handle
__STATIC const bass_cb_t bass_msg_cb =
{
    .cb_batt_level_upd_cmp = bass_cb_batt_level_upd_cmp,
    .cb_bond_data_upd      = bass_cb_bond_data_upd,
};
#endif // (HOST_MSG_API)



/*
 * PROFILE DEFAULT HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BASS module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    p_env        Collector or Service allocated environment data.
 * @param[in,out] p_start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     sec_lvl      Security level (see enum #gatt_svc_info_bf)
 * @param[in]     user_prio    GATT User priority
 * @param[in]     p_param      Configuration parameters of profile collector or service (32 bits aligned)
 * @param[in]     p_cb         Callback structure that handles event from profile
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
__STATIC uint16_t bass_init(prf_data_t *p_env, uint16_t *p_start_hdl, uint8_t sec_lvl, uint8_t user_prio,
                          struct bass_db_cfg *p_params, const bass_cb_t* p_cb)
{
    //------------------ create the attribute database for the profile -------------------

    // DB Creation Status
    uint16_t status = GAP_ERR_NO_ERROR;
    uint8_t user_lid = GATT_INVALID_USER_LID;

    do
    {
        bass_env_t* p_bass_env;
        uint16_t shdl[BASS_NB_BAS_INSTANCES_MAX];
        uint8_t  cursor;
        uint16_t features = 0;

        #if (HOST_MSG_API)
        if(p_cb == NULL)
        {
            p_cb = &(bass_msg_cb);
        }
        #endif // (HOST_MSG_API)

        if(   (p_params->bas_nb == 0) || (p_params->bas_nb > BASS_NB_BAS_INSTANCES_MAX)
           || (p_params == NULL) || (p_start_hdl == NULL) || (p_cb == NULL) || (p_cb->cb_batt_level_upd_cmp == NULL)
           || (p_cb->cb_bond_data_upd == NULL))
        {
            status = GAP_ERR_INVALID_PARAM;
            break;
        }

        // register BASS user
        status = gatt_user_srv_register(L2CAP_LE_MTU_MIN, user_prio, &bass_cb, &user_lid);
        if(status != GAP_ERR_NO_ERROR) break;


        for (cursor = 0; (cursor < p_params->bas_nb); cursor++)
        {
            // Service content flag
            uint32_t cfg_flag = BAS_CFG_FLAG_MANDATORY_MASK;
            features |= (p_params->features[cursor]) << cursor;

            // Check if notifications are supported
            if (p_params->features[cursor] == BAS_BATT_LVL_NTF_SUP)
            {
                cfg_flag |= BAS_CFG_FLAG_NTF_SUP_MASK;
            }

            // Check if multiple instances
            if (p_params->bas_nb > 1)
            {
                cfg_flag |= BAS_CFG_FLAG_MTP_BAS_MASK;
            }

            shdl[cursor] = *p_start_hdl;

            // Add GAP service
            status = gatt_db_svc16_add(user_lid, sec_lvl, GATT_SVC_BATTERY_SERVICE, BAS_IDX_NB,
                                       (uint8_t *)&cfg_flag, &(bass_att_db[0]), BAS_IDX_NB, &(shdl[cursor]));

            if(status != GAP_ERR_NO_ERROR) break;

            // update start handle for next service - only useful if multiple service, else not used.
            // 4 characteristics + optional notification characteristic.
            *p_start_hdl = shdl[cursor] + BAS_IDX_NB;

            //Set optional permissions
            if (p_params->features[cursor] == BAS_BATT_LVL_NTF_SUP)
            {
                // Battery Level characteristic value permissions
                uint16_t perm = PROP(RD) | PROP(N);
                gatt_db_att_info_set(user_lid, shdl[cursor] + BAS_IDX_BATT_LVL_VAL, perm);
            }
        }

        if(status != GAP_ERR_NO_ERROR) break;

        //-------------------- allocate memory required for the profile  ---------------------
        p_bass_env = (bass_env_t *) ke_malloc(sizeof(bass_env_t), KE_MEM_PROFILE);

        if(p_bass_env != NULL)
        {
            // allocate BASS required environment variable
            p_env->p_env = (prf_hdr_t *) p_bass_env;
            p_bass_env->start_hdl  = shdl[0];
            p_bass_env->features   = features;
            p_bass_env->user_lid   = user_lid;
            p_bass_env->nb_svc     = p_params->bas_nb;
            p_bass_env->op_ongoing = false;
            p_bass_env->in_exe_op  = false;

            co_list_init(&(p_bass_env->wait_queue));
            memset(p_bass_env->ntf_cfg, 0, BLE_CONNECTION_MAX);
            memcpy(p_bass_env->batt_level_pres_format, p_params->batt_level_pres_format,
                   sizeof(prf_char_pres_fmt_t) * BASS_NB_BAS_INSTANCES_MAX);


            // initialize profile environment variable
            p_bass_env->prf_env.p_cb     = p_cb;
            #if (HOST_MSG_API)
            p_env->desc.msg_handler_tab  = bass_msg_handler_tab;
            p_env->desc.msg_cnt          = ARRAY_LEN(bass_msg_handler_tab);
            #endif // (HOST_MSG_API)

            *p_start_hdl = shdl[0];
        }
        else
        {
            status = GAP_ERR_INSUFF_RESOURCES;
        }

    } while(0);

    if((status != GAP_ERR_NO_ERROR) && (user_lid != GATT_INVALID_USER_LID))
    {
        gatt_user_unregister(user_lid);
    }

    return (status);
}

/**
 ****************************************************************************************
 * @brief Destruction of the profile module - due to a reset or profile remove.
 *
 * This function clean-up allocated memory.
 *
 * @param[in,out]    p_env        Collector or Service allocated environment data.
 * @param[in]        reason       Destroy reason (see enum #prf_destroy_reason)
 *
 * @return status of the destruction, if fails, profile considered not removed.
 ****************************************************************************************
 */
__STATIC uint16_t bass_destroy(prf_data_t *p_env, uint8_t reason)
{
    uint16_t status = GAP_ERR_NO_ERROR;
    bass_env_t *p_bass_env = (bass_env_t *) p_env->p_env;

    if(reason != PRF_DESTROY_RESET)
    {
        status = gatt_user_unregister(p_bass_env->user_lid);
    }

    if(status == GAP_ERR_NO_ERROR)
    {
        if(reason != PRF_DESTROY_RESET)
        {
            // remove buffer in wait queue
            while(!co_list_is_empty(&p_bass_env->wait_queue))
            {
                co_buf_t* p_buf = (co_buf_t*) co_list_pop_front(&p_bass_env->wait_queue);
                co_buf_release(p_buf);
            }
        }

        // free profile environment variables
        p_env->p_env = NULL;
        ke_free(p_bass_env);
    }

    return (status);
}

/**
 ****************************************************************************************
 * @brief @brief Handles Connection creation
 *
 * @param[in,out]    env          Collector or Service allocated environment data.
 * @param[in]        conidx       Connection index
 * @param[in]        is_le_con    True if it's a BLE connection, False if it's a BT-Classic connection
 ****************************************************************************************
 */
__STATIC void bass_con_create(prf_data_t *p_env, uint8_t conidx, bool is_le_con)
{
    bass_env_t *p_bass_env = (bass_env_t *) p_env->p_env;
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);
    // force notification config to zero when peer device is connected
    p_bass_env->ntf_cfg[conidx] = 0;
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in,out]    p_env      Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
__STATIC void bass_con_cleanup(prf_data_t *p_env, uint8_t conidx, uint16_t reason)
{
    bass_env_t *p_bass_env = (bass_env_t *) p_env->p_env;
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);
    // force notification config to zero when peer device is disconnected
    p_bass_env->ntf_cfg[conidx] = 0;
}



/// BASS Task interface required by profile manager
const prf_task_cbs_t bass_itf =
{
    .cb_init          = (prf_init_cb) bass_init,
    .cb_destroy       = bass_destroy,
    .cb_con_create    = bass_con_create,
    .cb_con_cleanup   = bass_con_cleanup,
};

/**
 ****************************************************************************************
 * @brief Retrieve service profile interface
 *
 * @return service profile interface
 ****************************************************************************************
 */
const prf_task_cbs_t *bass_prf_itf_get(void)
{
    return &bass_itf;
}


#endif // (BLE_BATT_SERVER)

/// @} BASS
