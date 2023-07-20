#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__
#include "cmsis_os.h"
#include "net_defs.h"
#include "lwip/err.h"
#include "hal_trace.h"

extern void *nts_malloc(size_t size);
extern void nts_free(void *rmem);
#if 0
#define lwip_sys_malloc(size)({\
	unsigned char *ret = NULL;\
	ret = nts_malloc(size);\
	LOG_NET("sys_malloc: %s %d %s.\n", __func__, __LINE__, ret ? "ok" : "fail");\
	ret;\
	})

#else
#define lwip_sys_malloc(size)({\
	unsigned char *ret = NULL;\
	ret = malloc(size);\
	LOG_NET("sys_malloc: %s %d %s.\n", __func__, __LINE__, ret ? "ok" : "fail");\
	ret;\
	})
#endif
#define lwip_sys_free   free
#define MAX_MSG_IN_LWIP_MBOX 50

struct lwip_sem {
    osSemaphoreId sem_id;
    uint32_t os_semaphore_name[2];
};
typedef struct lwip_sem sys_sem_t;

struct lwip_message_queue {
  osMessageQDef_t queue_def;
  osMessageQId            msg_id;
  uint32_t                queue_sz;    ///< number of elements in the queue
#if defined(CMSIS_OS_RTX) && (osCMSIS < 0x20000U)
  void                    *pool;    ///< memory array for messages
#endif
};
typedef struct lwip_message_queue* sys_mbox_t;

struct lwip_mutex {
    osMutexId mutex_id;
    uint32_t  os_mutex_cb[3];
};
typedef struct lwip_mutex sys_mutex_t;

#ifdef CMSIS_OS_RTX
struct lwip_sys_thread {
    osThreadDef_t thread_def;
    void*         stack;
};
//typedef struct lwip_sys_thread  *sys_thread_t;
#endif

//#ifdef osCMSIS_FreeRTOS
//typedef osThreadId sys_thread_t;
//#endif
typedef osThreadId sys_thread_t;


#ifndef NULL
#define NULL 0
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define SYS_MBOX_NULL  (sys_mbox_t)NULL
#define SYS_SEM_NULL   (sys_sem_t)NULL
#define SYS_MUTEX_NULL   (sys_mutex_t)NULL

err_t sys_sem_new(sys_sem_t *sem, u8_t count);
void sys_sem_free(sys_sem_t *sem);
void sys_sem_signal(sys_sem_t *sem);

#endif /* __SYS_ARCH_H__ */


