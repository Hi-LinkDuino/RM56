/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "hal_trace.h"

/**
 * test case of pcm play by alsa.
 * **/
#include "alsa.h"

#ifdef __ALSA_EN__

/* 16k 2ch 16bit */
uint32_t test_pcm_1[] = {
#include "super_marie_stereo_pcm"
};

uint32_t test_pcm_2[] = {
#include "super_marie_stereo_pcm"
};

#define ALSA_SINGLE_INSTANCE_TEST
#define ALSA_MULTI_INSTANCE_TEST

#ifdef ALSA_SINGLE_INSTANCE_TEST
static int alsa_single_instance_test(void)
{
	alsa_handle_t *h = NULL;

	h = alsa_open(ALSA_MODE_OUT, 16000, 2, ALSA_PCM_FORMAT_S16_LE);
	if (h)
	{
		alsa_start(h);
	}
	else
	{
		return -1;
	}
	alsa_volume_set(17);
	alsa_write(h, (uint8_t *)test_pcm_1, sizeof(test_pcm_1));
	alsa_stop(h);
	alsa_close(h);

	h = alsa_open(ALSA_MODE_OUT, 16000, 2, ALSA_PCM_FORMAT_S16_LE);
	if (h)
	{
		alsa_start(h);
	}
	else
	{
		return -1;
	}
	alsa_volume_set(17);
	alsa_write(h, (uint8_t *)test_pcm_2, sizeof(test_pcm_2));
	alsa_stop(h);
	alsa_close(h);

	return 0;
}
#endif

#ifdef ALSA_MULTI_INSTANCE_TEST
#include "cmsis_os.h"
typedef struct
{
	alsa_handle_t *h;
	uint8_t *ptr;
	uint32_t length;
} alsa_multi_instance_test_t;

static alsa_multi_instance_test_t test_arg_1;
static alsa_multi_instance_test_t test_arg_2;

static int alsa_multi_instance_test_finish_flag = 0;

static void alsa_multi_instance_test_task(const void *argument)
{
	alsa_multi_instance_test_t *test_arg = (alsa_multi_instance_test_t *)argument;
	uint32_t remain_len = test_arg->length;
	uint8_t *ptr = test_arg->ptr;
	alsa_handle_t *h = test_arg->h;
	uint32_t write_len;

	while (remain_len)
	{
		write_len = remain_len > 4096 ? 4096 : remain_len;
		alsa_write(h, ptr, write_len);
		ptr += write_len;
		remain_len -= write_len;
	}
	alsa_stop(h);
	alsa_close(h);
	alsa_multi_instance_test_finish_flag++;
	osThreadExit();
}
osThreadDef(alsa_multi_instance_test_task, osPriorityNormal, 2, 8 * 1024, "alsa_test");

static int alsa_multi_instance_test()
{
	alsa_handle_t *h1 = alsa_open(ALSA_MODE_OUT, 16000, 2, ALSA_PCM_FORMAT_S16_LE);
	if (h1)
	{
		alsa_start(h1);
	}
	else
	{
		return -1;
	}

	alsa_handle_t *h2 = alsa_open(ALSA_MODE_OUT, 22050, 2, ALSA_PCM_FORMAT_S16_LE);
	if (h2)
	{
		alsa_start(h2);
	}
	else
	{
		return -1;
	}
	alsa_volume_set(17);
	memset(&test_arg_1, 0, sizeof(test_arg_1));
	test_arg_1.h = h1;
	test_arg_1.ptr = (uint8_t *)test_pcm_1;
	test_arg_1.length = sizeof(test_pcm_1);
	osThreadCreate(osThread(alsa_multi_instance_test_task), &test_arg_1);

	test_arg_2.h = h2;
	test_arg_2.ptr = (uint8_t *)test_pcm_2;
	test_arg_2.length = sizeof(test_pcm_2);
	osThreadCreate(osThread(alsa_multi_instance_test_task), &test_arg_2);

	while (1)
	{
		if (alsa_multi_instance_test_finish_flag == 2)
		{
			alsa_multi_instance_test_finish_flag = 0;
			break;
		}
		else
		{
			osDelay(1000);
		}
	}

	return 0;
}
#endif

#endif // __ALSA_EN__

int bes_pcm_test(void)
{
#ifdef ALSA_SINGLE_INSTANCE_TEST
	alsa_single_instance_test();
#endif

#ifdef ALSA_MULTI_INSTANCE_TEST
	alsa_multi_instance_test();
#endif
	return 0;
}



/**
 * test case of lplayer.
 * lplayer current support pcm, wav, mp3, mp4, m3u8.
 * **/
#ifdef LPLAYER_FRAMEWORK_EN
#ifdef BT_MEDIA_SUPPORT_LPLAYER
#include "aud_player.h"
#else
#include "lplayer.h"
#endif
//#include "los_memory.h"
#include "cmsis_os.h"
//#include "audio_os.h"
#include <stdlib.h>

#define LPLAYER_TEST_SINGLE_INSTANCE
#ifndef BT_MEDIA_SUPPORT_LPLAYER
#define LPLAYER_TEST_MULTI_INSTANCE
#endif

#define LPLAYER_TEST_ONCE
// #define LPLAYER_TEST_MEMORY

#ifdef LPLAYER_TEST_SINGLE_INSTANCE

#ifndef BT_MEDIA_SUPPORT_LPLAYER
static lplayer_t *lplayer = NULL;
#endif

// const char *test_url = "http://192.168.137.1/h1.mp3";

/**
 * if test_url == NULL, lplayer internal will use a test url.
 * **/
const char *test_url = "http://music.163.com/song/media/outer/url?id=5264842.mp3";

osSemaphoreDef(lplayer_complete_user_semaphore);
static osSemaphoreId lplayer_terminate_user_semaphore_id = NULL;

#ifdef BT_MEDIA_SUPPORT_LPLAYER

static void audplayer_on_complete_notify(int error_code)
{
	TRACE(0, "%s error_code = %d", __func__, error_code);
	osSemaphoreRelease(lplayer_terminate_user_semaphore_id);
}
#else
static int lplayer_on_complete_notify(void)
{
	TRACE(0, "%s", __func__);
	osSemaphoreRelease(lplayer_terminate_user_semaphore_id);
	return 0;
}

static int lplayer_on_error_notify(int error_code)
{
	TRACE(0, "%s error_code = %d", __func__, error_code);
	osSemaphoreRelease(lplayer_terminate_user_semaphore_id);
	return 0;
}

#endif

static int bes_lplayer_test_single_instance(void)
{
	if (lplayer_terminate_user_semaphore_id == NULL)
	{
		lplayer_terminate_user_semaphore_id = osSemaphoreCreate((osSemaphore(lplayer_complete_user_semaphore)), 0);
	}

#ifdef BT_MEDIA_SUPPORT_LPLAYER
	int ret = aud_player_start(test_url, audplayer_on_complete_notify);
	if(ret < 0)goto exit;
	osDelay(1000 * 10);
	aud_player_seek(180);
	
	osSemaphoreWait(lplayer_terminate_user_semaphore_id, osWaitForever);

	aud_player_stop();

exit:
	if (lplayer_terminate_user_semaphore_id){
		osSemaphoreDelete(lplayer_terminate_user_semaphore_id);
		lplayer_terminate_user_semaphore_id = NULL;
	}
	return ret;

#else
	if (lplayer == NULL)
	{
		lplayer = lplayer_open_url(test_url);
		if (lplayer == NULL)
			goto error_exit;
	}

	if (0 != lplayer_register_complete_notify(lplayer, lplayer_on_complete_notify))
		goto error_exit;

	if (0 != lplayer_register_error_notify(lplayer, lplayer_on_error_notify))
		goto error_exit;

	if (0 != lplayer_play(lplayer))
		goto error_exit;
	
	if (0 != lplayer_set_vol(lplayer, 100))
		goto error_exit;

	osDelay(1000 * 10);
	lplayer_seek(lplayer, 180);
	
	osSemaphoreWait(lplayer_terminate_user_semaphore_id, osWaitForever);

	lplayer_close(lplayer);

	lplayer = NULL;


	return 0;

error_exit:

	if (lplayer_terminate_user_semaphore_id)
	{
		osSemaphoreDelete(lplayer_terminate_user_semaphore_id);
		lplayer_terminate_user_semaphore_id = NULL;
	}

	if (lplayer)
	{
		lplayer_close(lplayer);
		lplayer = NULL;
	}
#endif
	return -1;
}
#endif // LPLAYER_TEST_SINGLE_INSTANCE

#ifdef LPLAYER_TEST_MULTI_INSTANCE
#ifdef BT_MEDIA_SUPPORT_LPLAYER
#error "audio player doesn't support multi instance
#endif
typedef struct
{
	lplayer_t *lplayer;
	osSemaphoreId sem;
} lplayer_user_t;

static lplayer_user_t *lplayer_user_open(const char *url)
{
	lplayer_user_t *lp_user = NULL;
	lplayer_t *lplayer = NULL;
	osSemaphoreId *sem = NULL;

	lp_user = (lplayer_user_t *)calloc(1, sizeof(lplayer_user_t));
	if (!lp_user)
	{
		TRACE(0, "lp_user alloc error");
		goto error_exit;
	}

	lplayer = lplayer_open_url(url);
	if (!lplayer)
	{
		TRACE(0, "lplayer open error");
		goto error_exit;
	}
	lp_user->lplayer = lplayer;

	sem = osSemaphoreCreate((osSemaphore(lplayer_complete_user_semaphore)), 0);
	if (!sem)
	{
		TRACE(0, "sem create error");
		goto error_exit;
	}
	lp_user->sem = sem;

	return lp_user;

error_exit:

	if (lplayer)
	{
		lplayer_close(lplayer);
	}

	if (sem)
	{
		osSemaphoreDelete(sem);
	}

	if (lp_user)
	{
		free(lp_user);
	}

	return NULL;
}

static int lplayer_user_close(lplayer_user_t *lp_user)
{
	if (lp_user)
	{
		if (lp_user->lplayer)
			lplayer_close(lp_user->lplayer);

		if (lp_user->sem)
			osSemaphoreDelete(lp_user->sem);

		free(lp_user);
	}
	return 0;
}

static lplayer_user_t *lplayer_user_1 = NULL;
static lplayer_user_t *lplayer_user_2 = NULL;

static int lplayer_user_1_on_complete_notify(void)
{
	TRACE(0, "%s", __func__);
	if (lplayer_user_1)
	{
		osSemaphoreRelease(lplayer_user_1->sem);
	}
	return 0;
}

static int lplayer_user_2_on_complete_notify(void)
{
	TRACE(0, "%s", __func__);
	if (lplayer_user_2)
	{
		osSemaphoreRelease(lplayer_user_2->sem);
	}
	return 0;
}

static int lplayer_user_1_on_error_notify(int error_code)
{
	TRACE(0, "%s error_code = %d", __func__, error_code);
	if (lplayer_user_1)
	{
		osSemaphoreRelease(lplayer_user_1->sem);
	}
	return 0;
}

static int lplayer_user_2_on_error_notify(int error_code)
{
	TRACE(0, "%s error_code = %d", __func__, error_code);
	if (lplayer_user_2)
	{
		osSemaphoreRelease(lplayer_user_2->sem);
	}
	return 0;
}

const char *user_1_url = "http://music.163.com/song/media/outer/url?id=490453044.mp3";
const char *user_2_url = "http://music.163.com/song/media/outer/url?id=5264842.mp3";

static int bes_lplayer_test_multi_instance(void)
{
	if (lplayer_user_1 == NULL)
	{
		lplayer_user_1 = lplayer_user_open(user_1_url);
		if (lplayer_user_1)
		{
			if (0 != lplayer_register_complete_notify(lplayer_user_1->lplayer, lplayer_user_1_on_complete_notify))
				TRACE(0, "%s %d error", __func__, __LINE__);

			if (0 != lplayer_register_error_notify(lplayer_user_1->lplayer, lplayer_user_1_on_error_notify))
				TRACE(0, "%s %d error", __func__, __LINE__);
		}
		else
		{
			TRACE(0, "lplayer_user_1 open error");
		}
	}

	if (lplayer_user_2 == NULL)
	{
		lplayer_user_2 = lplayer_user_open(user_2_url);
		if (lplayer_user_2)
		{
			if (0 != lplayer_register_complete_notify(lplayer_user_2->lplayer, lplayer_user_2_on_complete_notify))
				TRACE(0, "%s %d error", __func__, __LINE__);

			if (0 != lplayer_register_error_notify(lplayer_user_2->lplayer, lplayer_user_2_on_error_notify))
				TRACE(0, "%s %d error", __func__, __LINE__);
		}
		else
		{
			TRACE(0, "lplayer_user_2 open error");
		}
	}

	uint8_t test_count = 0;

#if 1
	while (1)
	{
		if (lplayer_user_1)
		{
			if (0 != lplayer_play(lplayer_user_1->lplayer))
				TRACE(0, "%s %d error", __func__, __LINE__);
		}
		if (0 != lplayer_set_vol(lplayer_user_1->lplayer, 100))
			return;

		osDelay(1000 * 10);

		if (lplayer_user_1)
			lplayer_pause(lplayer_user_1->lplayer);

		if (lplayer_user_2)
		{
			if (0 != lplayer_play(lplayer_user_2->lplayer))
				TRACE(0, "%s %d error", __func__, __LINE__);
		}
		if (0 != lplayer_set_vol(lplayer_user_2->lplayer, 100))
			return;

		osDelay(1000 * 10);

		if (lplayer_user_2)
			lplayer_pause(lplayer_user_2->lplayer);

		test_count++;

		if (test_count == 5)
		{
			break;
		}
	}
#else
	if (lplayer_user_1)
	{
		if (0 != lplayer_play(lplayer_user_1->lplayer))
			TRACE(0, "%s %d error", __func__, __LINE__);
	}

	if (lplayer_user_2)
	{
		if (0 != lplayer_play(lplayer_user_2->lplayer))
			TRACE(0, "%s %d error", __func__, __LINE__);
	}

	osDelay(1000 * 60 * 2);

	lplayer_pause(lplayer_user_1->lplayer);
	lplayer_pause(lplayer_user_2->lplayer);
#endif

	if (lplayer_user_1)
	{
		lplayer_user_close(lplayer_user_1);
		lplayer_user_1 = NULL;
	}

	if (lplayer_user_2)
	{
		lplayer_user_close(lplayer_user_2);
		lplayer_user_2 = NULL;
	}

	return 0;
}
#endif // LPLAYER_TEST_MULTI_INSTANCE

static int bes_lplayer_test_internal(void)
{
#ifdef LPLAYER_TEST_SINGLE_INSTANCE
	 bes_lplayer_test_single_instance();
#endif
	
#ifdef LPLAYER_TEST_MULTI_INSTANCE
	TRACE(0, "bes_lplayer_test_multi_instance() start\r\n");
	return bes_lplayer_test_multi_instance();
#endif

	return 0;
}
#if 0 
// todo based on freertos
static void show_os_heap_info(void)
{
	LOS_MEM_POOL_STATUS mem_pool_status;

	memset(&mem_pool_status, 0, sizeof(LOS_MEM_POOL_STATUS));

	LOS_MemInfoGet(OS_SYS_MEM_ADDR, &mem_pool_status);

	// TRACE(0, "freeNodeNum = %d, maxFreeNodeSize = %d, totalFreeSize = %d, totalUsedSize = %d, usageWaterLine = %d, usedNodeNum = %d",
	//       mem_pool_status.freeNodeNum,
	//       mem_pool_status.maxFreeNodeSize,
	//       mem_pool_status.totalFreeSize,
	//       mem_pool_status.totalUsedSize,
	//       mem_pool_status.usageWaterLine,
	//       mem_pool_status.usedNodeNum);

	static uint32_t last_pool_total_used_size = 0;
	uint32_t pool_total_used_size = mem_pool_status.totalUsedSize;
	int diff = pool_total_used_size - last_pool_total_used_size;
	last_pool_total_used_size = pool_total_used_size;

	TRACE(0, "====================================================");
	TRACE(0, "freeNodeNum     = %d", mem_pool_status.freeNodeNum);
	TRACE(0, "maxFreeNodeSize = %d", mem_pool_status.maxFreeNodeSize);
	TRACE(0, "totalFreeSize   = %d", mem_pool_status.totalFreeSize);
	TRACE(0, "totalUsedSize   = %d", mem_pool_status.totalUsedSize);
	TRACE(0, "usage_mem       = %d", diff);
	TRACE(0, "usageWaterLine  = %d", mem_pool_status.usageWaterLine);
	TRACE(0, "usedNodeNum     = %d", mem_pool_status.usedNodeNum);
	TRACE(0, "====================================================");

	// LOS_MemFreeNodeShow(OS_SYS_MEM_ADDR);

	// uint32_t pool_total_used_size = LOS_MemTotalUsedGet(OS_SYS_MEM_ADDR);
	// int diff = pool_total_used_size - last_pool_total_used_size;
	// TRACE(0, "total_used %d, last_total_used %d, diff ====> %d", pool_total_used_size, last_pool_total_used_size, diff);
	// last_pool_total_used_size = pool_total_used_size;
}
#endif

#endif // LPLAYER_FRAMEWORK_EN

int bes_lplayer_test(void)
{
#ifdef LPLAYER_FRAMEWORK_EN
#ifdef LPLAYER_TEST_ONCE
	return bes_lplayer_test_internal();
#endif // LPLAYER_TEST_ONCE
#ifdef LPLAYER_TEST_MEMORY
	while (1)
	{
		static uint32_t lplayer_test_count = 0;
		uint32_t pool_total_used_size_1 = LOS_MemTotalUsedGet(OS_SYS_MEM_ADDR);
		bes_lplayer_test_internal();
		osDelay(1000);
		show_os_heap_info();
		uint32_t pool_total_used_size_2 = LOS_MemTotalUsedGet(OS_SYS_MEM_ADDR);
		int mem_usage = pool_total_used_size_2 - pool_total_used_size_1;
		TRACE(0, "%s count %d, mem_usage = %d", __func__, ++lplayer_test_count, mem_usage);
	}
#endif // LPLAYER_TEST_MEMORY
#else
	TRACE(0, "%s need define LPLAYER_FRAMEWORK_EN", __func__);
	return -1;
#endif
}
