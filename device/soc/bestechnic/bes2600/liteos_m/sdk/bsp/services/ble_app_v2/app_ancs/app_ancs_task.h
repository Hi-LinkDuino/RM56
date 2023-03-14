/**
 * @file app_ancs_task.h
 * @author BES AI team
 * @version 0.1
 * @date 2020-12-31
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

#ifndef __APP_ANCS_TASK_H__
#define __APP_ANCS_TASK_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/
#include "rwapp_config.h"

#ifdef ANCS_ENABLED
#include "rwip_task.h"

/******************************macro defination*****************************/

/******************************type defination******************************/
/// Table of message handlers
extern const struct app_subtask_handlers app_ancs_table_handler;

/****************************function declearation**************************/

#endif /// #ifdef ANCS_ENABLED

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __APP_ANCS_TASK_H__ */