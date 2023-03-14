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

#ifndef __USB_MSGQ_H__
#define __USB_MSGQ_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MSGQ_OK = 0,
    MSGQ_ERR_FULL,
    MSGQ_ERR_EMPTY,
    MSGQ_ERR_NOT_FOUND,
    MSGQ_ERR_NOT_INIT,
    MSGQ_ERR_OTHER_ERR
} MSGQ_ERROR;

typedef struct {
    void * msgs;        // array to store messages
    int pack_size;      // msg pack size
    int max_len;        // array length, that is to say how many msgs can be store
    int used_index;     // current filled array index
    int free_index;     // current free array index
    int current_index;  // current pointer location, maintained it just for msg_find
    int current_count;  // current msg count
} MSGQ;

void msgq_init(MSGQ * q, void * msgs, int pack_len, int msg_len);
void msgq_show(MSGQ *q);

void msgq_empty(MSGQ *q);
int msgq_add(MSGQ * q, void * msg);
int msgq_get(MSGQ * q, void * msg);

int msgq_register(MSGQ * q, void * msg);
void msgq_rewind(MSGQ *q);
void * msgq_find(MSGQ * q, int msg_id); // find the msg callback function according to the msg_id

#ifdef __cplusplus
}
#endif

#endif //__MSGQ_H__