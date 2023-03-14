/**
 * @file amsp_task.h
 * @author BES AI team
 * @version 0.1
 * @date 2020-10-24
 * 
 * @copyright Copyright (c) 2015-2020 BES Technic.
 * All rights reserved. All unpublished rights reserved.
 * 
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 * 
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 */

#ifndef __AMSP_TASK_H__
#define __AMSP_TASK_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/

/******************************macro defination*****************************/

/******************************type defination******************************/
/// Message IDs
enum
{
    /// AMS read confirm command
    AMSP_READ_CFM = TASK_FIRST_MSG(TASK_ID_AMSP),

    /// AMS write confirm command
    AMSP_WRITE_CFM,

    /// AMS send notification/indication command
    AMSP_IND_EVT,
};

/****************************function declearation**************************/
/**
 * @brief Initialize the profile task
 * 
 * @param task_desc     AMS proxy task descriptor
 * @param p_env         pointer of environment variable
 */
void amsp_task_init(struct ke_task_desc *task_desc, void *p_env);

void amsp_set_ready_flag(uint8_t conidx, 
                         int handle_rem_cmd_char, int handle_rem_cmd_val,int handle_rem_cmd_cfg,
                         int handle_ent_upd_char, int handle_ent_upd_val, int handle_ent_upd_cfg,
                         int handle_ent_att_char, int handle_ent_att_val);

const gatt_srv_cb_t* amsp_task_get_srv_cbs(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __AMSP_TASK_H__ */