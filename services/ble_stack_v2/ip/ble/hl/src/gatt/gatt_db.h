/**
 ****************************************************************************************
 *
 * @file gatt_db.h
 *
 * @brief Header file - GATT Database API for GATT server
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef GATT_DB_H_
#define GATT_DB_H_

/**
 ****************************************************************************************
 * @addtogroup GATT
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_GATT)
#include "gatt.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */
// Maximum native attribute length
#define GATT_DB_ATT_MAX_NATIVE_VAL_LEN  (20)

/// Access operation to attribute element
enum gatt_db_access
{
    /// Read type of access to element
    GATT_DB_ACCESS_READ = 0x00,
    /// Write request type of access to element
    GATT_DB_ACCESS_WRITE,
    /// Write command type of access to element
    GATT_DB_ACCESS_WRITE_COMMAND,
    /// Write signed type of access to element
    GATT_DB_ACCESS_WRITE_SIGNED,
    /// Notify type of access to element
    GATT_DB_ACCESS_NOTIFY,
    /// Indication type of access to element
    GATT_DB_ACCESS_INDICATE,

    GATT_DB_ACCESS_MAX,
};




/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// Attribute description
typedef struct gatt_db_att
{
    /// Attribute permission (@see enum gatt_att_info_bf)
    uint16_t                perm;
    /// Attribute UUID (if 16-bit UUID else offset in memory where 32-bit or 128-bit UUID is present)
    uint16_t                uuid;
    /// Attribute extended information bit field (@see enum gatt_att_ext_info_bf)
    /// Note:
    ///   - For Included Services and Characteristic Declarations, this field contains targeted handle.
    ///   - For Characteristic Extended Properties, this field contains 2 byte value
    ///   - For Client Characteristic Configuration and Server Characteristic Configuration, this field is not used.
    uint16_t                ext_info;
} gatt_db_att_t;

/// Database service description
typedef struct gatt_db_svc
{
    /// Next element in the service list
    struct gatt_db_svc*     p_next;
    /// Service start handle
    uint16_t                start_hdl;
    /// Number of attribute in service
    uint8_t                 nb_att;
    /// Number of attribute reserved for service.
    uint8_t                 nb_att_rsvd;
    /// Service UUID (if 16-bit UUID else offset in memory where 32-bit or 128-bit UUID is present)
    uint16_t                uuid;
    /// Service permission (@see enum gatt_svc_info_bf)
    uint8_t                 perm;
    /// GATT User local Identifier
    uint8_t                 user_lid;
    /// List of attribute present in database
    gatt_db_att_t           att[__ARRAY_EMPTY];
} gatt_db_svc_t;
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief  Retrieve attribute in database from handle
 *
 * @param[in]  hdl        Handle of the attribute
 * @param[out] pp_svc     Pointer to service
 * @param[out] pp_att     Pointer to attribute
 *
 * @return @ref GAP_ERR_NO_ERROR if access is permitted, otherwise the ATT error code.
 *****************************************************************************************
 */
uint16_t gatt_db_att_get(uint16_t hdl, gatt_db_svc_t** pp_svc, gatt_db_att_t** pp_att);

/**
 ****************************************************************************************
 * @brief  Retrieve next attribute in database, and if needed according to a specific handle
 *
 * @param[in]  start_hdl     Search end handle
 * @param[in]  end_hdl       Search start handle
 * @param[in]  by_uuid       True to search by UUID, else find first attribute in range
 * @param[in]  uuid_type     Searched UUID type (@see enum gatt_uuid_type)
 * @param[in]  p_uuid        Searched UUID value
 *
 * @param[out] p_hdl         Pointer to the found attribute handle
 * @param[out] p_end_grp_hdl Pointer to end group handle value of found attribute
 * @param[out] pp_svc        Pointer to service
 * @param[out] pp_att        Pointer to attribute
 *
 * @return @ref GAP_ERR_NO_ERROR if access is permitted, otherwise the ATT error code.
 *****************************************************************************************
 */
uint16_t gatt_db_att_find(uint16_t start_hdl, uint16_t end_hdl, bool by_uuid, uint8_t uuid_type, uint8_t* p_uuid,
                          uint16_t* p_hdl, uint16_t* p_end_grp_hdl, gatt_db_svc_t** pp_svc, gatt_db_att_t** pp_att);

/**
 ****************************************************************************************
 * @brief  Check attribute access.
 *
 * @param[in]  conidx     Index of the connection with the peer device
 * @param[in]  access     Type of access to be performed (@see enum gatt_db_access)
 * @param[in]  hdl        Handle of the attribute to be accessed
 * @param[out] p_svc     Pointer to service
 * @param[out] p_att     Pointer to attribute
 *
 * @return @ref GAP_ERR_NO_ERROR if access is permitted, otherwise the ATT error code.
 *****************************************************************************************
 */
uint16_t gatt_db_att_access_check(uint8_t conidx, uint8_t access, gatt_db_svc_t* p_svc, gatt_db_att_t* p_att);

/**
 ****************************************************************************************
 * @brief  Retrieve Native attribute data value (handled by attribute database)
 *
 * @note Works for PRIMARY_SERVICE, SECONDARY_SERVICE, CHARACTERISTIC, INCLUDE_SERVICE
 *       and CHAR_EXT_PROPERTIES.
 *
 * @param[in]  hdl        Handle of the attribute
 * @param[in]  p_svc      Pointer to service
 * @param[in]  p_att      Pointer to attribute
 * @param[out] p_out      Pointer to the data buffer where value must be pushed
 * @param[out] p_length   Pointer where value length is set
 *
 * @return @ref GAP_ERR_NO_ERROR if access is permitted, otherwise the ATT error code.
 *****************************************************************************************
 */
uint16_t gatt_db_att_native_val_get(uint16_t hdl, gatt_db_svc_t* p_svc, gatt_db_att_t* p_att,
                                    uint8_t* p_out, uint16_t* p_length);

#endif // (BLE_GATT)
/// @} GATT

#endif // GATT_DB_H_
