/**
 ****************************************************************************************
 *
 * @file gatt.h
 *
 * @brief Header file - GATT Native API.
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef GATT_H_
#define GATT_H_

/**
 ****************************************************************************************
 * @addtogroup GATT Native API
 * @ingroup HOST
 * @brief Handles ALL messages to/from GATT block.
 *
 * The L2CAP module is responsible for SDU and PDU exchange over a BLE link, and Connection
 * oriented channel management.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "compiler.h"

#include <stdbool.h>
#include "gatt_msg.h"
#include "l2cap.h"
#include "co_buf.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
/// helper macro to define an attribute property
/// @param prop Property @see enum gatt_prop_bf
#define PROP(prop)          (GATT_PROP_##prop##_BIT)


/// helper macro to define an attribute option bit
/// @param option @see enum gatt_att_info_bf or @see enum gatt_att_ext_info_bf
#define OPT(opt)          (GATT_ATT_##opt##_BIT)

/// helper macro to set attribute security level on a specific permission
/// @param  lvl_name Security level @see enum gatt_sec_lvl
/// @param  perm     Permission @see enum gatt_att_info_bf (only RP, WP, NIP authorized)
#define SEC_LVL(perm, lvl_name)  (((GATT_SEC_##lvl_name) << (GATT_ATT_##perm##_LSB)) & (GATT_ATT_##perm##_MASK))

/// helper macro to set attribute security level on a specific permission
/// @param  lvl_val  Security level value
/// @param  perm     Permission @see enum gatt_att_info_bf (only RP, WP, NIP authorized)
#define SEC_LVL_VAL(perm, lvl_val)  (((lvl_val) << (GATT_ATT_##perm##_LSB)) & (GATT_ATT_##perm##_MASK))

/// helper macro to set attribute UUID type
/// @param type UUID type (16, 32, 128)
#define ATT_UUID(uuid_type)      (((GATT_UUID_##uuid_type) << (GATT_ATT_UUID_TYPE_LSB)) & (GATT_ATT_UUID_TYPE_MASK))

/// helper macro to set service security level
/// @param  lvl Security level @see enum gatt_sec_lvl
#define SVC_SEC_LVL(lvl_name)    (((GATT_SEC_##lvl_name) << (GATT_SVC_AUTH_LSB)) & (GATT_SVC_AUTH_MASK))
/// helper macro to set service security level
/// @param  lvl_val Security level value
#define SVC_SEC_LVL_VAL(lvl_val)    (((lvl_val) << (GATT_SVC_AUTH_LSB)) & (GATT_SVC_AUTH_MASK))

/// helper macro to set service UUID type
/// @param type UUID type (16, 32, 128)
#define SVC_UUID(uuid_type)      (((GATT_UUID_##uuid_type) << (GATT_SVC_UUID_TYPE_LSB)) & (GATT_SVC_UUID_TYPE_MASK))

/*
 * DEFINES
 ****************************************************************************************
 */

/// Length of Attribute signature
#define GATT_SIGNATURE_LEN              (12)

/// Length of Notification header length used for ATT transmission
#define GATT_NTF_HEADER_LEN             (1 + GATT_HANDLE_LEN)
/// Length of Write no response header length used for ATT transmission
#define GATT_WRITE_NO_RESP_HEADER_LEN   (1 + GATT_HANDLE_LEN)
/// Length of Write header length used for ATT transmission
#define GATT_WRITE_HEADER_LEN           (1 + GATT_HANDLE_LEN)

/// Invalid GATT user local index
#define GATT_INVALID_USER_LID           (0xFF)
/// Maximum number of handle that can be simultaneously read.
#define GATT_RD_MULTIPLE_MAX_NB_ATT     (8)

/// Invalid Attribute Index
#define GATT_INVALID_IDX                (0xFF)
/// Invalid Attribute Handle
#define GATT_INVALID_HDL                (0x0000)
/// Minimum Attribute Handle
#define GATT_MIN_HDL                    (0x0001)
/// Maximum Attribute Handle
#define GATT_MAX_HDL                    (0xFFFF)

/// Macro used to convert CPU integer define to LSB first 16-bits UUID
#if (!CPU_LE)
#define GATT_UUID_16_LSB(uuid)          (((uuid & 0xFF00) >> 8) | ((uuid & 0x00FF) << 8))
#define UUID_16_TO_ARRAY(uuid)          {(uuid & 0xFF00) >> 8, (uuid & 0x00FF)}
#else
#define GATT_UUID_16_LSB(uuid)          (uuid)
#define UUID_16_TO_ARRAY(uuid)          {(uuid & 0x00FF), (uuid & 0xFF00) >> 8}
#endif // CPU_LE

/// Length of Content Control ID
#define GATT_CCID_LEN                   (1)
/// Buffer Header length that must be reserved for GATT processing
#define GATT_BUFFER_HEADER_LEN          (L2CAP_BUFFER_HEADER_LEN + 7) // required for attribute packing
/// Buffer Tail length that must be reserved for GATT processing
#define GATT_BUFFER_TAIL_LEN            (L2CAP_BUFFER_TAIL_LEN)
/// Buffer Tail length that must be reserved for GATT Write signed processing
#define GATT_BUFFER_SIGN_TAIL_LEN       (L2CAP_BUFFER_TAIL_LEN + GATT_SIGNATURE_LEN)


/// extended characteristics
#define GATT_EXT_RELIABLE_WRITE         (0x0001)
#define GATT_EXT_WRITABLE_AUX           (0x0002)
#define GATT_EXT_RFU                    (0xFFFC)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

///    7    6    5   4   3    2    1    0
/// +-----+----+---+---+----+----+----+---+
/// | EXT | WS | I | N | WR | WC | RD | B |
/// +-----+----+---+---+----+----+----+---+
/// GATT Attribute properties Bit Field
enum gatt_prop_bf
{
    /// Broadcast descriptor present
    GATT_PROP_B_BIT          = 0x0001, 
    GATT_PROP_B_POS          = 0,      
    /// Read Access Mask
    GATT_PROP_RD_BIT          = 0x0002,
    GATT_PROP_RD_POS          = 1,     
    /// Write Command Enabled attribute Mask
    GATT_PROP_WC_BIT         = 0x0004, 
    GATT_PROP_WC_POS         = 2,      
    /// Write Request Enabled attribute Mask
    GATT_PROP_WR_BIT         = 0x0008, 
    GATT_PROP_WR_POS         = 3,      
    /// Notification Access Mask
    GATT_PROP_N_BIT          = 0x0010, 
    GATT_PROP_N_POS          = 4,      
    /// Indication Access Mask
    GATT_PROP_I_BIT          = 0x0020, 
    GATT_PROP_I_POS          = 5,      
    /// Write Signed Enabled attribute Mask
    GATT_PROP_WS_BIT         = 0x0040, 
    GATT_PROP_WS_POS         = 6,      
    /// Extended properties descriptor present
    GATT_PROP_EXT_BIT        = 0x0080, 
    GATT_PROP_EXT_POS        = 7,      
};


/// Common 16-bit Universal Unique Identifier
enum gatt_char
{
    GATT_INVALID_UUID                            = GATT_UUID_16_LSB(0x0000),

    /*----------------- SERVICES ---------------------*/
    /// Generic Access Profile
    GATT_SVC_GENERIC_ACCESS                      = GATT_UUID_16_LSB(0x1800),
    /// Attribute Profile
    GATT_SVC_GENERIC_ATTRIBUTE                   = GATT_UUID_16_LSB(0x1801),
    /// Immediate alert Service
    GATT_SVC_IMMEDIATE_ALERT                     = GATT_UUID_16_LSB(0x1802),
    /// Link Loss Service
    GATT_SVC_LINK_LOSS                           = GATT_UUID_16_LSB(0x1803),
    /// Tx Power Service
    GATT_SVC_TX_POWER                            = GATT_UUID_16_LSB(0x1804),
    /// Current Time Service Service
    GATT_SVC_CURRENT_TIME                        = GATT_UUID_16_LSB(0x1805),
    /// Reference Time Update Service
    GATT_SVC_REF_TIME_UPDATE                     = GATT_UUID_16_LSB(0x1806),
    /// Next DST Change Service
    GATT_SVC_NEXT_DST_CHANGE                     = GATT_UUID_16_LSB(0x1807),
    /// Glucose Service
    GATT_SVC_GLUCOSE                             = GATT_UUID_16_LSB(0x1808),
    /// Health Thermometer Service
    GATT_SVC_HEALTH_THERMOM                      = GATT_UUID_16_LSB(0x1809),
    /// Device Information Service
    GATT_SVC_DEVICE_INFO                         = GATT_UUID_16_LSB(0x180A),
    /// Heart Rate Service
    GATT_SVC_HEART_RATE                          = GATT_UUID_16_LSB(0x180D),
    /// Phone Alert Status Service
    GATT_SVC_PHONE_ALERT_STATUS                  = GATT_UUID_16_LSB(0x180E),
    /// Battery Service
    GATT_SVC_BATTERY_SERVICE                     = GATT_UUID_16_LSB(0x180F),
    /// Blood Pressure Service
    GATT_SVC_BLOOD_PRESSURE                      = GATT_UUID_16_LSB(0x1810),
    /// Alert Notification Service
    GATT_SVC_ALERT_NTF                           = GATT_UUID_16_LSB(0x1811),
    /// HID Service
    GATT_SVC_HID                                 = GATT_UUID_16_LSB(0x1812),
    /// Scan Parameters Service
    GATT_SVC_SCAN_PARAMETERS                     = GATT_UUID_16_LSB(0x1813),
    /// Running Speed and Cadence Service
    GATT_SVC_RUNNING_SPEED_CADENCE               = GATT_UUID_16_LSB(0x1814),
    /// Cycling Speed and Cadence Service
    GATT_SVC_CYCLING_SPEED_CADENCE               = GATT_UUID_16_LSB(0x1816),
    /// Cycling Power Service
    GATT_SVC_CYCLING_POWER                       = GATT_UUID_16_LSB(0x1818),
    /// Location and Navigation Service
    GATT_SVC_LOCATION_AND_NAVIGATION             = GATT_UUID_16_LSB(0x1819),
    /// Environmental Sensing Service
    GATT_SVC_ENVIRONMENTAL_SENSING               = GATT_UUID_16_LSB(0x181A),
    /// Body Composition Service
    GATT_SVC_BODY_COMPOSITION                    = GATT_UUID_16_LSB(0x181B),
    /// User Data Service
    GATT_SVC_USER_DATA                           = GATT_UUID_16_LSB(0x181C),
    /// Weight Scale Service
    GATT_SVC_WEIGHT_SCALE                        = GATT_UUID_16_LSB(0x181D),
    /// Bond Management Service
    GATT_SVC_BOND_MANAGEMENT                     = GATT_UUID_16_LSB(0x181E),
    /// Continuous Glucose Monitoring Service
    GATT_SVC_CONTINUOUS_GLUCOSE_MONITORING       = GATT_UUID_16_LSB(0x181F),
    /// Internet Protocol Support Service
    GATT_SVC_IP_SUPPORT                          = GATT_UUID_16_LSB(0x1820),
    /// Indoor Positioning Service
    GATT_SVC_INDOOR_POSITIONING                  = GATT_UUID_16_LSB(0x1821),
    /// Pulse Oximeter Service
    GATT_SVC_PULSE_OXIMETER                      = GATT_UUID_16_LSB(0x1822),
    /// HTTP Proxy Service
    GATT_SVC_HTTP_PROXY                          = GATT_UUID_16_LSB(0x1823),
    /// Transport Discovery Service
    GATT_SVC_TRANSPORT_DISCOVERY                 = GATT_UUID_16_LSB(0x1824),
    /// Object Transfer Service
    GATT_SVC_OBJECT_TRANSFER                     = GATT_UUID_16_LSB(0x1825),

    /// Mesh Provisioning Service
    GATT_SVC_MESH_PROVISIONING                   = GATT_UUID_16_LSB(0x1827),
    /// Mesh Proxy Service
    GATT_SVC_MESH_PROXY                          = GATT_UUID_16_LSB(0x1828),

    /// Volume Control Service
    GATT_SVC_VOLUME_CONTROL                      = GATT_UUID_16_LSB(0x8FD1),
    /// Volume Offset Control Service
    GATT_SVC_VOLUME_OFFSET_CONTROL               = GATT_UUID_16_LSB(0x8FD2),
    /// Audio Input Control Service
    GATT_SVC_AUDIO_INPUT_CONTROL                 = GATT_UUID_16_LSB(0x8FD3),
    /// Microphone Control Service
    GATT_SVC_MICROPHONE_CONTROL                  = GATT_UUID_16_LSB(0x8FD4),
    /// Telephone Bearer Service
    GATT_SVC_TELEPHONE_BEARER                    = GATT_UUID_16_LSB(0x8FD5),
    /// Generic Telephone Bearer Service
    GATT_SVC_GENERIC_TELEPHONE_BEARER            = GATT_UUID_16_LSB(0x8FD6),
    /// Media Control Service
    GATT_SVC_MEDIA_CONTROL                       = GATT_UUID_16_LSB(0x8FD7),
    /// Generic Media Control Service
    GATT_SVC_GENERIC_MEDIA_CONTROL               = GATT_UUID_16_LSB(0x8FD8),
    /// Published Audio Capabilities Service
    GATT_SVC_PUBLISHED_AUDIO_CAPA                = GATT_UUID_16_LSB(0x8FD9),
    /// Broadcast Audio Scan Service
    GATT_SVC_BCAST_AUDIO_SCAN                    = GATT_UUID_16_LSB(0x8FDA),
    /// Audio Stream Control Service
    GATT_SVC_AUDIO_STREAM_CTRL                   = GATT_UUID_16_LSB(0x8FDB),
    /// Coordinated Set Identification Service
    GATT_SVC_COORD_SET_IDENTIFICATION            = GATT_UUID_16_LSB(0x8FDC),
    /// Common Audio Service
    GATT_SVC_COMMON_AUDIO                        = GATT_UUID_16_LSB(0x8FDD),
    /// Broadcast audio Announcement service
    GATT_SVC_BCAST_AUDIO_ANNOUNCEMENT            = GATT_UUID_16_LSB(0x8FDE),
    /// Basic audio Announcement service
    GATT_SVC_BASIC_AUDIO_ANNOUNCEMENT            = GATT_UUID_16_LSB(0x8FDF),


    /*------------------- UNITS ---------------------*/
    /// No defined unit
    GATT_UNIT_UNITLESS                           = GATT_UUID_16_LSB(0x2700),
    /// Length Unit - Metre
    GATT_UNIT_METRE                              = GATT_UUID_16_LSB(0x2701),
    ///Mass unit - Kilogram
    GATT_UNIT_KG                                 = GATT_UUID_16_LSB(0x2702),
    ///Time unit - second
    GATT_UNIT_SECOND                             = GATT_UUID_16_LSB(0x2703),
    ///Electric current unit - Ampere
    GATT_UNIT_AMPERE                             = GATT_UUID_16_LSB(0x2704),
    ///Thermodynamic Temperature unit - Kelvin
    GATT_UNIT_KELVIN                             = GATT_UUID_16_LSB(0x2705),
    /// Amount of substance unit - mole
    GATT_UNIT_MOLE                               = GATT_UUID_16_LSB(0x2706),
    ///Luminous intensity unit - candela
    GATT_UNIT_CANDELA                            = GATT_UUID_16_LSB(0x2707),
    ///Area unit - square metres
    GATT_UNIT_SQ_METRE                           = GATT_UUID_16_LSB(0x2710),
    ///Colume unit - cubic metres
    GATT_UNIT_CUBIC_METRE                        = GATT_UUID_16_LSB(0x2710),
    ///Velocity unit - metres per second
    GATT_UNIT_METRE_PER_SECOND                   = GATT_UUID_16_LSB(0x2711),
    ///Acceleration unit - metres per second squared
    GATT_UNIT_METRES_PER_SEC_SQ                  = GATT_UUID_16_LSB(0x2712),
    ///Wavenumber unit - reciprocal metre
    GATT_UNIT_RECIPROCAL_METRE                   = GATT_UUID_16_LSB(0x2713),
    ///Density unit - kilogram per cubic metre
    GATT_UNIT_DENS_KG_PER_CUBIC_METRE            = GATT_UUID_16_LSB(0x2714),
    ///Surface density unit - kilogram per square metre
    GATT_UNIT_KG_PER_SQ_METRE                    = GATT_UUID_16_LSB(0x2715),
    ///Specific volume unit - cubic metre per kilogram
    GATT_UNIT_CUBIC_METRE_PER_KG                 = GATT_UUID_16_LSB(0x2716),
    ///Current density unit - ampere per square metre
    GATT_UNIT_AMPERE_PER_SQ_METRE                = GATT_UUID_16_LSB(0x2717),
    ///Magnetic field strength unit - Ampere per metre
    GATT_UNIT_AMPERE_PER_METRE                   = GATT_UUID_16_LSB(0x2718),
    ///Amount concentration unit - mole per cubic metre
    GATT_UNIT_MOLE_PER_CUBIC_METRE               = GATT_UUID_16_LSB(0x2719),
    ///Mass Concentration unit - kilogram per cubic metre
    GATT_UNIT_MASS_KG_PER_CUBIC_METRE            = GATT_UUID_16_LSB(0x271A),
    ///Luminance unit - candela per square metre
    GATT_UNIT_CANDELA_PER_SQ_METRE               = GATT_UUID_16_LSB(0x271B),
    ///Refractive index unit
    GATT_UNIT_REFRACTIVE_INDEX                   = GATT_UUID_16_LSB(0x271C),
    ///Relative permeability unit
    GATT_UNIT_RELATIVE_PERMEABILITY              = GATT_UUID_16_LSB(0x271D),
    ///Plane angle unit - radian
    GATT_UNIT_RADIAN                             = GATT_UUID_16_LSB(0x2720),
    ///Solid angle unit - steradian
    GATT_UNIT_STERADIAN                          = GATT_UUID_16_LSB(0x2721),
    ///Frequency unit - Hertz
    GATT_UNIT_HERTZ                              = GATT_UUID_16_LSB(0x2722),
    ///Force unit - Newton
    GATT_UNIT_NEWTON                             = GATT_UUID_16_LSB(0x2723),
    ///Pressure unit - Pascal
    GATT_UNIT_PASCAL                             = GATT_UUID_16_LSB(0x2724),
    ///Energy unit - Joule
    GATT_UNIT_JOULE                              = GATT_UUID_16_LSB(0x2725),
    ///Power unit - Watt
    GATT_UNIT_WATT                               = GATT_UUID_16_LSB(0x2726),
    ///electric Charge unit - Coulomb
    GATT_UNIT_COULOMB                            = GATT_UUID_16_LSB(0x2727),
    ///Electric potential difference - Volt
    GATT_UNIT_VOLT                               = GATT_UUID_16_LSB(0x2728),
    ///Capacitance unit - Farad
    GATT_UNIT_FARAD                              = GATT_UUID_16_LSB(0x2729),
    ///electric resistance unit - Ohm
    GATT_UNIT_OHM                                = GATT_UUID_16_LSB(0x272A),
    ///Electric conductance - Siemens
    GATT_UNIT_SIEMENS                            = GATT_UUID_16_LSB(0x272B),
    ///Magnetic flux unit - Weber
    GATT_UNIT_WEBER                              = GATT_UUID_16_LSB(0x272C),
    ///Magnetic flux density unit - Tesla
    GATT_UNIT_TESLA                              = GATT_UUID_16_LSB(0x272D),
    ///Inductance unit - Henry
    GATT_UNIT_HENRY                              = GATT_UUID_16_LSB(0x272E),
    ///Temperature unit - degree Celsius
    GATT_UNIT_CELSIUS                            = GATT_UUID_16_LSB(0x272F),
    ///Luminous flux unit - lumen
    GATT_UNIT_LUMEN                              = GATT_UUID_16_LSB(0x2730),
    ///Illuminance unit - lux
    GATT_UNIT_LUX                                = GATT_UUID_16_LSB(0x2731),
    ///Activity referred to a radionuclide unit - becquerel
    GATT_UNIT_BECQUEREL                          = GATT_UUID_16_LSB(0x2732),
    ///Absorbed dose unit - Gray
    GATT_UNIT_GRAY                               = GATT_UUID_16_LSB(0x2733),
    ///Dose equivalent unit - Sievert
    GATT_UNIT_SIEVERT                            = GATT_UUID_16_LSB(0x2734),
    ///Catalytic activity unit - Katal
    GATT_UNIT_KATAL                              = GATT_UUID_16_LSB(0x2735),
    ///Synamic viscosity unit - Pascal second
    GATT_UNIT_PASCAL_SECOND                      = GATT_UUID_16_LSB(0x2740),
    ///Moment of force unit - Newton metre
    GATT_UNIT_NEWTON_METRE                       = GATT_UUID_16_LSB(0x2741),
    ///surface tension unit - Newton per metre
    GATT_UNIT_NEWTON_PER_METRE                   = GATT_UUID_16_LSB(0x2742),
    ///Angular velocity unit - radian per second
    GATT_UNIT_RADIAN_PER_SECOND                  = GATT_UUID_16_LSB(0x2743),
    ///Angular acceleration unit - radian per second squared
    GATT_UNIT_RADIAN_PER_SECOND_SQ               = GATT_UUID_16_LSB(0x2744),
    ///Heat flux density unit - Watt per square metre
    GATT_UNIT_WGATT_PER_SQ_METRE                  = GATT_UUID_16_LSB(0x2745),
    ///HEat capacity unit - Joule per Kelvin
    GATT_UNIT_JOULE_PER_KELVIN                   = GATT_UUID_16_LSB(0x2746),
    ///Specific heat capacity unit - Joule per kilogram kelvin
    GATT_UNIT_JOULE_PER_KG_KELVIN                = GATT_UUID_16_LSB(0x2747),
    ///Specific Energy unit - Joule per kilogram
    GATT_UNIT_JOULE_PER_KG                       = GATT_UUID_16_LSB(0x2748),
    ///Thermal conductivity - Watt per metre Kelvin
    GATT_UNIT_WGATT_PER_METRE_KELVIN              = GATT_UUID_16_LSB(0x2749),
    ///Energy Density unit - joule per cubic metre
    GATT_UNIT_JOULE_PER_CUBIC_METRE              = GATT_UUID_16_LSB(0x274A),
    ///Electric field strength unit - volt per metre
    GATT_UNIT_VOLT_PER_METRE                     = GATT_UUID_16_LSB(0x274B),
    ///Electric charge density unit - coulomb per cubic metre
    GATT_UNIT_COULOMB_PER_CUBIC_METRE            = GATT_UUID_16_LSB(0x274C),
    ///Surface charge density unit - coulomb per square metre
    GATT_UNIT_SURF_COULOMB_PER_SQ_METRE          = GATT_UUID_16_LSB(0x274D),
    ///Electric flux density unit - coulomb per square metre
    GATT_UNIT_FLUX_COULOMB_PER_SQ_METRE          = GATT_UUID_16_LSB(0x274E),
    ///Permittivity unit - farad per metre
    GATT_UNIT_FARAD_PER_METRE                    = GATT_UUID_16_LSB(0x274F),
    ///Permeability unit - henry per metre
    GATT_UNIT_HENRY_PER_METRE                    = GATT_UUID_16_LSB(0x2750),
    ///Molar energy unit - joule per mole
    GATT_UNIT_JOULE_PER_MOLE                     = GATT_UUID_16_LSB(0x2751),
    ///Molar entropy unit - joule per mole kelvin
    GATT_UNIT_JOULE_PER_MOLE_KELVIN              = GATT_UUID_16_LSB(0x2752),
    ///Exposure unit - coulomb per kilogram
    GATT_UNIT_COULOMB_PER_KG                     = GATT_UUID_16_LSB(0x2753),
    ///Absorbed dose rate unit - gray per second
    GATT_UNIT_GRAY_PER_SECOND                    = GATT_UUID_16_LSB(0x2754),
    ///Radiant intensity unit - watt per steradian
    GATT_UNIT_WGATT_PER_STERADIAN                 = GATT_UUID_16_LSB(0x2755),
    ///Radiance unit - watt per square meter steradian
    GATT_UNIT_WGATT_PER_SQ_METRE_STERADIAN        = GATT_UUID_16_LSB(0x2756),
    ///Catalytic activity concentration unit - katal per cubic metre
    GATT_UNIT_KATAL_PER_CUBIC_METRE              = GATT_UUID_16_LSB(0x2757),
    ///Time unit - minute
    GATT_UNIT_MINUTE                             = GATT_UUID_16_LSB(0x2760),
    ///Time unit - hour
    GATT_UNIT_HOUR                               = GATT_UUID_16_LSB(0x2761),
    ///Time unit - day
    GATT_UNIT_DAY                                = GATT_UUID_16_LSB(0x2762),
    ///Plane angle unit - degree
    GATT_UNIT_ANGLE_DEGREE                       = GATT_UUID_16_LSB(0x2763),
    ///Plane angle unit - minute
    GATT_UNIT_ANGLE_MINUTE                       = GATT_UUID_16_LSB(0x2764),
    ///Plane angle unit - second
    GATT_UNIT_ANGLE_SECOND                       = GATT_UUID_16_LSB(0x2765),
    ///Area unit - hectare
    GATT_UNIT_HECTARE                            = GATT_UUID_16_LSB(0x2766),
    ///Volume unit - litre
    GATT_UNIT_LITRE                              = GATT_UUID_16_LSB(0x2767),
    ///Mass unit - tonne
    GATT_UNIT_TONNE                              = GATT_UUID_16_LSB(0x2768),
    ///Pressure unit - bar
    GATT_UNIT_BAR                                = GATT_UUID_16_LSB(0x2780),
    ///Pressure unit - millimetre of mercury
    GATT_UNIT_MM_MERCURY                         = GATT_UUID_16_LSB(0x2781),
    ///Length unit - angstrom
    GATT_UNIT_ANGSTROM                           = GATT_UUID_16_LSB(0x2782),
    ///Length unit - nautical mile
    GATT_UNIT_NAUTICAL_MILE                      = GATT_UUID_16_LSB(0x2783),
    ///Area unit - barn
    GATT_UNIT_BARN                               = GATT_UUID_16_LSB(0x2784),
    ///Velocity unit - knot
    GATT_UNIT_KNOT                               = GATT_UUID_16_LSB(0x2785),
    ///Logarithmic radio quantity unit - neper
    GATT_UNIT_NEPER                              = GATT_UUID_16_LSB(0x2786),
    ///Logarithmic radio quantity unit - bel
    GATT_UNIT_BEL                                = GATT_UUID_16_LSB(0x2787),
    ///Length unit - yard
    GATT_UNIT_YARD                               = GATT_UUID_16_LSB(0x27A0),
    ///Length unit - parsec
    GATT_UNIT_PARSEC                             = GATT_UUID_16_LSB(0x27A1),
    ///length unit - inch
    GATT_UNIT_INCH                               = GATT_UUID_16_LSB(0x27A2),
    ///length unit - foot
    GATT_UNIT_FOOT                               = GATT_UUID_16_LSB(0x27A3),
    ///length unit - mile
    GATT_UNIT_MILE                               = GATT_UUID_16_LSB(0x27A4),
    ///pressure unit - pound-force per square inch
    GATT_UNIT_POUND_FORCE_PER_SQ_INCH            = GATT_UUID_16_LSB(0x27A5),
    ///velocity unit - kilometre per hour
    GATT_UNIT_KM_PER_HOUR                        = GATT_UUID_16_LSB(0x27A6),
    ///velocity unit - mile per hour
    GATT_UNIT_MILE_PER_HOUR                      = GATT_UUID_16_LSB(0x27A7),
    ///angular velocity unit - revolution per minute
    GATT_UNIT_REVOLUTION_PER_MINUTE              = GATT_UUID_16_LSB(0x27A8),
    ///energy unit - gram calorie
    GATT_UNIT_GRAM_CALORIE                       = GATT_UUID_16_LSB(0x27A9),
    ///energy unit - kilogram calorie
    GATT_UNIT_KG_CALORIE                         = GATT_UUID_16_LSB(0x27AA),
    /// energy unit - kilowatt hour
    GATT_UNIT_KILOWGATT_HOUR                      = GATT_UUID_16_LSB(0x27AB),
    ///thermodynamic temperature unit - degree Fahrenheit
    GATT_UNIT_FAHRENHEIT                         = GATT_UUID_16_LSB(0x27AC),
    ///percentage
    GATT_UNIT_PERCENTAGE                         = GATT_UUID_16_LSB(0x27AD),
    ///per mille
    GATT_UNIT_PER_MILLE                          = GATT_UUID_16_LSB(0x27AE),
    ///period unit - beats per minute)
    GATT_UNIT_BEATS_PER_MINUTE                   = GATT_UUID_16_LSB(0x27AF),
    ///electric charge unit - ampere hours
    GATT_UNIT_AMPERE_HOURS                       = GATT_UUID_16_LSB(0x27B0),
    ///mass density unit - milligram per decilitre
    GATT_UNIT_MILLIGRAM_PER_DECILITRE            = GATT_UUID_16_LSB(0x27B1),
    ///mass density unit - millimole per litre
    GATT_UNIT_MILLIMOLE_PER_LITRE                = GATT_UUID_16_LSB(0x27B2),
    ///time unit - year
    GATT_UNIT_YEAR                               = GATT_UUID_16_LSB(0x27B3),
    ////time unit - month
    GATT_UNIT_MONTH                              = GATT_UUID_16_LSB(0x27B4),


    /*---------------- DECLARATIONS -----------------*/
    /// Primary service Declaration
    GATT_DECL_PRIMARY_SERVICE                     = GATT_UUID_16_LSB(0x2800),
    /// Secondary service Declaration
    GATT_DECL_SECONDARY_SERVICE                   = GATT_UUID_16_LSB(0x2801),
    /// Include Declaration
    GATT_DECL_INCLUDE                             = GATT_UUID_16_LSB(0x2802),
    /// Characteristic Declaration
    GATT_DECL_CHARACTERISTIC                      = GATT_UUID_16_LSB(0x2803),


    /*----------------- DESCRIPTORS -----------------*/
    /// Characteristic extended properties
    GATT_DESC_CHAR_EXT_PROPERTIES                 = GATT_UUID_16_LSB(0x2900),
    /// Characteristic user description
    GATT_DESC_CHAR_USER_DESCRIPTION               = GATT_UUID_16_LSB(0x2901),
    /// Client characteristic configuration
    GATT_DESC_CLIENT_CHAR_CFG                     = GATT_UUID_16_LSB(0x2902),
    /// Server characteristic configuration
    GATT_DESC_SERVER_CHAR_CFG                     = GATT_UUID_16_LSB(0x2903),
    /// Characteristic Presentation Format
    GATT_DESC_CHAR_PRES_FORMAT                    = GATT_UUID_16_LSB(0x2904),
    /// Characteristic Aggregate Format
    GATT_DESC_CHAR_AGGREGATE_FORMAT               = GATT_UUID_16_LSB(0x2905),
    /// Valid Range
    GATT_DESC_VALID_RANGE                         = GATT_UUID_16_LSB(0x2906),
    /// External Report Reference
    GATT_DESC_EXT_REPORT_REF                      = GATT_UUID_16_LSB(0x2907),
    /// Report Reference
    GATT_DESC_REPORT_REF                          = GATT_UUID_16_LSB(0x2908),
    /// Environmental Sensing Configuration
    GATT_DESC_ES_CONFIGURATION                    = GATT_UUID_16_LSB(0x290B),
    /// Environmental Sensing Measurement
    GATT_DESC_ES_MEASUREMENT                      = GATT_UUID_16_LSB(0x290C),
    /// Environmental Sensing Trigger Setting
    GATT_DESC_ES_TRIGGER_SETTING                  = GATT_UUID_16_LSB(0x290D),


    /*--------------- CHARACTERISTICS ---------------*/
    /// Device name
    GATT_CHAR_DEVICE_NAME                        = GATT_UUID_16_LSB(0x2A00),
    /// Appearance
    GATT_CHAR_APPEARANCE                         = GATT_UUID_16_LSB(0x2A01),
    /// Privacy flag
    GATT_CHAR_PRIVACY_FLAG                       = GATT_UUID_16_LSB(0x2A02),
    /// Reconnection address
    GATT_CHAR_RECONNECTION_ADDR                  = GATT_UUID_16_LSB(0x2A03),
    /// Peripheral preferred connection parameters
    GATT_CHAR_PERIPH_PREF_CON_PARAM              = GATT_UUID_16_LSB(0x2A04),
    /// Service handles changed
    GATT_CHAR_SERVICE_CHANGED                    = GATT_UUID_16_LSB(0x2A05),
    /// Alert Level characteristic
    GATT_CHAR_ALERT_LEVEL                        = GATT_UUID_16_LSB(0x2A06),
    /// Tx Power Level
    GATT_CHAR_TX_POWER_LEVEL                     = GATT_UUID_16_LSB(0x2A07),
    /// Date Time
    GATT_CHAR_DATE_TIME                          = GATT_UUID_16_LSB(0x2A08),
    /// Day of Week
    GATT_CHAR_DAY_WEEK                           = GATT_UUID_16_LSB(0x2A09),
    /// Day Date Time
    GATT_CHAR_DAY_DATE_TIME                      = GATT_UUID_16_LSB(0x2A0A),
    /// Exact time 256
    GATT_CHAR_EXACT_TIME_256                     = GATT_UUID_16_LSB(0x2A0C),
    /// DST Offset
    GATT_CHAR_DST_OFFSET                         = GATT_UUID_16_LSB(0x2A0D),
    /// Time zone
    GATT_CHAR_TIME_ZONE                          = GATT_UUID_16_LSB(0x2A0E),
    /// Local time Information
    GATT_CHAR_LOCAL_TIME_INFO                    = GATT_UUID_16_LSB(0x2A0F),
    /// Time with DST
    GATT_CHAR_TIME_WITH_DST                      = GATT_UUID_16_LSB(0x2A11),
    /// Time Accuracy
    GATT_CHAR_TIME_ACCURACY                      = GATT_UUID_16_LSB(0x2A12),
    ///Time Source
    GATT_CHAR_TIME_SOURCE                        = GATT_UUID_16_LSB(0x2A13),
    /// Reference Time Information
    GATT_CHAR_REFERENCE_TIME_INFO                = GATT_UUID_16_LSB(0x2A14),
    /// Time Update Control Point
    GATT_CHAR_TIME_UPDATE_CNTL_POINT             = GATT_UUID_16_LSB(0x2A16),
    /// Time Update State
    GATT_CHAR_TIME_UPDATE_STATE                  = GATT_UUID_16_LSB(0x2A17),
    /// Glucose Measurement
    GATT_CHAR_GLUCOSE_MEAS                       = GATT_UUID_16_LSB(0x2A18),
    /// Battery Level
    GATT_CHAR_BATTERY_LEVEL                      = GATT_UUID_16_LSB(0x2A19),
    /// Temperature Measurement
    GATT_CHAR_TEMPERATURE_MEAS                   = GATT_UUID_16_LSB(0x2A1C),
    /// Temperature Type
    GATT_CHAR_TEMPERATURE_TYPE                   = GATT_UUID_16_LSB(0x2A1D),
    /// Intermediate Temperature
    GATT_CHAR_INTERMED_TEMPERATURE               = GATT_UUID_16_LSB(0x2A1E),
    /// Measurement Interval
    GATT_CHAR_MEAS_INTERVAL                      = GATT_UUID_16_LSB(0x2A21),
    /// Boot Keyboard Input Report
    GATT_CHAR_BOOT_KB_IN_REPORT                  = GATT_UUID_16_LSB(0x2A22),
    /// System ID
    GATT_CHAR_SYS_ID                             = GATT_UUID_16_LSB(0x2A23),
    /// Model Number String
    GATT_CHAR_MODEL_NB                           = GATT_UUID_16_LSB(0x2A24),
    /// Serial Number String
    GATT_CHAR_SERIAL_NB                          = GATT_UUID_16_LSB(0x2A25),
    /// Firmware Revision String
    GATT_CHAR_FW_REV                             = GATT_UUID_16_LSB(0x2A26),
    /// Hardware revision String
    GATT_CHAR_HW_REV                             = GATT_UUID_16_LSB(0x2A27),
    /// Software Revision String
    GATT_CHAR_SW_REV                             = GATT_UUID_16_LSB(0x2A28),
    /// Manufacturer Name String
    GATT_CHAR_MANUF_NAME                         = GATT_UUID_16_LSB(0x2A29),
    /// IEEE Regulatory Certification Data List
    GATT_CHAR_IEEE_CERTIF                        = GATT_UUID_16_LSB(0x2A2A),
    /// CT Time
    GATT_CHAR_CT_TIME                            = GATT_UUID_16_LSB(0x2A2B),
    /// Magnetic Declination
    GATT_CHAR_MAGN_DECLINE                       = GATT_UUID_16_LSB(0x2A2C),
    /// Scan Refresh
    GATT_CHAR_SCAN_REFRESH                       = GATT_UUID_16_LSB(0x2A31),
    /// Boot Keyboard Output Report
    GATT_CHAR_BOOT_KB_OUT_REPORT                 = GATT_UUID_16_LSB(0x2A32),
    /// Boot Mouse Input Report
    GATT_CHAR_BOOT_MOUSE_IN_REPORT               = GATT_UUID_16_LSB(0x2A33),
    /// Glucose Measurement Context
    GATT_CHAR_GLUCOSE_MEAS_CTX                   = GATT_UUID_16_LSB(0x2A34),
    /// Blood Pressure Measurement
    GATT_CHAR_BLOOD_PRESSURE_MEAS                = GATT_UUID_16_LSB(0x2A35),
    /// Intermediate Cuff Pressure
    GATT_CHAR_INTERMEDIATE_CUFF_PRESSURE         = GATT_UUID_16_LSB(0x2A36),
    /// Heart Rate Measurement
    GATT_CHAR_HEART_RATE_MEAS                    = GATT_UUID_16_LSB(0x2A37),
    /// Body Sensor Location
    GATT_CHAR_BODY_SENSOR_LOCATION               = GATT_UUID_16_LSB(0x2A38),
    /// Heart Rate Control Point
    GATT_CHAR_HEART_RATE_CNTL_POINT              = GATT_UUID_16_LSB(0x2A39),
    /// Alert Status
    GATT_CHAR_ALERT_STATUS                       = GATT_UUID_16_LSB(0x2A3F),
    /// Ringer Control Point
    GATT_CHAR_RINGER_CNTL_POINT                  = GATT_UUID_16_LSB(0x2A40),
    /// Ringer Setting
    GATT_CHAR_RINGER_SETTING                     = GATT_UUID_16_LSB(0x2A41),
    /// Alert Category ID Bit Mask
    GATT_CHAR_ALERT_CAT_ID_BIT_MASK              = GATT_UUID_16_LSB(0x2A42),
    /// Alert Category ID
    GATT_CHAR_ALERT_CAT_ID                       = GATT_UUID_16_LSB(0x2A43),
    /// Alert Notification Control Point
    GATT_CHAR_ALERT_NTF_CTNL_PT                  = GATT_UUID_16_LSB(0x2A44),
    /// Unread Alert Status
    GATT_CHAR_UNREAD_ALERT_STATUS                = GATT_UUID_16_LSB(0x2A45),
    /// New Alert
    GATT_CHAR_NEW_ALERT                          = GATT_UUID_16_LSB(0x2A46),
    /// Supported New Alert Category
    GATT_CHAR_SUP_NEW_ALERT_CAT                  = GATT_UUID_16_LSB(0x2A47),
    /// Supported Unread Alert Category
    GATT_CHAR_SUP_UNREAD_ALERT_CAT               = GATT_UUID_16_LSB(0x2A48),
    /// Blood Pressure Feature
    GATT_CHAR_BLOOD_PRESSURE_FEATURE             = GATT_UUID_16_LSB(0x2A49),
    /// HID Information
    GATT_CHAR_HID_INFO                           = GATT_UUID_16_LSB(0x2A4A),
    /// Report Map
    GATT_CHAR_REPORT_MAP                         = GATT_UUID_16_LSB(0x2A4B),
    /// HID Control Point
    GATT_CHAR_HID_CTNL_PT                        = GATT_UUID_16_LSB(0x2A4C),
    /// Report
    GATT_CHAR_REPORT                             = GATT_UUID_16_LSB(0x2A4D),
    /// Protocol Mode
    GATT_CHAR_PROTOCOL_MODE                      = GATT_UUID_16_LSB(0x2A4E),
    /// Scan Interval Window
    GATT_CHAR_SCAN_INTV_WD                       = GATT_UUID_16_LSB(0x2A4F),
    /// PnP ID
    GATT_CHAR_PNP_ID                             = GATT_UUID_16_LSB(0x2A50),
    /// Glucose Feature
    GATT_CHAR_GLUCOSE_FEATURE                    = GATT_UUID_16_LSB(0x2A51),
    /// Record access control point
    GATT_CHAR_REC_ACCESS_CTRL_PT                 = GATT_UUID_16_LSB(0x2A52),
    /// RSC Measurement
    GATT_CHAR_RSC_MEAS                           = GATT_UUID_16_LSB(0x2A53),
    /// RSC Feature
    GATT_CHAR_RSC_FEAT                           = GATT_UUID_16_LSB(0x2A54),
    /// SC Control Point
    GATT_CHAR_SC_CNTL_PT                         = GATT_UUID_16_LSB(0x2A55),
    /// CSC Measurement
    GATT_CHAR_CSC_MEAS                           = GATT_UUID_16_LSB(0x2A5B),
    /// CSC Feature
    GATT_CHAR_CSC_FEAT                           = GATT_UUID_16_LSB(0x2A5C),
    /// Sensor Location
    GATT_CHAR_SENSOR_LOC                         = GATT_UUID_16_LSB(0x2A5D),
    /// PLX Spot-Check Measurement
    GATT_CHAR_PLX_SPOT_CHECK_MEASUREMENT_LOC     = GATT_UUID_16_LSB(0x2A5E),
    /// PLX Continuous Measurement
    GATT_CHAR_PLX_CONTINUOUS_MEASUREMENT_LOC     = GATT_UUID_16_LSB(0x2A5F),
    /// PLX Features
    GATT_CHAR_PLX_FEATURES_LOC                   = GATT_UUID_16_LSB(0x2A60),
    /// CP Measurement
    GATT_CHAR_CP_MEAS                            = GATT_UUID_16_LSB(0x2A63),
    /// CP Vector
    GATT_CHAR_CP_VECTOR                          = GATT_UUID_16_LSB(0x2A64),
    /// CP Feature
    GATT_CHAR_CP_FEAT                            = GATT_UUID_16_LSB(0x2A65),
    /// CP Control Point
    GATT_CHAR_CP_CNTL_PT                         = GATT_UUID_16_LSB(0x2A66),
    /// Location and Speed
    GATT_CHAR_LOC_SPEED                          = GATT_UUID_16_LSB(0x2A67),
    /// Navigation
    GATT_CHAR_NAVIGATION                         = GATT_UUID_16_LSB(0x2A68),
    /// Position Quality
    GATT_CHAR_POS_QUALITY                        = GATT_UUID_16_LSB(0x2A69),
    /// LN Feature
    GATT_CHAR_LN_FEAT                            = GATT_UUID_16_LSB(0x2A6A),
    /// LN Control Point
    GATT_CHAR_LN_CNTL_PT                         = GATT_UUID_16_LSB(0x2A6B),
    /// Elevation
    GATT_CHAR_ELEVATION                          = GATT_UUID_16_LSB(0x2A6C),
    /// Pressure
    GATT_CHAR_PRESSURE                           = GATT_UUID_16_LSB(0x2A6D),
    /// Temperature
    GATT_CHAR_TEMPERATURE                        = GATT_UUID_16_LSB(0x2A6E),
    /// Humidity
    GATT_CHAR_HUMIDITY                           = GATT_UUID_16_LSB(0x2A6F),
    /// True Wind Speed
    GATT_CHAR_TRUE_WIND_SPEED                    = GATT_UUID_16_LSB(0x2A70),
    /// True Wind Direction
    GATT_CHAR_TRUE_WIND_DIR                      = GATT_UUID_16_LSB(0x2A71),
    /// Apparent Wind Speed
    GATT_CHAR_APRNT_WIND_SPEED                   = GATT_UUID_16_LSB(0x2A72),
    /// Apparent Wind Direction
    GATT_CHAR_APRNT_WIND_DIRECTION               = GATT_UUID_16_LSB(0x2A73),
    /// Gust Factor
    GATT_CHAR_GUST_FACTOR                        = GATT_UUID_16_LSB(0x2A74),
    /// Pollen Concentration
    GATT_CHAR_POLLEN_CONC                        = GATT_UUID_16_LSB(0x2A75),
    /// UV Index
    GATT_CHAR_UV_INDEX                           = GATT_UUID_16_LSB(0x2A76),
    /// Irradiance
    GATT_CHAR_IRRADIANCE                         = GATT_UUID_16_LSB(0x2A77),
    /// Rainfall
    GATT_CHAR_RAINFALL                           = GATT_UUID_16_LSB(0x2A78),
    /// Wind Chill
    GATT_CHAR_WIND_CHILL                         = GATT_UUID_16_LSB(0x2A79),
    /// Heat Index
    GATT_CHAR_HEAT_INDEX                         = GATT_UUID_16_LSB(0x2A7A),
    /// Dew Point
    GATT_CHAR_DEW_POINT                          = GATT_UUID_16_LSB(0x2A7B),
    /// Descriptor Value Changed
    GATT_CHAR_DESCRIPTOR_VALUE_CHANGED           = GATT_UUID_16_LSB(0x2A7D),
    /// Aerobic Heart Rate Lower Limit
    GATT_CHAR_AEROBIC_HEART_RATE_LOW_LIM         = GATT_UUID_16_LSB(0x2A7E),
    /// Aerobic Threshhold
    GATT_CHAR_AEROBIC_THR                        = GATT_UUID_16_LSB(0x2A7F),
    /// Age
    GATT_CHAR_AGE                                = GATT_UUID_16_LSB(0x2A80),
    /// Anaerobic Heart Rate Lower Limit
    GATT_CHAR_ANAERO_HEART_RATE_LOW_LIM          = GATT_UUID_16_LSB(0x2A81),
    /// Anaerobic Heart Rate Upper Limit
    GATT_CHAR_ANAERO_HEART_RATE_UP_LIM           = GATT_UUID_16_LSB(0x2A82),
    /// Anaerobic Threshhold
    GATT_CHAR_ANAERO_THR                         = GATT_UUID_16_LSB(0x2A83),
    /// Aerobic Heart Rate Upper Limit
    GATT_CHAR_AEROBIC_HEART_RATE_UP_LIM          = GATT_UUID_16_LSB(0x2A84),
    /// Date Of Birth
    GATT_CHAR_DATE_OF_BIRTH                      = GATT_UUID_16_LSB(0x2A85),
    /// Date Of Threshold Assessment
    GATT_CHAR_DATE_OF_THR_ASSESS                 = GATT_UUID_16_LSB(0x2A86),
    /// Email Address
    GATT_CHAR_EMAIL_ADDRESS                      = GATT_UUID_16_LSB(0x2A87),
    /// Fat Burn Heart Rate Lower Limit
    GATT_CHAR_FAT_BURN_HEART_RATE_LOW_LIM        = GATT_UUID_16_LSB(0x2A88),
    /// Fat Burn Heart Rate Upper Limit
    GATT_CHAR_FAT_BURN_HEART_RATE_UP_LIM         = GATT_UUID_16_LSB(0x2A89),
    /// First Name
    GATT_CHAR_FIRST_NAME                         = GATT_UUID_16_LSB(0x2A8A),
    /// Five Zone Heart Rate Limits
    GATT_CHAR_FIVE_ZONE_HEART_RATE_LIMITS        = GATT_UUID_16_LSB(0x2A8B),
    /// Gender
    GATT_CHAR_GENDER                             = GATT_UUID_16_LSB(0x2A8C),
    /// Max Heart Rate
    GATT_CHAR_MAX_HEART_RATE                     = GATT_UUID_16_LSB(0x2A8D),
    /// Height
    GATT_CHAR_HEIGHT                             = GATT_UUID_16_LSB(0x2A8E),
    /// Hip Circumference
    GATT_CHAR_HIP_CIRCUMFERENCE                  = GATT_UUID_16_LSB(0x2A8F),
    /// Last Name
    GATT_CHAR_LAST_NAME                          = GATT_UUID_16_LSB(0x2A90),
    /// Maximum Recommended Heart Rate
    GATT_CHAR_MAX_RECO_HEART_RATE                = GATT_UUID_16_LSB(0x2A91),
    /// Resting Heart Rate
    GATT_CHAR_RESTING_HEART_RATE                 = GATT_UUID_16_LSB(0x2A92),
    /// Sport Type For Aerobic And Anaerobic Thresholds
    GATT_CHAR_SPORT_TYPE_FOR_AERO_ANAREO_THRS    = GATT_UUID_16_LSB(0x2A93),
    /// Three Zone Heart Rate Limits
    GATT_CHAR_THREE_ZONE_HEART_RATE_LIMITS       = GATT_UUID_16_LSB(0x2A94),
    /// Two Zone Heart Rate Limit
    GATT_CHAR_TWO_ZONE_HEART_RATE_LIMIT          = GATT_UUID_16_LSB(0x2A95),
    /// Vo2 Max
    GATT_CHAR_VO2_MAX                            = GATT_UUID_16_LSB(0x2A96),
    /// Waist Circumference
    GATT_CHAR_WAIST_CIRCUMFERENCE                = GATT_UUID_16_LSB(0x2A97),
    /// Weight
    GATT_CHAR_WEIGHT                             = GATT_UUID_16_LSB(0x2A98),
    /// Database Change Increment
    GATT_CHAR_DB_CHG_INCREMENT                   = GATT_UUID_16_LSB(0x2A99),
    /// User Index
    GATT_CHAR_USER_INDEX                         = GATT_UUID_16_LSB(0x2A9A),
    /// Body Composition Feature
    GATT_CHAR_BODY_COMPOSITION_FEATURE           = GATT_UUID_16_LSB(0x2A9B),
    /// Body Composition Measurement
    GATT_CHAR_BODY_COMPOSITION_MEASUREMENT       = GATT_UUID_16_LSB(0x2A9C),
    /// Weight Measurement
    GATT_CHAR_WEIGHT_MEASUREMENT                 = GATT_UUID_16_LSB(0x2A9D),
    /// Weight Scale Feature
    GATT_CHAR_WEIGHT_SCALE_FEATURE               = GATT_UUID_16_LSB(0x2A9E),
    /// User Control Point
    GATT_CHAR_USER_CONTROL_POINT                 = GATT_UUID_16_LSB(0x2A9F),
    /// Flux Density - 2D
    GATT_CHAR_MAGN_FLUX_2D                       = GATT_UUID_16_LSB(0x2AA0),
    /// Magnetic Flux Density - 3D
    GATT_CHAR_MAGN_FLUX_3D                       = GATT_UUID_16_LSB(0x2AA1),
    /// Language string
    GATT_CHAR_LANGUAGE                           = GATT_UUID_16_LSB(0x2AA2),
    /// Barometric Pressure Trend
    GATT_CHAR_BAR_PRES_TREND                     = GATT_UUID_16_LSB(0x2AA3),
    /// Central Address Resolution Support
    GATT_CHAR_CTL_ADDR_RESOL_SUPP                = GATT_UUID_16_LSB(0x2AA6),
    /// CGM Measurement
    GATT_CHAR_CGM_MEASUREMENT                    = GATT_UUID_16_LSB(0x2AA7),
    /// CGM Features
    GATT_CHAR_CGM_FEATURES                       = GATT_UUID_16_LSB(0x2AA8),
    /// CGM Status
    GATT_CHAR_CGM_STATUS                         = GATT_UUID_16_LSB(0x2AA9),
    /// CGM Session Start
    GATT_CHAR_CGM_SESSION_START                  = GATT_UUID_16_LSB(0x2AAA),
    /// CGM Session Run
    GATT_CHAR_CGM_SESSION_RUN                    = GATT_UUID_16_LSB(0x2AAB),
    /// CGM Specific Ops Control Point
    GATT_CHAR_CGM_SPECIFIC_OPS_CTRL_PT           = GATT_UUID_16_LSB(0x2AAC),

    /// Object Transfer Service - Feature characteristic
    GATT_CHAR_OTS_FEATURE                        = GATT_UUID_16_LSB(0x2ABD),
    /// Object Transfer Service - Object Name characteristic
    GATT_CHAR_OTS_OBJECT_NAME                    = GATT_UUID_16_LSB(0x2ABE),
    /// Object Transfer Service - Object Type characteristic
    GATT_CHAR_OTS_OBJECT_TYPE                    = GATT_UUID_16_LSB(0x2ABF),
    /// Object Transfer Service - Object Size characteristic
    GATT_CHAR_OTS_OBJECT_SIZE                    = GATT_UUID_16_LSB(0x2AC0),
    /// Object Transfer Service - Object First-Created characteristic
    GATT_CHAR_OTS_OBJECT_FIRST_CRAETED           = GATT_UUID_16_LSB(0x2AC1),
    /// Object Transfer Service - Object Last-Modified characteristic
    GATT_CHAR_OTS_OBJECT_LAST_MODIFIED           = GATT_UUID_16_LSB(0x2AC2),
    /// Object Transfer Service - Object ID characteristic
    GATT_CHAR_OTS_OBJECT_ID                      = GATT_UUID_16_LSB(0x2AC3),
    /// Object Transfer Service - Object Properties characteristic
    GATT_CHAR_OTS_OBJECT_PROPERTIES              = GATT_UUID_16_LSB(0x2AC4),
    /// Object Transfer Service - Object Action Control Point characteristic
    GATT_CHAR_OTS_OACP                           = GATT_UUID_16_LSB(0x2AC5),
    /// Object Transfer Service - Object List Control Point characteristic
    GATT_CHAR_OTS_OLCP                           = GATT_UUID_16_LSB(0x2AC6),
    /// Object Transfer Service - Object List Filter characteristic
    GATT_CHAR_OTS_OBJECT_LIST_FILTER             = GATT_UUID_16_LSB(0x2AC7),
    /// Object Transfer Service - Object Changed characteristic
    GATT_CHAR_OTS_OBJECT_CHANGED                 = GATT_UUID_16_LSB(0x2AC8),

    /// Resolvable Private Address only
    GATT_CHAR_RSLV_PRIV_ADDR_ONLY                = GATT_UUID_16_LSB(0x2AC9),

    /// Mesh Provisioning Data In
    GATT_CHAR_MESH_PROV_DATA_IN                  = GATT_UUID_16_LSB(0x2ADB),
    /// Mesh Provisioning Data Out
    GATT_CHAR_MESH_PROV_DATA_OUT                 = GATT_UUID_16_LSB(0x2ADC),
    /// Mesh Proxy Data In
    GATT_CHAR_MESH_PROXY_DATA_IN                 = GATT_UUID_16_LSB(0x2ADD),
    /// Mesh Proxy Data Out
    GATT_CHAR_MESH_PROXY_DATA_OUT                = GATT_UUID_16_LSB(0x2ADE),

    /// Volume Control Service - Volume State characteristic
    GATT_CHAR_VOLUME_STATE                       = GATT_UUID_16_LSB(0x8FB9),
    /// Volume Control Service - Volume Control Point characteristic
    GATT_CHAR_VOLUME_CP                          = GATT_UUID_16_LSB(0x8FBA),
    /// Volume Control Service - Volume Flags characteristic
    GATT_CHAR_VOLUME_FLAGS                       = GATT_UUID_16_LSB(0x8FBB),

    /// Microphone Control Service - Mute characteristic
    GATT_CHAR_MUTE                               = GATT_UUID_16_LSB(0x8FE1),

    /// Volume Offset Control Service - Offset State characteristic
    GATT_CHAR_OFFSET_STATE                       = GATT_UUID_16_LSB(0x8FBC),
    /// Volume Offset Control Service - Audio Location characteristic
    GATT_CHAR_AUDIO_LOC                          = GATT_UUID_16_LSB(0x8FBD),
    /// Volume Offset Control Service - Volume Offset Control Point characteristic
    GATT_CHAR_OFFSET_CP                          = GATT_UUID_16_LSB(0x8FBE),
    /// Volume Offset Control Service - Audio Output Description characteristic
    GATT_CHAR_OUTPUT_DESC                        = GATT_UUID_16_LSB(0x8FBF),

    /// Audio Input Control Service - Input State characteristic
    GATT_CHAR_INPUT_STATE                        = GATT_UUID_16_LSB(0x8FE2),
    /// Audio Input Control Service - Gain Setting Properties characteristic
    GATT_CHAR_GAIN_PROP                          = GATT_UUID_16_LSB(0x8FE3),
    /// Audio Input Control Service - Input Type characteristic
    GATT_CHAR_INPUT_TYPE                         = GATT_UUID_16_LSB(0x8FE5),
    /// Audio Input Control Service - Input Status characteristic
    GATT_CHAR_INPUT_STATUS                       = GATT_UUID_16_LSB(0x8FE6),
    /// Audio Input Control Service - Audio Input Control Point characteristic
    GATT_CHAR_INPUT_CP                           = GATT_UUID_16_LSB(0x8FE7),
    /// Audio Input Control Service - Audio Input Description characteristic
    GATT_CHAR_INPUT_DESC                         = GATT_UUID_16_LSB(0x8FE8),

    /// Published Audio Capabilities Service - Sink PAC characteristic
    GATT_CHAR_PAC_SINK                           = GATT_UUID_16_LSB(0x8F97),
    /// Published Audio Capabilities Service - Sink PAC characteristic
    GATT_CHAR_LOC_SINK                           = GATT_UUID_16_LSB(0x8F98),
    /// Published Audio Capabilities Service - Sink PAC characteristic
    GATT_CHAR_PAC_SRC                            = GATT_UUID_16_LSB(0x8F99),
    /// Published Audio Capabilities Service - Sink PAC characteristic
    GATT_CHAR_LOC_SRC                            = GATT_UUID_16_LSB(0x8F9A),
    /// Published Audio Capabilities Service - Available Audio Contexts
    GATT_CHAR_CONTEXT_AVA                        = GATT_UUID_16_LSB(0x8F9F),
    /// Published Audio Capabilities Service - Supported Audio Contexts
    GATT_CHAR_CONTEXT_SUPP                       = GATT_UUID_16_LSB(0x8FA0),

    /// Broadcast Audio Scan Service - Broadcast Audio Scan Control Point characteristic
    GATT_CHAR_BCAST_AUDIO_SCAN_CP                = GATT_UUID_16_LSB(0x8F9B),
    /// Broadcast Audio Scan Service - Broadcast Receive State characteristic
    GATT_CHAR_BCAST_RX_STATE                     = GATT_UUID_16_LSB(0x8F9C),

    /// Audio Stream Control Service - ASE characteristic
    GATT_CHAR_ASE                                = GATT_UUID_16_LSB(0x8F9D),
    /// Audio Stream Control Service - ASE Control Point characteristic
    GATT_CHAR_ASE_CP                             = GATT_UUID_16_LSB(0x8F9E),

    /// Coordinated Set Identification Service - Set Identity Resolving Key characteristic
    GATT_CHAR_CSIS_SIRK                          = GATT_UUID_16_LSB(0x8FA0),
    /// Coordinated Set Identification Service - Size characteristic
    GATT_CHAR_CSIS_SIZE                          = GATT_UUID_16_LSB(0x8FA1),
    /// Coordinated Set Identification Service - Lock characteristic
    GATT_CHAR_CSIS_LOCK                          = GATT_UUID_16_LSB(0x8FA2),
    /// Coordinated Set Identification Service - Rank characteristic
    GATT_CHAR_CSIS_RANK                          = GATT_UUID_16_LSB(0x8FA3),

    /// Telephone Bearer Service - Bearer Provider Name characteristic
    GATT_CHAR_TBS_PROV_NAME                      = GATT_UUID_16_LSB(0x8FA4),
    /// Telephone Bearer Service - Bearer UCI characteristic
    GATT_CHAR_TBS_UCI                       = GATT_UUID_16_LSB(0x8FA5),
    /// Telephone Bearer Service - Bearer Technology characteristic
    GATT_CHAR_TBS_TECHNO                         = GATT_UUID_16_LSB(0x8FA6),
    /// Telephone Bearer Service - Bearer URI Schemes Supported List characteristic
    GATT_CHAR_TBS_URI_SCHEMES_LIST               = GATT_UUID_16_LSB(0x8FA7),
    /// Telephone Bearer Service - Bearer Signal Strength characteristic
    GATT_CHAR_TBS_SIGN_STRENGTH                  = GATT_UUID_16_LSB(0x8FA8),
    /// Telephone Bearer Service - Bearer Signal Strength Reporting Interval characteristic
    GATT_CHAR_TBS_SIGN_STRENGTH_INTV             = GATT_UUID_16_LSB(0x8FA9),
    /// Telephone Bearer Service - Bearer List Current Calls characteristic
    GATT_CHAR_TBS_CURR_CALLS_LIST                = GATT_UUID_16_LSB(0x8FAA),
    /// Telephone Bearer Service - Status Flags characteristic
    GATT_CHAR_TBS_STATUS_FLAGS                   = GATT_UUID_16_LSB(0x8FAB),
    /// Telephone Bearer Service - Incoming Call Target Bearer URI characteristic
    GATT_CHAR_TBS_IN_TGT_CALLER_ID               = GATT_UUID_16_LSB(0x8FAC),
    /// Telephone Bearer Service - Call State characteristic
    GATT_CHAR_TBS_CALL_STATE                     = GATT_UUID_16_LSB(0x8FAD),
    /// Telephone Bearer Service - Call Control Point characteristic
    GATT_CHAR_TBS_CALL_CP                        = GATT_UUID_16_LSB(0x8FAE),
    /// Telephone Bearer Service - Call Control Point Optional Opcodes characteristic
    GATT_CHAR_TBS_CALL_CP_OPT_OPCODES            = GATT_UUID_16_LSB(0x8FAF),
    /// Telephone Bearer Service - Termination Reason characteristic
    GATT_CHAR_TBS_TERM_REASON                    = GATT_UUID_16_LSB(0x8FB0),
    /// Telephone Bearer Service - Incoming Call characteristic
    GATT_CHAR_TBS_INCOMING_CALL                  = GATT_UUID_16_LSB(0x8FB1),
    /// Telephone Bearer Service - Call Friendly Name characteristic
    GATT_CHAR_TBS_CALL_FRIENDLY_NAME             = GATT_UUID_16_LSB(0x8FB2),

    /// Media Control Service - Media Player Name characteristic
    GATT_CHAR_MCS_PLAYER_NAME                    = GATT_UUID_16_LSB(0x8FB3),
    /// Media Control Service - Media Player Icon Object ID characteristic
    GATT_CHAR_MCS_PLAYER_ICON_OBJ_ID             = GATT_UUID_16_LSB(0x8FB4),
    /// Media Control Service - Media Player Icon URL characteristic
    GATT_CHAR_MCS_PLAYER_ICON_URL                = GATT_UUID_16_LSB(0x8FB5),
    /// Media Control Service - Track Changed characteristic
    GATT_CHAR_MCS_TRACK_CHANGED                  = GATT_UUID_16_LSB(0x8FB6),
    /// Media Control Service - Track Title characteristic
    GATT_CHAR_MCS_TRACK_TITLE                    = GATT_UUID_16_LSB(0x8FB7),
    /// Media Control Service - Track Duration characteristic
    GATT_CHAR_MCS_TRACK_DURATION                 = GATT_UUID_16_LSB(0x8FB8),
    /// Media Control Service - Track Position characteristic
    GATT_CHAR_MCS_TRACK_POSITION                 = GATT_UUID_16_LSB(0x8FB9),
    /// Media Control Service - Playback Speed characteristic
    GATT_CHAR_MCS_PLAYBACK_SPEED                 = GATT_UUID_16_LSB(0x8FBA),
    /// Media Control Service - Seeking Speed characteristic
    GATT_CHAR_MCS_SEEKING_SPEED                  = GATT_UUID_16_LSB(0x8FBB),
    /// Media Control Service - Current Track Segments Object ID characteristic
    GATT_CHAR_MCS_CUR_TRACK_SEG_OBJ_ID           = GATT_UUID_16_LSB(0x8FBC),
    /// Media Control Service - Current Track Object ID characteristic
    GATT_CHAR_MCS_CUR_TRACK_OBJ_ID               = GATT_UUID_16_LSB(0x8FBD),
    /// Media Control Service - Next Track Object ID characteristic
    GATT_CHAR_MCS_NEXT_TRACK_OBJ_ID              = GATT_UUID_16_LSB(0x8FBE),
    /// Media Control Service - Current Group Object ID characteristic
    GATT_CHAR_MCS_CUR_GROUP_OBJ_ID               = GATT_UUID_16_LSB(0x8FBF),
    /// Media Control Service - Parent Group Object ID characteristic
    GATT_CHAR_MCS_PARENT_GROUP_OBJ_ID            = GATT_UUID_16_LSB(0x8FC0),
    /// Media Control Service - Playing Order characteristic
    GATT_CHAR_MCS_PLAYING_ORDER                  = GATT_UUID_16_LSB(0x8FC1),
    /// Media Control Service - Playing Order Supported characteristic
    GATT_CHAR_MCS_PLAYING_ORDER_SUPP             = GATT_UUID_16_LSB(0x8FC2),
    /// Media Control Service - Media State characteristic
    GATT_CHAR_MCS_MEDIA_STATE                    = GATT_UUID_16_LSB(0x8FC3),
    /// Media Control Service - Media Control Point characteristic
    GATT_CHAR_MCS_MEDIA_CP                       = GATT_UUID_16_LSB(0x8FC4),
    /// Media Control Service - Media Control Point Opcodes Supported characteristic
    GATT_CHAR_MCS_MEDIA_CP_OPCODES_SUPP          = GATT_UUID_16_LSB(0x8FC5),
    /// Media Control Service - Search Results Object ID characteristic
    GATT_CHAR_MCS_SEARCH_RESULTS_OBJ_ID          = GATT_UUID_16_LSB(0x8FC6),
    /// Media Control Service - Search Control Point characteristic
    GATT_CHAR_MCS_SEARCH_CP                      = GATT_UUID_16_LSB(0x8FC7),

    /// Content Control ID characteristic
    GATT_CHAR_CCID                               = GATT_UUID_16_LSB(0x8FC8),
    /// Client Supported Features
    GATT_CHAR_CLI_SUP_FEAT                       = GATT_UUID_16_LSB(0x2B29),
    /// Database Hash
    GATT_CHAR_DB_HASH                            = GATT_UUID_16_LSB(0x2B2A),
    /// Server Supported Features
    GATT_CHAR_SRV_SUP_FEAT                       = GATT_UUID_16_LSB(0x2B3A),
};


/// Format for Characteristic Presentation
enum gatt_format
{
    /// unsigned 1-bit: true or false
    GATT_FORMAT_BOOL     = 0x01,
    /// unsigned 2-bit integer
    GATT_FORMAT_2BIT,
    /// unsigned 4-bit integer
    GATT_FORMAT_NIBBLE,
    /// unsigned 8-bit integer
    GATT_FORMAT_UINT8,
    /// unsigned 12-bit integer
    GATT_FORMAT_UINT12,
    /// unsigned 16-bit integer
    GATT_FORMAT_UINT16,
    /// unsigned 24-bit integer
    GATT_FORMAT_UINT24,
    /// unsigned 32-bit integer
    GATT_FORMAT_UINT32,
    /// unsigned 48-bit integer
    GATT_FORMAT_UINT48,
    /// unsigned 64-bit integer
    GATT_FORMAT_UINT64,
    /// unsigned 128-bit integer
    GATT_FORMAT_UINT128,
    /// signed 8-bit integer
    GATT_FORMAT_SINT8,
    /// signed 12-bit integer
    GATT_FORMAT_SINT12,
    /// signed 16-bit integer
    GATT_FORMAT_SINT16,
    /// signed 24-bit integer
    GATT_FORMAT_SINT24,
    /// signed 32-bit integer
    GATT_FORMAT_SINT32,
    /// signed 48-bit integer
    GATT_FORMAT_SINT48,
    /// signed 64-bit integer
    GATT_FORMAT_SINT64,
    /// signed 128-bit integer
    GATT_FORMAT_SINT128,
    /// IEEE-754 32-bit floating point
    GATT_FORMAT_FLOAT32,
    /// IEEE-754 64-bit floating point
    GATT_FORMAT_FLOAT64,
    /// IEEE-11073 16-bit SFLOAT
    GATT_FORMAT_SFLOAT,
    /// IEEE-11073 32-bit FLOAT
    GATT_FORMAT_FLOAT,
    /// IEEE-20601 format
    GATT_FORMAT_DUINT16,
    /// UTF-8 string
    GATT_FORMAT_UTF8S,
    /// UTF-16 string
    GATT_FORMAT_UTF16S,
    /// Opaque structure
    GATT_FORMAT_STRUCT,
};

/// Client Characteristic Configuration Codes
enum gatt_ccc_val
{
    /// Stop notification/indication
    GATT_CCC_STOP_NTFIND = 0x0000,
    /// Start notification
    GATT_CCC_START_NTF   = 0x0001,
    /// Start indication
    GATT_CCC_START_IND   = 0x0002,
};

/*
 * CALLBACK DEFINITION
 ****************************************************************************************
 */

/// 16-bit UUID Attribute Description structure
typedef struct gatt_att16_desc
{
    /// Attribute UUID (16-bit UUID - LSB First)
    uint16_t uuid16;
    /// Attribute information bit field (@see enum gatt_att_info_bf)
    uint16_t info;
    /// Attribute extended information bit field (@see enum gatt_att_ext_info_bf)
    /// Note:
    ///   - For Included Services and Characteristic Declarations, this field contains targeted handle.
    ///   - For Characteristic Extended Properties, this field contains 2 byte value
    ///   - For Client Characteristic Configuration and Server Characteristic Configuration, this field is not used.
    uint16_t ext_info;
} gatt_att16_desc_t;


/// GATT Database Hash callback set
typedef struct gatt_db_hash_cb
{
    /**
     ****************************************************************************************
     * @brief This function is called when hash value for local attribute database hash has
     *        been computed.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution.
     * @param[in] status        Status of the operation (@see enum hl_err)
     * @param[in] p_hash        Pointer to the 128-bit database hash value
     ****************************************************************************************
     */
    void (*cb_db_hash) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status, const uint8_t* p_hash);
} gatt_db_hash_cb_t;


/// GATT server user callback set
typedef struct gatt_srv_cb
{
    /**
     ****************************************************************************************
     * @brief This function is called when GATT server user has initiated event send to peer
     *        device or if an error occurs.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] status        Status of the procedure (@see enum hl_err)
     ****************************************************************************************
     */
    void (*cb_event_sent) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status);

    /**
     ****************************************************************************************
     * @brief This function is called when peer want to read local attribute database value.
     *
     *        @see gatt_srv_att_read_get_cfm shall be called to provide attribute value
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] token         Procedure token that must be returned in confirmation function
     * @param[in] hdl           Attribute handle
     * @param[in] offset        Value offset
     * @param[in] max_length    Maximum value length to return
     ****************************************************************************************
     */
    void (*cb_att_read_get) (uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl, uint16_t offset,
                             uint16_t max_length);

    /**
     ****************************************************************************************
     * @brief This function is called when GATT server user has initiated event send procedure,
     *
     *        @see gatt_srv_att_event_get_cfm shall be called to provide attribute value
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] token         Procedure token that must be returned in confirmation function
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution.
     * @param[in] hdl           Attribute handle
     * @param[in] max_length    Maximum value length to return
     ****************************************************************************************
     */
    void (*cb_att_event_get) (uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t dummy, uint16_t hdl,
                              uint16_t max_length);

    /**
     ****************************************************************************************
     * @brief This function is called during a write procedure to get information about a
     *        specific attribute handle.
     *
     *        @see gatt_srv_att_info_get_cfm shall be called to provide attribute information
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] token         Procedure token that must be returned in confirmation function
     * @param[in] hdl           Attribute handle
     ****************************************************************************************
     */
    void (*cb_att_info_get) (uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl);

    /**
     ****************************************************************************************
     * @brief This function is called during a write procedure to modify attribute handle.
     *
     *        @see gatt_srv_att_val_set_cfm shall be called to accept or reject attribute
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
    void (*cb_att_val_set) (uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl, uint16_t offset,
                            co_buf_t* p_data);
} gatt_srv_cb_t;



/// GATT client user callback set
typedef struct gatt_cli_cb
{
    /**
     ****************************************************************************************
     * @brief This function is called when GATT client user discovery procedure is over.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] status        Status of the procedure (@see enum hl_err)
     ****************************************************************************************
     */
    void (*cb_discover_cmp) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status);

    /**
     ****************************************************************************************
     * @brief This function is called when GATT client user read procedure is over.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] status        Status of the procedure (@see enum hl_err)
     ****************************************************************************************
     */
    void (*cb_read_cmp) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status);

    /**
     ****************************************************************************************
     * @brief This function is called when GATT client user write procedure is over.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] status        Status of the procedure (@see enum hl_err)
     ****************************************************************************************
     */
    void (*cb_write_cmp) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status);

    /**
     ****************************************************************************************
     * @brief This function is called when GATT client user has initiated a write procedure.
     *
     *        @see gatt_cli_att_val_get_cfm shall be called to provide attribute value.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] token         Procedure token that must be returned in confirmation function
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution - 0x0000 else.
     * @param[in] hdl           Attribute handle
     * @param[in] offset        Value offset
     * @param[in] max_length    Maximum value length to return
     ****************************************************************************************
     */
    void (*cb_att_val_get) (uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t dummy,
                            uint16_t hdl, uint16_t offset, uint16_t max_length);

    /**
     ****************************************************************************************
     * @brief This function is called when a full service has been found during a discovery procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] hdl           First handle value of following list
     * @param[in] disc_info     Discovery information (@see enum gatt_svc_disc_info)
     * @param[in] nb_att        Number of attributes
     * @param[in] p_atts        Pointer to attribute information present in a service
     ****************************************************************************************
     */
    void (*cb_svc) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t hdl, uint8_t disc_info,
                    uint8_t nb_att, const gatt_svc_att_t* p_atts);

    /**
     ****************************************************************************************
     * @brief This function is called when a service has been found during a discovery procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] start_hdl     Service start handle
     * @param[in] end_hdl       Service end handle
     * @param[in] uuid_type     UUID Type (@see enum gatt_uuid_type)
     * @param[in] p_uuid        Pointer to service UUID (LSB first)
     ****************************************************************************************
     */
    void (*cb_svc_info) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t start_hdl, uint16_t end_hdl,
                         uint8_t uuid_type, const uint8_t* p_uuid);
    /**
     ****************************************************************************************
     * @brief This function is called when an include service has been found during a discovery procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] inc_svc_hdl   Include service attribute handle
     * @param[in] start_hdl     Service start handle
     * @param[in] end_hdl       Service end handle
     * @param[in] uuid_type     UUID Type (@see enum gatt_uuid_type)
     * @param[in] p_uuid        Pointer to service UUID (LSB first)
     ****************************************************************************************
     */
    void (*cb_inc_svc) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t inc_svc_hdl,
                        uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_type, const uint8_t* p_uuid);
    /**
     ****************************************************************************************
     * @brief This function is called when a characteristic has been found during a discovery procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] char_hdl      Characteristic attribute handle
     * @param[in] val_hdl       Value handle
     * @param[in] prop          Characteristic properties (@see enum gatt_att_info_bf - bits [0-7])
     * @param[in] uuid_type     UUID Type (@see enum gatt_uuid_type)
     * @param[in] p_uuid        Pointer to characteristic value UUID (LSB first)
     ****************************************************************************************
     */
    void (*cb_char) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t char_hdl, uint16_t val_hdl,
                     uint8_t prop, uint8_t uuid_type, const uint8_t* p_uuid);
    /**
     ****************************************************************************************
     * @brief This function is called when a descriptor has been found during a discovery procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] desc_hdl      Characteristic descriptor attribute handle
     * @param[in] uuid_type     UUID Type (@see enum gatt_uuid_type)
     * @param[in] p_uuid        Pointer to attribute UUID (LSB first)
     ****************************************************************************************
     */
    void (*cb_desc) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t desc_hdl,
                     uint8_t uuid_type, const uint8_t* p_uuid);
    /**
     ****************************************************************************************
     * @brief This function is called during a read procedure when attribute value is retrieved
     *        form peer device.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] dummy         Dummy parameter provided by upper layer for command execution
     * @param[in] hdl           Attribute handle
     * @param[in] offset        Value offset
     * @param[in] p_data        Pointer to buffer that contains attribute value starting from offset
     ****************************************************************************************
     */
    void (*cb_att_val) (uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t hdl, uint16_t offset,
                        co_buf_t* p_data);
    /**
     ****************************************************************************************
     * @brief This function is called when a notification or an indication is received onto
     *        register handle range (@see gatt_cli_event_register).
     *
     *        @see gatt_cli_val_event_cfm must be called to confirm event reception.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] token         Procedure token that must be returned in confirmation function
     * @param[in] evt_type      Event type triggered (@see enum gatt_evt_type)
     * @param[in] complete      True if event value if complete value has been received
     *                          False if data received is equals to maximum attribute protocol value.
     *                          In such case GATT Client User should perform a read procedure.
     * @param[in] hdl           Attribute handle
     * @param[in] p_data        Pointer to buffer that contains attribute value
     ****************************************************************************************
     */
    void (*cb_att_val_evt) (uint8_t conidx, uint8_t user_lid, uint16_t token, uint8_t evt_type, bool complete, 
                            uint16_t hdl, co_buf_t* p_data);

    /**
     ****************************************************************************************
     * @brief Event triggered when a service change has been received or if an attribute
     *        transaction triggers an out of sync error.
     *
     * @param[in] conidx        Connection index
     * @param[in] user_lid      GATT user local identifier
     * @param[in] out_of_sync   True if an out of sync error has been received
     * @param[in] start_hdl     Service start handle
     * @param[in] end_hdl       Service end handle
     ****************************************************************************************
     */
    void (*cb_svc_changed) (uint8_t conidx, uint8_t user_lid, bool out_of_sync, uint16_t start_hdl, uint16_t end_hdl);
} gatt_cli_cb_t;

/*
 * FUCTION DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Compare if two UUIDs matches
 *
 * @param[in]  p_uuid_a     UUID A value
 * @param[in]  uuid_a_type  UUID A type (@see enum gatt_uuid_type)
 * @param[in]  p_uuid_b     UUID B value
 * @param[in]  uuid_b_type  UUID B type (@see enum gatt_uuid_type)
 *
 * @return true if UUIDs matches, false otherwise
 ****************************************************************************************
 */
bool gatt_uuid_comp(const uint8_t *p_uuid_a, uint8_t uuid_a_type, const uint8_t *p_uuid_b, uint8_t uuid_b_type);

/**
 ****************************************************************************************
 * @brief Check if two UUIDs matches (2nd UUID is a 16 bits UUID with LSB First)
 *
 * @param[in]  p_uuid_a     UUID A value
 * @param[in]  uuid_a_type  UUID A type (@see enum gatt_uuid_type)
 * @param[in]  uuid_b       UUID B 16 bit value
 *
 * @return true if UUIDs matches, false otherwise
 ****************************************************************************************
 */
bool gatt_uuid16_comp(const uint8_t *p_uuid_a, uint8_t uuid_a_type, uint16_t uuid_b);

/**
 ****************************************************************************************
 * @brief Check if it's a Bluetooth 16-bits UUID for 128-bit input
 *
 * @param[in]  p_uuid128      128-bit UUID
 *
 * @return true if uuid  is a Bluetooth 16-bit UUID, false else.
 ****************************************************************************************
 */
bool gatt_is_uuid16(const uint8_t *p_uuid128);

/**
 ****************************************************************************************
 * @brief Check if it's a Bluetooth 32 bits UUID for 128-bit input
 *
 * @param[in]  p_uuid128      128-bit UUID
 *
 * @return true if uuid  is a Bluetooth 32-bits UUID, false else.
 ****************************************************************************************
 */
bool gatt_is_uuid32(const uint8_t *p_uuid128);

/**
 ****************************************************************************************
 * @brief Convert UUID value to 128 bit UUID
 *
 * @param[in]  p_uuid      UUID to convert to 128-bit UUID
 * @param[in]  uuid_type   UUID type (@see enum gatt_uuid_type)
 * @param[out] p_uuid128   converted 32-bit Bluetooth UUID to 128-bit UUID
 *
 ****************************************************************************************
 */
void gatt_uuid128_convert(const uint8_t *p_uuid, uint8_t uuid_type, uint8_t *p_uuid128);

/**
 ****************************************************************************************
 * @brief Extract UUID from a 16-bit or 128-bit UUID (reduce result to a minimum length)
 *
 * @param[out] p_out_uuid        Output UUID (16-bit / 32-bit or 128-bit UUID LSB-first)
 * @param[out] p_out_uuid_type   Output UUID type (@see enum gatt_uuid_type)
 * @param[in]  p_in_uuid         Pointer to the 16-bit or 128-bit UUID (LSB-first)
 * @param[in]  in_uuid_len       Length of the input UUID: 2 or 16 bytes.
 *
 * @return Status of the function execution (@see enum hl_err)
 */
uint16_t gatt_uuid_extract(uint8_t *p_out_uuid, uint8_t* p_out_uuid_type, const uint8_t *p_in_uuid, uint8_t in_uuid_len);

/**
 ****************************************************************************************
 * @brief Command used to register a GATT user. This must be done prior to any GATT
 *        procedure execution.
 *
 *        A GATT client user can initiate any client procedure, and shall be able to
 *        handle all client initiated message events
 *
 *        Same module can register multiple GATT users.
 *
 * @param[in]  pref_mtu     Preferred MTU for attribute exchange.
 * @param[in]  prio_level   User attribute priority level
 * @param[in]  p_cb         Pointer to set of callback functions to be used for communication
 *                          with the GATT server user
 * @param[out] p_user_lid   Pointer where GATT user local identifier will be set
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_user_cli_register(uint16_t pref_mtu, uint8_t prio_level, const gatt_cli_cb_t* p_cb, uint8_t* p_user_lid);

/**
 ****************************************************************************************
 * @brief Command used to register a GATT user. This must be done prior to any GATT
 *        procedure execution.
 *
 *        A GATT server can manipulate local attribute database and initiate server
 *        procedures. It shall be able to handle all server initiated events
 *
 *        Same module can register multiple GATT users.
 *
 * @param[in]  pref_mtu     Preferred MTU for attribute exchange.
 * @param[in]  prio_level   User attribute priority level
 * @param[in]  p_cb         Pointer to set of callback functions to be used for communication
 *                          with the GATT server user
 * @param[out] p_user_lid   Pointer where GATT user local identifier will be set
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_user_srv_register(uint16_t pref_mtu, uint8_t prio_level, const gatt_srv_cb_t* p_cb, uint8_t* p_user_lid);

/**
 ****************************************************************************************
 * @brief Command used to unregister a GATT user (client or server).
 *
 * @param[in]  user_lid     GATT User Local identifier
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_user_unregister(uint8_t user_lid);


/**
 ****************************************************************************************
 * @brief alloc service db for new service
 *
 * @param[in]  nb_att      Number of attribute(s) in service
 *
 * @return service db memory space.
 ****************************************************************************************
 */
gatt_att_desc_t* gatt_db_svc_alloc_atts_mem(uint8_t nb_att);

/**
 ****************************************************************************************
 * @brief calcute uuid mem for attribute in a service
 *
 * @param[in]  uuid_len   uuid length 
 *
 * @return service db memory space.
 ****************************************************************************************
 */
uint16_t gatt_db_svc_cal_uuid_mem_size(uint16_t uuid_len);

/**
 ****************************************************************************************
 * @brief add a attribute in service database
 *
 * @param[in]  p_atts     pointer to service database 
 * @param[in]  item       pointer to attribute need to be added
 * @param[in]  offset     offset of the attribute in the service db
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_svc_add_item(gatt_att_desc_t*p_atts,  const gatt_att_desc_t* item, uint16_t offset);

/**
 ****************************************************************************************
 * @brief used to add a service into local attribute database.
 *
 * @param[in]  p_atts     pointer to service database 
 * @param[in]  p_uuid    ervice UUID (16-bit UUID - LSB First)
 * @param[in]     info         Service Information bit field (@see enum gatt_svc_info_bf)
 * @param[in|out] p_start_hdl  Pointer to Service Start Handle (0 = chosen by GATT module)
 *                             Pointer updated with service start handle associated to
 *                             created service.
 * @param[in]     nb_att       Number of attribute(s) in service
 * @param[in]     user_lid     GATT User Local identifier
 * @param[in]     uuid_mem_size     uuid mem size for uuid-128
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_svc_start(gatt_att_desc_t*p_atts,const uint8_t* p_uuid, uint8_t info, uint16_t* p_start_hdl, 
                                                                     uint8_t nb_att ,uint8_t user_lid, uint16_t uuid_mem_size);

/**
 ****************************************************************************************
 * @brief Command used to add a service into local attribute database.
 *
 *        Service and attributes UUIDs in service must be 16-bit
 *
 *        If start handle is set to zero (invalid attribute handle), GATT looks for a
 *        free handle block matching with number of attributes to reserve.
 *        Else, according to start handle, GATT checks if attributes to reserve are
 *        not overlapping part of existing database.
 *
 *        An added service is automatically visible for peer device.
 *
 *        @note First attribute in attribute array must be a Primary or a Secondary service
 *
 * @param[in]     user_lid     GATT User Local identifier
 * @param[in]     info         Service Information bit field (@see enum gatt_svc_info_bf)
 * @param[in]     uuid16       Service UUID (16-bit UUID - LSB First)
 * @param[in]     nb_att       Number of attribute(s) in service
 * @param[in]     p_att_mask   Pointer to mask of attribute to insert in database:
 *                               - If NULL insert all attributes
 *                               - If bit set to 1: attribute inserted
 *                               - If bit set to 0: attribute not inserted
 * @param[in]     p_atts       Pointer to List of attribute (with 16-bit uuid) description present in service.
 * @param[in]     nb_att_rsvd  Number of attribute(s) reserved for the service (shall be equals or greater nb_att)
 *                             Prevent any services to be inserted between start_hdl and (start_hdl + nb_att_rsvd - 1)
 * @param[in|out] p_start_hdl  Pointer to Service Start Handle (0 = chosen by GATT module)
 *                             Pointer updated with service start handle associated to
 *                             created service.
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_svc16_add(uint8_t user_lid, uint8_t info, uint16_t uuid16, uint8_t nb_att, const uint8_t* p_att_mask,
                           const gatt_att16_desc_t* p_atts, uint8_t nb_att_rsvd, uint16_t* p_start_hdl);

/**
 ****************************************************************************************
 * @brief Command used to add a service into local attribute database.
 *
 *        If start handle is set to zero (invalid attribute handle), GATT looks for a
 *        free handle block matching with number of attributes to reserve.
 *        Else, according to start handle, GATT checks if attributes to reserve are
 *        not overlapping part of existing database.
 *
 *        An added service is automatically visible for peer device.
 *
 *        @note First attribute in attribute array must be a Primary or a Secondary service
 *
 * @param[in]     user_lid     GATT User Local identifier
 * @param[in]     info         Service Information bit field (@see enum gatt_svc_info_bf)
 * @param[in]     p_uuid       Pointer to service UUID (LSB first)
 * @param[in]     nb_att       Number of attribute(s) in service
 * @param[in]     p_att_mask   Pointer to mask of attribute to insert in database:
 *                               - If NULL insert all attributes
 *                               - If bit set to 1: attribute inserted
 *                               - If bit set to 0: attribute not inserted
 * @param[in]     p_atts       Pointer to List of attribute description present in service.
 * @param[in]     nb_att_rsvd  Number of attribute(s) reserved for the service (shall be equals or greater nb_att)
 *                             Prevent any services to be inserted between start_hdl and (start_hdl + nb_att_rsvd -1)
 * @param[in|out] p_start_hdl  Pointer to Service Start Handle (0 = chosen by GATT module)
 *                             Pointer updated with service start handle associated to
 *                             created service.
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_svc_add(uint8_t user_lid, uint8_t info, const uint8_t* p_uuid, uint8_t nb_att, const uint8_t* p_att_mask,
                         const gatt_att_desc_t* p_atts, uint8_t nb_att_rsvd, uint16_t* p_start_hdl);

/**
 ****************************************************************************************
 * @brief Command used to remove a service from local attribute database.
 *
 *        Only GATT user responsible of service can remove it.
 *
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  start_hdl    Service Start Handle
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_svc_remove(uint8_t user_lid, uint16_t start_hdl);

/**
 ****************************************************************************************
 * @brief Function use to verify if several services can be allocated on a contiguous
 *        handle range.
 *
 * @param[in]     user_lid     GATT User Local identifier
 * @param[in]     nb_att       Number of attribute(s) to reserve
 * @param[in|out] p_start_hdl  Pointer to Service Start Handle (0 = chosen by GATT module)
 *                             Pointer updated with service start handle associated to
 *                             first attribute range available.
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_handle_range_reserve(uint8_t user_lid, uint8_t nb_att, uint16_t* p_start_hdl);

/**
 ****************************************************************************************
 * @brief Command used to control visibility and usage authorization of a local service.
 *        A hidden service is present in database but cannot be discovered or manipulated
 *        by a peer device.
 *        A disabled service can be discovered by a peer device but it is not authorized to
 *        use it.
 *
 *        Only GATT user responsible of service can update its properties
 *
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  start_hdl    Service Start Handle
 * @param[in]  enable       True: Authorize usage of the service
 *                          False: reject usage of the service
 * @param[in]  visible      Service visibility (@see enum gatt_svc_visibility)
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_svc_ctrl(uint8_t user_lid, uint16_t start_hdl, uint8_t enable, uint8_t visible);

/**
 ****************************************************************************************
 * @brief Command used to retrieve (or compute) the local database hash value.
 *
 * @param[in]  conidx   Connection index (not used but returned as requested)
 * @param[in]  user_lid GATT User Local identifier (must be a server user)
 * @param[in]  dummy    Dummy parameter whose meaning is upper layer dependent and which
 *                      is returned in command complete.
 * @param[in]  p_cb     Callback where database hash is returned
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_db_hash_get(uint8_t conidx, uint8_t user_lid, uint16_t dummy, const gatt_db_hash_cb_t* p_cb);

/**
 ****************************************************************************************
 * @brief Command used to retrieve information of an attribute.
 *
 * @param[in]  user_lid     GATT User Local identifier
 * @param[out] p_info       Attribute information bit field
 *                          (@see enum gatt_att_info_bf)
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_att_info_get(uint8_t user_lid, uint16_t hdl, uint16_t* p_info);

/**
 ****************************************************************************************
 * @brief Command used to set information of an attribute.
 *
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  hdl          Attribute Handle
 * @param[in]  info         Attribute information bit field (@see enum gatt_att_info_bf)
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_db_att_info_set(uint8_t user_lid, uint16_t hdl, uint16_t info);

/**
 ****************************************************************************************
 * @brief Command used by a GATT server user to send notifications or indications for
 *        some attributes values to peer device.
 *        Number of attributes must be set to one for GATT_INDICATE event type.
 *
 *        This function is consider reliable because GATT user is aware of maximum packet
 *        size that can be transmitted over the air.
 *
 *        Attribute value will be requested by GATT using @see cb_att_event_get function
 *        Wait for @see cb_event_sent execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  evt_type     Event type to trigger (@see enum gatt_evt_type)
 * @param[in]  nb_att       Number of attribute
 * @param[in]  p_atts       Pointer to List of attribute
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_srv_event_reliable_send(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t evt_type,
                                      uint8_t nb_att, const gatt_att_t* p_atts);

/**
 ****************************************************************************************
 * @brief Command used by a GATT server user to send notifications or indications.
 *
 *        Since user is not aware of MTU size of the bearer used for attribute
 *        transmission it cannot be considered reliable. If size of the data buffer is too
 *        big, data is truncated to max supported length.
 *
 *        Wait for @see cb_event_sent execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  evt_type     Event type to trigger (@see enum gatt_evt_type)
 * @param[in]  hdl          Attribute handle
 * @param[in]  p_data       Data buffer that must be transmitted
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_srv_event_send(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t evt_type, uint16_t hdl,
                             co_buf_t* p_data);

/**
 ****************************************************************************************
 * @brief Command used by a GATT server user to send notifications or indications of an
 *        attribute to multiple connected devices.
 *
 *        Since user is not aware of MTU size of the bearer used for attribute
 *        transmission it cannot be considered reliable. If size of the data buffer is too
 *        big, data is truncated to max supported length.
 *
 *        Event are sent sequentially over each connection and can take some time.
 *        The cb_event_sent callback is called once when procedure is completed.
 *
 *        It is possible to cancel an on-going multi-point procedure using
 *        @see gatt_srv_event_mtp_cancel function. In such case GATT user must ensure
 *        that the couple user_lid + dummy parameters are unique for GATT module.
 *
 *        Wait for @see cb_event_sent execution before starting a new procedure
 *
 * @param[in]  conidx_bf    Connection index bit field
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  evt_type     Event type to trigger (@see enum gatt_evt_type)
 * @param[in]  hdl          Attribute handle
 * @param[in]  p_data       Data buffer that must be transmitted
 * @param[in]  filter       True to filter intermediate command completed event triggered
 *                          when event is sent onto a specific connection.
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_srv_event_mtp_send(uint32_t conidx_bf, uint8_t user_lid, uint16_t dummy, uint8_t evt_type,
                                 uint16_t hdl, co_buf_t* p_data, bool filter);

/**
 ****************************************************************************************
 * @brief Command used by a GATT server user to cancel a multi connection event transmission
 *
 *        @note Once procedure is done, @see cb_event_sent function is called.
 *
 * @param[in]  user_lid     GATT User Local identifier used in @see gatt_srv_event_mtp_send
 * @param[in]  dummy        Dummy parameter used in @see gatt_srv_event_mtp_send
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_srv_event_mtp_cancel(uint8_t user_lid, uint16_t dummy);

/**
 ****************************************************************************************
 * @brief Upper layer provide attribute value requested by GATT Layer for a read procedure
 *        If rejected, value is not used.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  token        Procedure token provided in corresponding callback
 * @param[in]  status       Status of attribute value get (@see enum hl_err)
 * @param[in]  att_length   Complete Length of the attribute value
 * @param[in]  p_data       Pointer to buffer that contains attribute data
 *                          (starting from offset and does not exceed maximum size provided)
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_srv_att_read_get_cfm(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t status,
                                   uint16_t att_length, co_buf_t* p_data);

/**
 ****************************************************************************************
 * @brief Upper layer provide attribute value requested by GATT Layer for an event procedure.
 *        If rejected, value is not used.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  token        Procedure token provided in corresponding callback
 * @param[in]  status       Status of attribute value get (@see enum hl_err)
 * @param[in]  att_length   Complete Length of the attribute value
 * @param[in]  p_data       Pointer to buffer that contains attribute data
 *                          (data size does not exceed maximum size provided)
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_srv_att_event_get_cfm(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t status,
                                    uint16_t att_length, co_buf_t* p_data);

/**
 ****************************************************************************************
 * @brief Upper layer provide information about attribute requested by GATT Layer.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  token        Procedure token provided in corresponding callback
 * @param[in]  status       Status of attribute info get (@see enum hl_err)
 * @param[in]  att_length   Attribute value length
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_srv_att_info_get_cfm(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t status, uint16_t att_length);

/**
 ****************************************************************************************
 * @brief Upper layer provide status of attribute value modification by GATT server user.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  token        Procedure token provided in corresponding callback
 * @param[in]  status       Status of attribute value set (@see enum hl_err)
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_srv_att_val_set_cfm(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t status);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to discover primary or secondary services,
 *        exposed by peer device in its attribute database.
 *
 *        All services can be discovered or filtering services having a specific UUID.
 *        The discovery is done between start handle and end handle range.
 *        For a complete discovery start handle must be set to 0x0001 and end handle to
 *        0xFFFF.
 *
 *        Wait for @see cb_discover_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  disc_type    GATT Service discovery type (@see enum gatt_svc_discovery_type)
 * @param[in]  full         Perform discovery of all information present in the service
 *                          (True: enable, False: disable)
 * @param[in]  start_hdl    Search start handle
 * @param[in]  end_hdl      Search end handle
 * @param[in]  uuid_type    UUID Type (@see enum gatt_uuid_type)
 * @param[in]  p_uuid       Pointer to searched Service UUID (meaningful only for
 *                          discovery by UUID)
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_discover_svc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t disc_type, bool full,
                               uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_type, const uint8_t* p_uuid);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to discover included services, exposed
 *        by peer device in its attribute database.
 *
 *        The discovery is done between start handle and end handle range.
 *        For a complete discovery start handle must be set to 0x0001 and end handle to
 *        0xFFFF.
 *
 *        Wait for @see cb_discover_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  start_hdl    Search start handle
 * @param[in]  end_hdl      Search end handle
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_discover_inc_svc(uint8_t conidx, uint8_t user_lid, uint16_t dummy,
                                   uint16_t start_hdl, uint16_t end_hdl);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to discover all or according to a specific
 *        UUID characteristics exposed by peer device in its attribute database.
 *
 *        The discovery is done between start handle and end handle range.
 *        For a complete discovery start handle must be set to 0x0001 and end handle to
 *        0xFFFF.
 *
 *        Wait for @see cb_discover_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  disc_type    GATT characteristic discovery type (@see enum gatt_char_discovery_type)
 * @param[in]  start_hdl    Search start handle
 * @param[in]  end_hdl      Search end handle
 * @param[in]  uuid_type    UUID Type (@see enum gatt_uuid_type)
 * @param[in]  p_uuid       Pointer to searched Attribute Value UUID (meaningful only
 *                          for discovery by UUID)
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_discover_char(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t disc_type,
                                uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_type, const uint8_t* p_uuid);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to discover characteristic descriptor
 *        exposed by peer device in its attribute database.
 *
 *        The discovery is done between start handle and end handle range.
 *        For a complete discovery start handle must be set to 0x0001 and end handle to
 *        0xFFFF.
 *
 *        Wait for @see cb_discover_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  start_hdl    Search start handle
 * @param[in]  end_hdl      Search end handle
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_discover_desc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t start_hdl, uint16_t end_hdl);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to cancel an on-going discovery procedure.
 *        The dummy parameter in the request must be equals to dummy parameter used for
 *        service discovery command.
 *
 *        The discovery is aborted as soon as on-going discovery attribute transaction
 *        is over.
 *
 *        Wait for @see cb_discover_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_discover_cancel(uint8_t conidx, uint8_t user_lid, uint16_t dummy);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to read value of an attribute (identified
 *        by its handle) present in peer database.
 *
 *        Wait for @see cb_read_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  hdl          Attribute handle
 * @param[in]  offset       Value offset
 * @param[in]  length       Value length to read (0 = read all)
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_read(uint8_t conidx, uint8_t user_lid, uint16_t dummy,
                       uint16_t hdl, uint16_t offset, uint16_t length);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to read value of an attribute with a given
 *        UUID in peer database.
 *
 *        Wait for @see cb_read_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  start_hdl    Search start handle
 * @param[in]  end_hdl      Search end handle
 * @param[in]  uuid_type    UUID Type (@see enum gatt_uuid_type)
 * @param[in]  p_uuid       Pointer to searched attribute UUID (LSB First)
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_read_by_uuid(uint8_t conidx, uint8_t user_lid, uint16_t dummy,
                               uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_type, const uint8_t* p_uuid);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to read multiple attribute at the same time.
 *        If one of attribute length is unknown, the read multiple variable length
 *        procedure is used.
 *
 *        Wait for @see cb_read_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  nb_att       Number of attribute
 * @param[in]  p_atts       Pointer to list of attribute
 *                          If Attribute length is zero (consider length unknown):
  *                            - Attribute protocol read multiple variable length
  *                              procedure used
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_read_multiple(uint8_t conidx, uint8_t user_lid, uint16_t dummy,
                                uint8_t nb_att, const gatt_att_t* p_atts);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to request to write value of an attribute
 *        in peer database.
 *
 *        This function is consider reliable because GATT user is aware of maximum packet
 *        size that can be transmitted over the air.
 *
 *        Attribute value will be requested by GATT using @see cb_att_val_get function
 *
 *        Wait for @see cb_write_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  write_type   GATT write type (@see enum gatt_write_type)
 * @param[in]  write_mode   Write execution mode (@see enum gatt_write_mode).
 *                          Valid only for GATT_WRITE.
 * @param[in]  hdl          Attribute handle
 * @param[in]  offset       Value offset, valid only for GATT_WRITE
 * @param[in]  length       Value length to write
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_write_reliable(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t write_type, uint8_t write_mode,
                                 uint16_t hdl, uint16_t offset, uint16_t length);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to request to write value of an attribute
 *        in peer database.
 *
 *        Since user is not aware of MTU size of the bearer used for attribute
 *        transmission it cannot be considered reliable.
 *
 *        For a GATT_WRITE_NO_RESP if attribute bearer max transmission size isn't sufficient,
 *        a GATT_WRITE (with response) procedure will be used.
 *
 *        For a GATT_WRITE_SIGNED, if attribute bearer max transmission size isn't sufficient,
 *        the procedure is aborted with L2CAP_ERR_INVALID_MTU error code.
 *
 *        Wait for @see cb_write_cmp execution before starting a new procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  write_type   GATT write type (@see enum gatt_write_type)
 * @param[in]  hdl          Attribute handle
 * @param[in]  offset       Value offset, valid only for GATT_WRITE
 * @param[in]  p_data       Data buffer that must be transmitted
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_write(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t write_type,
                        uint16_t hdl, uint16_t offset, co_buf_t* p_data);

/**
 ****************************************************************************************
 * @brief Upper layer provide attribute value requested by GATT Layer, length shall be
 *        set to zero if request is rejected.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  token        Procedure token provided in corresponding callback
 * @param[in]  status       Status of attribute value get (@see enum hl_err)
 * @param[in]  p_data       Pointer to buffer that contains data to write (starting
 *                          from offset)
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_cli_att_val_get_cfm(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t status, co_buf_t* p_data);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to request peer server to execute prepare
 *        write queue.
 *
 *        Wait for @see cb_write_cmp execution before starting a new procedure
 *
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in command complete.
 * @param[in]  execute      True: Perform pending write operations
 *                          False: Cancel pending write operations
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t gatt_cli_write_exe(uint8_t conidx, uint8_t user_lid, uint16_t dummy, bool execute);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to register for reception of events
 *        (notification / indication) for a given handle range.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  start_hdl    Attribute start handle
 * @param[in]  end_hdl      Attribute end handle
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_cli_event_register(uint8_t conidx, uint8_t user_lid, uint16_t start_hdl, uint16_t end_hdl);

/**
 ****************************************************************************************
 * @brief Command used by a GATT client user to stop reception of events (notification /
 *        indication) onto a specific handle range.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  start_hdl    Attribute start handle
 * @param[in]  end_hdl      Attribute end handle
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_cli_event_unregister(uint8_t conidx, uint8_t user_lid, uint16_t start_hdl, uint16_t end_hdl);

/**
 ****************************************************************************************
 * @brief Upper layer provide status of attribute event handled by GATT client user.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  token        Procedure token provided in corresponding callback
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_cli_att_event_cfm(uint8_t conidx, uint8_t user_lid, uint16_t token);

/**
 ****************************************************************************************
 * @brief Request a MTU exchange on legacy attribute bearer.
 *        There is no callback executed when the procedure is over.
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_cli_mtu_exch(uint8_t conidx, uint8_t user_lid);


/**
 ****************************************************************************************
 * @brief Retrieve minimum MTU of Maximum MTU negotiated for all available ATT bearers.
 *
 * @param[in]  conidx       Connection index
 *
 * @return Minimum of Maximum MTU negotiated for all available ATT bearers
 ****************************************************************************************
 */
uint16_t gatt_bearer_mtu_min_get(uint8_t conidx);

/// @} GATT

#endif // GATT_H_
