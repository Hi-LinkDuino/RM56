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
#ifndef __IBRT_MGR_QUEUES_H__
#define __IBRT_MGR_QUEUES_H__

#define  IBRT_EVENT_Q_LENGTH                   (15)

typedef enum _ret
{
    RET_OK,
    RET_INVALID_PARAMS,
    RET_FAIL
}result_t;

typedef result_t (*data_visit_func)(void* ctx, void* data);

typedef struct {
    ibrt_mgr_evt_t event;
    bt_bdaddr_t    addr;
    uint8_t        mask;
    void*          param;
} ibrt_mgr_event_t;

typedef struct
{
    ibrt_mgr_event_t ibrt_event_que[IBRT_EVENT_Q_LENGTH];
    uint8_t          front;
    uint8_t          rear;
    uint16_t         size;
    uint16_t         capacity;
} ibrt_mgr_evt_queue_t;

result_t ibrt_mgr_queue_init(ibrt_mgr_evt_queue_t *queue);
uint8_t ibrt_mgr_queue_size(const ibrt_mgr_evt_queue_t *queue);
result_t ibrt_mgr_queue_push(ibrt_mgr_evt_queue_t *queue, const ibrt_mgr_event_t *event);
result_t ibrt_mgr_queue_pop(ibrt_mgr_evt_queue_t *queue);
result_t ibrt_mgr_queue_front(const ibrt_mgr_evt_queue_t *queue,ibrt_mgr_event_t *event);
result_t ibrt_mgr_queue_back(const ibrt_mgr_evt_queue_t *queue,ibrt_mgr_event_t *event);
result_t ibrt_mgr_queue_foreach(const ibrt_mgr_evt_queue_t *queue,data_visit_func visit, void* ctx);
bool ibrt_mgr_queue_find_data(const ibrt_mgr_evt_queue_t *queue,ibrt_mgr_event_t *event);
bool ibrt_mgr_queue_find_data_ext(const ibrt_mgr_evt_queue_t *queue,data_visit_func visit,void* ctx);
bool ibrt_mgr_queue_empty(const ibrt_mgr_evt_queue_t *queue);
bool ibrt_mgr_queue_full(const ibrt_mgr_evt_queue_t *queue);
uint8_t ibrt_mgr_queue_find_data_count(const ibrt_mgr_evt_queue_t *queue,ibrt_mgr_evt_t event);

int ibrt_mgr_queue_test();




#endif

