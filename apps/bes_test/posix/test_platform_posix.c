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
#include "hal_trace.h"
#ifdef KERNEL_FREERTOS
#include "aws_test_runner.h"
#endif
#include "cmsis_os.h"
#include "hal_timer.h"
#include <stdio.h>
#include <string.h>
#define FLAGS_MSK1 0x00000001U
#define OS_THREAD_DETACH_TEST_OPEN 0
#define OS_THREAD_ENUMERATE_TEST_OPEN 0
#define OS_THREAD_THREADFLAGS_TEST_OPEN 0
#define MSG_TEST_RESET 0
#define OS_THREAD_JOIN_TEST 0
#define OS_MESSAGE_QUEUE_GET_NAME_TEST 0
void bes_posix_test(void)
{
#ifdef KERNEL_FREERTOS
    TRACE( 0 , 0, "%s start-------------------", __FUNCTION__);
    TEST_RUNNER_RunTests_task(NULL);
    TRACE( 0 , 0, "%s done--------------------", __FUNCTION__);
#endif
}
#define _MY_MSG_NUM 5
typedef struct
{
	u8 test1;
	u16 test2;
	u32 test3;
}my_msg_t;
static osPoolId _my_pool_id = 0;
static void _my_msg_main(const void *argument)
{
    osMessageQId _my_msg_id = *(osMessageQId *)argument;
    my_msg_t *msg = NULL;
    for (uint32_t i=0; i<_MY_MSG_NUM; i++)
    {
        if (osMessageGetSpace(_my_msg_id) != i*sizeof(my_msg_t *)) // for rhino
        // if (osMessageGetSpace(_my_msg_id) != i)
        {
            TRACE( 0 , "%s %d fail to osMessageGetSpace i=%d, Space=%d",__func__,__LINE__, i, \
                    osMessageGetSpace(_my_msg_id));
            break;
        }
        osEvent event = osMessageGet(_my_msg_id, 0);
        if (event.status != osEventMessage)
        {
            TRACE( 0 , "%s %d fail to osMessageGetSpace i=%d",__func__,__LINE__, i);
            break;
        }
        msg = (my_msg_t *)event.value.p;
        if (msg->test1 != i)
        {
            TRACE( 0 , "%s %d fail to osMessageGet i=%d, test1=%d, test2=%d, test3=%d",__func__,\
                    __LINE__, i, msg->test1, msg->test2, msg->test3);
            break;
        }
        TRACE( 0 , "%s %d osMessageGet i=%d",__func__,__LINE__, i);
        osPoolFree(_my_pool_id, msg);
    }
    osEvent event = osMessageGet(_my_msg_id, 0);
    if (event.status == osEventMessage)
    {
        TRACE( 0 , "%s %d fail to osMessageGet i=%d",__func__,__LINE__, _MY_MSG_NUM);
    }

    TRACE( 0 , "%s %d osMessageGet wait 1000ms",__func__,__LINE__);
    event = osMessageGet(_my_msg_id, 1000);
    TRACE( 0 , "%s %d osMessageGet wait 2000ms",__func__,__LINE__);
    event = osMessageGet(_my_msg_id, 2000);
    TRACE( 0 , "%s %d osMessageGet wait forever",__func__,__LINE__);
    event = osMessageGet(_my_msg_id, osWaitForever);
    TRACE( 0 , "%s %d never be here",__func__,__LINE__);

    osThreadExit();
}
static int bes_msg_test(void)
{
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    my_msg_t *msg = NULL;
    osPoolDef(_my_pool, _MY_MSG_NUM, my_msg_t);
    _my_pool_id = osPoolCreate(osPool(_my_pool));
    if (_my_pool_id == NULL)
    {
        TRACE( 0 , "%s %d fail to osPoolCreate",__func__,__LINE__);
        return -1;
    }
    osMessageQId _my_msg_id = 0;
    osMessageQDef(_my_msg, _MY_MSG_NUM, my_msg_t*);
    _my_msg_id = osMessageCreate(osMessageQ(_my_msg), NULL);
    if (_my_msg_id == NULL)
    {
        TRACE( 0 , "%s %d fail to osMessageCreate",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osMessageCreate id=%d, Space=%d, sizeof(my_msg_t *)=%d", \
            __func__,__LINE__, _my_msg_id, osMessageGetSpace(_my_msg_id), sizeof(my_msg_t *));

    for (uint32_t i=0; i<_MY_MSG_NUM; i++)
    {
        if (osMessageGetSpace(_my_msg_id) != (_MY_MSG_NUM-i)*sizeof(my_msg_t *))//for rhino
        //if (osMessageGetSpace(_my_msg_id) != (_MY_MSG_NUM-i))
        {
            TRACE( 0 , "%s %d fail to osMessageGetSpace i=%d, Space=%d",__func__,__LINE__, i, \
                    osMessageGetSpace(_my_msg_id));
            return -1;
        }
        msg = (my_msg_t *)osPoolAlloc(_my_pool_id);
        if (msg == NULL)
        {
            TRACE( 0 , "%s %d fail to osPoolAlloc i=%d",__func__,__LINE__, i);
            return -1;
        }
        osPoolFree(_my_pool_id, msg);
        msg = (my_msg_t *)osPoolCAlloc(_my_pool_id);
        if (msg == NULL)
        {
            TRACE( 0 , "%s %d fail to osPoolCAlloc i=%d",__func__,__LINE__, i);
            return -1;
        }
        msg->test1 = msg->test2 = msg->test3 = i;
        if (osOK != osMessagePut(_my_msg_id, (uint32_t)msg, 0))
        {
            TRACE( 0 , "%s %d fail to osMessagePut i=%d",__func__,__LINE__, i);
            return -1;
        }
        TRACE( 0 , "%s %d osMessagePut i=%d, Space=%d",__func__,__LINE__, i, osMessageGetSpace(_my_msg_id));
    }

    osThreadId my_thread_id;
    osThreadDef(_my_msg_main, osPriorityAboveNormal, 1, (4*1024), "_my_msg_main");
    my_thread_id = osThreadCreate(osThread(_my_msg_main), &_my_msg_id);
    if (my_thread_id == NULL){
        TRACE( 0 , "%s %d fail to create _my_msg_main task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_msg_main task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id);

    osDelay(5000);
    TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, my_thread_id);
    osThreadTerminate(my_thread_id);

    if (osMessageQueueDelete(_my_msg_id) != osOK) {
        TRACE( 0 , "%s %d fail osMessageQueueDelete",__func__,__LINE__);
        return -1;
    }

    return 0;
}
static void _my_mail_main(const void *argument)
{
    osMailQId _my_mail_id = *(osMailQId *)argument;
    my_msg_t *msg = NULL;
    osEvent event;
    for (uint32_t i=0; i<_MY_MSG_NUM; i++)
    {
        event = osMailGet(_my_mail_id, 0);
        if (event.status != osEventMail)
        {
            TRACE( 0 , "%s %d fail to osMailGet i=%d",__func__,__LINE__, i);
            break;
        }
        msg = (my_msg_t *)event.value.p;
        if (msg->test1 != i)
        {
            TRACE( 0 , "%s %d fail to osMailGet i=%d",__func__,__LINE__, i);
            break;
        }
        TRACE( 0 , "%s %d osMailGet i=%d",__func__,__LINE__, i);
        osMailFree(_my_mail_id, msg);
    }
    event = osMailGet(_my_mail_id, 0);
    if (event.status == osEventMail)
    {
        TRACE( 0 , "%s %d should fail to osMailGet i=%d",__func__,__LINE__, _MY_MSG_NUM);
    }
    while (1)
    {
        TRACE(0 , "%s loop",__func__);
        osDelay(500);
    }
    osThreadExit();
}
static int bes_mail_test(void)
{
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    my_msg_t *msg = NULL;
    osMailQId my_mail_id = 0;
    osMailQDef (_my_mail, _MY_MSG_NUM, my_msg_t);
    my_mail_id = osMailCreate(osMailQ(_my_mail), NULL);
    if (my_mail_id == NULL)
    {
        TRACE( 0 , "%s %d fail to osMailCreate",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osMailCreate id=%d",__func__,__LINE__,my_mail_id);

    for (uint32_t i=0; i<_MY_MSG_NUM; i++)
    {
        msg = (my_msg_t *)osMailAlloc(my_mail_id, 0);
        if (msg == NULL)
        {
            TRACE( 0 , "%s %d fail to osMailAlloc i=%d",__func__,__LINE__, i);
            return -1;
        }
        osMailFree(my_mail_id, msg);
        msg = (my_msg_t *)osMailAlloc(my_mail_id, 0);
        if (msg == NULL)
        {
            TRACE( 0 , "%s %d fail to osMailCAlloc i=%d",__func__,__LINE__, i);
            return -1;
        }
        msg->test1 = msg->test2 = msg->test3 = i;
        if (osOK != osMailPut(my_mail_id, msg))
        {
            TRACE( 0 , "%s %d fail to osMailPut i=%d",__func__,__LINE__, i);
            return -1;
        }
        TRACE( 0 , "%s %d osMailPut i=%d",__func__,__LINE__, i);
    }

    osThreadId my_thread_id;
    osThreadDef(_my_mail_main, osPriorityAboveNormal, 1, (4*1024), "_my_mail_main");
    my_thread_id = osThreadCreate(osThread(_my_mail_main), &my_mail_id);
    if (my_thread_id == NULL){
        TRACE( 0 , "%s %d fail to create _my_mail_main task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_mail_main task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id);

    osDelay(5000);
    TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, my_thread_id);
    osThreadTerminate(my_thread_id);

    return 0;
}

static void _my_sem_main(const void *argument)
{
    osSemaphoreId _my_sem_id = (osSemaphoreId)argument;
    osDelay(1000);
    TRACE( 0 , "%s %d osSemaphoreRelease id=%d",__func__,__LINE__,_my_sem_id);
    if (osOK != osSemaphoreRelease((void *)argument))
    {
        TRACE( 0 , "%s %d fail osSemaphoreRelease",__func__,__LINE__);
    }
    osDelay(1000);
    TRACE( 0 , "%s %d osSemaphoreWait id=%d",__func__,__LINE__,_my_sem_id);
    if (-1 == osSemaphoreWait((void *)argument, osWaitForever))
    {
        TRACE( 0 , "%s %d fail osSemaphoreWait",__func__,__LINE__);
    }
    int ret = osSemaphoreWait((void *)argument, 1000);
    TRACE( 0 , "%s %d osSemaphoreWait 1000ms ret=%d",__func__,__LINE__, ret);
    ret = osSemaphoreWait((void *)argument, 2000);
    TRACE( 0 , "%s %d osSemaphoreWait 2000ms ret=%d",__func__,__LINE__, ret);

    while (1)
    {
        TRACE( 0 , "%s loop",__func__);
        osDelay(500);
    }
    osThreadExit();
}
static int bes_sem_test(void)
{
    int period = 12;
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    osSemaphoreId _my_sem_id = 0;
    osSemaphoreDef(_my_sem);
    _my_sem_id = osSemaphoreCreate(osSemaphore(_my_sem), 0);
    if (_my_sem_id == NULL)
    {
        TRACE( 0 , "%s %d fail to osSemaphoreCreate",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osSemaphoreCreate id=%d",__func__,__LINE__,_my_sem_id);

    osThreadId my_thread_id;
    osThreadDef(_my_sem_main, osPriorityAboveNormal, 1, (4*1024), "_my_sem_main");
    my_thread_id = osThreadCreate(osThread(_my_sem_main), _my_sem_id);
    if (my_thread_id == NULL){
        TRACE( 0 , "%s %d fail to create _my_sem_main task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_sem_main task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id);

    TRACE( 0 , "%s %d osSemaphoreWait id=%d",__func__,__LINE__,_my_sem_id);
    if (-1 == osSemaphoreWait(_my_sem_id, osWaitForever))
    {
        TRACE( 0 , "%s %d fail osSemaphoreWait",__func__,__LINE__);
        return -1;
    }

    osDelay(2000);
    TRACE( 0 , "%s %d osSemaphoreRelease id=%d",__func__,__LINE__,_my_sem_id);
    if (osOK != osSemaphoreRelease(_my_sem_id))
    {
        TRACE( 0 , "%s %d fail osSemaphoreWait",__func__,__LINE__);
        return -1;
    }
    osDelay(2000);

    if (osOK != osThreadTerminate(my_thread_id))
    {
        TRACE( 0 , "%s %d fail osThreadTerminate",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osThreadTerminate _my_sem_main",__func__,__LINE__);
    if (osOK != osSemaphoreDelete(_my_sem_id))
    {
        TRACE( 0 , "%s %d fail osSemaphoreDelete",__func__,__LINE__);
        return -1;
    }

    return 0;
}

static int _my_timer_tick = 0;
static void my_timer_callback(osTimerId my_timer_id, void* arg)
{
    TRACE( 0 , "%s:%d, my_timer_id=%d, arg=%d", __func__, __LINE__, my_timer_id, arg);
    _my_timer_tick++;
}

static int bes_timer_test(void)
{
    osTimerId my_timer_id = 0;
    uint32_t param = 0;
    osTimerDef(_my_timer, my_timer_callback);
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    my_timer_id = osTimerCreate(osTimer(_my_timer), osTimerOnce, NULL);
    if (my_timer_id == NULL)
    {
        TRACE( 0 , "%s %d osTimerCreate osTimerOnce fail",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osTimerCreate osTimerOnce, id=%d",__func__,__LINE__,my_timer_id);
    if (osOK != osTimerStart(my_timer_id, 1000))
    {
        TRACE( 0 , "%s %d osTimerStart fail",__func__,__LINE__);
        return -1;
    }
    osDelay(3000);
    if (osOK != osTimerDelete(my_timer_id))
    {
        TRACE( 0 , "%s %d osTimerDelete fail",__func__,__LINE__);
        return -1;
    }

    param = 100;
    my_timer_id = osTimerCreate(osTimer(_my_timer), osTimerPeriodic, (void *)param);
    if (my_timer_id == NULL)
    {
        TRACE( 0 , "%s %d osTimerCreate osTimerPeriodic fail",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osTimerCreate osTimerPeriodic, id=%d",__func__,__LINE__,my_timer_id);
    if (osOK != osTimerStart(my_timer_id, 1000))
    {
        TRACE( 0 , "%s %d osTimerStart fail",__func__,__LINE__);
        return -1;
    }
    osDelay(5000);
    if (osOK != osTimerStop(my_timer_id))
    {
        TRACE( 0 , "%s %d osTimerStop fail",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osTimerStop done",__func__,__LINE__);
    osDelay(5000);
    if (osOK != osTimerStart(my_timer_id, 1000))
    {
        TRACE( 0 , "%s %d osTimerStart fail",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osTimerStart done",__func__,__LINE__);
    osDelay(5000);
    if (osOK != osTimerDelete(my_timer_id))
    {
        TRACE( 0 , "%s %d osTimerDelete fail",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osTimerDelete done",__func__,__LINE__);
    _my_timer_tick = 0;
    osDelay(5000);
    return _my_timer_tick;
}

static int bes_mutex_test(void)
{
    int period = 12;
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    osMutexId my_mutex_id = 0;
    osMutexDef(_my_mutex);
    my_mutex_id = osMutexCreate(osMutex(_my_mutex));
    if (my_mutex_id == NULL)
    {
        TRACE( 0 , "%s %d osMutexCreate fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexWait(my_mutex_id, osWaitForever))
    {
        TRACE( 0 , "%s %d osMutexWait fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexRelease(my_mutex_id))
    {
        TRACE( 0 , "%s %d osMutexRelease fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexDelete(my_mutex_id))
    {
        TRACE( 0 , "%s %d osMutexDelete fail",__func__,__LINE__);
        return -1;
    }
    return 0;
}

osMutexId my_thread_mutex_id = 0;
osMutexDef(_my_thread_mutex);
static int bes_thread_mutex_create(void)
{
    my_thread_mutex_id = osMutexCreate(osMutex(_my_thread_mutex));
    if (my_thread_mutex_id == NULL)
    {
        TRACE( 0 , "%s %d osMutexCreate fail",__func__,__LINE__);
        return -1;
    }
}

static void mutex_thread2(void)
{
    int begin_time;
    int duration_time;
    int test_num = 0;
    while(1)
    {
        osMutexRelease(my_thread_mutex_id);
        osDelay(500);
        begin_time = hal_sys_timer_get();
        osStatus ret = osMutexWait(my_thread_mutex_id, osWaitForever);
        duration_time = TICKS_TO_MS(hal_sys_timer_get() - begin_time);

        if (ret != osOK )
            TRACE( 0 , "[%s] mutex wait ERR", __FUNCTION__);

        TRACE( 0 , "[%s] test_num = %d mutex wait %d ms", __FUNCTION__, test_num, duration_time);
        test_num++;
        osDelay(3000);
        TRACE( 0 , "[%s] after delay 3500 ms osMutexRelease", __FUNCTION__);

        if (11 == test_num)
        {
            TRACE( 0 , "%s exit", __func__);
            break;
        }
    }
    osThreadExit();

}

static void mutex_thread1(void)
{
    int begin_time;
    int duration_time;
    int test_num = 0;
    while(1)
    {
        begin_time = hal_sys_timer_get();
        osStatus ret = osMutexWait(my_thread_mutex_id, osWaitForever);
        duration_time = TICKS_TO_MS(hal_sys_timer_get() - begin_time);

        if (ret != osOK )
            TRACE( 0 , "[%s] mutex wait ERR", __FUNCTION__);

        TRACE( 0 , "[%s] test_num = %d mutex wait %d ms", __FUNCTION__, test_num, duration_time);
        osDelay(3500);
        TRACE( 0 , "[%s] after delay 3500 ms osMutexRelease", __FUNCTION__);
        osMutexRelease(my_thread_mutex_id);
        test_num++;

        if (11 == test_num)
        {
            TRACE( 0 , "%s exit", __func__);
            break;
        }
    }
    osThreadExit();
}

int bes_thread_mutex_test(void)
{

    bes_thread_mutex_create();

    osThreadDef(mutex_thread1, osPriorityHigh, 1, (4*1024), "mutex_thread1");
    osThreadId mutex_thread_id1= osThreadCreate(osThread(mutex_thread1), NULL);
    osThreadDef(mutex_thread2, osPriorityAboveNormal, 1, (4*1024), "mutex_thread2");
    osThreadId mutex_thread_id2= osThreadCreate(osThread(mutex_thread2), NULL);

    osDelay(100000);//in case of died early, for rhino;
}





typedef struct _signal_param_tst
{
    osThreadId threadID;
    int32_t    signal_value;
}_signal_thread_t;

static void _my_signal_main(const void *argument)
{
    _signal_thread_t* _this = (_signal_thread_t *)argument;
    for (int i=0; i<31; i++) {
        osEvent evt;
        _this->signal_value = 1<<i;
        TRACE( 0 , "%s wait signal=0x%X", __FUNCTION__, _this->signal_value);
        evt = osSignalWait(_this->signal_value, osWaitForever);
        if (evt.status == osEventSignal)
        {
            TRACE( 0 , "%s get signal=0x%X", __FUNCTION__, evt.value.signals);
            osSignalClear(_this->threadID, evt.value.signals);
        } else {
            TRACE( 0 , "%s get signal fail status 0x%X", __FUNCTION__, evt.status);
            osDelay(10);
        }
    }
    osThreadExit();
}

static _signal_thread_t signal_thread;
static int bes_signal_test(void)
{
    int ret = 0;
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);

    /* the priority of _my_signal_main should be higher than current thread */
    osThreadDef(_my_signal_main, osPriorityHigh, 1, (4*1024), "_my_sem_main");
    signal_thread.threadID = osThreadCreate(osThread(_my_signal_main), &signal_thread);
    if (signal_thread.threadID == NULL){
        TRACE( 0 , "%s %d fail to create _my_signal_main task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_signal_main task = 0x%X", __FUNCTION__, (uint32_t)signal_thread.threadID);

    /* test from bit0~bit30, bit31 is reserved for error status */
    for (int i=0; i<31; i++)
    {
        TRACE(0, "[%d] => %s osSignalSet 0x%x", i, __FUNCTION__, signal_thread.signal_value);
        osSignalSet(signal_thread.threadID, signal_thread.signal_value);
        osDelay(20);
    }
    return 0;
}

static void _my_thread_main1(const void *argument)
{
    TRACE( 0 , "%s osThreadGetId=%d", __FUNCTION__, osThreadGetId());

    for (int i=0; i<20; i++)
    {
        osDelay(1000);
        if (i%3 == 0)
        {
            osThreadYield();
        }
        TRACE( 0 , "%s id=%d heart beat...", __FUNCTION__, osThreadGetId());
    }
    TRACE( 0 , "%s now exit osThreadGetId=%d", __FUNCTION__, osThreadGetId());
    osThreadExit();
}
static void _my_thread_main2(const void *argument)
{
    TRACE( 0 , "%s osThreadGetId=%d", __FUNCTION__, osThreadGetId());

    for (int i=0; i<20; i++)
    {
        osDelay(1000);
        if (i%3 == 0)
        {
            osThreadYield();
        }
        TRACE( 0 , "%s id=%d heart beat...", __FUNCTION__, osThreadGetId());
    }
    TRACE( 0 , "%s now exit osThreadGetId=%d", __FUNCTION__, osThreadGetId());
    osThreadExit();
}
static int bes_thread_test(void)
{
    osThreadId my_thread_id1;
    osThreadDef(_my_thread_main1, osPriorityAboveNormal, 1, (4*1024), "_my_thread_main1");
    osThreadId my_thread_id2;
    osThreadDef(_my_thread_main2, osPriorityAboveNormal, 1, (4*1024), "_my_thread_main2");

    int period = 12;
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);

    my_thread_id1 = osThreadCreate(osThread(_my_thread_main1), NULL);
    if (my_thread_id1 == NULL){
        TRACE( 0 , "%s %d fail to create _my_thread_main1 task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_thread_main1 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id1);
    my_thread_id2 = osThreadCreate(osThread(_my_thread_main2), NULL);
    if (my_thread_id2 == NULL){
        TRACE( 0 , "%s %d fail to create _my_thread_main2 task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_thread_main2 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id2);

    osDelay(2000);
    if (osOK != osThreadSetPriority(my_thread_id1, osPriorityNormal))
    {
        TRACE( 0 , "%s osThreadSetPriority fail!", __FUNCTION__);
        return -1;
    }
    osDelay(2000);
    if (osPriorityNormal != osThreadGetPriority(my_thread_id1))
    {
        TRACE( 0 , "%s osThreadGetPriority fail!", __FUNCTION__);
        return -1;
    }

    TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, my_thread_id1);
    osThreadTerminate(my_thread_id1);
    osDelay(2000);
    TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, my_thread_id2);
    osThreadTerminate(my_thread_id2);
    return 0;
}

static int bes_delay_test(void)
{
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);

    int _begin_time = TICKS_TO_MS(hal_sys_timer_get());
    int time = 0;
    int fail = 0;
    for (int i=10; i<=1000; i*=10)
    {
        _begin_time = hal_sys_timer_get();
        osDelay(i);
        time = TICKS_TO_MS(hal_sys_timer_get()-_begin_time);
        if (time > i+5)
        {
            TRACE( 0 , "FAIL delay i=%dms, actually=%dms\n", i, time);
            fail = -1;
        }
    }

    return fail;
}

static int bes_print_test(void)
{
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);

    TRACE( 0 , "223=%d", 223);
    TRACE( 0 , "-232=%d", -232);

    TRACE( 0 , "223.11=%f",223.11);
    TRACE( 0 , "232.11111111=%f", 232.11111111);
    TRACE( 0 , "-223.11=%f", -223.11);
    TRACE( 0 , "-232.11111111=%f", -232.11111111);

    TRACE( 0 , "223.11=%e", 223.11);
    TRACE( 0 , "232.11111111=%e", 232.11111111);
    TRACE( 0 , "-223.11=%e", -223.11);
    TRACE( 0 , "-232.11111111=%e", -232.11111111);

    TRACE( 0 , "223.11=%g", 223.11);
    TRACE( 0 , "232.111111111111=%g", 232.111111111111);
    TRACE( 0 , "-223.11=%g", -223.11);
    TRACE( 0 , "-232.111111111111=%g", -232.111111111111);

    TRACE( 0 , "223.11=%-10f", 223.11);
    TRACE( 0 , "232.11111111=%+10.5f", 232.11111111);

    int a=29;
    float b=1243.2341;
    double c=24212345.24232;
    long long int e=1234567890;
    TRACE( 0 , "a=29 d=%d\t,ld=%ld\t,o=%o\t,lld=%lld\t",a,a,a,a);
    TRACE( 0 , "b=1243.2341 f=%f\t,lf=%lf\t,5.4lf=%5.4lf\t,e=%e\t",b,b,b,b);
    TRACE( 0 , "c=24212345.24232 lf=%lf\t,f=%f\t,8.4lf=%8.4lf\t",c,c,c);
    TRACE( 0 , "e=1234567890 d=%d\t,ld=%ld\t,lld=%lld\t", e,e,e);

    return 0;
}


static int bes_sscanf_test(void)
{
    TRACE( 0 , "%s L%d ()>", __FUNCTION__, __LINE__);

    //case 1: %s %7s %d %2d
    int day = 0;
    int year = 0;
    char weekday[20] = {0};
    char month[20] = {0};
    char date[100] = {0};

    strcpy(date, "Thursday January 21 1");
    sscanf(date, "%s %7s %2d %d", weekday, month, &day, &year);

    TRACE( 0 , "%s L%d Thursday January 21 1 = %s %s %d %d", __FUNCTION__, __LINE__, weekday, month, day, year);

    //case 2: %[]
    const char *website = "http://www.bestechnic.com:1900";
    char protocol[32] = {0};
    char host[128] = {0};
    char port[8] = {0};

    sscanf(website, "%[^:]://%[^:]:%[0-9]", protocol, host, port);

    TRACE( 0 , "%s L%d http://www.bestechnic.com:1900 = %s//%s:%s", __FUNCTION__, __LINE__, protocol, host, port);

    //case 3: %f %lf %x
    int iTemp = 0;
    float fTemp = 0.0;
    double dTemp = 0.0;
    long long int lliTemp = 0;

    sscanf("29", "%d", &iTemp);
    TRACE( 0 , "%s L%d 29 = %d", __FUNCTION__, __LINE__, iTemp);

    sscanf("1243.2341", "%f", &fTemp);
    TRACE( 0 , "%s L%d 1243.2341 = %f", __FUNCTION__, __LINE__, fTemp);

    sscanf("24212345.24232", "%lf", &dTemp);
    TRACE( 0 , "%s L%d 24212345.24232 = %lf", __FUNCTION__, __LINE__, dTemp);

    sscanf("0x4567", "%x", &iTemp);
    TRACE( 0 , "%s L%d 0x4567 = 0x%x", __FUNCTION__, __LINE__, iTemp);

    TRACE( 0 , "%s L%d ()<", __FUNCTION__, __LINE__);
    return 0;
}

int bes_cmsis_test (void)
{
    bes_print_test();
    bes_sscanf_test();
    bes_delay_test();
    bes_thread_test();
    bes_signal_test();
    bes_mutex_test();
    bes_timer_test();
    bes_sem_test();
    bes_mail_test();
    bes_msg_test();
    bes_thread_mutex_test();
    return 0;
}

static void _my_thread_main3(const void *argument)
{
    TRACE( 0 , "%s osThreadGetId=%d", __FUNCTION__, osThreadGetId());

    TRACE( 0 , "%s id=%d is joined, waiting to end", __FUNCTION__, osThreadGetId());
    osDelay(200);
    TRACE( 0 , "%s id=%d is joined, waiting to end", __FUNCTION__, osThreadGetId());
    osDelay(200);
    TRACE( 0 , "%s now exit osThreadGetId=%d", __FUNCTION__, osThreadGetId());
    osThreadExit();
}


//in this function ,i supplement some thread function use-tests,
//Overrides the following functions:
//1.osThreadGetName          6.osThreadResume        11.osThreadFlagsGet(not)
//2.osThreadGetState         7.osThreadDetach(not)   12.osThreadFlagswait(not)
//3.osThreadGetStateSpace    8.osThreadGetCount      13.osThreadFlagsClear(not)
//4.osThreadGetStackSize     9.osThreadEnumerate(not)
//5.osThreadSuspend          10.osThreadFlagsSet(not)
//@kyhu 2021/7/12
static int bes_thread_test_supplement(void)
{
    osThreadId my_thread_id1;
    osThreadDef(_my_thread_main1, osPriorityAboveNormal, 1, (4*1024), "_my_thread_main1");
    osThreadId my_thread_id2;
    osThreadDef(_my_thread_main2, osPriorityAboveNormal, 1, (4*1024), "_my_thread_main2");

    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);

#if OS_THREAD_JOIN_TEST == 1
    osThreadId my_thread_id3;
    osThreadDef(_my_thread_main3, osPriorityAboveNormal, 1, (4*1024), "_my_thread_main3");

    my_thread_id3 = osThreadCreate(osThread(_my_thread_main3), NULL);
    if (my_thread_id3 == NULL){
        TRACE( 0 , "%s %d fail to create _my_thread_main3 task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_thread_main3 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id3);

    osDelay(2000);
    if (osOK != osThreadJoin(my_thread_id3))
    {
        TRACE( 0 , "%s osThreadjoin fail!", __FUNCTION__);
        return -1;
    }
    TRACE( 0 , "%s let _my_thread_main3 join", __FUNCTION__);
#endif

    my_thread_id1 = osThreadCreate(osThread(_my_thread_main1), NULL);
    if (my_thread_id1 == NULL){
        TRACE( 0 , "%s %d fail to create _my_thread_main1 task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_thread_main1 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id1);

    my_thread_id2 = osThreadCreate(osThread(_my_thread_main2), NULL);
    if (my_thread_id2 == NULL){
        TRACE( 0 , "%s %d fail to create _my_thread_main2 task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create _my_thread_main2 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id2);

    osDelay(2000);
    const char* name = osThreadGetName(my_thread_id1);
    if (name == NULL) {
        TRACE( 0 , "Failed to get the thread name; not in a thread");
        return -1;
    }
    TRACE( 0 , "%s get _my_thread_main1 name = %s", __FUNCTION__, name);

    osDelay(2000);
    osThreadState_t ret = osThreadGetState(my_thread_id1);
    if (ret == osThreadError) {
        TRACE( 0 , "Failed to get the thread state or it is called from an ISR");
        return -1;
    }
    TRACE( 0 , "%s get _my_thread_main1 state_code = %d(check in enum osthreadstate)", __FUNCTION__, ret);

    osDelay(2000);
    int stacksize = osThreadGetStackSize(my_thread_id1);
    if (stacksize == 0)
    {
        TRACE( 0 , "%s osThreadGetStackSize fail!", __FUNCTION__);
        return -1;
    }
    TRACE( 0 , "%s get _my_thread_main1 stacksize = %d bytes", __FUNCTION__, (uint32_t)stacksize);

    osDelay(2000);
    int stackspace = osThreadGetStackSpace(my_thread_id1);
    if (stackspace == 0)
    {
        TRACE( 0 , "%s osThreadGetStackSpace fail!", __FUNCTION__);
        return -1;
    }
    TRACE( 0 , "%s get _my_thread_main1 remaining stackspace = %d bytes", __FUNCTION__, (uint32_t)stackspace);

    osDelay(2000);
    if (osOK != osThreadSuspend(my_thread_id1))
    {
        TRACE( 0 , "%s osThreadsuspend fail!", __FUNCTION__);
        return -1;
    }
    TRACE( 0 , "%s let _my_thread_main1 suspend", __FUNCTION__);

    osDelay(2000);
    if (osOK != osThreadResume(my_thread_id1))
    {
        TRACE( 0 , "%s osThreadresume fail!", __FUNCTION__);
        return -1;
    }
    TRACE( 0 , "%s let _my_thread_main1 resume", __FUNCTION__);
#if OS_THREAD_DETACH_TEST_OPEN == 1
    osDelay(2000);
    if (osOK != osThreadDetach(my_thread_id2))
    {
        TRACE( 0 , "%s osThreaddetach fail!", __FUNCTION__);
        return -1;
    }
    TRACE( 0 , "%s let _my_thread_main2 detach", __FUNCTION__);
#endif
    osDelay(2000);
    uint32_t threadcount = osThreadGetCount();
    if (threadcount == 0)
        TRACE( 0 , "osgetthreadcount fail!");
    else
    TRACE( 0 , "os get thread count = %d", (uint32_t)threadcount);

#if OS_THREAD_ENUMERATE_TEST_OPEN == 1
    osDelay(2000);
    osThreadId_t * 	thread_array;//pointer to array for retrieving thread IDs.
    uint32_t 	array_items = 5;//maximum number of items i n array for retrieving thread IDs.
    if(osThreadEnumerate(thread_array,array_items) != 0)
        TRACE( 0 , "os get thread enmerate success, save in pointer thread_array(in code)");
    else
    {
        TRACE( 0 , "os get thread enmerate failed,exit!");
        return -1;
    }
#endif

#if OS_THREAD_THREADFLAGS_TEST_OPEN == 1
    osDelay(2000);
    osThreadId_t tid;
    uint32_t     flagsX;
    uint32_t     flags;
    uint32_t     flags_get;

    void threadX (void *argument)
    {
        osDelay(1U);
        flagsX = osThreadFlagsWait(0x0001U, osFlagsWaitAny, osWaitForever); /* B */ // wait until any flag change
                                                                                    // then minus 0x0001U on flags
        if(flagsX != 0x001U)
        {
            TRACE( 0 , "osThreadFlagswait failed,flagsX is %d, not 0x01",flagsX);
            return -1;
        }
    }                                                                               // so when it's called it
                                                                                    //  will return 1 (2-1=1).

    tid = osThreadNew(threadX, NULL, NULL);
    TRACE( 0 , "osThreadflag start to testing");

    flags = osThreadFlagsSet(tid, 0x0002U); /* A */ //now flags is 2
    osDelay(2U);
    flags = osThreadFlagsSet(tid, 0x0005U); /* C */ //2+5=7,meanwhile,change tigger threadx,flags-=1.C will return 6.
    osDelay(2U);
    flags_get = osThreadFlagsGet();

    if(flags_get != 0x006U)
    {
        TRACE( 0 , "osThreadFlags sth failed,flags is %d, not 0x06",flags_get);
        return -1;
    }
    else
    TRACE(0, "osthreadflags test success!");

    osThreadFlagsClear(flags);
    osThreadTerminate(tid);
#endif
    TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, my_thread_id1);
    osThreadTerminate(my_thread_id1);
    osDelay(2000);

    TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, my_thread_id2);
    osThreadTerminate(my_thread_id2);

    return 0;
}

/**
* test osdelayuntil,use TICKS_TO_MS(hal_sys_timer_get()) in this test,
* test will fail, reason possible related to function which related to
* get system time used in cmsis_liteos2.c.
*/
static int bes_delay_test_supplement(void)
{
    int now_time;
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);

    //int _begin_time = TICKS_TO_MS(hal_sys_timer_get());
    int _begin_time = osKernelGetTickCount();
    int _end_time = _begin_time + 10000;
    TRACE( 0 , "begin time is %d, end time is %d",_begin_time,_end_time);
    osDelayUntil(_end_time);
    //if (_begin_time == TICKS_TO_MS(hal_sys_timer_get()) - 10000)
    if ( (osKernelGetTickCount() - 10000 - _begin_time) == 0 )
    {
        TRACE( 0 , "delay until success!");
        return 0;
    }
    else
    {
        //now_time = TICKS_TO_MS(hal_sys_timer_get());
        now_time = osKernelGetTickCount();
        TRACE( 0 , "osdelayuntil time fail!,now time is %d", now_time);
        return -1;
    }
    TRACE( 0 , "osdelayuntil fail!");
    return -1;
}

//supplement_test ostimergetname ostimeeisrunning
static int bes_timer_test_supplement(void)
{
    osTimerId my_timer_id = 0;
    osTimerDef(_my_timer, my_timer_callback);
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    my_timer_id = osTimerCreate(osTimer(_my_timer), osTimerOnce, NULL);
    if (my_timer_id == NULL)
    {
        TRACE( 0 , "%s %d osTimerCreate fail",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osTimerCreate , id=%d",__func__,__LINE__,my_timer_id);
    if (osOK != osTimerStart(my_timer_id, 1000))
    {
        TRACE( 0 , "%s %d osTimerStart fail",__func__,__LINE__);
        osTimerDelete(my_timer_id);
        return -1;
    }

    osDelay(3000);
    if (osTimerIsRunning(my_timer_id) == 1)
    {
        TRACE( 0 , "%s %d osTimer is running.",__func__,__LINE__);
    } else {
        TRACE( 0 , "%s %d osTimer isn't running.",__func__,__LINE__);
    }

    osDelay(3000);
    const char* timername;
    timername = osTimerGetName(my_timer_id);
    TRACE( 0 , "%s %d osTimergettimer name = %s ",__func__,__LINE__,timername);

    osDelay(3000);
    if (osOK != osTimerDelete(my_timer_id))
    {
        TRACE( 0 , "%s %d osTimerDelete fail",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "bes_timer_test_supplement success, exit");
    return 0;
}

osEventFlagsId_t evt_id;                        // event flags id

 void Thread_EventSender (void *argument)
{
    osEventFlagsSet(evt_id, FLAGS_MSK1);
    TRACE( 0 , "%s %d oseventflagSet success\n",__func__,__LINE__);
    osDelay(2000);                            // suspend thread
}

void Thread_EventReceiver (void *argument)
{
    uint32_t flags;
    flags = osEventFlagsWait(evt_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
    TRACE( 0 , "%s %d oseventflagwait success\n",__func__,__LINE__);//handle event
    osDelay(1000);

    TRACE( 0 , "%s now exit osThreadGetId=%d\n", __FUNCTION__, osThreadGetId());
    osThreadExit();
}


//supplement_test event_flag function
static int bes_event_flag_test(void)
{

    osThreadId tid_Thread_EventSender;       // thread id 1
    osThreadId tid_Thread_EventReceiver;          // thread id 2
    int event_flags_num;

    osThreadDef(Thread_EventSender, osPriorityAboveNormal, 1, (4*1024), "Thread_EventSender");
    osThreadDef(Thread_EventReceiver, osPriorityAboveNormal, 1, (4*1024), "Thread_EventEventReceiver");

    evt_id = osEventFlagsNew(NULL);
    if (evt_id == NULL) {
        TRACE( 0 , "%s %d oseventflagCreate fail",__func__,__LINE__);
        return -1; // Event Flags object not created, handle failure
    }

    tid_Thread_EventReceiver = osThreadCreate(osThread(Thread_EventReceiver), NULL);
    if (tid_Thread_EventReceiver == NULL){
        TRACE( 0 , "%s %d fail to create Thread_EventReceiver task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create Thread_EventReceiver task = 0x%X", __FUNCTION__, (uint32_t)tid_Thread_EventReceiver);

    tid_Thread_EventSender = osThreadCreate(osThread(Thread_EventSender), NULL);
    if (tid_Thread_EventSender == NULL){
        TRACE( 0 , "%s %d fail to create Thread_EventSender task",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s create Thread_EventSender task = 0x%X", __FUNCTION__, (uint32_t)tid_Thread_EventSender);

    event_flags_num = osEventFlagsGet(evt_id);
    TRACE( 0 , "%s %d oseventflagget success,flag is %d",__func__,__LINE__,event_flags_num);

    osDelay(2000);
    if (osOK != osEventFlagsDelete(evt_id))
    {
        TRACE( 0 , "%s %d osEnventFlagrDelete fail",__func__,__LINE__);
        return -1;
    }

    if (osThreadTerminate(tid_Thread_EventSender) == osOK){
        TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, Thread_EventSender);
    } else {
        TRACE( 0 , "%s osThreadTerminate fail %d", __FUNCTION__, Thread_EventSender);
    }

    return(0);
}

//supplement_test osMutexGetName and osMutexGetOwner
static int bes_mutex_test_supplement(void)
{
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    osMutexId my_mutex_id = 0;
    osMutexDef(_my_mutex);
    my_mutex_id = osMutexCreate(osMutex(_my_mutex));
    if (my_mutex_id == NULL)
    {
        TRACE( 0 , "%s %d osMutexCreate fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexWait(my_mutex_id, osWaitForever))
    {
        TRACE( 0 , "%s %d osMutexWait fail",__func__,__LINE__);
        return -1;
    }

    osThreadId mutexowner = osMutexGetOwner(my_mutex_id);
    if(mutexowner == NULL)
    {
        TRACE( 0 , "%s %d osMutexgetwoner fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexRelease(my_mutex_id))
    {
        TRACE( 0 , "%s %d osMutexRelease fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexDelete(my_mutex_id))
    {
        TRACE( 0 , "%s %d osMutexDelete fail",__func__,__LINE__);
        return -1;
    }
    return 0;
}

//supplement_test osmessagereset,but did not realize in cmsis_liteos2.c
static int bes_msgreset_test_supplement(void)
{
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    osPoolDef(_my_pool, _MY_MSG_NUM, my_msg_t);
    _my_pool_id = osPoolCreate(osPool(_my_pool));
    if (_my_pool_id == NULL)
    {
        TRACE( 0 , "%s %d fail to osPoolCreate",__func__,__LINE__);
        return -1;
    }
    osMessageQId _my_msg_id = 0;
    osMessageQDef(_my_msg, _MY_MSG_NUM, my_msg_t*);
    _my_msg_id = osMessageCreate(osMessageQ(_my_msg), NULL);
    if (_my_msg_id == NULL)
    {
        TRACE( 0 , "%s %d fail to osMessageCreate",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d osMessageCreate id=%d, Space=%d, sizeof(my_msg_t *)=%d",__func__,__LINE__, \
            _my_msg_id, osMessageGetSpace(_my_msg_id), sizeof(my_msg_t *));

    if (osMessageQueueReset(_my_msg_id) != osOK)
    {
        TRACE( 0 , "%s %d fail to osMessagereset",__func__,__LINE__);
        return -1;
    }

    if (osMessageQueueDelete(_my_msg_id) != osOK) {
        TRACE( 0 , "%s %d fail osMessageQueueDelete",__func__,__LINE__);
        return -1;
    }

    return 0;
}

/******************************    Thread Message Queue Test    ******************************/

#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects

typedef struct {                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;

osMessageQueueId_t mid_MsgQueue;                // message queue id

osThreadId_t tid_Thread_MsgQueue1;              // thread id 1
osThreadId_t tid_Thread_MsgQueue2;              // thread id 2

static int Thread_MsgQueue1 (void *argument);         // thread function 1
static int Thread_MsgQueue2 (void *argument);         // thread function 2

static int Thread_MsgQueue1 (void *argument)
{
    MSGQUEUE_OBJ_t msg;
    osStatus_t status;
    uint32_t ret_num = 0;
    const char* msg_char = "cmsis_os2_test";

    msg.Buf[0] = 0x55U;                                         // do some work...
    msg.Idx    = 0U;
    status = osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
    if (status == osOK) {
        TRACE( 4 , "%s %d osMessageQueuePutBuf[0] = %d, Idx = %d",__func__,__LINE__, msg.Buf[0],msg.Idx);
    }
    status = osMessageQueuePut(mid_MsgQueue, &msg_char, 0U, 0U);
    if (status == osOK) {
        TRACE( 4 , "%s %d osMessageQueuePutmsgchar = %s",__func__,__LINE__, msg_char); 
    }
    ret_num = osMessageQueueGetSpace(mid_MsgQueue);
    if (ret_num == 0)
    {
        TRACE( 0 , "%s %d fail to osMessageQueueGetSpace",__func__,__LINE__);
        return -1;
    }
    TRACE( 3 , "%s %d osMessageQueueGetSpace = %d",__func__,__LINE__, ret_num);

    osThreadYield();// suspend thread
    osDelay(7000);//in case of rekill event happend in rhino?
    return 0;
}

static int Thread_MsgQueue2 (void *argument)
{
    MSGQUEUE_OBJ_t msg;
    osStatus_t status;
    uint32_t ret_num = 0;
    char* msg_recv;

    status = osMessageQueueGet(mid_MsgQueue, &msg, NULL, 0U);   // wait for message
    if (status == osOK) {
        TRACE( 4 , "%s %d osMessageQueueGetBuf[0] = %d, Idx = %d",__func__,__LINE__, msg.Buf[0],msg.Idx);
    }

    ret_num = osMessageQueueGetCount(mid_MsgQueue);
    if (ret_num == 0)
    {
        TRACE( 0 , "%s %d fail to osMessageQueueGetCount",__func__,__LINE__);
        return -1;
    }
    TRACE( 3 , "%s %d osMessageQueueGetCount = %d",__func__,__LINE__, ret_num);

    status = osMessageQueueGet(mid_MsgQueue, &msg_recv, NULL, 0U);   // wait for message
    if (status == osOK) {
        TRACE( 3 , "%s %d osMessageQueueGetstr = %s",__func__,__LINE__, msg_recv);
    } else {
        TRACE( 2 , "%s %d fail to get messagequeue here,osstatus = %d",__func__,__LINE__,status);
    }

    osThreadYield();
    osDelay(7000);
    return 0;
}

static int bes_msg_queue_test(void)
{
    TRACE( 0 , "\n\n%-32s testing......\n", __FUNCTION__);
    int ret_num = 0;

    mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
    if (mid_MsgQueue == NULL) {
        TRACE( 2 , "%s %d fail to osMessagecreat",__func__,__LINE__);
        return -1;
    }
#if OS_MESSAGE_QUEUE_GET_NAME_TEST ==1
    my_msg_name = osMessageQueueGetName(mid_MsgQueue);
    if (my_msg_name == NULL)
    {
        TRACE( 2 , "%s %d fail to osMessageQueueGetName",__func__,__LINE__);
        return -1;
    }
    TRACE( 3 , "%s %d osMessageQueueGetName name = %s",__func__,__LINE__, my_msg_name);
#endif
    ret_num = osMessageQueueGetCapacity(mid_MsgQueue);
    if (ret_num == 0)
    {
        TRACE( 2 , "%s %d fail to osMessageQueueGetCapacity",__func__,__LINE__);
        return -1;
    }
    TRACE( 3 , "%s %d osMessageQueueGetCapacity = %d",__func__,__LINE__, ret_num);

    ret_num = 0;
    ret_num = osMessageQueueGetMsgSize(mid_MsgQueue);
    if (ret_num == 0)
    {
        TRACE( 2 , "%s %d fail to osMessageQueueGetMsgSize",__func__,__LINE__);
        return -1;
    }
    TRACE( 3 , "%s %d osMessageQueueGetMsgSize = %d",__func__,__LINE__, ret_num);

    tid_Thread_MsgQueue1 = osThreadNew(Thread_MsgQueue1, NULL, NULL);
    if (tid_Thread_MsgQueue1 == NULL) {
        TRACE( 0 , "%s %d fail to osThreadNew",__func__,__LINE__);
        return(-1);
    }
    tid_Thread_MsgQueue2 = osThreadNew(Thread_MsgQueue2, NULL, NULL);
    if (tid_Thread_MsgQueue2 == NULL) {
        TRACE( 0 , "%s %d fail to osThreadNew",__func__,__LINE__);
        return(-1);
    }

    osDelay(6000);
    if (osMessageQueueDelete(mid_MsgQueue) != osOK)
    {
        TRACE( 0 , "%s %d fail to osMessageQueueDelete",__func__,__LINE__);
        return -1;
    }
    TRACE( 0 , "%s %d success to osMessageQueueDelete",__func__,__LINE__);

    if ((osThreadTerminate(tid_Thread_MsgQueue1)) == osOK)
        TRACE( 0 , "%s %d success to osMessagesender kill",__func__,__LINE__);
    else
        TRACE( 0 , "%s %d success to osMessagesender kill fail!!!",__func__,__LINE__);
    if ((osThreadTerminate(tid_Thread_MsgQueue2)) == osOK)
        TRACE( 0 , "%s %d success to osMessagesrecivier kill,test over & success",__func__,__LINE__);
    else
        TRACE( 0 , "%s %d success to osMessagesrecivier kill fail!!!",__func__,__LINE__);

    return(0);
}

int bes_cmsis_os2_test_supplement(void)
{
    bes_thread_test_supplement();
    bes_delay_test_supplement();
    bes_event_flag_test();
    bes_mutex_test_supplement();
#if MSG_TEST_RESET == 1
    bes_msgreset_test_supplement();
#endif
    bes_timer_test_supplement();
    bes_msg_queue_test();
    return 0;
}