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

#include "hal_trace.h"
#include "cmsis_os.h"
#include "bes_kv.h"
#include "string.h"

static bes_global_stat_t bes_stat;

int bes_kv_init(void)
{
    memset((char *)bes_stat.bes_key, 0, sizeof(bes_kv_t)*MAX_KEY_NUMBER);
 
    return 0;
}

static int find_unused_alias(void)
{
    int i = 0;
 
    for( i=0; i < MAX_KEY_NUMBER; i++ ) {
        if( bes_stat.bes_key[i].alias == 0 )
            break;
    }
 
    return i;
}

int bes_kv_add(char *key)
{
    int index;

    u8 key_len = strlen(key);
    index = find_unused_alias();
    if ( index < MAX_KEY_NUMBER && key_len < MAX_NEME_LEN ) {
        memcpy(bes_stat.bes_key[index].key_name, key, key_len);
        bes_stat.bes_key[index].key_name[key_len] = '\0';
        bes_stat.bes_key[index].alias = (ALIAS_BASE + index);
    } else
        index = -1;

    return index;
}

int bes_kv_item_delete(int index)
{
    memset((char *)&(bes_stat.bes_key[index]), 0, sizeof(bes_kv_t));
    return 0;
}

int bes_kv_item_set(int index, u32 val)
{
    int ret = 0;
    if ( index < MAX_KEY_NUMBER ) {
        bes_stat.bes_key[index].real_value += val;
        bes_stat.bes_key[index].accu_value += val;
        ret = bes_stat.bes_key[index].accu_value;
    }

    return ret;
}

static int dump_global_kv(void)
{
    int i = 0;
    uint32_t rt_val, ac_val;

    printf("--------------------------interval_sec=%d----------------------------\r\n", bes_stat.interval_sec);

    for(i = 0; i < MAX_KEY_NUMBER; i++) {
        if( bes_stat.bes_key[i].alias != 0 ) {
            rt_val = bes_stat.bes_key[i].real_value;
            ac_val = bes_stat.bes_key[i].accu_value;
            printf("key%-2d:%-32s rt_val:%9d ac_val:%9d\r\n", i, bes_stat.bes_key[i].key_name, rt_val, ac_val);
            bes_stat.bes_key[i].real_value = 0;
        }
    }
    printf("--------------------------------------------------------------------\r\n");

    return 0;
}

static void global_stat_task(const void *arg) 
{
    arg = arg;

    while (1) {
        if ( bes_stat.en == 1 ) {
            dump_global_kv();
            osDelay(bes_stat.interval_sec * 1000);
        } else {
            break;
        }
    }
    osThreadExit();
    bes_stat.thread_id = NULL;
}
osThreadDef(global_stat_task, osPriorityNormal, 1, (2*1024), "global_stat_task");

int bes_kv_item_get(u8 en, u32 interval_sec)
{
    bes_stat.en = en;
    bes_stat.interval_sec = interval_sec;

    if ( bes_stat.en == 1 && bes_stat.thread_id == NULL ) {
        // clear when start
        for(int i = 0; i < MAX_KEY_NUMBER; i++) {
            if( bes_stat.bes_key[i].alias != 0 ) {
                bes_stat.bes_key[i].real_value = 0;
                bes_stat.bes_key[i].accu_value = 0;
            }
        }

        bes_stat.thread_id = osThreadCreate(osThread(global_stat_task), NULL);
        ASSERT(bes_stat.thread_id != NULL, "Failed to create bes_stat Thread");
    }

    return 0;
}
