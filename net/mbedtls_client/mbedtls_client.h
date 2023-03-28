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
#ifndef MBEDTLS_CLIET_H
#define MBEDTLS_CLIET_H

#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

#ifdef  __cplusplus
extern "C"
{
#endif

typedef enum _mbedtls_client_err {
    MBEDTLS_CLIENT_OK = 0,
    MBEDTLS_CLIENT_ERR_FAILED = -1,
    MBEDTLS_CLIENT_ERR_NO_MEM = -2,
} mbedtls_client_err_t;

typedef struct
{
    mbedtls_ssl_context ssl;
    mbedtls_net_context server_fd;
    mbedtls_ssl_config conf;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
    mbedtls_x509_crt default_cacert;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
    int has_ca; /* customer ca */
} mbedtls_context_t;

int mbedtls_client_init(mbedtls_context_t **p_context);

int mbedtls_client_deinit(mbedtls_context_t *context);

int mbedtls_client_add_root_ca(mbedtls_context_t *context, 
                            const unsigned char *ca_buf, size_t ca_buflen);

int mbedtls_client_add_client_crt(mbedtls_context_t *context, 
        const unsigned char *ca_buf, size_t ca_buflen, const unsigned char *key, 
        size_t keylen, const unsigned char *pwd, size_t pwdlen);

int mbedtls_client_open(mbedtls_context_t *context, const char *host, 
        const char *port, const char **alpn_list, int auth, int timeout);

int mbedtls_client_close(mbedtls_context_t *context);
int mbedtls_client_close2(mbedtls_context_t *context, int normal_close);

int mbedtls_client_send(mbedtls_context_t *context, const unsigned char *buf, 
                        size_t len);

int mbedtls_client_recv(mbedtls_context_t *context, unsigned char *buf, size_t len);

/* recv response header end with \r\n\r\n */
int mbedtls_client_recv_header(mbedtls_context_t *context, unsigned char *buf, 
                                size_t len);

int mbedtls_client_set_timeout(mbedtls_context_t *context, uint32_t timeout);

#ifdef  __cplusplus
}
#endif

#endif /*MBEDTLS_CLIET_H*/

