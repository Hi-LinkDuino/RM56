/**
 ****************************************************************************************
 *
 * @file app_display.h
 *
 * @brief Application Display entry point
 *
 * Copyright (C) RivieraWaves 2019-2019
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup app
 * @{
 ****************************************************************************************
 */

#ifndef APP_DISPLAY_H_
#define APP_DISPLAY_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if (BLE_APP_PRESENT)
#include <co_bt.h>           // Common BT definitions

#if (BLE_APP_MESH)
/*
 * INTERNAL CALLBACK FUNCTIONS PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief The prototype of generic server OnOff callback function
 ****************************************************************************************
 */
typedef void (*app_display_mesh_gens_onoff_cb)(bool onoff);

/**
 ****************************************************************************************
 * @brief The prototype of generic client OnOff callback function
 ****************************************************************************************
 */
typedef void (*app_display_mesh_genc_onoff_cb)(bool onoff);

/**
 ****************************************************************************************
 * @brief The prototype of lightness client lightness callback function
 ****************************************************************************************
 */
typedef void (*app_display_mesh_lightc_lightness_cb)(uint16_t lightness);

/**
 ****************************************************************************************
 * @brief The prototype of saving configuration callback function
 ****************************************************************************************
 */
typedef void (*app_display_mesh_save_config_cb)(void);

/**
 ****************************************************************************************
 * @brief The prototype of removing mesh specific callback function
 ****************************************************************************************
 */
typedef void (*app_display_mesh_remove_mesh_spec_cb)(void);



/*
 * TYPE DEFINTIIONS
 ****************************************************************************************
 */

/// Callback Structure
typedef struct app_display_mesh_cb
{
    /// Update generic server on off state
    app_display_mesh_gens_onoff_cb          cb_gens_onoff;
    /// Update generic client on off state
    app_display_mesh_genc_onoff_cb          cb_genc_onoff;
    /// Update Lightness Client Lightness value
    app_display_mesh_lightc_lightness_cb    cb_lightc_lightness;
    /// Save the configuretion
    app_display_mesh_save_config_cb         cb_save;
    /// Remove the mesh specific
    app_display_mesh_remove_mesh_spec_cb    cb_remove;
} app_display_mesh_cb_t;
#endif //(BLE_APP_MESH)


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the application display
 *
 * @param[in] dev_name                Device name
 * @param[in] demo_type               Demonstration type
 ****************************************************************************************
 */
void app_display_init(uint8_t* p_dev_name, uint8_t demo_type);

#if (BLE_APP_PRF)
/**
 ****************************************************************************************
 * @brief Update data of the advertising screen
 ****************************************************************************************
 */
void app_display_set_adv(bool enable);

/**
 ****************************************************************************************
 * @brief Update data of the advertising screen
 ****************************************************************************************
 */
void app_display_set_con(bool state);

/**
 ****************************************************************************************
 * @brief Update data of the bond status screen
 ****************************************************************************************
 */
void app_display_set_bond(bool bonded);

#if (KE_PROFILING)
//void app_display_hdl_env_size(uint16_t heap_size, uint16_t used_size);
//void app_display_hdl_db_size(uint16_t heap_size, uint16_t used_size);
//void app_display_hdl_msg_size(uint16_t heap_size, uint16_t used_size);
#endif //(KE_PROFILING)

#if (BLE_APP_HT)
/**
 ****************************************************************************************
 * @brief Display the screen containing the PIN Code to use during a pairing process.
 ****************************************************************************************
 */
void app_display_pin_code(uint32_t pin_code);

/**
 ****************************************************************************************
 * @brief Update data of the temperature value screen
 ****************************************************************************************
 */
void app_display_update_temp_val_screen(uint32_t value);

/**
 ****************************************************************************************
 * @brief Update data of the temperature type screen
 ****************************************************************************************
 */
void app_display_update_temp_type_screen(const char* p_type_string);
#endif //(BLE_APP_HT)

#if (BLE_APP_HID)
/**
 ****************************************************************************************
 * @brief Update mouse data of the HID Mouse screen
 ****************************************************************************************
 */
void app_display_update_hid_value_screen(uint8_t b, uint8_t x,
                                         uint8_t y, uint8_t w);
#endif //(BLE_APP_HID)

#if (BLE_APP_AM0)
/**
 ****************************************************************************************
 * @brief Update Audio over LE info
 ****************************************************************************************
 */
void app_display_update_audio_info(uint8_t state, uint8_t volume);
#endif //(BLE_APP_AM0)
#endif //(BLE_APP_PRF)

#if (BLE_APP_MESH)
/**
 ****************************************************************************************
 * @brief Register set of callback function for communication with application mesh module.
 *
 * @param[in] p_mesh_cb        Pointer to set of callback
 ****************************************************************************************
 */
void app_display_register_cb_mesh(const app_display_mesh_cb_t* p_mesh_cb);

/**
 ****************************************************************************************
 * @brief Inform the display module about an update of the connection state
 *
 * @param[in] state        Current connection state
 ****************************************************************************************
 */
void app_display_mesh_set_con(bool state);

/**
 ****************************************************************************************
 * @brief Inform the display module about an update of the provisioning state
 *
 * @param[in] state        Current provisioning state
 ****************************************************************************************
 */
void app_display_mesh_set_prov(bool state);

/**
 ****************************************************************************************
 * @brief Inform the display module about current IV Index
 *
 * @param[in] iv           IV Index
 ****************************************************************************************
 */
void app_display_mesh_set_iv(uint32_t iv);

/**
 ****************************************************************************************
 * @brief Inform the display module about Unicast Address assigned by the provisioner
 *
 * @param[in] addr         Current unicast address
 ****************************************************************************************
 */
void app_display_mesh_set_unicast_addr(uint16_t addr);

/**
 ****************************************************************************************
 * @brief Inform the display module about new attention timer duration
 *
 * @param[in] duration_s      Attention timer duration in seconds
 ****************************************************************************************
 */
void app_display_mesh_set_attention(uint32_t att_s);

/**
 ****************************************************************************************
 * @brief Inform the display module about an update of the Generic OnOff state
 *
 * @param[in] trans_time_ms   Transition time in milliseconds
 * @param[in] state           Current Generic OnOff state
 ****************************************************************************************
 */
void app_display_mesh_set_onoff(uint32_t trans_time_ms, bool state);

/**
 ****************************************************************************************
 * @brief Inform the display module about Group Address used in the demonstration
 *
 * @param[in] group_addr           Current Group Address
 ****************************************************************************************
 */
void app_display_mesh_set_group_addr(uint16_t group_addr);

/**
 ****************************************************************************************
 * @brief Inform the display module about an update of the Light Lightness state value
 *
 * @param[in] trans_time_ms       Transition time in milliseconds
 * @param[in] lightness           Light Lightness state value
 ****************************************************************************************
 */
void app_display_mesh_set_lightness(uint32_t trans_time_ms, uint16_t lightness);

/**
 ****************************************************************************************
 * @brief Inform the display module about end of saving procedure
 ****************************************************************************************
 */
void app_display_mesh_save_end(void);

#endif //(BLE_APP_MESH)
#endif //(BLE_APP_PRESENT)

#endif // APP_DISPLAY_H_

/// @} app
