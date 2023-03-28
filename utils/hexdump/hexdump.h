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
#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

void dump_buffer(const void *buf, size_t len);

void dump_buffer_imm(const void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif

