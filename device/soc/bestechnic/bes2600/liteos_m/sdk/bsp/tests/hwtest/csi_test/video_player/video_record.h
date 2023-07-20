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
#ifndef __VIDEO_RECORD_H__
#define __VIDEO_RECORD_H__

#ifdef __cpulsplus
extern "C" {
#endif

typedef struct {
	uint32_t addr;
	uint32_t len;
	uint32_t seq;
} video_record_msg_t;

int video_record_init(void);

int video_record_open(struct VP_STREAM_CONFIG_T *cfg);

int video_record_start(void);

int video_record_stop(void);

int video_record_close(void);

int video_record_ping(video_record_msg_t *msg);

#ifdef __cpulsplus
}
#endif

#endif
