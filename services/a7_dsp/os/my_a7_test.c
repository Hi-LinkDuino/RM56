#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_trace.h"
#include <string.h>
#include "hal_timer.h"

#define MY_A7_MSG_NUM 5
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
        if (osMessageGetSpace(_my_msg_id) != i)
        {
            TRACE(0, "%s %d fail to osMessageGetSpace i=%d, Space=%d",__func__,__LINE__, i, osMessageGetSpace(_my_msg_id));
            break;
        }
        osEvent event = osMessageGet(_my_msg_id, 0);
        if (event.status != osEventMessage)
        {
            TRACE(0, "%s %d fail to osMessageGetSpace i=%d",__func__,__LINE__, i);
            break;
        }
        msg = (my_msg_t *)event.value.p;
        if (msg->test1 != i)
        {
            TRACE(0, "%s %d fail to osMessageGet i=%d, test1=%d, test2=%d, test3=%d",__func__,__LINE__, i, msg->test1, msg->test2, msg->test3);
            break;
        }
        TRACE(0, "%s %d osMessageGet i=%d",__func__,__LINE__, i);
        osPoolFree(_my_pool_id, msg);
    }
    osEvent event = osMessageGet(_my_msg_id, 0);
    if (event.status == osEventMessage)
    {
        TRACE(0, "%s %d fail to osMessageGet i=%d",__func__,__LINE__, _MY_MSG_NUM);
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

static int my_msg_test(void)
{
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
    my_msg_t *msg = NULL;
    osPoolDef(_my_pool, _MY_MSG_NUM, my_msg_t);
    _my_pool_id = osPoolCreate(osPool(_my_pool));
    if (_my_pool_id == NULL)
    {
        TRACE(0, "%s %d fail to osPoolCreate",__func__,__LINE__);
        return -1;
    }
    osMessageQId _my_msg_id = NULL;
    osMessageQDef(_my_msg, _MY_MSG_NUM, my_msg_t*);
    _my_msg_id = osMessageCreate(osMessageQ(_my_msg), NULL);
    if (_my_msg_id == NULL)
    {
        TRACE(0, "%s %d fail to osMessageCreate",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osMessageCreate id=%d, Space=%d, sizeof(my_msg_t *)=%d",__func__,__LINE__, _my_msg_id, osMessageGetSpace(_my_msg_id), sizeof(my_msg_t *));

    for (uint32_t i=0; i<_MY_MSG_NUM; i++)
    {
        if (osMessageGetSpace(_my_msg_id) != (_MY_MSG_NUM-i))
        {
            TRACE(0, "%s %d fail to osMessageGetSpace i=%d, Space=%d",__func__,__LINE__, i, osMessageGetSpace(_my_msg_id));

            goto MSG_EXIT;
        }
        msg = (my_msg_t *)osPoolAlloc(_my_pool_id);
        if (msg == NULL)
        {
            TRACE(0, "%s %d fail to osPoolAlloc i=%d",__func__,__LINE__, i);
            goto MSG_EXIT;
        }
        osPoolFree(_my_pool_id, msg);
        msg = (my_msg_t *)osPoolCAlloc(_my_pool_id);
        if (msg == NULL)
        {
            TRACE(0, "%s %d fail to osPoolCAlloc i=%d",__func__,__LINE__, i);
           goto MSG_EXIT;
        }
        msg->test1 = msg->test2 = msg->test3 = i;
        if (osOK != osMessagePut(_my_msg_id, (uint32_t)msg, 0))
        {
            TRACE(0, "%s %d fail to osMessagePut i=%d",__func__,__LINE__, i);
            osPoolFree(_my_pool_id, msg);
            goto MSG_EXIT;
        }
        TRACE(0, "%s %d osMessagePut i=%d, Space=%d",__func__,__LINE__, i, osMessageGetSpace(_my_msg_id));
    }

    osThreadId my_thread_id;
    osThreadDef(_my_msg_main, osPriorityAboveNormal, 1, (4*1024), "_my_msg_main");
    my_thread_id = osThreadCreate(osThread(_my_msg_main), &_my_msg_id);
    if (my_thread_id == NULL){
        TRACE(0, "%s %d fail to create _my_msg_main task",__func__,__LINE__);
        osPoolFree(_my_pool_id, msg);
        return -1;
    }
    TRACE(0, "%s create _my_msg_main task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id);

    osDelay(5000);
    TRACE( 0 , "%s osThreadTerminate %d", __FUNCTION__, my_thread_id);
    osThreadTerminate(my_thread_id);
MSG_EXIT:
    if (osMessageQueueDelete(_my_msg_id) != osOK) {
        TRACE(0, "%s %d fail osMessageQueueDelete",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
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
            TRACE(0, "%s %d fail to osMailGet i=%d",__func__,__LINE__, i);
            break;
        }
        msg = (my_msg_t *)event.value.p;
        if (msg->test1 != i)
        {
            TRACE(0, "%s %d fail to osMailGet i=%d",__func__,__LINE__, i);
            break;
        }
        TRACE(0, "%s %d osMailGet i=%d",__func__,__LINE__, i);
        osMailFree(_my_mail_id, msg);
    }
    event = osMailGet(_my_mail_id, 0);
    if (event.status == osEventMail)
    {
        TRACE(0, "%s %d should fail to osMailGet i=%d",__func__,__LINE__, _MY_MSG_NUM);
    }
    while (1)
    {
        TRACE(0, "%s loop",__func__);
        osDelay(500);
    }
    osThreadExit();
}
static int my_mail_test(void)
{
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
    my_msg_t *msg = NULL;
    osMailQId my_mail_id = NULL;
    osMailQDef (_my_mail, _MY_MSG_NUM, my_msg_t);
    my_mail_id = osMailCreate(osMailQ(_my_mail), NULL);
    if (my_mail_id == NULL)
    {
        TRACE(0, "%s %d fail to osMailCreate",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osMailCreate id=%d",__func__,__LINE__,my_mail_id);

    for (uint32_t i=0; i<_MY_MSG_NUM; i++)
    {
        msg = (my_msg_t *)osMailAlloc(my_mail_id, 0);
        if (msg == NULL)
        {
            TRACE(0, "%s %d fail to osMailAlloc i=%d",__func__,__LINE__, i);
            return -1;
        }
        osMailFree(my_mail_id, msg);
        msg = (my_msg_t *)osMailAlloc(my_mail_id, 0);
        if (msg == NULL)
        {
            TRACE(0, "%s %d fail to osMailCAlloc i=%d",__func__,__LINE__, i);
            return -1;
        }
        msg->test1 = msg->test2 = msg->test3 = i;
        if (osOK != osMailPut(my_mail_id, msg))
        {
            TRACE(0, "%s %d fail to osMailPut i=%d",__func__,__LINE__, i);
            return -1;
        }
        TRACE(0, "%s %d osMailPut i=%d",__func__,__LINE__, i);
    }

    osThreadId my_thread_id;
    osThreadDef(_my_mail_main, osPriorityAboveNormal, 1, (4*1024), "_my_mail_main");
    my_thread_id = osThreadCreate(osThread(_my_mail_main), &my_mail_id);
    if (my_thread_id == NULL){
        TRACE(0, "%s %d fail to create _my_mail_main task",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s create _my_mail_main task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id);

    osDelay(5000);
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    TRACE(0, "%s osThreadTerminate %d", __FUNCTION__, my_thread_id);
    osThreadTerminate(my_thread_id);

    return 0;
}

static void _my_sem_main(const void *argument)
{
    osSemaphoreId _my_sem_id = (osSemaphoreId)argument;
    osDelay(1000);
    TRACE(0, "%s %d osSemaphoreRelease id=%d",__func__,__LINE__,_my_sem_id);
    if (osOK != osSemaphoreRelease((void *)argument))
    {
        TRACE(0, "%s %d fail osSemaphoreRelease",__func__,__LINE__);
    }
    osDelay(1000);
    TRACE(0, "%s %d osSemaphoreWait id=%d",__func__,__LINE__,_my_sem_id);
    if (-1 == osSemaphoreWait((void *)argument, osWaitForever))
    {
        TRACE(0, "%s %d fail osSemaphoreWait",__func__,__LINE__);
    }
    int ret = osSemaphoreWait((void *)argument, 1000);
    TRACE( 0 , "%s %d osSemaphoreWait 1000ms ret=%d",__func__,__LINE__, ret);
    ret = osSemaphoreWait((void *)argument, 2000);
    TRACE( 0 , "%s %d osSemaphoreWait 2000ms ret=%d",__func__,__LINE__, ret);

    while (1)
    {
        TRACE(0, "%s loop",__func__);
        osDelay(500);
    }
    osThreadExit();
}

static int my_sem_test(void)
{
    int period = 12;
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
    osSemaphoreId _my_sem_id = NULL;
    osSemaphoreDef(_my_sem);
    _my_sem_id = osSemaphoreCreate(osSemaphore(_my_sem), 0);
    if (_my_sem_id == NULL)
    {
        TRACE(0, "%s %d fail to osSemaphoreCreate",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osSemaphoreCreate id=%d",__func__,__LINE__,_my_sem_id);

    osThreadId my_thread_id;
    osThreadDef(_my_sem_main, osPriorityAboveNormal, 1, (4*1024), "_my_sem_main");
    my_thread_id = osThreadCreate(osThread(_my_sem_main), _my_sem_id);
    if (my_thread_id == NULL){
        TRACE(0, "%s %d fail to create _my_sem_main task",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s create _my_sem_main task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id);

    TRACE(0, "%s %d osSemaphoreWait id=%d",__func__,__LINE__,_my_sem_id);
    if (-1 == osSemaphoreWait(_my_sem_id, osWaitForever))
    {
        TRACE(0, "%s %d fail osSemaphoreWait",__func__,__LINE__);
        return -1;
    }

    osDelay(2000);
    TRACE(0, "%s %d osSemaphoreRelease id=%d",__func__,__LINE__,_my_sem_id);
    if (osOK != osSemaphoreRelease(_my_sem_id))
    {
        TRACE(0, "%s %d fail osSemaphoreWait",__func__,__LINE__);
        return -1;
    }
    osDelay(2000);

    if (osOK != osThreadTerminate(my_thread_id))
    {
        TRACE(0, "%s %d fail osThreadTerminate",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osThreadTerminate _my_sem_main",__func__,__LINE__);
    if (osOK != osSemaphoreDelete(_my_sem_id))
    {
        TRACE(0, "%s %d fail osSemaphoreDelete",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return 0;
}

static int _my_timer_tick = 0;
static void my_timer_callback(osTimerId my_timer_id, void* arg)
{
    TRACE(0, "%s:%d, my_timer_id=%d, arg=%d", __func__, __LINE__, my_timer_id, arg);
    _my_timer_tick++;
}

static int my_timer_test(void)
{
    osTimerId my_timer_id = NULL;
    uint32_t param = 0;
    osTimerDef(_my_timer, my_timer_callback);
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
    my_timer_id = osTimerCreate(osTimer(_my_timer), osTimerOnce, NULL);
    if (my_timer_id == NULL)
    {
        TRACE(0, "%s %d osTimerCreate osTimerOnce fail",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osTimerCreate osTimerOnce, id=%d",__func__,__LINE__,my_timer_id);
    if (osOK != osTimerStart(my_timer_id, 1000))
    {
        TRACE(0, "%s %d osTimerStart fail",__func__,__LINE__);
        return -1;
    }
    osDelay(3000);
    if (osOK != osTimerDelete(my_timer_id))
    {
        TRACE(0, "%s %d osTimerDelete fail",__func__,__LINE__);
        return -1;
    }

    param = 100;
    my_timer_id = osTimerCreate(osTimer(_my_timer), osTimerPeriodic, (void *)param);
    if (my_timer_id == NULL)
    {
        TRACE(0, "%s %d osTimerCreate osTimerPeriodic fail",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osTimerCreate osTimerPeriodic, id=%d",__func__,__LINE__,my_timer_id);
    if (osOK != osTimerStart(my_timer_id, 1000))
    {
        TRACE(0, "%s %d osTimerStart fail",__func__,__LINE__);
        return -1;
    }
    osDelay(5000);
    if (osOK != osTimerStop(my_timer_id))
    {
        TRACE(0, "%s %d osTimerStop fail",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osTimerStop done",__func__,__LINE__);
    osDelay(5000);
    if (osOK != osTimerStart(my_timer_id, 1000))
    {
        TRACE(0, "%s %d osTimerStart fail",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osTimerStart done",__func__,__LINE__);
    osDelay(5000);
    if (osOK != osTimerDelete(my_timer_id))
    {
        TRACE(0, "%s %d osTimerDelete fail",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s %d osTimerDelete done",__func__,__LINE__);
    _my_timer_tick = 0;
    osDelay(5000);
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return _my_timer_tick;
}

static int my_mutex_test(void)
{
    int period = 12;
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
    osMutexId my_mutex_id = NULL;
    osMutexDef(_my_mutex);
    my_mutex_id = osMutexCreate(osMutex(_my_mutex));
    if (my_mutex_id == NULL)
    {
        TRACE(0, "%s %d osMutexCreate fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexWait(my_mutex_id, osWaitForever))
    {
        TRACE(0, "%s %d osMutexWait fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexRelease(my_mutex_id))
    {
        TRACE(0, "%s %d osMutexRelease fail",__func__,__LINE__);
        return -1;
    }
    if (osOK != osMutexDelete(my_mutex_id))
    {
        TRACE(0, "%s %d osMutexDelete fail",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return 0;
}

static int mutex_thread_cnt = 0;
static osMutexId my_thread_mutex_id = NULL;
osMutexDef(_my_thread_mutex);
static int my_thread_mutex_create(void)
{
    my_thread_mutex_id = osMutexCreate(osMutex(_my_thread_mutex));
    if (my_thread_mutex_id == NULL)
    {
        TRACE( 0 , "%s %d osMutexCreate fail",__func__,__LINE__);
        return -1;
    }
    return 0;
}

static void mutex_thread2(const void *argument)
{
    int begin_time;
    int duration_time;
    int test_num = 0;
    osDelay(10);
    while(1)
    {
        osMutexRelease(my_thread_mutex_id);
        osDelay(500);
        begin_time = hal_sys_timer_get();
        osStatus_t ret = osMutexWait(my_thread_mutex_id, osWaitForever);
        duration_time = TICKS_TO_MS(hal_sys_timer_get() - begin_time);

        if (ret != osOK )
            TRACE( 0 , "[%s] mutex wait ERR", __FUNCTION__);

        TRACE( 0 , "[%s] test_num = %d mutex wait %d ms", __FUNCTION__, test_num, duration_time);
        test_num++;
        osDelay(3000);
        TRACE( 0 , "[%s] after delay 3000 ms osMutexRelease", __FUNCTION__);

        if (11 == test_num)
        {
            TRACE( 0 , "%s exit", __func__);
            break;
        }
    }
    mutex_thread_cnt--;
    osThreadExit();

}

static void mutex_thread1(const void *argument)
{
    int begin_time;
    int duration_time;
    int test_num = 0;
    while(1)
    {
        begin_time = hal_sys_timer_get();
        osStatus_t ret = osMutexWait(my_thread_mutex_id, osWaitForever);
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
    mutex_thread_cnt--;
    osThreadExit();
}

static int my_thread_mutex_test(void)
{
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
    int ret = my_thread_mutex_create();

    if(!ret){
        osThreadDef(mutex_thread1, osPriorityHigh, 1, (4*1024), "mutex_thread1");
        osThreadId mutex_thread_id1= osThreadCreate(osThread(mutex_thread1), NULL);
        if(mutex_thread_id1)
            mutex_thread_cnt++;
        osThreadDef(mutex_thread2, osPriorityAboveNormal, 1, (4*1024), "mutex_thread2");
        osThreadId mutex_thread_id2= osThreadCreate(osThread(mutex_thread2), NULL);
        if(mutex_thread_id2)
            mutex_thread_cnt++;
        while(mutex_thread_cnt){
            osDelay(10);
        }
    }
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return ret;
}

typedef struct _signal_param_tst
{
    osThreadId threadID;
    int32_t    signal_value;
}_signal_thread_t;

static void _my_signal_main(const void *argument)
{
    _signal_thread_t* _this = (_signal_thread_t *)argument;
    TRACE(0, "%s _this:%p,threadID: %p", __FUNCTION__, _this, _this->threadID);
    while (1)
    {
        osEvent evt;
        evt = osSignalWait(_this->signal_value, osWaitForever);
        if (evt.status == osEventSignal){
            TRACE( 0 , "\n<<= %s get signal=%u,_this->signal_value:%u", __FUNCTION__, evt.value.signals,_this->signal_value);
            osSignalClear(_this->threadID, evt.value.signals);
        } else {
            TRACE(0, "%s _this:0x%x, evt.status=0x%02x !!!!!", __FUNCTION__, _this, evt.status);
        }
        osDelay(50);
        if(_this->signal_value == (1 << (osFeature_Signals - 1)))
        {
            TRACE(0, "%s exit now!", __FUNCTION__);
            break;
        }
    }
    osThreadExit();
}

static int my_signal_test(void)
{
    int ret = 0;
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);

    _signal_thread_t signal_thread;
    osThreadDef(_my_signal_main, osPriorityAboveNormal, 1, (4*1024), "_my_signal_main");
    signal_thread.signal_value = 0;
    signal_thread.threadID = osThreadCreate(osThread(_my_signal_main), &signal_thread);
    if (signal_thread.threadID == NULL) {
        TRACE( 0 , "%s %d fail to create _my_signal_main task",__func__,__LINE__);
        return -1;
    }

    /* set last signal first, then iterate other signals*/
    osDelay(100);
    signal_thread.signal_value = osFeature_Signals-1;
    for (int i=0; i< osFeature_Signals; i++) {
        TRACE(0 , "\n[%d] =>> %s osSignalSet: %u", i, __FUNCTION__, signal_thread.signal_value);
        osSignalSet(signal_thread.threadID, signal_thread.signal_value);
        signal_thread.signal_value = (1 << i);
        osDelay(500);
    }
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return 0;
}

WEAK osStatus_t osThreadBindCpu(osThreadId_t thread_id, uint8_t cpu_id)
{
    TRACE(0, "%s WEAK", __FUNCTION__);
    return osOK;
}

WEAK  uint8_t osThreadGetCpu(void)
{
    TRACE(0, "%s WEAK", __FUNCTION__);
    return 0;
}

static void _my_thread_main1(const void *argument)
{
    TRACE(0, "%s osThreadGetId=%d ,cpu:%d", __FUNCTION__, osThreadGetId(), osThreadGetCpu());
    for (int i=0; i<20; i++)
    {
        osDelay(1000);
        if (i%3 == 0)
        {
            TRACE(0, "[%d]%s Yield... osThreadGetId=%d , cpu:%d", i, __FUNCTION__, osThreadGetId(), osThreadGetCpu());
            osThreadYield();
        }
        //TRACE(0, "%s id=%d ...", __FUNCTION__, osThreadGetId());
    }
    TRACE( 0 , "%s now exit osThreadGetId=%d", __FUNCTION__, osThreadGetId());
    osThreadExit();
}

static void _my_thread_main2(const void *argument)
{
    TRACE(0, "%s osThreadGetId=%d , cpu:%d", __FUNCTION__, osThreadGetId(), osThreadGetCpu());
    for (int i=0; i<20; i++)
    {
        osDelay(1000);
        if (i%3 == 0)
        {
            TRACE(0, "[%d] %s Yield... osThreadGetId=%d , cpu:%d", i, __FUNCTION__, osThreadGetId(), osThreadGetCpu());
            osThreadYield();
        }
        //TRACE(0, "%s id=%d ...", __FUNCTION__, osThreadGetId());
    }
    TRACE( 0 , "%s now exit osThreadGetId=%d", __FUNCTION__, osThreadGetId());
    osThreadExit();
}

static int my_thread_test(void)
{
    osThreadId my_thread_id1 = NULL;
    osThreadId my_thread_id2 = NULL;
    osThreadDef(_my_thread_main1, osPriorityAboveNormal, 1, (4*1024), "_my_thread_main1");
    osThreadDef(_my_thread_main2, osPriorityAboveNormal, 1, (4*1024), "_my_thread_main2");

    int period = 12;
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);

    my_thread_id1 = osThreadCreate(osThread(_my_thread_main1), NULL);
    if (my_thread_id1 == NULL) {
        TRACE(0, "%s %d fail to create _my_thread_main1 task",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s create _my_thread_main1 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id1);

    my_thread_id2 = osThreadCreate(osThread(_my_thread_main2), NULL);
    if (my_thread_id2 == NULL) {
        TRACE(0, "%s %d fail to create _my_thread_main2 task",__func__,__LINE__);
        return -1;
    }
    TRACE(0, "%s create _my_thread_main2 task = 0x%X", __FUNCTION__, (uint32_t)my_thread_id2);

    osDelay(2000);
    if (osOK != osThreadSetPriority(my_thread_id1, osPriorityNormal)) {
        TRACE(0, "%s osThreadSetPriority fail!", __FUNCTION__);
    }
    osDelay(2000);
    if (osPriorityNormal != osThreadGetPriority(my_thread_id1)) {
        TRACE(0, "%s osThreadGetPriority fail!", __FUNCTION__);
    }

    TRACE(0, "%s osThreadTerminate1 %d", __FUNCTION__, my_thread_id1);
    osThreadTerminate(my_thread_id1);

    osDelay(2000);
    TRACE(0, "%s osThreadTerminate2 %d", __FUNCTION__, my_thread_id2);
    osThreadTerminate(my_thread_id2);

    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return 0;
}

static int my_a7_delay_test(void)
{
    TRACE(0, "%s", __FUNCTION__);
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
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
            TRACE(0, "FAIL delay i=%dms, actually=%dms\n", i, time);
            fail = -1;
        }
    }
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return fail;
}

static int my_a7_print_test(void)
{
    TRACE(0, "\n\n%-32s testing...............................>\n", __FUNCTION__);
    TRACE(0, "%s", __FUNCTION__);

    TRACE(0, "223=%d", 223);
    TRACE(0, "-232=%d", -232);

    TRACE(0, "223.11=%f",223.11);
    TRACE(0, "232.11111111=%f", 232.11111111);
    TRACE(0, "-223.11=%f", -223.11);
    TRACE(0, "-232.11111111=%f", -232.11111111);

    TRACE(0, "223.11=%e", 223.11);
    TRACE(0, "232.11111111=%e", 232.11111111);
    TRACE(0, "-223.11=%e", -223.11);
    TRACE(0, "-232.11111111=%e", -232.11111111);

    TRACE(0, "223.11=%g", 223.11);
    TRACE(0, "232.111111111111=%g", 232.111111111111);
    TRACE(0, "-223.11=%g", -223.11);
    TRACE(0, "-232.111111111111=%g", -232.111111111111);

    TRACE(0, "223.11=%-10f", 223.11);
    TRACE(0, "232.11111111=%+10.5f", 232.11111111);

    int a=29;
    float b=1243.2341;
    double c=24212345.24232;
    long long int e=1234567890;
    TRACE(0, "a=29 d=%d\t,ld=%ld\t,o=%o\t,lld=%lld\t",a,a,a,a);
    TRACE(0, "b=1243.2341 f=%f\t,lf=%lf\t,5.4lf=%5.4lf\t,e=%e\t",b,b,b,b);
    TRACE(0, "c=24212345.24232 lf=%lf\t,f=%f\t,8.4lf=%8.4lf\t",c,c,c);
    TRACE(0, "e=1234567890 d=%d\t,ld=%ld\t,lld=%lld\t", e,e,e);
    TRACE(0, "\n\n<...............................%-32s test end\n", __FUNCTION__);
    return 0;
}

void a7_cmsis_test(void)
{

    my_a7_print_test();

    my_msg_test();

    my_mail_test();

    my_sem_test();

    my_timer_test();

    my_mutex_test();

    my_signal_test();

    my_a7_delay_test();

    my_thread_mutex_test();

    my_thread_test();

}
