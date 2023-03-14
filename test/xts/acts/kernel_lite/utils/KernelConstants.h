/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KERNEL_CONSTANTS_H
#define KERNEL_CONSTANTS_H

#include <unistd.h>

/**
 * ================ DAC and Caps ================
 */
const uid_t SHELL_UID = 2;
const uid_t SHELL_GID = 2;


/**
 * ================ Process Manager ================
 */
#define MAX_PROCESS_GROUPS  255  // max number of groups a process can have
#define MAX_PROCESS_NUMBER  63   // max allowd process [0,63]
#define MAX_TASK_NUMBER     128  // max allowd task(process+thread)

#define INIT_PROCESS_ID     1    // pid of init
#define KERNEL_PROCESS_ID   2    // pid of KProcess

#define HIGHEST_USER_PROCESS_PRIORITY    10
#define LOWEST_USER_PROCESS_PRIORITY     31
#define HIGHEST_USER_THREAD_PRIORITY     0
#define LOWEST_USER_THREAD_PRIORITY      31

#define DEFAULT_SHELL_PROCESS_PRIORITY   15
#define DEFAULT_INIT_PROCESS_PRIORITY    28
#define DEFAULT_KERNEL_PROCESS_PRIORITY  0
#define DEFAULT_THREAD_PRIORITY          25
#define DEFAULT_RR_SCHED_INTERVAL        5000000    // defalult sched interval of RR, in ms

/**
 * ================ Memory Manager ================
 */
typedef unsigned long addr_t;

#define USER_ASPACE_BASE            ((addr_t)0x01000000UL)
#define USER_ASPACE_TOP_MAX         ((addr_t)0x3FFFFFFFUL)
#define USER_ASPACE_SIZE            ((addr_t)(USER_ASPACE_TOP_MAX - USER_ASPACE_BASE))
#define USER_HEAP_BASE              ((addr_t)(USER_ASPACE_TOP_MAX >> 2))
#define USER_MAP_BASE               ((addr_t)(USER_ASPACE_TOP_MAX >> 1))
#define USER_MAP_SIZE               ((addr_t)(USER_ASPACE_SIZE >> 3))

#ifndef PAGE_SIZE
#define PAGE_SIZE                   0x1000U
#endif
#define PAGE_MASK                   (~(PAGE_SIZE - 1))
#define PAGE_SHIFT                  12

#define MEM_PAGESTART(addr)         ((addr) & ~(PAGE_SIZE - 1))
#define MEM_PAGEOFFSET(addr)        ((addr) & (PAGE_SIZE - 1))
#define MEM_PAGEALIGN(addr)         (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))


/**
 * ================ IPC ================
 */
const int MAX_SIGNAL_NUMBER = 64;   // max number of allowed signal, [1,64]
const int MAX_PIPE_BUFFER = 1023;   // max size of a pipe buffer
const int MAX_PIPE_NUMBER = 32;     // max pipe number

const int MAX_MQ_NUMBER   = 256;   // max mqueue number
const int MAX_MQ_NAME_LEN = 2560;    // max mqueue name length
const int MAX_MQ_MSG_SIZE = 65530;  // max mqueue message size


/**
 * ================ FS ================
 */
const int MAX_PATH_SIZE = 256;      // max size of path string

/**
 * ================ SYSTEM ================
 */
#define SYSINFO_SYSNAME "Huawei LiteOS"   // sys name from 'uname'

/**
 * ================ XTS ================
 */
#define RES_DIR_KERNEL "/test_root/kernel/"   // top dir of test resource of kernel
#define WRITABLE_TEST_DIR "/storage/"         // writable dir for test file

#endif
