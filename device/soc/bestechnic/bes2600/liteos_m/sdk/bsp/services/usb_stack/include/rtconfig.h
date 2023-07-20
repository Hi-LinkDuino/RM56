#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 32
#ifdef __ARM_ARCH_ISA_ARM
#define RT_USING_BES
#define RT_USING_FPU
#define RT_USING_SMP
#define RT_CPUS_NR 2
#else
#define RT_CPUS_NR 1
#endif
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 4096
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 4096
#define RT_DEBUG
#define RT_DEBUG_COLOR
#define RT_DEBUG_INIT_CONFIG
#define RT_DEBUG_INIT 1

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_MEMHEAP_AS_HEAP
#define RT_USING_MEMTRACE
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
// #define RT_USING_DEVICE_OPS
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 512

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 1024 * 64
#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */

#define RT_USING_CPLUSPLUS

/* Command shell */

/* Device virtual file system */


/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_PIN
#define RT_USING_SPI
#define RT_USING_I2C
#define RT_USING_AUDIO
#define RT_AUDIO_REPLAY_MP_BLOCK_SIZE 4096
#define RT_AUDIO_REPLAY_MP_BLOCK_COUNT 2
#define RT_AUDIO_RECORD_PIPE_SIZE 2048

/* Using USB */
// #define RT_USBH_RNDIS
#define RT_USBH_ECM
// #define RT_USBH_MSTORAGE

/* POSIX layer and C standard library */
// #define RT_USING_LIBC

/* Network */

/* Socket abstraction layer */


/* Network interface device */

/* light weight TCP/IP stack */
#define RT_USING_LWIP

/* Static IPv4 Address */

/* AT commands */

#define RT_USING_AT
#define AT_DEBUG
#define AT_SW_VERSION_NUM 0x10301

/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */


/* IoT Cloud */


/* security packages */


/* language packages */


/* multimedia packages */


/* tools packages */


/* system packages */


/* Micrium: Micrium software products porting for RT-Thread */


/* peripheral libraries and drivers */


/* miscellaneous packages */


/* samples: kernel and components samples */


/* games: games run on RT-Thread console */

#endif
