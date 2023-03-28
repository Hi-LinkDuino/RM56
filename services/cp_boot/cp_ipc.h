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

#ifndef __CP_IPC_H__
#define __CP_IPC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CP_IPC_MSG_TRACE = 0,
#ifdef WIFI_ON_CP
    CP_IPC_MSG_NET_MGMT,
#else
    CP_IPC_MSG_NET_MGMT_AGENT,
    CP_IPC_MSG_NET_MGMT_SRV,
#endif
    CP_IPC_EVENT,
    CP_IPC_MSG_TEST,
    IPC_MSG_TYPE_NUM
}CP_IPC_MSG_TYPE;

typedef struct{
    uint16_t id;
    uint16_t len;
    void * data;
}CP_IPC_MSG_HDR;

typedef void (*CP_IPC_MSG_CB)(void *param);

int cp_ipc_init();
int cp_ipc_start();
int cp_ipc_send(CP_IPC_MSG_HDR *msg);
int cp_ipc_send_self(CP_IPC_MSG_HDR *msg);
int cp_ipc_cb_set(CP_IPC_MSG_TYPE type, CP_IPC_MSG_CB func, bool tx);

#ifdef __cplusplus
}
#endif
#endif
