/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

/**
 * @addtogroup ohos_mem_pool
 * @{
 *
 * @brief Provides functions for memory alloc and memory free.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ohos_mem_pool.h
 *
 * @brief Provides functions for memory alloc and memory free.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_MEM_POOL_H
#define OHOS_MEM_POOL_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/**
 * @brief memory pool enum defination.
 */
typedef enum {
    MEM_TYPE_BEGIN = 100,
    MEM_TYPE_UIKIT = MEM_TYPE_BEGIN, /* UIKIT memory pool */
    MEM_TYPE_UIKIT_LSRAM,  /* UIKIT low speed memory pool */
    MEM_TYPE_APPFMK,       /* appexecfmk memory pool */
    MEM_TYPE_APPFMK_LSRAM, /* appexecfmk low speed memory pool */
    MEM_TYPE_ACE,          /* ACE memory pool */
    MEM_TYPE_ACE_LSRAM,    /* ACE low speed memory pool */
    MEM_TYPE_JERRY,        /* jerry script memory pool */
    MEM_TYPE_JERRY_LSRAM,  /* jerry script low speed memory pool */
    MEM_TYPE_HICHAIN,      /* security memory pool */
    MEM_TYPE_SOFTBUS_LSRAM,  /* softbus low speed memory pool */

    MEM_TYPE_MAX           /* maximum defination of memory ,add a new memory pool before it */
} MemType;

/**
 * @brief According to the input parameters type and size,
 * memory is applied in the corresponding memory pool.
 *
 * Because memory management needs to consume memory,
 * it is not recommended to use the memory application interface to apply for memory
 * if the application memory is small (less than 8 bytes).
 * Applying for small memory blocks is easy to cause memory fragmentation.
 *
 * @param type identify which memory pool to apply for memory in
 * @param size size of memory block to apply for
 * @return requested memory block address
 * return if the memory request fails <b>NULL</b>.
 * @since 1.0.
 * @version 1.0.
 */
void *OhosMalloc(MemType type, uint32 size);

/**
 * @brief free incoming memory space.
 *
 * @param ptr identify the memory blocks that need to be released.
 * @attention Do not release null pointers or memory blocks not requested by OhosMalloc!
 * @return void
 * @since 1.0.
 * @version 1.0.
 */
void OhosFree(void *ptr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OHOS_MEM_POOL_H */
/** @} */