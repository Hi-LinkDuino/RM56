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
#ifndef THREADING_ALT_H
#define THREADING_ALT_H

/* Mutex protection for multi threaded mbedtls */
#include "cmsis_os.h"

typedef struct
{
    uint32_t para[3];
    osMutexDef_t m;
    osMutexId id;
} mbedtls_threading_mutex_t;

void mbedtls_client_mutex_init(mbedtls_threading_mutex_t *mutex);
void mbedtls_client_mutex_free(mbedtls_threading_mutex_t *mutex);
int mbedtls_client_mutex_lock(mbedtls_threading_mutex_t *mutex);
int mbedtls_client_mutex_unlock(mbedtls_threading_mutex_t *mutex);

void mbedtls_threading_set_alt( void (*mutex_init)( mbedtls_threading_mutex_t * ),
                       void (*mutex_free)( mbedtls_threading_mutex_t * ),
                       int (*mutex_lock)( mbedtls_threading_mutex_t * ),
                       int (*mutex_unlock)( mbedtls_threading_mutex_t * ) );

void mbedtls_threading_free_alt( void );

#endif /*THREADING_ALT_H*/

