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
#include <stdlib.h>
#include <string.h>

#include "hal_trace.h"
#if defined(__NET_FTP_CLIENT_SUPPORT__)
#include "net_dump.h"
#endif
#ifdef AOS_VFS_H
#include "aos/vfs.h"
#endif

#define FTP_SINGLETON_TEST
//#define FTP_MULTIPLE_CASES_TEST

char * server_address = "192.168.0.104";
char * file_name = "/data/1.mp3";
#define USERNAME "bestest"
#define PASSWORD " "

#if defined(FTP_SINGLETON_TEST) && defined(__NET_FTP_CLIENT_SUPPORT__)
static void net_dump_singleton_test(void)
{
	//uint32_t dump_len;
	//char *dump_data;
	int size;
	int port = 21;
#ifdef AOS_VFS_H
	int file_data_size;
	int no_of_bytes;
	struct aos_stat ftp_buff;
#endif
	net_dump_handle_t net_dump_handle;
	net_dump_handle = net_dump_init();
    if(!net_dump_handle) {
        return;
    }
	if(net_dump_connect(net_dump_handle,server_address,port) < 0)
        return;
	net_dump_login(net_dump_handle,USERNAME,PASSWORD);

	if(net_dump_notice_server(net_dump_handle,file_name)==0){
#ifdef AOS_VFS_H
		char data[1024];// Data transfer
		memset(data,0,1024);
		int fd = aos_open(file_name,0);
		aos_fstat(fd,&ftp_buff);
		size = (int)ftp_buff.st_size;
		if(size>0){
			file_data_size = size;
		}
		printf("Uploading %d\r\n",size);
		while(size > 0)
		{
			no_of_bytes = aos_read(fd,data,1024);
			net_dump_data(net_dump_handle,data,no_of_bytes);
			size -= no_of_bytes;
		}
#else
		char data[1024];// Data transfer
		for(int i=0;i<1024;i++)
		{
			memset(data,i,1024);
			net_dump_data(net_dump_handle,data,1024);
		}

#endif
	}
	size = net_get_dump_size(net_dump_handle,file_name);
	TRACE(0, "dump file size %d\r\n",size);
	net_dump_disconnect(net_dump_handle);
	net_dump_free(net_dump_handle);
}
#endif
#if defined(FTP_MULTIPLE_CASES_TEST) && defined(__NET_FTP_CLIENT_SUPPORT__)
static int _my_net_dump_main1(void *argument)
{
    TRACE(0, "%s osThreadGetId=%d", __FUNCTION__, osThreadGetId());
    int _read_size = 0;
    unsigned int size = 1024;
    net_dump_handle_t net_dump_t;
    void *buf = malloc(size);
    if (!buf) {
        TRACE(0, "%s fail since malloc fail for %d\n", __FUNCTION__, size);
        return -1;
    }
    net_dump_t = net_dump_init();
    if(!net_dump_t){
        free(buf);
        return -1;
    }
    if(net_dump_connect(net_dump_t,"192.168.9.184", 21)<0){
        net_dump_free(net_dump_t);
        free(buf);
        return -1;
    }
    net_dump_login(net_dump_t,"bestest", " ");
    char _dump_path[128];
    sprintf(_dump_path, "/data/prompt/audio_record_%d.data", TICKS_TO_MS(hal_sys_timer_get()));
    if(net_dump_notice_server(net_dump_t,_dump_path) !=0 ){
        TRACE(0, "%s net_dump_notice_server fail\n", __FUNCTION__);
        goto exit;
    }
    TRACE(0, "%s dump to [%s]\n", __FUNCTION__, _dump_path);
	for(int i=0;i<100;i++)
	{
		//memset(buf,i,1024);
		net_dump_data(net_dump_t,buf,size);
	}
    TRACE(0, "%s dump to [%s], totol %d\n", __FUNCTION__, _dump_path, net_get_dump_size(net_dump_t,_dump_path));
exit:
    net_dump_disconnect(net_dump_t);
	net_dump_free(net_dump_t);
	free(buf);
    TRACE(0, "%s osThreadGetId=%d done", __FUNCTION__, osThreadGetId());
    //osThreadExitPub();
    //return -1;
}

static int _my_net_dump_main2(void *argument)
{
    TRACE(0, "%s osThreadGetId=%d", __FUNCTION__, osThreadGetId());
    int _read_size = 0;
    unsigned int size = 1024;
    net_dump_handle_t net_dump_t;
    void *buf = malloc(size);
    if (!buf) {
        TRACE(0, "%s fail since malloc fail for %d\n", __FUNCTION__, size);
        return -1;
    }
    net_dump_t = net_dump_init();
    if(!net_dump_t){
        free(buf);
        return -1;
    }
    if(net_dump_connect(net_dump_t,"192.168.9.184", 21)<0){
        free(buf);
        net_dump_free(net_dump_t);
        return -1;
    }
    net_dump_login(net_dump_t,NULL, " ");
    char _dump_path[128];
    sprintf(_dump_path, "/data/audio_record_%d.data", TICKS_TO_MS(hal_sys_timer_get()));
    if(net_dump_notice_server(net_dump_t,_dump_path) !=0 ){
        TRACE(0, "%s net_dump_notice_server fail\n", __FUNCTION__);
        goto exit;
    }
    TRACE(0, "%s dump to [%s]\n", __FUNCTION__, _dump_path);
	for(int i=0;i<100;i++)
	{
		//memset(buf,0,size);
		net_dump_data(net_dump_t,buf,size);
	}
    TRACE(0, "%s dump to [%s], totol %d\n", __FUNCTION__, _dump_path, net_get_dump_size(net_dump_t,_dump_path));
exit:
    net_dump_disconnect(net_dump_t);
	net_dump_free(net_dump_t);
	free(buf);
    TRACE(0, "%s osThreadGetId=%d done", __FUNCTION__, osThreadGetId());
    //osThreadExitPub();
    //return -1;
}

static void net_dump_multiple_cases_test(void)
{
	TRACE(0, "\n\n%-32s testing......\n", __FUNCTION__);
    osThreadId my_thread_id1;
    osThreadDef(_my_net_dump_main1, osPriorityAboveNormal, 1, (2*1024), "_my_net_dump_main1");
    osThreadId my_thread_id2;
    osThreadDef(_my_net_dump_main2, osPriorityAboveNormal, 1, (2*1024), "_my_net_dump_main2");

    my_thread_id1 = osThreadCreate(osThread(_my_net_dump_main1), NULL);
    if (my_thread_id1 == NULL){
        TRACE(0, "%s %d fail to create _my_audio_input_main1 task",__func__,__LINE__);
        return;
    }
    TRACE(0, "%s create _my_net_dump_main1 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id1);
    my_thread_id2 = osThreadCreate(osThread(_my_net_dump_main2), NULL);
    if (my_thread_id2 == NULL){
        TRACE(0, "%s %d fail to create _my_audio_input_main2 task",__func__,__LINE__);
        return;
    }
    TRACE(0, "%s create _my_net_dump_main2 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id2);

    osDelay(12000);

    TRACE(0, "%s osThreadTerminate %d", __FUNCTION__, my_thread_id1);
    osThreadTerminate(my_thread_id1);
    //osDelay(2000);
    TRACE(0, "%s osThreadTerminate %d", __FUNCTION__, my_thread_id2);
    osThreadTerminate(my_thread_id2);
}
#endif

int bes_ftp_client_dump_test(void)
{
    int ret = 1;
#if defined(FTP_SINGLETON_TEST) && defined(__NET_FTP_CLIENT_SUPPORT__)
    net_dump_singleton_test();
    ret = 0;
#elif defined(FTP_MULTIPLE_CASES_TEST) && defined(__NET_FTP_CLIENT_SUPPORT__)
    net_dump_multiple_cases_test();
    ret = 0;
#endif
    return ret;
}