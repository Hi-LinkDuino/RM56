/**
 ****************************************************************************************
 * @addtogroup DISS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_DIS_SERVER)
#include "diss.h"
#include "diss_task.h"
#include "prf_utils.h"
#include "prf.h"
#include "gatt.h"
#include "ke_mem.h"
#include "prf_dbg.h"


#define PRF_TAG "[DISS]"

/*
 * MACROS
 ****************************************************************************************
 */

/// Maximal length for Characteristic values - 128 bytes
#define DIS_VAL_MAX_LEN                         (128)
///System ID string length
#define DIS_SYS_ID_LEN                          (0x08)
///IEEE Certif length (min 6 bytes)
#define DIS_IEEE_CERTIF_MIN_LEN                 (0x06)
///PnP ID length
#define DIS_PNP_ID_LEN                          (0x07)


/*
 * DIS ATTRIBUTES
 ****************************************************************************************
 */

/// Full DIS Database Description - Used to add attributes into the database
const struct gatt_att16_desc diss_att_db[DIS_IDX_NB] =
{
        // Device Information Service Declaration
        [DIS_IDX_SVC]                           =  {GATT_DECL_PRIMARY_SERVICE, PROP(RD), 0},
    
        // Manufacturer Name Characteristic Declaration
        [DIS_IDX_MANUFACTURER_NAME_CHAR]        =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // Manufacturer Name Characteristic Value
        [DIS_IDX_MANUFACTURER_NAME_VAL]         =   {GATT_CHAR_MANUF_NAME, PROP(RD), DIS_VAL_MAX_LEN},
    
        // Model Number String Characteristic Declaration
        [DIS_IDX_MODEL_NB_STR_CHAR]             =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // Model Number String Characteristic Value
        [DIS_IDX_MODEL_NB_STR_VAL]              =   {GATT_CHAR_MODEL_NB, PROP(RD), DIS_VAL_MAX_LEN},
    
        // Serial Number String Characteristic Declaration
        [DIS_IDX_SERIAL_NB_STR_CHAR]            =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // Serial Number String Characteristic Value
        [DIS_IDX_SERIAL_NB_STR_VAL]             =   {GATT_CHAR_SERIAL_NB, PROP(RD), DIS_VAL_MAX_LEN},
    
        // Hardware Revision String Characteristic Declaration
        [DIS_IDX_HARD_REV_STR_CHAR]             =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // Hardware Revision String Characteristic Value
        [DIS_IDX_HARD_REV_STR_VAL]              =   {GATT_CHAR_HW_REV, PROP(RD), DIS_VAL_MAX_LEN},
    
        // Firmware Revision String Characteristic Declaration
        [DIS_IDX_FIRM_REV_STR_CHAR]             =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // Firmware Revision String Characteristic Value
        [DIS_IDX_FIRM_REV_STR_VAL]              =   {GATT_CHAR_FW_REV, PROP(RD), DIS_VAL_MAX_LEN},
    
        // Software Revision String Characteristic Declaration
        [DIS_IDX_SW_REV_STR_CHAR]               =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // Software Revision String Characteristic Value
        [DIS_IDX_SW_REV_STR_VAL]                =   {GATT_CHAR_SW_REV, PROP(RD), DIS_VAL_MAX_LEN},
    
        // System ID Characteristic Declaration
        [DIS_IDX_SYSTEM_ID_CHAR]                =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // System ID Characteristic Value
        [DIS_IDX_SYSTEM_ID_VAL]                 =   {GATT_CHAR_SYS_ID, PROP(RD), DIS_SYS_ID_LEN},
    
        // IEEE 11073-20601 Regulatory Certification Data List Characteristic Declaration
        [DIS_IDX_IEEE_CHAR]                     =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // IEEE 11073-20601 Regulatory Certification Data List Characteristic Value
        [DIS_IDX_IEEE_VAL]                      =   {GATT_CHAR_IEEE_CERTIF, PROP(RD), DIS_SYS_ID_LEN},
    
        // PnP ID Characteristic Declaration
        [DIS_IDX_PNP_ID_CHAR]                   =   {GATT_DECL_CHARACTERISTIC, PROP(RD), 0},
        // PnP ID Characteristic Value
        [DIS_IDX_PNP_ID_VAL]                    =   {GATT_CHAR_PNP_ID, PROP(RD), DIS_PNP_ID_LEN},

};

/**
 ****************************************************************************************
 * @brief Retrieve Service and attribute index form attribute handle
 *
 * @param[out] handle     Attribute handle
 * @param[out] p_svc_idx  DIS Service index
 * @param[out] p_att_idx  Attribute index
 *
 * @return Success if attribute and service index found, else Application error
 ****************************************************************************************
 */
__STATIC uint16_t diss_get_att_idx(uint16_t handle, uint8_t *p_svc_idx, uint8_t *p_att_idx)
{
    diss_env_t *p_diss_env = PRF_ENV_GET(DISS, diss);
    uint16_t hdl_cursor = p_diss_env->start_hdl;
    uint16_t status = 0x80;

    // Browse list of services
    // handle must be greater than current index
    for (*p_svc_idx = 0; (handle >= hdl_cursor); (*p_svc_idx)++)  //(*p_svc_idx < p_diss_env->nb_svc) && 
    {
        // check if handle is within service range
        if (handle <= (hdl_cursor + DIS_IDX_NB))
        {
            *p_att_idx = handle - hdl_cursor;

            // check if notification are present
            if ((*p_att_idx >= DIS_IDX_NB)) //&& ((p_diss_env->features >> *p_svc_idx) & 0x01) != BAS_BATT_LVL_NTF_SUP)
            {
                *p_att_idx += 1;
            }

            // If Battery level presentation format but should be not present, there is an error
            /*if((*p_att_idx == BAS_IDX_BATT_LVL_PRES_FMT) && (p_diss_env->nb_svc == 1))
            {
                break;
            }*/

            // search succeed
            status = GAP_ERR_NO_ERROR;
            break;
        }

        hdl_cursor += DIS_IDX_NB;
    }

    return (status);
}

__STATIC void diss_gatt_cb_event_sent(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status)
{
    LOG_I("%s",__func__);
}

__STATIC void diss_gatt_cb_att_read_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl, uint16_t offset,
                             uint16_t max_length)
{
    LOG_I("%s",__func__);
    co_buf_t* p_data = NULL;
    // Pointer to the data
    uint8_t *data = NULL;
    uint16_t len = 0;
    uint16_t status = GAP_ERR_NO_ERROR;
    
    // Get the address of the environment
    diss_env_t *diss_env = PRF_ENV_GET(DISS, diss);
    LOG_I("read hdl %d diss_env->start_hdl:%d ", hdl,diss_env->start_hdl);
    uint8_t svc_idx = 0, att_idx = 0;
    status = diss_get_att_idx(hdl, &svc_idx, &att_idx);
    LOG_I("att_idx:%d status:%d",att_idx,status);
    if(status == GAP_ERR_NO_ERROR)
    {
       switch(att_idx)
       {
           //case DIS_MANUFACTURER_NAME_CHAR:
           case DIS_IDX_MANUFACTURER_NAME_VAL:
           {
               // Set information
               len = APP_DIS_MANUFACTURER_NAME_LEN;
               data = (uint8_t *)APP_DIS_MANUFACTURER_NAME;
           } break;
           
           case DIS_IDX_MODEL_NB_STR_VAL:
           {
               // Set information
               len = APP_DIS_MODEL_NB_STR_LEN;
               data = (uint8_t *)APP_DIS_MODEL_NB_STR;
           } break;
           
           case DIS_IDX_SYSTEM_ID_VAL:
           {
               // Set information
               len = APP_DIS_SYSTEM_ID_LEN;
               data = (uint8_t *)APP_DIS_SYSTEM_ID;
           } break;
           
           case DIS_IDX_PNP_ID_VAL:
           {
               // Set information
               len = APP_DIS_PNP_ID_LEN;
               data = (uint8_t *)APP_DIS_PNP_ID;
           } break;
           
           case DIS_IDX_HARD_REV_STR_VAL:
           {
               // Set information
               len = APP_DIS_HARD_REV_STR_LEN;
               data = (uint8_t *)APP_DIS_HARD_REV_STR;
           } break;
           
           case DIS_IDX_FIRM_REV_STR_VAL:
           {
               // Set information
               len = APP_DIS_FIRM_REV_STR_LEN;
               data = (uint8_t *)APP_DIS_FIRM_REV_STR;
           } break;
           
           case DIS_IDX_SW_REV_STR_VAL:
           {
               // Set information
               len = APP_DIS_SW_REV_STR_LEN;
               data = (uint8_t *)APP_DIS_SW_REV_STR;
           } break;
           
           case DIS_IDX_IEEE_VAL:
           {
               // Set information
               len = APP_DIS_IEEE_LEN;
               data = (uint8_t *)APP_DIS_IEEE;
           } break;
		 case DIS_IDX_SERIAL_NB_STR_VAL:
		  {
			  // Set information
			  len = APP_DIS_SERIAL_NB_STR_LEN;
			  data = (uint8_t *)APP_DIS_SERIAL_NB_STR;
		  } break;
         default:
            ASSERT_ERR(0);
            break;
       }
    }
    prf_buf_alloc(&p_data, len);
    memcpy(co_buf_data(p_data), data, len);
    co_buf_tail_reserve(p_data, len);

    if(status == GAP_ERR_NO_ERROR)
    {
       status = gatt_srv_att_read_get_cfm(conidx, user_lid, token, status, len, p_data);
       LOG_I("gatt_srv_att_read_get_cfm %d",status);
       TRACE(1, "dataLen %d", len);
       DUMP8("%02x ",p_data,len);
       // Release the buffer
       if(p_data != NULL)
       {
          co_buf_release(p_data);
       }
    }
}
                             //(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl, uint16_t offset,
                             //                    uint16_t max_length)

__STATIC void diss_gatt_cb_att_event_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t dummy, uint16_t hdl,
                              uint16_t max_length)

{
    LOG_I("%s",__func__);
           
}

__STATIC void diss_gatt_cb_att_info_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl)
{
    LOG_I("%s",__func__);
}

__STATIC void diss_gatt_cb_att_set(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl,
                                          uint16_t offset, co_buf_t* p_buf)
{
    LOG_I("%s",__func__);
    
}


/// Set of callbacks functions for communication with GATT as a GATT User Client
__STATIC const gatt_srv_cb_t diss_gatt_cb = {
    .cb_event_sent = diss_gatt_cb_event_sent,
    .cb_att_read_get = diss_gatt_cb_att_read_get,
    .cb_att_event_get = diss_gatt_cb_att_event_get,
    .cb_att_info_get = diss_gatt_cb_att_info_get,
    .cb_att_val_set = diss_gatt_cb_att_set,
};


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the DISS module.
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
static uint16_t diss_init (prf_data_t *env, uint16_t* start_hdl, uint8_t sec_lvl,  uint8_t user_prio,
                                    struct diss_db_cfg *params, const void* p_cb)

{
    //------------------ create the attribute database for the profile -------------------

    // Service content flag
    uint32_t cfg_flag;
    // DB Creation Statis
    LOG_I("%s",__func__);
    uint8_t status = GAP_ERR_NO_ERROR;

    // Compute Attribute Table and save it in environment
    cfg_flag = diss_compute_cfg_flag(params->features);

    /*status = attm_svc_create_db(start_hdl, ATT_SVC_DEVICE_INFO, (uint8_t *)&cfg_flag,
            DIS_IDX_NB, NULL, env->task, &diss_att_db[0],
            (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)));*/

    //-------------------- allocate memory required for the profile  ---------------------
 
    diss_env_t* diss_env =
        (diss_env_t* ) ke_malloc(sizeof(diss_env_t), KE_MEM_PROFILE);
    
    memset((uint8_t *)diss_env, 0, sizeof(diss_env));
    
    // allocate DISS required environment variable
    env->p_env = (prf_hdr_t*) diss_env;

    // Register as GATT User Client
    status = gatt_user_srv_register(PREFERRED_BLE_MTU, user_prio, &diss_gatt_cb,
                                    &(diss_env->user_lid));
    LOG_I("gatt_user_srv_register status:%x",status);
    if (GAP_ERR_NO_ERROR == status)
    {
        diss_env->start_hdl = *start_hdl;  //(uint8_t *)&cfg_flag
        status = gatt_db_svc16_add(diss_env->user_lid, sec_lvl, GATT_SVC_DEVICE_INFO,
                                   DIS_IDX_NB, (uint8_t *)&cfg_flag, &(diss_att_db[0]), DIS_IDX_NB,
                                   &diss_env->start_hdl);
        LOG_I("gatt_db_svc16_add status:%x",status);
        if (GAP_ERR_NO_ERROR == status)
        {
            // initialize environment variable
            env->api_id                     = TASK_ID_DISS;
            diss_task_init(&(env->desc), (void *)diss_env);

            /* Put HRS in Idle state */
            //ke_state_set(env->prf_task, DISS_IDLE);
        }

    }
    
    LOG_I("DISS uid %d shdl %d", diss_env->user_lid, diss_env->start_hdl);
    if((status != GAP_ERR_NO_ERROR) && (diss_env->user_lid != GATT_INVALID_USER_LID))
    {
        gatt_user_unregister(diss_env->user_lid);
    }
    return (status);
}
/**
 ****************************************************************************************
 * @brief Destruction of the DISS module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static uint16_t diss_destroy(struct prf_data* env,uint8_t reason)
{
    diss_env_t* diss_env = (diss_env_t*) env->p_env;

    // remove all values present in list
    while(!co_list_is_empty(&(diss_env->values)))
    {
        struct co_list_hdr* hdr = co_list_pop_front(&(diss_env->values));
        ke_free(hdr);
    }

    // free profile environment variables
    env->p_env = NULL;
    ke_free(diss_env);
    
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
static void diss_create(prf_data_t* p_env, uint8_t conidx, const gap_le_con_param_t* p_con_param)
{
    /* Nothing to do */
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
static void diss_cleanup(struct prf_data* env, uint8_t conidx, uint16_t reason)
{
    /* Nothing to do */
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// DISS Task interface required by profile manager
const struct prf_task_cbs diss_itf =
{
        //(pf_init_cb) 
    (prf_init_cb)diss_init,
    diss_destroy,
    diss_create,
    diss_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* diss_prf_itf_get(void)
{
   return &diss_itf;
}

uint32_t diss_compute_cfg_flag(uint16_t features)
{
    //Service Declaration
    uint32_t cfg_flag = 1;

    for (uint8_t i = 0; i<DIS_CHAR_MAX; i++)
    {
        if (((features >> i) & 1) == 1)
        {
            cfg_flag |= (3 << (i*2 + 1));
        }
    }

    return cfg_flag;
}


uint8_t diss_handle_to_value(diss_env_t* env, uint16_t handle)
{
    uint8_t value = DIS_CHAR_MAX;

    // handle cursor, start from first characteristic of service handle
    uint16_t cur_hdl = env->start_hdl + 1;

    for (uint8_t i = 0; i<DIS_CHAR_MAX; i++)
    {
        if (((env->features >> i) & 1) == 1)
        {
            // check if value handle correspond to requested handle
            if((cur_hdl +1) == handle)
            {
                value = i;
                break;
            }
            cur_hdl += 2;
        }
    }

    return value;
}

uint16_t diss_value_to_handle(diss_env_t* env, uint8_t value)
{
    uint16_t handle = env->start_hdl + 1;
    int8_t i;

    for (i = 0; i<DIS_CHAR_MAX; i++)
    {
        if (((env->features >> i) & 1) == 1)
        {
            // requested value
            if(value == i)
            {
                handle += 1;
                break;
            }
            handle += 2;
        }
    }

    // check if handle found
    return ((i == DIS_CHAR_MAX) ? GATT_INVALID_HDL : handle);
}

uint8_t diss_check_val_len(uint8_t char_code, uint8_t val_len)
{
    uint8_t status = GAP_ERR_NO_ERROR;

    // Check if length is upper than the general maximal length
    if (val_len > DIS_VAL_MAX_LEN)
    {
        status = PRF_ERR_UNEXPECTED_LEN;
    }
    else
    {
        // Check if length matches particular requirements
        switch (char_code)
        {
            case DIS_SYSTEM_ID_CHAR:
                if (val_len != DIS_SYS_ID_LEN)
                {
                    status = PRF_ERR_UNEXPECTED_LEN;
                }
                break;
            case DIS_IEEE_CHAR:
                if (val_len < DIS_IEEE_CERTIF_MIN_LEN)
                {
                    status = PRF_ERR_UNEXPECTED_LEN;
                }
                break;
            case DIS_PNP_ID_CHAR:
                if (val_len != DIS_PNP_ID_LEN)
                {
                    status = PRF_ERR_UNEXPECTED_LEN;
                }
                break;
            default:
                break;
        }
    }

    return (status);
}



#endif //BLE_DIS_SERVER

/// @} DISS
