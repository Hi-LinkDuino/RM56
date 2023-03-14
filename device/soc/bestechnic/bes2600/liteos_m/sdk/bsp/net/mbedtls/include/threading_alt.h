/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __THREADING_ALT_H__
#define __THREADING_ALT_H__
#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MBEDTLS_CMSIS_FILE)
#include "cmsis_os.h"
#else
#include MBEDTLS_CMSIS_FILE
#endif

typedef struct
{
    osMutexId mutex;
    char is_valid;
} mbedtls_threading_mutex_t;

void mbedtls_threading_set_alt( void ( * mutex_init )( mbedtls_threading_mutex_t * ),
                                       void ( * mutex_free )( mbedtls_threading_mutex_t * ),
                                       int ( * mutex_lock )( mbedtls_threading_mutex_t * ),
                                       int ( * mutex_unlock )( mbedtls_threading_mutex_t * ) );

void mbedtls_threading_free_alt( void );

void mbedtlsThreading_MutexInit(mbedtls_threading_mutex_t* mbedtlsMutex );
void mbedtlsThreading_MutexFree(mbedtls_threading_mutex_t* mbedtlsMutex );
int mbedtlsThreading_MutexLock(mbedtls_threading_mutex_t* mbedtlsMutex );
int mbedtlsThreading_MutexUnlock(mbedtls_threading_mutex_t* mbedtlsMutex );

#ifdef __cplusplus
}
#endif
#endif /* ifndef __THREADING_ALT_H__ */
