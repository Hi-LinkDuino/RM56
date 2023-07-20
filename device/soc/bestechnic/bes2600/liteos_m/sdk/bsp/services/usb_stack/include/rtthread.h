/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-18     Bernard      the first version
 * 2006-04-26     Bernard      add semaphore APIs
 * 2006-08-10     Bernard      add version information
 * 2007-01-28     Bernard      rename RT_OBJECT_Class_Static to RT_Object_Class_Static
 * 2007-03-03     Bernard      clean up the definitions to rtdef.h
 * 2010-04-11     yi.qiu       add module feature
 * 2013-06-24     Bernard      add rt_kprintf re-define when not use RT_USING_CONSOLE.
 * 2016-08-09     ArdaFu       add new thread and interrupt hook.
 * 2018-11-22     Jesven       add all cpu's lock and ipi handler
 * 2021-02-28     Meco Man     add RT_KSERVICE_USING_STDLIB
 */

#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

#include <rtconfig.h>
#include <rtdebug.h>
#include <rtdef.h>
#include <rtservice.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "cmsis.h"
#include "cmsis_os2.h"
#include "hal_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTM_EXPORT(symbol)
#define rt_hw_interrupt_disable int_lock
#define rt_hw_interrupt_enable int_unlock

/**
 * @addtogroup KernelObject
 */

/**@{*/

/*
 * kernel object interface
 */
struct rt_object_information *
rt_object_get_information(enum rt_object_class_type type);
int rt_object_get_length(enum rt_object_class_type type);
int rt_object_get_pointers(enum rt_object_class_type type, rt_object_t *pointers, int maxlen);

void rt_object_init(struct rt_object         *object,
                    enum rt_object_class_type type,
                    const char               *name);
void rt_object_detach(rt_object_t object);
rt_object_t rt_object_allocate(enum rt_object_class_type type,
                               const char               *name);
void rt_object_delete(rt_object_t object);
rt_bool_t rt_object_is_systemobject(rt_object_t object);
rt_uint8_t rt_object_get_type(rt_object_t object);
rt_object_t rt_object_find(const char *name, rt_uint8_t type);

#ifdef RT_USING_HOOK
void rt_object_attach_sethook(void (*hook)(struct rt_object *object));
void rt_object_detach_sethook(void (*hook)(struct rt_object *object));
void rt_object_trytake_sethook(void (*hook)(struct rt_object *object));
void rt_object_take_sethook(void (*hook)(struct rt_object *object));
void rt_object_put_sethook(void (*hook)(struct rt_object *object));
#endif

/**@}*/


/**
 * @addtogroup Thread
 */

/**@{*/

/*
 * thread interface
 */
#define rt_thread_self()    osThreadGetId()
#define rt_thread_delay     osDelay

/*
 * schedule service
 */
#define rt_enter_critical() osKernelLock()
#define rt_exit_critical()  osKernelUnlock()

/**@}*/

#ifdef RT_USING_HEAP
#define rt_malloc   malloc
#define rt_free     free
#define rt_realloc  realloc
#define rt_calloc   calloc
#endif

#ifdef RT_USING_DEVICE
/**
 * @addtogroup Device
 */

/**@{*/

/*
 * device (I/O) system interface
 */
rt_device_t rt_device_find(const char *name);

rt_err_t rt_device_register(rt_device_t dev,
                            const char *name,
                            rt_uint16_t flags);
rt_err_t rt_device_unregister(rt_device_t dev);

rt_device_t rt_device_create(int type, int attach_size);
void rt_device_destroy(rt_device_t device);

rt_err_t
rt_device_set_rx_indicate(rt_device_t dev,
                          rt_err_t (*rx_ind)(rt_device_t dev, rt_size_t size));
rt_err_t
rt_device_set_tx_complete(rt_device_t dev,
                          rt_err_t (*tx_done)(rt_device_t dev, void *buffer));

rt_err_t  rt_device_init (rt_device_t dev);
rt_err_t  rt_device_open (rt_device_t dev, rt_uint16_t oflag);
rt_err_t  rt_device_close(rt_device_t dev);
rt_size_t rt_device_read (rt_device_t dev,
                          rt_off_t    pos,
                          void       *buffer,
                          rt_size_t   size);
rt_size_t rt_device_write(rt_device_t dev,
                          rt_off_t    pos,
                          const void *buffer,
                          rt_size_t   size);
rt_err_t  rt_device_control(rt_device_t dev, int cmd, void *arg);

/**@}*/
#endif

/**
 * @addtogroup KernelService
 */

/**@{*/

/*
 * general kernel service
 */
#ifdef RT_USING_CONSOLE
#define rt_kprintf printf
#else
#define rt_kprintf(...)
#endif

void rt_set_errno(rt_err_t no);

#define rt_memset(src, c, n)        memset(src, c, n)
#define rt_memcpy(dest, src, n)     memcpy(dest, src, n)
#define rt_memmove(dest, src, n)    memmove(dest, src, n)
#define rt_memcmp(cs, ct, count)    memcmp(cs, ct, count)
#define rt_strstr(str1, str2)       strstr(str1, str2)
#define rt_strcasecmp(a, b)         strcasecmp(a, b)
#define rt_strncpy(dest, src, n)    strncpy(dest, src, n)
#define rt_strncmp(cs, ct, count)   strncmp(cs, ct, count)
#define rt_strcmp(cs, ct)           strcmp(cs, ct)
#define rt_strnlen(s, maxlen)       strnlen(s, maxlen)
#define rt_strlen(src)              strlen(src)

void rt_show_version(void);

#ifdef RT_DEBUG
extern void (*rt_assert_hook)(const char *ex, const char *func, rt_size_t line);
void rt_assert_set_hook(void (*hook)(const char *ex, const char *func, rt_size_t line));

void rt_assert_handler(const char *ex, const char *func, rt_size_t line);

rt_inline void LOG_HEX(const char *name, const void *buf, rt_uint32_t size)
{
    if (!buf || !size) {
        return;
    }
    if (name) {
        rt_kprintf("%s(%d):", name, size);
    }
    for (rt_uint32_t i = 0; i < size; ++i) {
        rt_kprintf("%02X ", *((const rt_uint8_t *)buf + i));
    }
    rt_kprintf("\n");
}
#endif /* RT_DEBUG */

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
