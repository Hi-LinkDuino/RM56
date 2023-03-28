/**
 ****************************************************************************************
 *
 * @file bass_msg.h
 *
 * @brief Header file - Battery Service Server Role - Native API.
 *
 * Copyright (C) RivieraWaves 2009-2019
 *
 ****************************************************************************************
 */


#ifndef _BASS_H_
#define _BASS_H_

/**
 ****************************************************************************************
 * @addtogroup BASS
 * @ingroup Profile
 * @brief  Battery Service Server Role - Native API.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "bass_msg.h"


/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Battery Service server callback set
typedef struct bass_cb
{
    /**
     ****************************************************************************************
     * @brief Completion of battery level update
     *
     * @param[in] status Status of the procedure execution (see enum #hl_err)
     ****************************************************************************************
     */
    void (*cb_batt_level_upd_cmp)(uint16_t status);

    /**
     ****************************************************************************************
     * @brief Inform that Bond data updated for the connection.
     *
     * @param[in] conidx        Connection index
     * @param[in] ntf_cfg       Notification Configuration
     ****************************************************************************************
     */
    void (*cb_bond_data_upd)(uint8_t conidx, uint8_t ntf_ind_cfg);
} bass_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Restore bond data of a known peer device (at connection establishment)
 *
 * @param[in] conidx          Connection index
 * @param[in] ntf_cfg         Notification Configuration
 * @param[in] p_old_batt_lvl  Old Battery Level used to decide if notification should be triggered
 *                            Array of BASS_NB_BAS_INSTANCES_MAX size.
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t bass_enable(uint8_t conidx, uint8_t ntf_cfg, const uint8_t* p_old_batt_lvl);

/**
 ****************************************************************************************
 * @brief Update a battery level
 *
 * Wait for #cb_batt_level_upd_cmp execution before starting a new procedure
 *
 * @param[in] bas_instance  Battery service instance
 * @param[in] batt_level    New Battery level
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t bass_batt_level_upd(uint8_t bas_instance, uint8_t batt_level);

/// @} BASS

#endif /* _BASS_H_ */
