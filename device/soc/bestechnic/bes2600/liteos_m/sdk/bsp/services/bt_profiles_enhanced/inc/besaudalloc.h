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

#ifndef __BESAUDALLOC_H__
#define __BESAUDALLOC_H__

typedef struct _BtBesaudContext {
    struct list_node channelList;
    uint16 besaudcpsm;
} BtBesaudContext;

extern BtBesaudContext besaudContext;
#define BESAUD(s) (besaudContext.s)

int8 BesaudAlloc(void);

#endif /* __BESAUDALLOC_H__ */