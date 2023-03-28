/**
 ****************************************************************************************
 *
 * @file app_mesh.h
 *
 * @brief Application Mesh - Header File
 *
 * Copyright (C) RivieraWaves 2019-2019
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_MESH
 * @{
 ****************************************************************************************
 */

#ifndef APP_MESH_H_
#define APP_MESH_H_

#if (BLE_APP_MESH)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ke_task.h"
#include "../../ip/ble/mesh/api/mesh_api_msg.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Mesh application state machine
enum app_mesh_evt_state
{
    /// Mesh profile task added
    APP_MESH_EVT_TASK_ADDED = 0,
    /// Register server configured
    APP_MESH_EVT_MDL_REGISTERED,
    /// Storage loaded
    APP_MESH_EVT_STORED_INFO_LOADED,
    /// Models configured
    APP_MESH_EVT_MDL_CONFIGURED,
    /// Mesh task enabled
    APP_MESH_EVT_ENABLED,
};

/// Mesh demonstration type
enum app_mesh_demo_type
{
    /// Proxy
    APP_MESH_DEMO_PROXY = 0,
    /// Generic OnOff Server
    APP_MESH_DEMO_GENS_ONOFF,
    /// Generic OnOff Client
    APP_MESH_DEMO_GENC_ONOFF,
    /// Light Lightness Server
    APP_MESH_DEMO_LIGHTS_LN,
    /// Light Lightness Client
    APP_MESH_DEMO_LIGHTC_LN,
};

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */

extern const struct app_subtask_handlers app_mesh_handlers;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the mesh application
 *
 * @param[in] mesh_demo_type    Mesh demonstration type
 ****************************************************************************************
 */
void app_mesh_init(uint8_t mesh_demo_type);

/**
 ****************************************************************************************
 * @brief Add a Mesh Service instance in the database
 ****************************************************************************************
 */
void app_mesh_add_svc(void);

/**
 ****************************************************************************************
 * @brief Management of the mesh state machine
 *
 * @param[in] evt    Event code of current event status
 ****************************************************************************************
 */
void app_mesh_evt(uint8_t evt);

/**
 ****************************************************************************************
 * @brief Update the profile task number
 *
 * @param[in] task    Profile task number.
 ****************************************************************************************
 */
void app_mesh_set_prf_task(uint16_t task);

#endif //(BLE_APP_MESH)

#endif // APP_MESH_H_

/// @} APP_MESH

