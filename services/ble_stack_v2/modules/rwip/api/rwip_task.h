/**
 ****************************************************************************************
 *
 * @file rwip_task.h
 *
 * @brief Task Identifier description for the RW IP
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */

#ifndef RWIP_TASK_H_
#define RWIP_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 *
 *  Information about RW SW TASK
 *
 * @name RW TASK Configuration
 * @{
 ****************************************************************************************
 */

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */


/// Build the first message ID of a task. (in fact a ke_msg_id_t)
#define TASK_FIRST_MSG(task) ((uint16_t)((task) << 8))

/// Builds the task identifier from the type and the index of that task.
#define TASK_BUILD(type, index) ((uint16_t)(((index) << 8)|(type)) )

/// Retrieves task type from task id.
#define TASK_TYPE_GET(ke_task_id) (((uint16_t)ke_task_id) & 0xFF)

/// Retrieves task index number from task id.
#define TASK_IDX_GET(ke_task_id) ((((uint16_t)ke_task_id) >> 8) & 0xFF)

/// Message identifier index
#define MSG_ID(task, idx) (TASK_FIRST_MSG((TASK_ID_ ## task)) + idx)

/// Tasks types definition, this value shall be in [0-254] range
/*@TRACE*/
enum TASK_API_ID
{
    // -----------------------------------------------------------------------------------
    // ---------------------- Controller Task identifer ----------------------------------
    // -----------------------------------------------------------------------------------
    // Link Layer Tasks
    TASK_ID_LLM          = 0,   // BLE Link manager
    TASK_ID_LLC          = 1,   // BLE Link controller
    TASK_ID_LLD          = 2,   // BLE Link driver
    TASK_ID_LLI          = 3,   // BLE Link ISO

    TASK_ID_DBG          = 4,   // Debug task

    // BT Controller Tasks
    TASK_ID_LM           = 5,   // BT Link manager
    TASK_ID_LC           = 6,   // BT Link controller
    TASK_ID_LB           = 7,   // BT Broadcast
    TASK_ID_LD           = 8,   // BT Link driver

    // -----------------------------------------------------------------------------------
    // --------------------- BLE HL TASK API Identifiers ---------------------------------
    // ---------------------     SHALL NOT BE CHANGED    ---------------------------------
    // -----------------------------------------------------------------------------------

    TASK_ID_L2CAP        = 10,   // L2CAP Task
    TASK_ID_GATT         = 11,   // Generic Attribute Profile Task
    TASK_ID_GAPM         = 13,   // Generic Access Profile Manager
    TASK_ID_GAPC         = 14,   // Generic Access Profile Controller
    TASK_ID_APP          = 15,   // Application API

    // -----------------------------------------------------------------------------------
    // --------------------- TRANSPORT AND PLATFORM TASKS --------------------------------
    // -----------------------------------------------------------------------------------
    TASK_ID_AHI          = 16,   // Application Host Interface
    TASK_ID_HCI          = 17,   // Host to Control Interface
    TASK_ID_DISPLAY      = 19,   // LCD/Display task

    // -----------------------------------------------------------------------------------
    // --------------------- BLE Profile TASK API Identifiers ----------------------------
    // ---------------------     SHALL NOT BE CHANGED    ---------------------------------
    // -----------------------------------------------------------------------------------
    TASK_ID_DISS         = 20,   // Device Information Service Server Task
    TASK_ID_DISC         = 21,   // Device Information Service Client Task

    TASK_ID_PROXM        = 22,   // Proximity Monitor Task
    TASK_ID_PROXR        = 23,   // Proximity Reporter Task

    TASK_ID_FINDL        = 24,   // Find Me Locator Task
    TASK_ID_FINDT        = 25,   // Find Me Target Task

    TASK_ID_HTPC         = 26,   // Health Thermometer Collector Task
    TASK_ID_HTPT         = 27,   // Health Thermometer Sensor Task

    TASK_ID_BLPS         = 28,   // Blood Pressure Sensor Task
    TASK_ID_BLPC         = 29,   // Blood Pressure Collector Task

    TASK_ID_HRPS         = 30,   // Heart Rate Sensor Task
    TASK_ID_HRPC         = 31,   // Heart Rate Collector Task

    TASK_ID_TIPS         = 32,   // Time Server Task
    TASK_ID_TIPC         = 33,   // Time Client Task

    TASK_ID_SCPPS        = 34,   // Scan Parameter Profile Server Task
    TASK_ID_SCPPC        = 35,   // Scan Parameter Profile Client Task

    TASK_ID_BASS         = 36,   // Battery Service Server Task
    TASK_ID_BASC         = 37,   // Battery Service Client Task

    TASK_ID_HOGPD        = 38,   // HID Device Task
    TASK_ID_HOGPBH       = 39,   // HID Boot Host Task
    TASK_ID_HOGPRH       = 40,   // HID Report Host Task

    TASK_ID_GLPS         = 41,   // Glucose Profile Sensor Task
    TASK_ID_GLPC         = 42,   // Glucose Profile Collector Task

    TASK_ID_RSCPS        = 43,   // Running Speed and Cadence Profile Server Task
    TASK_ID_RSCPC        = 44,   // Running Speed and Cadence Profile Collector Task

    TASK_ID_CSCPS        = 45,   // Cycling Speed and Cadence Profile Server Task
    TASK_ID_CSCPC        = 46,   // Cycling Speed and Cadence Profile Client Task

    TASK_ID_ANPS         = 47,   // Alert Notification Profile Server Task
    TASK_ID_ANPC         = 48,   // Alert Notification Profile Client Task

    TASK_ID_PASPS        = 49,   // Phone Alert Status Profile Server Task
    TASK_ID_PASPC        = 50,   // Phone Alert Status Profile Client Task

    TASK_ID_CPPS         = 51,   // Cycling Power Profile Server Task
    TASK_ID_CPPC         = 52,   // Cycling Power Profile Client Task

    TASK_ID_LANS         = 53,   // Location and Navigation Profile Server Task
    TASK_ID_LANC         = 54,   // Location and Navigation Profile Client Task

    TASK_ID_IPSS         = 55,   // Internet Protocol Support Profile Server Task
    TASK_ID_IPSC         = 56,   // Internet Protocol Support Profile Client Task

    TASK_ID_ENVS         = 57,   // Environmental Sensing Profile Server Task
    TASK_ID_ENVC         = 58,   // Environmental Sensing Profile Client Task

    TASK_ID_WSCS         = 59,   // Weight Scale Profile Server Task
    TASK_ID_WSCC         = 60,   // Weight Scale Profile Client Task

    TASK_ID_UDSS         = 61,   // User Data Service Server Task
    TASK_ID_UDSC         = 62,   // User Data Service Client Task

    TASK_ID_BCSS         = 63,   // Body Composition Server Task
    TASK_ID_BCSC         = 64,   // Body Composition Client Task

    TASK_ID_GFPSP        = 65,   //google fast pair service provider
    TASK_ID_GFPSS        = 66,   // seeker 

    TASK_ID_VOICEPATH    = 67,   // Voice Path Task

    TASK_ID_OTA          = 68,   // OTA Task

    TASK_ID_ANCC         = 69,   // ANCS Client Task

    TASK_ID_AMSC         = 70,   // AMS Client Task

    TASK_ID_BMS          = 71,   // BMS Task

    TASK_ID_ANCSP        = 72,   // ANCS Proxy Task

    TASK_ID_AMSP         = 73,   // AMS Proxy Task

    TASK_ID_DATAPATHPS   = 74,   // Datapath Server Task

    TASK_ID_AI           = 75,   // ai Task

    TASK_ID_TOTA         = 76,   // TOTA Task

    TASK_ID_TILE         = 77,   // skull tile task

    TASK_ID_CSISM        = 78,   // Coordinated Set Identification Profile Set Member Task
    TASK_ID_CSISC        = 79,   // Coordinated Set Identification Profile Set Coordinator Task

    TASK_ID_OTS          = 80,   // Object Transfer Profile Server Task
    TASK_ID_OTC          = 81,   // Object Transfer Profile Client Task

    TASK_ID_AMA          = 82,   // AMA Task

    TASK_ID_SMART        = 83,   // SMART Task

    TASK_ID_DMA          = 84,   // DMA Task

    TASK_ID_TENCENT      = 85,   // TENCENT Task

    TASK_ID_GMA          = 86,   // GMA Task

    TASK_ID_CUSTOMIZE    = 87,   // CUSTOMIZE Task

#ifdef USE_BT_ADAPTER
    TASK_ID_BLE_ADAPTER_SERVER,
    TASK_ID_BLE_ADAPTER_SERVER_END = TASK_ID_BLE_ADAPTER_SERVER + (BLE_ADAPTER_PROFILES_NUM - 1),
    TASK_ID_BLE_ADAPTER_CLIENT,
#endif 
    TASK_ID_MESH         = 200,  // Mesh Task

    TASK_ID_GAF          = 210,  // Generic Audio Framework
    
    TASK_ID_BUDS         = 211,

    TASK_ID_AM0          = 240,  // BLE Audio Mode 0

    TASK_ID_INVALID      = 0xFF, // Invalid Task Identifier
};

/// @} BT Stack Configuration
/// @} ROOT

#endif //RWIP_CONFIG_H_
