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

#ifndef __SCO_I_H__
#define __SCO_I_H__

#include "btlib_type.h"

/* notify upper layer */
enum sco_event_enum{
    SCO_OPENED,
    SCO_CLOSED
};

#if defined(__cplusplus)
extern "C" {
#endif

int8 sco_open_link(struct bdaddr_t *bdaddr);

int8 sco_close_link(struct bdaddr_t *bdaddr1, uint8 reason);

void sco_conn_opened_ind(uint8_t device_id, struct bdaddr_t *bdaddr_remote, uint8 codec);

void sco_conn_closed_ind(uint8_t device_id, struct bdaddr_t *bdaddr_remote);

#if defined(__cplusplus)
}
#endif

#endif /* __SCO_I_H__ */

