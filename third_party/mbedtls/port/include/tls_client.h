/**
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBEDTLS_CLIENT_H
#define MBEDTLS_CLIENT_H

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"

#define RET_ERROR -1;
#define RET_EOK 0

typedef struct MbedTLSSession {
    char *host;
    char *port;

    unsigned char *buffer;
    size_t buffer_len;

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_net_context server_fd;
    mbedtls_x509_crt cacert;
} MbedTLSSession;

int MbedtlsClientInit(MbedTLSSession *session, void *entropy, size_t entropyLen);
int MbedtlsClientClose(MbedTLSSession *session);
int MbedtlsClientContext(MbedTLSSession *session);
int MbedtlsClientConnect(MbedTLSSession *session);
int MbedtlsClientRead(MbedTLSSession *session, unsigned char *buf, size_t len);
int MbedtlsClientWrite(MbedTLSSession *session, const unsigned char *buf, size_t len);

#endif
