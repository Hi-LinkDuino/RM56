#include "sys_arch.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "api.h"
#include "net_timer.h"
#include "net_debug.h"
#include <stdarg.h>

// #include "mbed_error.h"
#include "hal_trace.h"

//void error(const char* format, ...);

#define error(format, ...)              ASSERT(false, format, ##__VA_ARGS__)

#if 0
void error(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
    exit(1);
}
#endif

osMutexId lwip_sys_mutex;
osMutexDef(lwip_sys_mutex);

void sys_init(void) {
    lwip_sys_mutex = osMutexCreate(osMutex(lwip_sys_mutex));
    if (lwip_sys_mutex == NULL)
        error("sys_init error\n");
}

u32_t sys_jiffies(void)
{
    return jiffies;
}

u32_t sys_now(void)
{
    uint32_t ticks = sys_jiffies();
    //return (MSEC_PER_SEC * ticks / NET_HZ);
    return ticks;
}

void *tcpip_malloc(size_t size)
{
   return lwip_sys_malloc(size);
}

void tcpip_free(void * mem)
{
    lwip_sys_free(mem);
}

err_t sys_sem_new(sys_sem_t *sem,u8_t count)
{
    osSemaphoreDef_t osSemaphoreDef_tmp;

    memset(&osSemaphoreDef_tmp, 0, sizeof(osSemaphoreDef_tmp));
#ifdef CMSIS_OS_RTX
    memset(sem, 0, sizeof(*sem));
#if (osCMSIS < 0x20000U)
    osSemaphoreDef_tmp.semaphore = sem->os_semaphore_name;
#else
    memset(&osSemaphoreDef_tmp, 0, sizeof(osSemaphoreDef_t));
#endif
#endif
    sem->sem_id = osSemaphoreCreate(&osSemaphoreDef_tmp,(INT32)count);
    if(sem->sem_id == NULL){
        return ERR_VAL;
    }
    return ERR_OK;
}

int sys_sem_valid(sys_sem_t *sem)
{
    if(sem->sem_id == NULL)
        return 0;
    return 1;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    sem->sem_id = NULL;
}

/*
;*****************************************************************************************************
;* �������� : sys_sem_free
;* ��    �� : ɾ��һ���ź�
;* �䡡  �� : sem: �źž��
;*
;* �䡡  �� : ��
;*****************************************************************************************************
;*/
void sys_sem_free(sys_sem_t *sem)
{
    osSemaphoreDelete(sem->sem_id);
    sem->sem_id = NULL;
}

/*
;*****************************************************************************************************
;* �������� : sys_sem_signal
;* ��    �� : ����һ���ź�
;* �䡡  �� : sem: �źž��
;*
;* �䡡  �� : ��
;*****************************************************************************************************
;*/
void sys_sem_signal(sys_sem_t *sem)
{
   // printk(KERN_DEBUG, "sys_sem_signal sem_id = 0x%x \n", sem->sem_id);
    osSemaphoreRelease(sem->sem_id);
}


/*
;*****************************************************************************************************
;* �������� : sys_arch_sem_wait
;* ��    �� : �ȴ�һ���ź�
;* �䡡  �� : sem: �źž��, timeout: �ȴ���ʱ��΢����
;*
;* �䡡  �� : �ȴ����õ�΢����
;*****************************************************************************************************
;*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    uint32_t wait_ms = sys_now();
    uint32_t ret = 0;

    if(timeout == 0)
        timeout = osWaitForever;
   // printk(KERN_DEBUG, "sys_arch_sem_wait sem_id = 0x%x \n", sem->sem_id);

    osSemaphoreWait(sem->sem_id, timeout);
    ret = sys_now() - wait_ms;
    return ret;
}
osMutexId osMutexCreateEx (osMutexDef_t *mutex_def,uint32 caller) ;

err_t sys_mutex_new(sys_mutex_t *mutex)
{
    osMutexDef_t mutex_def;

    memset(&mutex_def, 0, sizeof(mutex_def));

#ifdef CMSIS_OS_RTX
    memset(mutex->os_mutex_cb, 0, sizeof(mutex->os_mutex_cb));
#if (osCMSIS < 0x20000U)
    mutex_def.mutex = mutex->os_mutex_cb;
#else
    mutex_def.name = NULL;
    mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
    mutex_def.cb_mem = NULL;
    mutex_def.cb_size = 0;
#endif
#endif

#ifdef osCMSIS_FreeRTOS
    mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit;
#endif

    mutex->mutex_id = osMutexCreate(&mutex_def);
    //mutex->mutex_id = osMutexCreateEx(&mutex_def,__func__);
    if(mutex->mutex_id  > 0)
        return ERR_OK;
    else
        return ERR_VAL;
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    osMutexWait(mutex->mutex_id, osWaitForever);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    osMutexRelease(mutex->mutex_id);
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    osMutexDelete(mutex->mutex_id);
    mutex->mutex_id = NULL;
}

int sys_mutex_valid(sys_mutex_t *mutex)
{
    if(mutex->mutex_id > 0)
        return 1;
    else
        return 0;
}

void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
    mutex->mutex_id = NULL;
}

/*
;*****************************************************************************************************
;* �������� : sys_mbox_new
;* ��    �� : ����һ������
;* �䡡  �� : size: ��������
;*
;* �䡡  �� : sys_mbox_t: ������
;*****************************************************************************************************
;*/
err_t sys_mbox_new(sys_mbox_t *pMbox, int size)
{
    err_t ret = ERR_OK;
    int real_size = max(size, MAX_MSG_IN_LWIP_MBOX);

    struct lwip_message_queue* p_mbox;
    osMessageQDef_t * queue_def;
    uint32_t r_size = 0;

#if defined(CMSIS_OS_RTX) && (osCMSIS < 0x20000U)
    r_size = sizeof(struct lwip_message_queue) + (real_size + 4) * sizeof(u32_t);
#else
    r_size = sizeof(struct lwip_message_queue);
#endif
    p_mbox = (struct lwip_message_queue*)lwip_sys_malloc(r_size);
    if (!p_mbox) {
        ret = ERR_MEM;
        *pMbox = NULL;
        printk(KERN_ERR, "sys_mbox_new no memory \n" );
        return ret;
    }
    memset(p_mbox, 0, r_size);
    queue_def = &p_mbox->queue_def;
    p_mbox->queue_sz = real_size;
#if defined(CMSIS_OS_RTX) && (osCMSIS < 0x20000U)
    queue_def->pool = &p_mbox->pool;
#endif

    queue_def->queue_sz = real_size;
    p_mbox->msg_id = osMessageCreate(queue_def, NULL);
    //printk(KERN_DEBUG, "sys_mbox_new pool = 0x%x id = 0x%x\n", &p_mbox->pool, p_mbox->msg_id);
    if (p_mbox->msg_id == NULL){
        ret = ERR_MEM;
        lwip_sys_free(p_mbox);
        printk(KERN_ERR, "sys_mbox_new msg_id failed p_mbox = %p\n", p_mbox);
        return ret;
    }
    *pMbox = p_mbox;
    //printk(KERN_DEBUG, "sys_mbox_new p_mbox = 0x%x  msg_id = 0x%x \n",p_mbox,  p_mbox->msg_id);
    return ret;
}

int sys_mbox_valid(sys_mbox_t *pMbox)
{
    struct lwip_message_queue* p_mbox;
    p_mbox = *pMbox;
    if(p_mbox == NULL || p_mbox->msg_id == NULL)
        return 0;
    return 1;
}

void sys_mbox_set_invalid(sys_mbox_t *pMbox)
{
    /*struct lwip_message_queue* p_mbox;
    p_mbox = *pMbox;
    if(p_mbox)
        p_mbox->msg_id = NULL;*/
    *pMbox = SYS_MBOX_NULL;
}
/*
;*****************************************************************************************************
;* �������� : sys_mbox_post
;* ��    �� : �����ʼ�������
;* �䡡  �� : mbox: ������, msg: �ʼ�
;*
;* �䡡  �� : ��
;*****************************************************************************************************
;*/
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    osStatus status;
    struct lwip_message_queue* p_mbox;
    p_mbox = *mbox;

    status = osMessagePut(p_mbox->msg_id, (uint32_t)msg, osWaitForever);
    if (status != osOK) {
        printk(KERN_ERR, "sys_mbox_post failed status = %d\n", status);
        if (msg)
            lwip_sys_free(msg);
    }
}

/*
;*****************************************************************************************************
;* �������� : sys_mbox_trypost
;* ��    �� : ���Է����ʼ�������
;* �䡡  �� : mbox: ������, msg: �ʼ�
;*
;* �䡡  �� : ERROR: ERR_MEM | OK: ERR_OK
;*****************************************************************************************************
;*/

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    struct lwip_message_queue* p_mbox;
    p_mbox = *mbox;
    err_t ret = ERR_OK;

    if (osMessagePut(p_mbox->msg_id, (uint32_t)msg, 0) != osOK) {
        printk(KERN_WARN, "sys_mbox_trypost failed");
        ret = ERR_MEM;
    }
    return ret;
}
/*
;*****************************************************************************************************
;* �������� : sys_arch_mbox_fetch
;* ��    �� : ������ȴ�һ���ʼ�
;* �䡡  �� : mbox: ������, msg: �ʼ�, timeout: �ȴ���ʱ��΢����
;*
;* �䡡  �� : �ȴ����õ�΢����
;*****************************************************************************************************
;*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    struct lwip_message_queue* p_mbox;
    p_mbox = *mbox;
    u32_t wait_ms = sys_now();
    osEvent event;
    int ret = 0;

    if(timeout == 0)
        timeout = osWaitForever;
    event = osMessageGet(p_mbox->msg_id, timeout);
    if(event.status != osEventMessage){
        *msg = NULL;
        ret = SYS_ARCH_TIMEOUT;
    }else{
       *msg = event.value.p;
       ret = sys_now() - wait_ms;
    }

    //printk(KERN_DEBUG, "sys_arch_mbox_fetch msg = 0x%x timeout = 0x%x status =  0x%x\n", *msg, timeout, event.status);
    return ret;
}

/*
;*****************************************************************************************************
;* �������� : sys_arch_mbox_tryfetch
;* ��    �� : ���Դ�����ȴ�һ���ʼ�
;* �䡡  �� : mbox: ������, msg: �ʼ�
;*
;* �䡡  �� : ERROR: SYS_MBOX_EMPTY | OK: 0
;*****************************************************************************************************
;*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    struct lwip_message_queue* p_mbox;
    p_mbox = *mbox;
    osEvent event;
    int ret = 0;

    event = osMessageGet(p_mbox->msg_id, 0);
    if(event.status != osEventMessage){
        *msg = NULL;
        ret = SYS_MBOX_EMPTY;
    }else{
       *msg = event.value.p;
    }
    //printk(KERN_DEBUG, "sys_arch_mbox_fetch msg = 0x%x \n", *msg);
    return ret;
}

/*
;*****************************************************************************************************
;* �������� : sys_mbox_free
;* ��    �� : ɾ��һ������
;* �䡡  �� : mbox: ������
;*
;* �䡡  �� : ��
;*****************************************************************************************************
;*/
void sys_mbox_free(sys_mbox_t *mbox)
{
    struct lwip_message_queue* p_mbox;
    p_mbox = *mbox;

    if (p_mbox) {
        osMessageQueueDelete(p_mbox->msg_id);
        p_mbox->msg_id = 0;
        lwip_sys_free(p_mbox);
        *mbox = NULL;
    }
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    osThreadId    thread_id;

    if(stacksize <= 0)
        stacksize = 512;

#ifdef CMSIS_OS_RTX
    struct lwip_sys_thread *p_thread;
#ifdef __RTX_CPU_STATISTICS__
#endif
#if (osCMSIS < 0x20000U)
    p_thread = ( struct lwip_sys_thread *)lwip_sys_malloc(stacksize + sizeof(struct lwip_sys_thread) + 3);
    ASSERT(p_thread, "MALLOC sys_arch thread fail");
    p_thread->thread_def.pthread = (os_pthread)thread;
    if (name)
        p_thread->thread_def.name_str = name;
    else
        p_thread->thread_def.name_str = "sys_arch";
    p_thread->thread_def.stacksize = stacksize;
    p_thread->thread_def.tpriority = prio;
    p_thread->thread_def.stack_pointer = (uint32_t*)(p_thread->stack);
#else
    stacksize = 8*((stacksize+7)/8);
    p_thread = ( struct lwip_sys_thread *)lwip_sys_malloc(stacksize + sizeof(struct lwip_sys_thread) + 8);
    ASSERT(p_thread, "MALLOC sys_arch thread fail");
    memset(p_thread, 0, sizeof(*p_thread));
    p_thread->stack = &p_thread->stack;
    p_thread->stack = (void *)(((uint32_t)p_thread->stack + 7) & (~0x7));
    p_thread->thread_def.pthread = (os_pthread)thread;
    memset(&p_thread->thread_def.attr, 0, sizeof(osThreadAttr_t));
    if (name)
        p_thread->thread_def.attr.name = name;
    else
        p_thread->thread_def.attr.name = "sys_arch";
    p_thread->thread_def.attr.stack_size = stacksize;
    p_thread->thread_def.attr.priority = prio;
    p_thread->thread_def.attr.stack_mem = p_thread->stack;
    p_thread->thread_def.attr.attr_bits = osThreadDetached;
    LOG_NET("p_thread:%p, stac:%p", p_thread, p_thread->stack);
#endif
    thread_id = osThreadCreate(&p_thread->thread_def, arg);
#endif
#ifdef osCMSIS_FreeRTOS
    osThreadDef_t thread_def;
    thread_def.pthread = (os_pthread)thread;
    memset((void *)&thread_def.attr, 0, sizeof(thread_def.attr));
    thread_def.attr.name = name;
    thread_def.attr.stack_size = 8*((stacksize+7)/8);
    thread_def.attr.priority =  prio;
    thread_id = osThreadCreate(&thread_def, arg);
#endif

#ifdef osCMSIS_RHINO
    osThreadDef_t thread_def;
    memset(&thread_def, 0, sizeof(osThreadDef_t));
    if (name)
        thread_def.attr.name = (char *)name;
    else
        thread_def.attr.name = "sys_arch";
    thread_def.pthread = (os_pthread)thread;
    thread_def.attr.stack_size = stacksize;
    thread_def.attr.priority = prio;
    thread_id = osThreadCreate(&thread_def, arg);
#endif

#ifdef osCMSIS_RTT
    osThreadDef_t thread_def;
    thread_def.pthread = (os_pthread)thread;
    memset((void *)&thread_def.attr, 0, sizeof(thread_def.attr));
    if (name)
        thread_def.attr.name = (char *)name;
    else
        thread_def.attr.name = "sys_arch";
    thread_def.attr.stack_size = 8*((stacksize+7)/8);
    thread_def.attr.priority =  prio;
    thread_id = osThreadCreate(&thread_def, arg);
#endif

#ifdef KERNEL_LITEOS_M
    osThreadDef_t thread_def = {0};
    thread_def.pthread         = (os_pthread)thread;
    thread_def.attr.name       = name == NULL ? "sys_arch" : name;
    thread_def.attr.stack_size = 8*((stacksize+7)/8);
    thread_def.attr.priority   = prio;

    thread_id = osThreadCreate(&thread_def, arg);
#endif

    if (thread_id == NULL){
#ifdef CMSIS_OS_RTX
        lwip_sys_free(p_thread);
#endif
        printk(KERN_ERR, "sys_thread_new failed\r\n");
        return NULL;
    }
    return thread_id;
}

void sys_thread_exit(void)
{
#if LWIP_NETCONN_SEM_PER_THREAD
    netconn_thread_cleanup();
#endif

    osThreadExit();
}

#if LWIP_NETCONN_SEM_PER_THREAD
#define _LWIP_SYS_SEM_THREADS_NUM 16
struct _sys_sem_thread_t{
    int thread_id;
    sys_sem_t sem;
}_lwip_sys_sem_threads[_LWIP_SYS_SEM_THREADS_NUM];

sys_sem_t *LWIP_NETCONN_THREAD_SEM_ALLOC(void)
{
    sys_sem_t *sem = NULL;
    for (int i=0; i<_LWIP_SYS_SEM_THREADS_NUM; i++)
    {
        if (_lwip_sys_sem_threads[i].thread_id == 0)
        {
            _lwip_sys_sem_threads[i].thread_id = osGetThreadIntId();
            sem = &_lwip_sys_sem_threads[i].sem;
            sys_sem_new(sem, 0);
            printk(KERN_DEBUG, "%s thread %d sem=0x%X", __FUNCTION__, _lwip_sys_sem_threads[i].thread_id, sem);
            break;
        }
    }

    return sem;
}

sys_sem_t *LWIP_NETCONN_THREAD_SEM_GET(void)
{
    sys_sem_t *sem = NULL;
    for (int i=0; i<_LWIP_SYS_SEM_THREADS_NUM; i++)
    {
        if (_lwip_sys_sem_threads[i].thread_id == osGetThreadIntId())
        {
            sem = &_lwip_sys_sem_threads[i].sem;
            break;
        }
    }

    if (sem == NULL)
    {
        printk(KERN_ERR, "%s thread %d FAIL, now do init!!!!!!!!!!!!!", __FUNCTION__, osGetThreadIntId());
        sem = LWIP_NETCONN_THREAD_SEM_ALLOC();
    }

    return sem;
}

void LWIP_NETCONN_THREAD_SEM_FREE(void)
{
    sys_sem_t *sem = NULL;
    for (int i=0; i<_LWIP_SYS_SEM_THREADS_NUM; i++)
    {
        if (_lwip_sys_sem_threads[i].thread_id == osGetThreadIntId())
        {
            sem = &_lwip_sys_sem_threads[i].sem;
            printk(KERN_DEBUG, "%s thread %d sem=0x%X", __FUNCTION__, _lwip_sys_sem_threads[i].thread_id, sem);
            _lwip_sys_sem_threads[i].thread_id = 0;
            sys_sem_free(sem);
            break;
        }
    }

    return sem;
}
#endif

uint32_t get_random(void)
{
    uint32_t seed = hal_sys_timer_get();
    seed = seed * 1103515245 + 12345;
    return ( (unsigned int) (seed / 65536) % (RAND_MAX) );
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_protect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      operating system.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
sys_prot_t sys_arch_protect(void) {
    if (osMutexWait(lwip_sys_mutex, osWaitForever) != osOK)
        error("sys_arch_protect error\n");
    return (sys_prot_t) 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_unprotect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an operating system.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
void sys_arch_unprotect(sys_prot_t p) {
    if (osMutexRelease(lwip_sys_mutex) != osOK)
        error("sys_arch_unprotect error\n");
}


