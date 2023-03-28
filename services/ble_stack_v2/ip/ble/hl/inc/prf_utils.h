/**
 ****************************************************************************************
 *
 * @file prf_utils.h
 *
 * @brief  Header file - Profile Utilities
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 *
 ****************************************************************************************
 */


#ifndef _PRF_UTILS_H_
#define _PRF_UTILS_H_

/**
 ****************************************************************************************
 * @addtogroup PRF_UTILS
 * @ingroup Profile
 *
 * @brief Definitions of shared profiles functions that can be used by several profiles
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#if(BLE_PROFILES)
#include "prf_types.h"
#include "gatt.h"
#include "gapc.h"
#include "gapc_msg.h"
#include "prf.h"
#include "ke_task.h"

#if ((BLE_OT_CLIENT) || (BLE_OT_SERVER))
#include "otp.h"
#endif //((BLE_OT_CLIENT) || (BLE_OT_SERVER))

/*
 * MACROS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Macro used to called the prf_client_get_env function.
 *
 * @param prf_id    Profile Task ID   (In Upper case, ex: HTPC, DISC, ...)
 * @param type      Profile task type (In lower case, ex: htpc, disc, ...)
 ****************************************************************************************
 */
#define PRF_ENV_T(type)           type##_env_t
#define PRF_ENV_TAG(type)         struct type##_env_tag
#define PRF_ENV_GET(prf_id, type) ((type##_env_t *)prf_env_get((PRF_ID_##prf_id)))

/// macro used to retrieve source task for profile
#define PRF_SRC_TASK(prf_id) prf_src_task_get(PRF_ID_##prf_id)
/// macro used to retrieve destination task for profile
#define PRF_DST_TASK(prf_id) prf_dst_task_get(PRF_ID_##prf_id)


// 16 bits attributes in a 128 bits array
#define ATT_16_TO_128_ARRAY(uuid) { (uuid) & 0xFF, (uuid >> 8) & 0xFF, 0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
//
#define ATT_128_PRIMARY_SERVICE     ATT_16_TO_128_ARRAY(GATT_DECL_PRIMARY_SERVICE)
#define ATT_128_CHARACTERISTIC      ATT_16_TO_128_ARRAY(GATT_DECL_CHARACTERISTIC)
#define ATT_128_CLIENT_CHAR_CFG     ATT_16_TO_128_ARRAY(GATT_DESC_CLIENT_CHAR_CFG)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Profile destruction reason
enum prf_destroy_reason
{
    /// Profile removed due to a system reset
    PRF_DESTROY_RESET  = 0x00,
    /// Profile removed due to an application request
    PRF_DESTROY_REMOVE = 0x01,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef struct prf_task_cbs prf_task_cbs_t;

/// Profile Header Data
typedef struct prf_hdr
{
    /// Callback API - meaning specific to the profile
    const void*             p_cb;
} prf_hdr_t;


/// Profile environment variable definition to dynamically allocate a Task.
typedef struct prf_data
{
    /// pointer to the allocated memory used by profile during runtime.
    prf_hdr_t*            p_env;
    #if (BLE_HL_MSG_API)
    /// Profile Task description
    ke_task_desc_t        desc;
    /// Profile Task Number
    uint8_t               prf_task;
    /// Application Task Number
    uint8_t               app_task;
    #endif // (BLE_HL_MSG_API)
    /// API Identifier (@see enum TASK_API_ID)
    uint8_t               api_id;
    /// Profile Identifier (@see enum prf_id)
    uint8_t               prf_id;
} prf_data_t;

/**
 ****************************************************************************************
 * @brief Initialization of the Profile module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    p_env        Collector or Service allocated environment data.
 * @param[in|out] p_start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     sec_lvl      Security level (@see enum gatt_svc_info_bf)
 * @param[in]     user_prio    GATT User priority
 * @param[in]     p_param      Configuration parameters of profile collector or service (32 bits aligned)
 * @param[in]     p_cb         Callback structure that handles event from profile
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
typedef uint16_t (*prf_init_cb) (prf_data_t* p_env, uint16_t* p_start_hdl, uint8_t sec_lvl, uint8_t user_prio,
                                   const void* p_params, const void* p_cb);

/**
 ****************************************************************************************
 * @brief Destruction of the profile module - due to a reset or profile remove.
 *
 * This function clean-up allocated memory.
 *
 * @param[in|out]    p_env        Collector or Service allocated environment data.
 * @param[in]        reason       Destroy reason (@see enum prf_destroy_reason)
 *
 * @return status of the destruction, if fails, profile considered not removed.
 ****************************************************************************************
 */
typedef uint16_t (*prf_destroy_cb) (prf_data_t* p_env, uint8_t reason);

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env          Collector or Service allocated environment data.
 * @param[in]        conidx       Connection index
 * @param[in]        p_con_param  Pointer to connection parameters information
 ****************************************************************************************
 */
typedef void    (*prf_con_create_cb) (prf_data_t* p_env, uint8_t conidx, const gap_con_param_t* p_con_param);

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
typedef void    (*prf_con_cleanup_cb) (prf_data_t* p_env, uint8_t conidx, uint16_t reason);

/**
 ****************************************************************************************
 * @brief Indicates update of connection parameters
 *
 * @param[in|out]    env          Collector or Service allocated environment data.
 * @param[in]        conidx       Connection index
 * @param[in]        p_con_param  Pointer to new connection parameters information
 ****************************************************************************************
 */
typedef void    (*prf_con_upd_cb) (prf_data_t* p_env, uint8_t conidx, const gap_con_param_t* p_con_param);

/// Profile task callbacks.
typedef struct prf_task_cbs
{
    /// Initialization callback
    prf_init_cb        cb_init;
    /// Destroy profile callback
    prf_destroy_cb     cb_destroy;
    /// Connection callback
    prf_con_create_cb  cb_con_create;
    /// Disconnection callback
    prf_con_cleanup_cb cb_con_cleanup;
    /// Connection parameter updated
    prf_con_upd_cb     cb_con_upd;
} prf_task_cbs_t;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve environment variable allocated for a profile
 *
 * @param[in] prf_id        Profile Identifier (@see enum rwprf_id)
 *
 * @return Environment variable allocated for a profile
 ****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif
prf_hdr_t* prf_env_get(uint8_t prf_id);
#ifdef __cplusplus
}
#endif

#if (BLE_HL_MSG_API)

/**
 ****************************************************************************************
 * @brief Retrieve source profile task number value
 *
 * @param[in] prf_id     Profile Identifier (@see enum rwprf_id)
 *
 * @return Source profile task number value
 ****************************************************************************************
 */
ke_task_id_t prf_src_task_get(uint8_t prf_id);

/**
 ****************************************************************************************
 * @brief Retrieve destination application task number value
 *
 * @param[in] prf_id     Profile Identifier (@see enum rwprf_id)
 *
 * @return Destination application task number value
 ****************************************************************************************
 */
ke_task_id_t prf_dst_task_get(uint8_t prf_id);

/**
 ****************************************************************************************
 * @brief Retrieve Task Identifier from Task number
 * (automatically update index of task in returned task id)
 *
 * @param[in]  task Task number
 * @return Profile API Identifier (@see enum TASK_API_ID)
 ****************************************************************************************
 */
uint8_t prf_get_api_id_from_task_num(uint8_t task);
uint8_t prf_get_id_from_task(uint8_t task);

/**
 ****************************************************************************************
 * @brief Retrieve Task Number from Task Identifier
 * (automatically update index of task in returned task id)
 *
 * @param[in] api_id Profile API Identifier (@see enum TASK_API_ID)
 * @return Task Number
 ****************************************************************************************
 */
uint8_t prf_get_task_num_from_api_id(uint8_t api_id);
uint8_t prf_get_task_from_id(uint8_t api_id);
#endif // (BLE_HL_MSG_API)


#if (BLE_BATT_SERVER)
/**
 ****************************************************************************************
 * @brief Pack Characteristic Presentation Format descriptor value
 ****************************************************************************************
 */
void prf_pack_char_pres_fmt(co_buf_t* p_buf, const prf_char_pres_fmt_t* char_pres_fmt);
#endif // (BLE_BATT_SERVER)

#if (BLE_BATT_CLIENT)
/**
 ****************************************************************************************
 * @brief Unpack Characteristic Presentation Format descriptor value
 ****************************************************************************************
 */
void prf_unpack_char_pres_fmt(co_buf_t* p_buf, prf_char_pres_fmt_t* char_pres_fmt);
#endif // (BLE_BATT_CLIENT)

#if (BLE_CLIENT_PRF)

/**
 ****************************************************************************************
 * @brief Write peer characteristic using GATT.
 *
 * It will request write modification of peer handle
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT Client User Local Identifier
 * @param[in] dummy         Dummy parameter returned in procedure completion
 * @param[in] write_type    GATT Write Type (@see gatt_write_type)
 * @param[in] hdl           Peer handle to modify
 * @param[in] length        Value length
 * @param[in] p_data        Pointer to data value
 *
 * @return Execution status (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t prf_gatt_write(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t write_type,
                        uint16_t hdl, uint16_t length, const uint8_t* p_data);


/**
 ****************************************************************************************
 * @brief Check service characteristic validity
 *
 * For each characteristic in service it verifies handles.
 *
 * If some handles are not present, it checks if they shall be present or they are optional.
 *
 * @param nb_chars      Number of Characteristics in the service
 * @param p_chars       Characteristics values (char handles, val handles, properties)
 * @param p_chars_req   Characteristics requirements.
 *
 * @return Execution status (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t prf_check_svc_char_validity(uint8_t nb_chars, const prf_char_t* p_chars,
                                     const prf_char_def_t* p_chars_req);


#if (BLE_PRF_PROPRIETARY_SVC_SUPPORT)
/**
 ****************************************************************************************
 * @brief Check 128-bit service characteristic validity
 *
 * For each characteristic in service it verifies handles.
 *
 * If some handles are not present, it checks if they shall be present or they are optional.
 *
 * @param nb_chars     Number of Characteristics in the service
 * @param p_chars      Characteristics values (char handles, val handles, properties)
 * @param p_chars_req  Characteristics requirements.
 *
 * @return Execution status (@see enum hl_err)
 ****************************************************************************************
 */

uint16_t prf_check_svc128_char_validity(uint8_t nb_chars, const prf_char_t* p_chars,
                                        const prf_char128_def_t* p_chars_req);
#endif // (BLE_PRF_PROPRIETARY_SVC_SUPPORT)

/**
 ****************************************************************************************
 * @brief Check service characteristic descriptors validity
 *
 * For each characteristic descriptors in service it verifies handles.
 *
 * If some handles are not present, according to characteristic properties it verify if
 * descriptor is optional or not.
 *
 * @param nb_descs       Number of Characteristic descriptors in the service
 * @param p_descs        Characteristic descriptors values (handles)
 * @param p_descs_req    Characteristics descriptors requirements.
 *
 * @return Execution status (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t prf_check_svc_desc_validity(uint8_t nb_descs, const prf_desc_t* p_descs,
                                     const prf_desc_def_t* p_descs_req, const prf_char_t* p_chars);

/**
 ****************************************************************************************
 * @brief Check service validity (both characteristics and descriptors)
 *
 * @param nb_chars      Number of Characteristics in the service
 * @param p_chars       Characteristics values (char handles, val handles, properties)
 * @param p_chars_req   Characteristics requirements.
 * @param nb_descs      Number of Characteristic descriptors in the service
 * @param p_descs       Characteristic descriptors values (handles)
 * @param p_descs_req   Characteristics descriptors requirements.
 *
 * @return Execution status (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t prf_check_svc_validity(uint8_t nb_chars,
                                const prf_char_t* p_chars, const prf_char_def_t* p_chars_req,
                                uint8_t nb_descs,
                                const prf_desc_t* p_descs, const prf_desc_def_t* p_descs_req);

#if (BLE_PRF_PROPRIETARY_SVC_SUPPORT)
/**
 ****************************************************************************************
 * @brief Check service validity (both characteristics and descriptors)
 *
 * @param nb_chars      Number of Characteristics in the service
 * @param p_chars       Characteristics values (char handles, val handles, properties)
 * @param p_chars_req   Characteristics requirements.
 * @param nb_descs      Number of Characteristic descriptors in the service
 * @param p_descs       Characteristic descriptors values (handles)
 * @param p_descs_req   Characteristics descriptors requirements.
 *
 * @return Execution status (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t prf_check_svc128_validity(uint8_t nb_chars,
                                   const prf_char_t* p_chars, const prf_char128_def_t* p_chars_req,
                                   uint8_t nb_descs,
                                   const prf_desc_t* p_descs, const prf_desc_def_t* p_descs_req);
#endif // (BLE_PRF_PROPRIETARY_SVC_SUPPORT)

/**
 ****************************************************************************************
 * @brief Extract information of the service according to the service description
 *
 * @param[in]     first_hdl    First handle value of following list
 * @param[in]     nb_att       Number of attributes
 * @param[in]     p_atts       Pointer to attribute information present in a service
 * @param[in]     nb_chars     Length of provided arrays (chars and chars_req)
 * @param[in]     p_chars_req    Characteristics requirements
 * @param[in|out] p_chars            Characteristics
 * @param[in]     nb_descs         Length of provided arrays (descs and descs_req)
 * @param[in]     p_descs_req        Descriptors requirements
 * @param[in|out] p_descs            Descriptors
 ****************************************************************************************
 */
void prf_extract_svc_info(uint16_t first_hdl, uint8_t nb_att, const gatt_svc_att_t* p_atts,
                          uint8_t nb_chars, const prf_char_def_t* p_chars_req, prf_char_t* p_chars,
                          uint8_t nb_descs, const prf_desc_def_t* p_descs_req, prf_desc_t* p_descs);


#if (BLE_PRF_PROPRIETARY_SVC_SUPPORT)

/**
 ****************************************************************************************
 * @brief Extract information of the service 128-Bits according to the service description
 *
 * @param[in]     first_hdl    First handle value of following list
 * @param[in]     nb_att       Number of attributes
 * @param[in]     p_atts       Pointer to attribute information present in a service / UUIDs in array shall be in 128-bit format
 * @param[in]     nb_chars     Length of provided arrays (chars and chars_req)
 * @param[in]     p_chars_req    Characteristics requirements
 * @param[in|out] p_chars            Characteristics
 * @param[in]     nb_descs         Length of provided arrays (descs and descs_req)
 * @param[in]     p_descs_req        Descriptors requirements
 * @param[in|out] p_descs            Descriptors
 ****************************************************************************************
 */
void prf_extract_svc128_info(uint16_t first_hdl, uint8_t nb_att, const gatt_svc_att_t* p_atts,
                             uint8_t nb_chars, const prf_char128_def_t* p_chars_req, prf_char_t* p_chars,
                             uint8_t nb_descs, const prf_desc_def_t* p_descs_req, prf_desc_t* p_descs);
#endif // (BLE_PRF_PROPRIETARY_SVC_SUPPORT)
#endif //(BLE_CLIENT_PRF)


#if (BLE_SERVER_PRF || BLE_CLIENT_PRF)
/**
 ****************************************************************************************
 * @brief Pack date time value
 *
 * @param[in] p_buf       Pointer to the output buffer
 * @param[in] p_date_time Pointer to structure date time
 ****************************************************************************************
 */
void prf_pack_date_time(co_buf_t*p_buf, const prf_date_time_t* p_date_time);

/**
 ****************************************************************************************
 * @brief Pack date value
 *
 * @param[in] p_buf       Pointer to the output buffer
 * @param[in] p_date Pointer to structure date
 ****************************************************************************************
 */
void prf_pack_date(co_buf_t*p_buf, const prf_date_t* p_date);


/**
 ****************************************************************************************
 * @brief Unpack date time value
 *
 * @param[in]  p_buf       Pointer to input buffer
 * @param[out] p_date_time Pointer to structure date time
 ****************************************************************************************
 */
void prf_unpack_date_time(co_buf_t* p_buf, prf_date_time_t* p_date_time);

/**
 ****************************************************************************************
 * @brief Unpack date  value
 *
 * @param[in]  p_buf       Pointer to input buffer
 * @param[out] p_date      Pointer to structure date
 ****************************************************************************************
 */
void prf_unpack_date(co_buf_t* p_buf, prf_date_t* p_date);

/**
 ****************************************************************************************
 * @brief Allocate a buffer that will be provided to GATT for transmission of an ATT
 * message
 *
 * @param[in] pp_buf    Pointer at which allocated buffer is returned
 * @param[in] data_len  Buffer data length
 *
 * @return Allocation status
 ****************************************************************************************
 */
uint8_t prf_buf_alloc(co_buf_t** pp_buf, uint16_t data_len);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
uint32_t prf_evt_get_con_bf(uint8_t* p_cli_cfg_bf, uint8_t char_type);

#if ((BLE_OT_CLIENT) || (BLE_OT_SERVER))
/**
 ****************************************************************************************
 * @brief Check the validity of an Object ID
 *
 * @param[in] p_object_id       Pointer to Object ID
 *
 * @return True if valid and false if invalid
 ****************************************************************************************
 */
bool prf_is_object_id_valid(const ot_object_id_t* p_object_id);
#endif // ((BLE_OT_CLIENT) || (BLE_OT_SERVER))

#endif /* (BLE_SERVER_PRF || BLE_CLIENT_PRF) */

#if ((BLE_CGM_SERVER || BLE_CGM_CLIENT))
/**
 ****************************************************************************************
 * @brief Calculate E2E-CRC according to the profile specification
 *
 * @param[in] p_buf Pointer to buffer that contains CRC data
 *
 * @return E2E-CRC according to the specification.
 ****************************************************************************************
 */
uint16_t prf_e2e_crc(co_buf_t* p_buf);

#endif /* ((BLE_CGM_SERVER || BLE_CGM_CLIENT)) */

#endif // (BLE_PROFILES)

/// @} prf_utils

#endif /* _PRF_UTILS_H_ */
