/**
 ****************************************************************************************
 *
 * @file hogp_common.h
 *
 * @brief Header File - HID Over GATT Profile common types.
 *
 * Copyright (C) RivieraWaves 2009-2020
 *
 *
 ****************************************************************************************
 */


#ifndef _HOGP_COMMON_H_
#define _HOGP_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup HOGP HID Over GATT Profile
 * @ingroup Profile
 * @brief HID Over GATT Profile
 *
 * The HOGP module is the responsible block for implementing the HID Over GATT Profile
 * functionalities in the BLE Host.
 *
 * The HID Over GATT Profile defines the functionality required to allow HID reports and
 * HID data to be exchanged between a HID Device and a HID Host
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "prf_types.h"
#include "co_math.h"

//#include "prf_dbg.h"

//#define PRF_TAG "[HOGPD]"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximal length of Report Char. Value
#define HOGP_REPORT_MAX_LEN                (45)
/// Maximal length of Report Map Char. Value
#define HOGP_REPORT_MAP_MAX_LEN            (512)
/// Length of Boot Report Char. Value Maximal Length
#define HOGP_BOOT_REPORT_MAX_LEN           (8)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// HID Information bit values
enum hogp_info_bit
{
    /// Device capable of providing wake-up signal to a HID host
    HIDS_REMOTE_WAKE_CAPABLE           = 0x01,
    /// Normally connectable support bit
    HIDS_NORM_CONNECTABLE              = 0x02,
};

/// HID Control Point Characteristic value keys
enum hogp_ctnl_pt
{
    /// Suspend
    HOGP_CTNL_PT_SUSPEND            = 0x00,
    /// Exit suspend
    HOGP_CTNL_PT_EXIT_SUSPEND,
};

/// Protocol Mode Char. value Keys
enum hogp_boot_prot_mode
{
    /// Boot Protocol Mode
    HOGP_BOOT_PROTOCOL_MODE         = 0x00,
    /// Report Protocol Mode
    HOGP_REPORT_PROTOCOL_MODE,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// HID Information structure
typedef struct hogp_hid_info
{
    /// bcdHID
    uint16_t bcdHID;
    /// bCountryCode
    uint8_t  bCountryCode;
    /// Flags
    uint8_t  flags;
} hogp_hid_info_t;

/// HIDS report info (Boot report, Host report, report Map)
typedef struct hogp_report
{
    /// Report Length
    uint16_t length;
    /// Boot Report value
    uint8_t  value[__ARRAY_EMPTY];
} hogp_report_t;


/// HID report Reference
typedef struct hogp_report_ref
{
    /// Report ID
    uint8_t id;
    /// Report Type
    uint8_t type;
} hogp_report_ref_t;


/// HID report MAP reference
typedef struct hogp_report_map_ref
{
    /// Reference UUID length
    uint8_t uuid_len;
    /// Reference UUID Value
    uint8_t uuid[GATT_UUID_128_LEN];
} hogp_report_map_ref_t;


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// @} hogp_common

#endif /* _HOGP_COMMON_H_ */
