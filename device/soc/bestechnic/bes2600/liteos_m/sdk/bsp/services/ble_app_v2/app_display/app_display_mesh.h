/**
 ****************************************************************************************
 *
 * @file app_display_mesh.h
 *
 * @brief Application Display for Mesh Demonstration - Header File
 *
 * Copyright (C) RivieraWaves 2019-2019
 *
 ****************************************************************************************
 */

#ifndef APP_DISPLAY_MESH_H_
#define APP_DISPLAY_MESH_H_

#if (BLE_APP_MESH)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ke_msg.h"

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern const struct app_subtask_handlers app_display_mesh_handlers;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initilize display module for mesh demonstration
 *
 * @param[in] p_dev_name        Device name
 * @param[in] mesh_demo_type    Mesh demonstration type
 ****************************************************************************************
 */
void app_display_mesh_init(uint8_t* p_dev_name, uint8_t mesh_demo_type);

#endif //(BLE_APP_MESH)

#endif //APP_DISPLAY_MESH_H_

