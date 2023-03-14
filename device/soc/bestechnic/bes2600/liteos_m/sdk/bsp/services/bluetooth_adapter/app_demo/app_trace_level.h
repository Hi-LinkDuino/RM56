/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
 *
 ****************************************************************************/
#ifndef __APP_TRACE_LEVEL_H__
#define __APP_TRACE_LEVEL_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/
#include "hal_trace.h"

#define MODULE_TRACE_LEVEL  TR_LEVEL_DEBUG

/******************************macro defination*****************************/
#define APP_LOG_V(str, ...) if (MODULE_TRACE_LEVEL >= TR_LEVEL_VERBOSE) TR_VERBOSE(TR_MOD(APP_DM), str, ##__VA_ARGS__)
#define APP_LOG_D(str, ...) if (MODULE_TRACE_LEVEL >= TR_LEVEL_DEBUG) TR_DEBUG(TR_MOD(APP_DM), str, ##__VA_ARGS__)
#define APP_LOG_I(str, ...) if (MODULE_TRACE_LEVEL >= TR_LEVEL_INFO) TR_INFO(TR_MOD(APP_DM), str, ##__VA_ARGS__)
#define APP_LOG_W(str, ...) if (MODULE_TRACE_LEVEL >= TR_LEVEL_WARN) TR_WARN(TR_MOD(APP_DM), str, ##__VA_ARGS__)
#define APP_LOG_E(str, ...) if (MODULE_TRACE_LEVEL >= TR_LEVEL_ERROR) TR_ERROR(TR_MOD(APP_DM), str, ##__VA_ARGS__)


/******************************type defination******************************/

/****************************function declearation**************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __APP_TRACE_LEVEL_H__ */

