/**
 ****************************************************************************************
 *
 * @file rwprf_config.h
 *
 * @brief Header file - Profile Configuration
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */


#ifndef _RWPRF_CONFIG_H_
#define _RWPRF_CONFIG_H_

#include "rwapp_config.h"

#ifdef GFPS_ENABLED
#if BLE_APP_GFPS_VER==FAST_PAIR_REV_2_0
#define CFG_PRF_GFPS_PROVIDER
#endif
#endif

#if BLE_AUDIO_ENABLED
#define CFG_PRF_CSISM
#define CFG_PRF_CSISC
#define CFG_PRF_OTS
#define CFG_PRF_OTC
#endif

#ifdef VOICE_DATAPATH
#define CFG_VOICEPATH
#else
#define CFG_PRF_DATAPATH_SERVER
#endif

#ifdef __AI_VOICE_BLE_ENABLE__
#define CFG_AI_VOICE
#endif

#ifdef BES_OTA
#define CFG_OTA
#endif

#ifdef BLE_TOTA_ENABLED
#define CFG_TOTA
#endif

#ifdef TILE_DATAPATH
#define CFG_TILE
#endif

#if defined(BLE_HID_ENABLE)
#define CFG_PRF_HOGPD
#endif

#if defined(BLE_BATT_ENABLE)
#define CFG_PRF_BASS
#endif

#if defined(BLE_DISS_ENABLE)
#define CFG_PRF_DISS
#endif
/**
 ****************************************************************************************
 * @addtogroup PRF_CONFIG
 * @ingroup Profile
 * @brief Profile configuration
 *
 * @{
 ****************************************************************************************
 */

//ATT DB,Testing and Qualification related flags
#if (BLE_CENTRAL || BLE_PERIPHERAL)
/// Proximity Profile Monitor Role
#if defined(CFG_PRF_PXPM)
#define BLE_PROX_MONITOR        1
#else
#define BLE_PROX_MONITOR        0
#endif // defined(CFG_PRF_PXPM)

/// Proximity Profile Reporter Role
#if defined(CFG_PRF_PXPR)
#define BLE_PROX_REPORTER       1
#else
#define BLE_PROX_REPORTER       0
#endif // defined(CFG_PRF_PXPR)

///Find Me Profile Locator role
#if defined(CFG_PRF_FMPL)
#define BLE_FINDME_LOCATOR      1
#else
#define BLE_FINDME_LOCATOR      0
#endif // defined(CFG_PRF_FMPL)

///Find Me Profile Target role
#if defined(CFG_PRF_FMPT)
#define BLE_FINDME_TARGET       1
#else
#define BLE_FINDME_TARGET       0
#endif // defined(CFG_PRF_FMPT)

///Health Thermometer Profile Collector Role
#if defined(CFG_PRF_HTPC)
#define BLE_HT_COLLECTOR        1
#else
#define BLE_HT_COLLECTOR        0
#endif // defined(CFG_PRF_HTPC)

///Health Thermometer Profile Thermometer Role
#if defined(CFG_PRF_HTPT)
#define BLE_HT_THERMOM          1
#else
#define BLE_HT_THERMOM          0
#endif // defined(CFG_PRF_HTPT)

///Device Information Service Client Role
#if defined(CFG_PRF_DISC)
#define BLE_DIS_CLIENT          1
#else
#define BLE_DIS_CLIENT          0
#endif // defined(CFG_PRF_DISC)

///Device Information Service Server Role
#if defined(CFG_PRF_DISS)
#define BLE_DIS_SERVER          1
#else
#define BLE_DIS_SERVER          0
#endif // defined(CFG_PRF_DISS)

///Blood Pressure Profile Collector Role
#if defined(CFG_PRF_BLPC)
#define BLE_BP_COLLECTOR        1
#else
#define BLE_BP_COLLECTOR        0
#endif // defined(CFG_PRF_BLPC)

///Blood Pressure Profile Sensor Role
#if defined(CFG_PRF_BLPS)
#define BLE_BP_SENSOR           1
#else
#define BLE_BP_SENSOR           0
#endif // defined(CFG_PRF_BLPS)

///Time Profile Client Role
#if defined(CFG_PRF_TIPC)
#define BLE_TIP_CLIENT          1
#else
#define BLE_TIP_CLIENT          0
#endif // defined(CFG_PRF_TIPC)

///Time Profile Server Role
#if defined(CFG_PRF_TIPS)
#define BLE_TIP_SERVER          1
#else
#define BLE_TIP_SERVER          0
#endif // defined(CFG_PRF_TIPS)

///Heart Rate Profile Collector Role
#if defined(CFG_PRF_HRPC)
#define BLE_HR_COLLECTOR        1
#else
#define BLE_HR_COLLECTOR        0
#endif // defined(CFG_PRF_HRPC)

///Heart Rate Profile Sensor Role
#if defined(CFG_PRF_HRPS)
#define BLE_HR_SENSOR           1
#else
#define BLE_HR_SENSOR           0
#endif // defined(CFG_PRF_HRPS)

///Scan Parameter Profile Client Role
#if defined(CFG_PRF_SCPPC)
#define BLE_SP_CLIENT           1
#else
#define BLE_SP_CLIENT           0
#endif // defined(CFG_PRF_SCPPC)

///Scan Parameter Profile Server Role
#if defined(CFG_PRF_SCPPS)
#define BLE_SP_SERVER           1
#else
#define BLE_SP_SERVER           0
#endif // defined(CFG_PRF_SCPPS)

///Battery Service Client Role
#if defined(CFG_PRF_BASC)
#define BLE_BATT_CLIENT         1
#define BASC_NB_BAS_INSTANCES_MAX (2) // Maximum number of Battery Service instances we can handle
#else
#define BLE_BATT_CLIENT         0
#endif // defined(CFG_PRF_BASC)

///Battery Service Server Role
#if defined(CFG_PRF_BASS)
#define BLE_BATT_SERVER         1
#define BASS_NB_BAS_INSTANCES_MAX (2) // Maximal number of BAS that can be added in the DB
#else
#define BLE_BATT_SERVER         0
#endif // defined(CFG_PRF_BASS)

///HID Device Role
#if defined(CFG_PRF_HOGPD)
#define BLE_HID_DEVICE          1
#else
#define BLE_HID_DEVICE          0
#endif // defined(CFG_PRF_HOGPD)

///HID Boot Host Role
#if defined(CFG_PRF_HOGPBH)
#define BLE_HID_BOOT_HOST       1
#else
#define BLE_HID_BOOT_HOST       0
#endif // defined(CFG_PRF_HOGPBH)

///HID Report Host Role
#if defined(CFG_PRF_HOGPRH)
#define BLE_HID_REPORT_HOST     1
#else
#define BLE_HID_REPORT_HOST     0
#endif // defined(CFG_PRF_HOGPRH)

/// Glucose Profile Collector Role
#if defined(CFG_PRF_GLPC)
#define BLE_GL_COLLECTOR        1
#else
#define BLE_GL_COLLECTOR        0
#endif // defined(CFG_PRF_GLPC)

/// Glucose Profile Sensor Role
#if defined(CFG_PRF_GLPS)
#define BLE_GL_SENSOR           1
#else
#define BLE_GL_SENSOR           0
#endif // defined(CFG_PRF_GLPS)

/// Running Speed and Cadence Profile Collector Role
#if defined(CFG_PRF_RSCPC)
#define BLE_RSC_COLLECTOR       1
#else
#define BLE_RSC_COLLECTOR       0
#endif // defined(CFG_PRF_RSCPC)

/// Running Speed and Cadence Profile Server Role
#if defined(CFG_PRF_RSCPS)
#define BLE_RSC_SENSOR          1
#else
#define BLE_RSC_SENSOR          0
#endif // defined(CFG_PRF_RSCPS)

/// Cycling Speed and Cadence Profile Collector Role
#if defined(CFG_PRF_CSCPC)
#define BLE_CSC_COLLECTOR       1
#else
#define BLE_CSC_COLLECTOR       0
#endif // defined(CFG_PRF_CSCPC)

/// Cycling Speed and Cadence Profile Server Role
#if defined(CFG_PRF_CSCPS)
#define BLE_CSC_SENSOR          1
#else
#define BLE_CSC_SENSOR          0
#endif // defined(CFG_PRF_CSCPS)

/// Cycling Power Profile Collector Role
#if defined(CFG_PRF_CPPC)
#define BLE_CP_COLLECTOR        1
#else
#define BLE_CP_COLLECTOR        0
#endif // defined (CFG_PRF_CPPC)

/// Cycling Power Profile Server Role
#if defined(CFG_PRF_CPPS)
#define BLE_CP_SENSOR            1
#else
#define BLE_CP_SENSOR            0
#endif // defined (CFG_PRF_CPPS)

/// Location and Navigation Profile Collector Role
#if defined(CFG_PRF_LANC)
#define BLE_LN_COLLECTOR        1
#else
#define BLE_LN_COLLECTOR        0
#endif // defined (CFG_PRF_LANC)

/// Location and Navigation Profile Server Role
#if defined(CFG_PRF_LANS)
#define BLE_LN_SENSOR            1
#else
#define BLE_LN_SENSOR            0
#endif // defined (CFG_PRF_LANS)

/// Alert Notification Profile Client Role
#if defined(CFG_PRF_ANPC)
#define BLE_AN_CLIENT           1
#else
#define BLE_AN_CLIENT           0
#endif // defined(CFG_PRF_ANPC)

/// Alert Notification Profile Server Role
#if defined(CFG_PRF_ANPS)
#define BLE_AN_SERVER           1
#else
#define BLE_AN_SERVER           0
#endif // defined(CFG_PRF_ANPS)

/// Phone Alert Status Profile Client Role
#if defined(CFG_PRF_PASPC)
#define BLE_PAS_CLIENT          1
#else
#define BLE_PAS_CLIENT          0
#endif // defined(CFG_PRF_PASPC)

/// Phone Alert Status Profile Server Role
#if defined(CFG_PRF_PASPS)
#define BLE_PAS_SERVER          1
#else
#define BLE_PAS_SERVER          0
#endif // defined(CFG_PRF_PASPS)

/// Internet Protocol Support Profile Server Role
#if defined(CFG_PRF_IPSS)
#define BLE_IPS_SERVER          1
#else
#define BLE_IPS_SERVER          0
#endif // defined(CFG_PRF_IPSS)

/// Internet Protocol Support Profile Client Role
#if defined(CFG_PRF_IPSC)
#define BLE_IPS_CLIENT          1
#else
#define BLE_IPS_CLIENT          0
#endif // defined(CFG_PRF_IPSC)

/// Environmental Sensing Profile Server Role
#if defined(CFG_PRF_ENVS)
#define BLE_ENV_SERVER          1
#else
#define BLE_ENV_SERVER          0
#endif // defined(CFG_PRF_ENVS)

/// Environmental Sensing Profile Client Role
#if defined(CFG_PRF_ENVC)
#define BLE_ENV_CLIENT          1
#else
#define BLE_ENV_CLIENT          0
#endif // defined(CFG_PRF_ENVC)

/// Weight Scale Profile Server Role
#if defined(CFG_PRF_WSCS)
#define BLE_WSC_SERVER          1
#else
#define BLE_WSC_SERVER          0
#endif // defined(CFG_PRF_WSCS)

/// Weight Scale Profile Client Role
#if defined(CFG_PRF_WSCC)
#define BLE_WSC_CLIENT          1
#else
#define BLE_WSC_CLIENT          0
#endif // defined(CFG_PRF_WSCC)

/// Body Composition Server Role
#if defined(CFG_PRF_BCSS)
#define BLE_BCS_SERVER          1
#else
#define BLE_BCS_SERVER          0
#endif // defined(CFG_PRF_BCSS)

/// Body Composition Client Role
#if defined(CFG_PRF_BCSC)
#define BLE_BCS_CLIENT          1
#else
#define BLE_BCS_CLIENT          0
#endif // defined(CFG_PRF_BCSC)

/// User Data Service Server Role
#if defined(CFG_PRF_UDSS)
#define BLE_UDS_SERVER          1
#else
#define BLE_UDS_SERVER          0
#endif // defined(CFG_PRF_UDSS)

/// User Data Service Client Role
#if defined(CFG_PRF_UDSC)
#define BLE_UDS_CLIENT          1
#else
#define BLE_UDS_CLIENT          0
#endif // defined(CFG_PRF_UDSC)

#if defined(CFG_VOICEPATH)
#define BLE_VOICEPATH            1
#else
#define BLE_VOICEPATH            0
#endif    // defined(CFG_VOICEPATH)

#if defined(CFG_TILE)
#define BLE_TILE            1
#else
#define BLE_TILE            0
#endif    // 

#if defined(__AMA_VOICE__)
#define BLE_AMA_VOICE    1
#else
#define BLE_AMA_VOICE    0
#endif

#if defined(__DMA_VOICE__)
#define BLE_DMA_VOICE    1
#else
#define BLE_DMA_VOICE    0
#endif

#if defined(__GMA_VOICE__)
#define BLE_GMA_VOICE    1
#else
#define BLE_GMA_VOICE    0
#endif

#if defined(__SMART_VOICE__)
#define BLE_SMART_VOICE    1
#else
#define BLE_SMART_VOICE    0
#endif

#if defined(__TENCENT_VOICE__)
#define BLE_TENCENT_VOICE    1
#else
#define BLE_TENCENT_VOICE    0
#endif

#if defined(__CUSTOMIZE_VOICE__)
#define BLE_CUSTOMIZE_VOICE    1
#else
#define BLE_CUSTOMIZE_VOICE    0
#endif

#if defined(DUAL_MIC_RECORDING)
#define BLE_DUAL_MIC_REC_VOICE    1
#else
#define BLE_DUAL_MIC_REC_VOICE    0
#endif

#if defined(CFG_AI_VOICE)
#define BLE_AI_VOICE            1
#else
#define BLE_AI_VOICE            0
#endif

/// Data Path Service Server Role
#if defined(CFG_PRF_DATAPATH_SERVER)
#define BLE_DATAPATH_SERVER     1
#else
#define BLE_DATAPATH_SERVER     0
#endif // defined(CFG_PRF_DATAPATH_SERVER)

#if defined(CFG_OTA)
#define BLE_OTA            1
#else
#define BLE_OTA            0
#endif    // defined(CFG_OTA)

#if defined(CFG_TOTA)
#define BLE_TOTA            1
#else
#define BLE_TOTA            0
#endif    // defined(CFG_TOTA)

#if defined(BMS_ENABLED)
#define BLE_BMS     1
#else
#define BLE_BMS     0
#endif  // defined(BMS_ENABLED)

/// ANCS Profile Client Role
#if defined(ANCC_ENABLED)
#define BLE_ANC_CLIENT          1
#else
#define BLE_ANC_CLIENT          0
#endif // defined(ANCC_ENABLED)

/// AMS Profile Client Role
#if defined(AMSC_ENABLED)
#define BLE_AMS_CLIENT          1
#else
#define BLE_AMS_CLIENT          0
#endif // defined(AMSC_ENABLED)

#if defined(ANCS_ENABLED)
#define BLE_ANCS_PROXY          1
#else
#define BLE_ANCS_PROXY          0
#endif // defined(ANCS_ENABLED)

#if defined(AMS_ENABLED)
#define BLE_AMS_PROXY           1
#else
#define BLE_AMS_PROXY           0
#endif // defined(AMS_ENABLED)

#if defined(CFG_PRF_GFPS_PROVIDER)
#define BLE_GFPS_PROVIDER       1
#else 
#define BLE_GFPS_PROVIDER       0
#endif

#ifdef AOB_UX_ENABLED
#define BLE_BUDS                1
#else
#define BLE_BUDS                0
#endif

/// Coordinated Set Identification Profile Set Member Role
#if defined(CFG_PRF_CSISM)
#define BLE_CSIS_MEMBER         1
#if defined(CFG_HL_MSG_API) && (!defined(CFG_ATC_CSISM) || defined(CFG_GAF_MSG_DBG))
#define BLE_CSISM_MSG_API       1
#else //defined(CFG_HL_MSG_API) && (!defined(CFG_ATC_CSISM) || defined(CFG_GAF_MSG_DBG))
#define BLE_CSISM_MSG_API       0
#endif //defined(CFG_HL_MSG_API) && (!defined(CFG_ATC_CSISM) || defined(CFG_GAF_MSG_DBG))
#else
#define BLE_CSIS_MEMBER         0
#define BLE_CSISM_MSG_API       0
#endif // defined(CFG_PRF_CSISM)

/// Coordinated Set Identification Profile Set Coordinator Role
#if defined(CFG_PRF_CSISC)
#define BLE_CSIS_COORD          1
#if defined(CFG_HL_MSG_API) && (!defined(CFG_ATC_CSISC) || defined(CFG_GAF_MSG_DBG))
#define BLE_CSISC_MSG_API       1
#else //defined(CFG_HL_MSG_API) && (!defined(CFG_ATC_CSISC) || defined(CFG_GAF_MSG_DBG))
#define BLE_CSISC_MSG_API       0
#endif //defined(CFG_HL_MSG_API) && (!defined(CFG_ATC_CSISC) || defined(CFG_GAF_MSG_DBG))
#else
#define BLE_CSIS_COORD          0
#define BLE_CSISC_MSG_API       0
#endif // defined(CFG_PRF_CSISC)

/// Object Transfer Profile Server Role
#if defined(CFG_PRF_OTS)
#define BLE_OT_SERVER           1
#else
#define BLE_OT_SERVER           0
#endif // defined(CFG_PRF_OTS)

/// Object Transfer Profile Client Role
#if defined(CFG_PRF_OTC)
#define BLE_OT_CLIENT           1
#else
#define BLE_OT_CLIENT           0
#endif // defined(CFG_PRF_OTC)

#if defined(USE_BT_ADAPTER)
#define BLE_ADAPTER_SERVER_CLIENT 1
#else
#define BLE_ADAPTER_SERVER_CLIENT 0
#endif

/// BLE_CLIENT_PRF indicates if at least one client profile is present
#if (BLE_PROX_MONITOR || BLE_FINDME_LOCATOR || BLE_HT_COLLECTOR || BLE_BP_COLLECTOR \
        || BLE_HR_COLLECTOR || BLE_DIS_CLIENT || BLE_TIP_CLIENT || BLE_SP_CLIENT \
        || BLE_BATT_CLIENT || BLE_GL_COLLECTOR || BLE_HID_BOOT_HOST || BLE_HID_REPORT_HOST \
        || BLE_RSC_COLLECTOR || BLE_CSC_COLLECTOR || BLE_CP_COLLECTOR || BLE_LN_COLLECTOR || BLE_AN_CLIENT \
        || BLE_PAS_CLIENT || BLE_IPS_CLIENT || BLE_ENV_CLIENT || BLE_WSC_CLIENT \
        || BLE_UDS_CLIENT || BLE_BCS_CLIENT || BLE_VOICEPATH || BLE_DATAPATH_SERVER \
        || BLE_BUDS || BLE_ANC_CLIENT || BLE_BMS || BLE_OTA||BLE_AI_VOICE || BLE_TOTA \
        || BLE_TILE || BLE_CSIS_COORD || BLE_OT_CLIENT || BLE_MESH || BLE_ADAPTER_SERVER_CLIENT)
#define BLE_CLIENT_PRF          1
#else
#define BLE_CLIENT_PRF          0
#endif //(BLE_PROX_MONITOR || BLE_FINDME_LOCATOR ...)

/// BLE_SERVER_PRF indicates if at least one server profile is present
#if (BLE_PROX_REPORTER || BLE_FINDME_TARGET || BLE_HT_THERMOM || BLE_BP_SENSOR \
        || BLE_TIP_SERVER || BLE_HR_SENSOR || BLE_DIS_SERVER || BLE_SP_SERVER \
        || BLE_BATT_SERVER || BLE_HID_DEVICE || BLE_GL_SENSOR || BLE_RSC_SENSOR  \
        || BLE_CSC_SENSOR || BLE_CP_SENSOR || BLE_LN_SENSOR || BLE_AN_SERVER \
        || BLE_PAS_SERVER || BLE_IPS_SERVER || BLE_ENV_SERVER || BLE_WSC_SERVER \
        || BLE_UDS_SERVER || BLE_BCS_SERVER || BLE_TOTA || BLE_CSIS_MEMBER \
        || BLE_OT_SERVER || BLE_MESH || BLE_ADAPTER_SERVER_CLIENT)
#define BLE_SERVER_PRF          1
#else
#define BLE_SERVER_PRF          0
#endif //(BLE_PROX_REPORTER || BLE_FINDME_TARGET ...)


#if (BLE_ISO_MODE_0_PROFILE || BLE_WPT_SERVER || BLE_WPT_CLIENT || defined(BISTO_ENABLED) || defined(ANCS_ENABLED) || defined(ANCC_ENABLED) || defined(AMS_ENABLED) || defined(AMSC_ENABLED))
#define BLE_PRF_PROPRIETARY_SVC_SUPPORT     (1)
#else // !(BLE_ISO_MODE_0_PROFILE || BLE_WPT_SERVER || BLE_WPT_CLIENT)
#define BLE_PRF_PROPRIETARY_SVC_SUPPORT     (0)
#endif // (BLE_ISO_MODE_0_PROFILE || BLE_WPT_SERVER || BLE_WPT_CLIENT)

//Force ATT parts depending on profile roles or compile options
/// Attribute Client
#if (BLE_CLIENT_PRF)
#define BLE_ATTC                    1
#endif //(BLE_CLIENT_PRF)

/// Attribute Server
#if (BLE_SERVER_PRF)
#define BLE_ATTS                    1
#endif //(BLE_SERVER_PRF)


#elif (BLE_OBSERVER || BLE_BROADCASTER)
/// Proximity Profile Monitor Role
#define BLE_PROX_MONITOR            0
/// Proximity Profile Reporter Role
#define BLE_PROX_REPORTER           0
///Find Me Profile Locator role
#define BLE_FINDME_LOCATOR          0
///Find Me Profile Target role
#define BLE_FINDME_TARGET           0
///Health Thermometer Profile Collector Role
#define BLE_HT_COLLECTOR            0
///Health Thermometer Profile Thermometer Role
#define BLE_HT_THERMOM              0
///Blood Pressure Profile Collector Role
#define BLE_BP_COLLECTOR            0
///Blood Pressure Profile Sensor Role
#define BLE_BP_SENSOR               0
///Heart Rate Profile Collector Role
#define BLE_HR_COLLECTOR            0
///Heart Rate Profile Sensor Role
#define BLE_HR_SENSOR               0
///Time Profile Client Role
#define BLE_TIP_CLIENT              0
///Time Profile Server Role
#define BLE_TIP_SERVER              0
/// Device Information Service Client Role
#define BLE_DIS_CLIENT              0
/// Device Information Service Server Role
#define BLE_DIS_SERVER              0
/// Scan Parameter Profile Client Role
#define BLE_SP_CLIENT               0
/// Scan Parameter Profile Server Role
#define BLE_SP_SERVER               0
/// Battery Service Client Role
#define BLE_BATT_CLIENT             0
/// Battery Service Server Role
#define BLE_BATT_SERVER             0
/// HID Device Role
#define BLE_HID_DEVICE              0
/// HID Boot Host Role
#define BLE_HID_BOOT_HOST           0
/// HID Report Host Role
#define BLE_HID_REPORT_HOST         0
/// Glucose Profile Collector Role
#define BLE_GL_COLLECTOR            0
/// Glucose Profile Sensor Role
#define BLE_GL_SENSOR               0
/// Running Speed and Cadence Collector Role
#define BLE_RSC_COLLECTOR           0
/// Running Speed and Cadence Server Role
#define BLE_RSC_SENSOR              0
/// Cycling Speed and Cadence Collector Role
#define BLE_CSC_COLLECTOR           0
/// Cycling Speed and Cadence Server Role
#define BLE_CSC_SENSOR              0
/// Cycling Power Collector Role
#define BLE_CP_COLLECTOR            0
/// Cycling Power Server Role
#define BLE_CP_SENSOR               0
/// Location and Navigation Collector Role
#define BLE_LN_COLLECTOR            0
/// Location and Navigation Server Role
#define BLE_LN_SENSOR               0
/// Alert Notification Client Role
#define BLE_AN_CLIENT               0
/// Alert Notification Server Role
#define BLE_AN_SERVER               0
/// Phone Alert Status Client Role
#define BLE_PAS_CLIENT              0
/// Phone Alert Status Server Role
#define BLE_PAS_SERVER              0
/// Internet Protocol Support Profile Server Role
#define BLE_IPS_SERVER              0
/// Internet Protocol Support Profile Client Role
#define BLE_IPS_CLIENT              0
/// Environmental Sensing Profile Server Role
#define BLE_ENV_SERVER              0
/// Environmental Sensing Profile Client Role
#define BLE_ENV_CLIENT              0
/// Weight Scale Profile Server Role
#define BLE_WSC_SERVER              0
/// Weight Scale Profile Client Role
#define BLE_WSC_CLIENT              0
/// Body Composition Profile Client Role 
#define BLE_BCS_CLIENT              0
/// Body Composition Profile Server Role
#define BLE_BCS_SERVER              0
/// User Data Service Server Role
#define BLE_UDS_SERVER              0
/// User Data Service Client Role
#define BLE_UDS_CLIENT              0

//Force ATT parts to 0
/// External database management
#define BLE_EXT_ATTS_DB             0
/// Profile Server
#define BLE_SERVER_PRF              0
/// Profile Client
#define BLE_CLIENT_PRF              0
#endif //(BLE_OBSERVER || BLE_BROADCASTER)


/// Profile identifiers
enum rwprf_id
{
    #if (BLE_HT_THERMOM)
    PRF_ID_HTPT,
    #endif // (BLE_HT_THERMOM)

    #if (BLE_HT_COLLECTOR)
    PRF_ID_HTPC,
    #endif // (BLE_HT_COLLECTOR)

    #if (BLE_DIS_SERVER)
    PRF_ID_DISS,
    #endif // (BLE_DIS_SERVER)

    #if (BLE_DIS_CLIENT)
    PRF_ID_DISC,
    #endif // (BLE_DIS_CLIENT)

    #if (BLE_BP_SENSOR)
    PRF_ID_BLPS,
    #endif // (BLE_BP_SENSOR)

    #if (BLE_BP_COLLECTOR)
    PRF_ID_BLPC,
    #endif // (BLE_BP_COLLECTOR)

    #if (BLE_TIP_SERVER)
    PRF_ID_TIPS,
    #endif // (BLE_TIP_SERVER)

    #if (BLE_TIP_CLIENT)
    PRF_ID_TIPC,
    #endif // (BLE_TIP_CLIENT)

    #if (BLE_HR_SENSOR)
    PRF_ID_HRPS,
    #endif // (BLE_HR_SENSOR)

    #if (BLE_HR_COLLECTOR)
    PRF_ID_HRPC,
    #endif // (BLE_HR_COLLECTOR)

    #if (BLE_FINDME_LOCATOR)
    PRF_ID_FINDL,
    #endif // (BLE_FINDME_LOCATOR)

    #if (BLE_FINDME_TARGET)
    PRF_ID_FINDT,
    #endif // (BLE_FINDME_TARGET)

    #if (BLE_PROX_MONITOR)
    PRF_ID_PROXM,
    #endif // (BLE_PROX_MONITOR)

    #if (BLE_PROX_REPORTER)
    PRF_ID_PROXR,
    #endif // (BLE_PROX_REPORTER)

    #if (BLE_SP_SERVER)
    PRF_ID_SCPPS,
    #endif // (BLE_SP_SERVER)

    #if (BLE_SP_CLIENT)
    PRF_ID_SCPPC,
    #endif // (BLE_SP_CLIENT)

    #if (BLE_BATT_SERVER)
    PRF_ID_BASS,
    #endif // (BLE_BATT_SERVER)

    #if (BLE_BATT_CLIENT)
    PRF_ID_BASC,
    #endif // (BLE_BATT_CLIENT)

    #if (BLE_HID_DEVICE)
    PRF_ID_HOGPD,
    #endif // (BLE_HID_DEVICE)

    #if (BLE_HID_BOOT_HOST)
    PRF_ID_HOGPBH,
    #endif // (BLE_HID_BOOT_HOST)

    #if (BLE_HID_REPORT_HOST)
    PRF_ID_HOGPRH,
    #endif // (BLE_HID_REPORT_HOST)

    #if (BLE_GL_COLLECTOR)
    PRF_ID_GLPC,
    #endif // (BLE_GL_COLLECTOR)

    #if (BLE_GL_SENSOR)
    PRF_ID_GLPS,
    #endif // (BLE_GL_SENSOR)

    #if (BLE_RSC_COLLECTOR)
    PRF_ID_RSCPC,
    #endif // (BLE_RSC_COLLECTOR)

    #if (BLE_RSC_SENSOR)
    PRF_ID_RSCPS,
    #endif // (BLE_RSC_SENSOR)

    #if (BLE_CSC_COLLECTOR)
    PRF_ID_CSCPC,
    #endif // (BLE_CSC_COLLECTOR)

    #if (BLE_CSC_SENSOR)
    PRF_ID_CSCPS,
    #endif // (BLE_CSC_SENSOR)

    #if (BLE_CP_COLLECTOR)
    PRF_ID_CPPC,
    #endif // (BLE_CP_COLLECTOR)

    #if (BLE_CP_SENSOR)
    PRF_ID_CPPS,
    #endif // (BLE_CP_SENSOR)

    #if (BLE_LN_COLLECTOR)
    PRF_ID_LANC,
    #endif // (BLE_LN_COLLECTOR)

    #if (BLE_LN_SENSOR)
    PRF_ID_LANS,
    #endif // (BLE_LN_SENSOR)

    #if (BLE_AN_CLIENT)
    PRF_ID_ANPC,
    #endif // (BLE_AN_CLIENT)

    #if (BLE_AN_SERVER)
    PRF_ID_ANPS,
    #endif // (BLE_AN_SERVER)

    #if (BLE_PAS_CLIENT)
    PRF_ID_PASPC,
    #endif // (BLE_PAS_CLIENT)

    #if (BLE_PAS_SERVER)
    PRF_ID_PASPS,
    #endif // (BLE_PAS_SERVER)

    #if (BLE_ISO_MODE_0_PROFILE)
    PRF_ID_AM0,
    #endif // (BLE_ISO_MODE_0_PROFILE)

    #if (BLE_IPS_SERVER)
    PRF_ID_IPSS,
    #endif //(BLE_IPS_SERVER)

    #if (BLE_IPS_CLIENT)
    PRF_ID_IPSC,
    #endif //(BLE_IPS_CLIENT)

    #if (BLE_ENV_SERVER)
    PRF_ID_ENVS,
    #endif //(BLE_ENV_SERVER)

    #if (BLE_ENV_CLIENT)
    PRF_ID_ENVC,
    #endif //(BLE_ENV_CLIENT)

    #if (BLE_WSC_SERVER)
    PRF_ID_WSCS,
    #endif //(BLE_WSC_SERVER)

    #if (BLE_WSC_CLIENT)
    PRF_ID_WSCC,
    #endif //(BLE_WSC_CLIENT)

    #if (BLE_BCS_SERVER)
    PRF_ID_BCSS,
    #endif //(BLE_BCS_SERVER)

    #if (BLE_BCS_CLIENT)
    PRF_ID_BCSC,
    #endif //(BLE_BCS_CLIENT)

    #if (BLE_UDS_SERVER)
    PRF_ID_UDSS,
    #endif //(BLE_UDS_SERVER)

    #if (BLE_UDS_CLIENT)
    PRF_ID_UDSC,
    #endif //(BLE_UDS_CLIENT)

    #if (BLE_CSIS_MEMBER)
    PRF_ID_CSISM,
    #endif //(BLE_CSIS_MEMBER)

    #if (BLE_CSIS_COORD)
    PRF_ID_CSISC,
    #endif //(BLE_CSIS_COORD)

    #if (BLE_OT_SERVER)
    PRF_ID_OTS,
    #endif //(BLE_OT_SERVER)

    #if (BLE_OT_CLIENT)
    PRF_ID_OTC,
    #endif //(BLE_OT_CLIENT)

    #if (BLE_MESH)
    PRF_ID_MESH,
    #endif //(BLE_MESH)

    #if (BLE_VOICEPATH)
    PRF_ID_VOICEPATH,
    #endif //(BLE_VOICEPATH)

    #if (BLE_OTA)
    PRF_ID_OTA,
    #endif //(BLE_OTA)

    #if (BLE_TOTA)
    PRF_ID_TOTA,
    #endif //(BLE_TOTA)

    #if (BLE_BMS)
    PRF_ID_BMS,
    #endif //(BLE_BMS)

    #if (BLE_ANC_CLIENT)
    PRF_ID_ANCC,
    #endif //(BLE_ANC_CLIENT)

    #if (BLE_AMS_CLIENT)
    PRF_ID_AMSC,
    #endif //(BLE_AMS_CLIENT)

    #if (BLE_TILE)
    PRF_ID_TILE,
    #endif //(BLE_TILE)

    #if (BLE_ANCS_PROXY)
    PRF_ID_ANCSP,
    #endif //(BLE_ANCS_PROXY)

    #if (BLE_AMS_PROXY)
    PRF_ID_AMSP,
    #endif //(BLE_AMS_PROXY)

    #if (BLE_GFPS_PROVIDER)
    PRF_ID_GFPSP,
    #endif //(BLE_GFPS_PROVIDER)

    #if (BLE_AI_VOICE)
    PRF_ID_AI,
    #endif //(BLE_AI_VOICE)

    #if (BLE_AMA_VOICE)
    PRF_ID_AMA,
    #endif //(BLE_AI_VOICE)

    #if (BLE_DMA_VOICE)
    PRF_ID_DMA,
    #endif //(BLE_AI_VOICE)

    #if (BLE_GMA_VOICE)
    PRF_ID_GMA,
    #endif //(BLE_AI_VOICE)

    #if (BLE_SMART_VOICE)
    PRF_ID_SMVOICE,
    #endif //(BLE_AI_VOICE)

    #if (BLE_TENCENT_VOICE)
    PRF_ID_TENCENT,
    #endif //(BLE_AI_VOICE)

    #if (BLE_CUSTOMIZE_VOICE)
    PRF_ID_CUSTOMIZE,
    #endif //(BLE_AI_VOICE)

    #if (BLE_DATAPATH_SERVER)
    PRF_ID_DATAPATHPS,
    #endif //(BLE_DATAPATH_SERVER)

    #if (BLE_BUDS)
    PRF_ID_BUDS,
    #endif //(BLE_BUDS)

#ifdef USE_BT_ADAPTER
    PRF_ID_BLE_ADAPTER_SERVER,
    PRF_ID_BLE_ADAPTER_SERVER_END = PRF_ID_BLE_ADAPTER_SERVER + (BLE_ADAPTER_PROFILES_NUM -1),
    PRF_ID_BLE_ADAPTER_CLIENT,
#endif
    PRF_ID_MAX,

    /// Invalid profile identifier
    PRF_ID_INVALID  = 0xFF,
};

/// @} PRF_CONFIG

#endif /* _RWPRF_CONFIG_H_ */
