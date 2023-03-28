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


#ifndef PING_H
#define PING_H

#include <stdint.h>
#include <netdb.h>

#ifdef __cplusplus
extern "C" {
#endif

void ping_init(const in_addr_t *ping_dst, uint32_t count, uint32_t delay_time, uint32_t len, uint32_t echo_level);

#ifdef __cplusplus
}
#endif

#endif /* PING_H */
