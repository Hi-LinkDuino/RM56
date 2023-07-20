/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef _HTTP_H_
#define _HTTP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HTTP_STATUS_IDLE = 0,
    HTTP_STATUS_PLAY,
    HTTP_STATUS_PAUSE,
    HTTP_STATUS_SEEK,
    HTTP_STATUS_END,
    HTTP_STATUS_STOP
} HTTP_STATUS_T;

typedef struct {
    void (*onerror)(int http_error, int local_error, void* param, int support_retry);
    void* onerror_param;
    int buffer_size;
    int priority;
    char cert_file[1024];
} http_attr_t;


void* http_open(char* url, http_attr_t* attr);
void http_close(void* handle);
int http_read(void* handle, unsigned char *buff, unsigned int want_read_length, unsigned int* real_read_length, int* flag);
int http_peek(void* handle, unsigned char *buff, unsigned int want_read_length, unsigned int* real_read_length);
int http_seek(void* handle, int offset);
int http_pause(void* handle);
int http_get_content_length(void* handle, long* ret);
int http_restart(void* handle);

HTTP_STATUS_T http_get_status(void* handle);
void http_set_status(void* handle, HTTP_STATUS_T status);
uint32_t http_get_last_read_data_time(void* handle); // 外层上次调用http_read接口的时间戳

// 获取buffer中的数据属于media文件的哪个区间，read参数只有再http_read操作后才发生变化
int http_get_buffer_read_write_position(void* handle, uint32_t* read, uint32_t* write); 
// 获取当前播放节目是否是广播
int http_is_streaming(void* handle);
// 设置buffer的高水线参数，参数单位为百分比
int http_set_waterline(void* handle, int high);

// 获取当前播放节目时长
int http_get_duration(void *handle, uint32_t *duration_ms);

// 获取缓冲区数据的大小
int http_get_cache_size(void* handle, unsigned int* size);
// 获取最近10s内的平均下载速度 单位:byte/s.
int http_get_download_speed(void* handle, unsigned int* speed);

int http_is_eof(void* handle, unsigned int* eof);

#ifdef HTTP_UNIT_TEST
void http_unit_test(char* url);
#endif

#ifdef __cplusplus
}
#endif


#endif /* _HTTP_H_ */
