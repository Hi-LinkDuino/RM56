/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __BT_COMMON_H__
#define __BT_COMMON_H__

/* This file should be included in every source code file in stack */

#include "bt_sys_cfg.h" // include cobt.h, cobt.h is independent header for including definations about bluetooth
#include "btlib_type.h" // global types
#include "platform_deps.h" // all platform dependence are in here
#include "debug_cfg.h"
#include "btlib.h" // some utility or helper macros, functions
#include "btlib_more.h" // some utility or helper macros, functions

#include "debug_print.h"
#include "packer.h"
#include "cobuf.h"
#include "btm_hci.h"
#include "hci.h"

#endif /* __BT_COMMON_H__ */