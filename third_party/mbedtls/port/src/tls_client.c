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

#include "tls_client.h"

#include <stdio.h>
#include <string.h>

#include "securec.h"
#include "tls_certificate.h"
#include "mbedtls_log.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

static void SslDebug(void *ctx, int level, const char *file, int line, const char *str)
{
    LOGD("%s:%04d: %s.", file, line, str);
}

static int MbedtlsSslCertificateVerify(MbedTLSSession *session)
{
    if (session == NULL) {
        return -RET_ERROR;
    }
    int ret = mbedtls_ssl_get_verify_result(&session->ssl);
    if (ret != 0) {
        LOGD("verify peer certificate fail...");
        (void)memset_s(session->buffer, session->buffer_len, 0x00, session->buffer_len);
        mbedtls_x509_crt_verify_info((char *)session->buffer, session->buffer_len, "  ! ", ret);
        LOGD("verification info: %s.", session->buffer);
        return -RET_ERROR;
    }
    return RET_EOK;
}

int MbedtlsClientInit(MbedTLSSession *session, void *entropy, size_t entropyLen)
{
    if (session == NULL || entropy == NULL) {
        return -RET_ERROR;
    }
    mbedtls_net_init(&session->server_fd);
    mbedtls_ssl_init(&session->ssl);
    mbedtls_ssl_config_init(&session->conf);
    mbedtls_ctr_drbg_init(&session->ctr_drbg);
    mbedtls_entropy_init(&session->entropy);
    mbedtls_x509_crt_init(&session->cacert);
    int ret = mbedtls_ctr_drbg_seed(&session->ctr_drbg, mbedtls_entropy_func, &session->entropy,
                                    (unsigned char *)entropy, entropyLen);
    if (ret != 0) {
        LOGD("mbedtls_ctr_drbg_seed error, return -0x%x.", -ret);
        return ret;
    }
    LOGD("mbedtls client struct init success...");
    return RET_EOK;
}

int MbedtlsClientClose(MbedTLSSession *session)
{
    if (session == NULL) {
        return -RET_ERROR;
    }
    mbedtls_ssl_close_notify(&session->ssl);
    mbedtls_net_free(&session->server_fd);
    mbedtls_x509_crt_free(&session->cacert);
    mbedtls_entropy_free(&session->entropy);
    mbedtls_ctr_drbg_free(&session->ctr_drbg);
    mbedtls_ssl_config_free(&session->conf);
    mbedtls_ssl_free(&session->ssl);
    LOGD("MbedTLS connection close success.");
    return RET_EOK;
}

int MbedtlsClientContext(MbedTLSSession *session)
{
    if (session == NULL) {
        return -RET_ERROR;
    }
    int ret = mbedtls_x509_crt_parse(&session->cacert, (const unsigned char *)G_MBEDTLS_ROOT_CERTIFICATE,
                                     G_MBEDTLS_ROOT_CERTIFICATE_LEN);
    if (ret < 0) {
        LOGE("mbedtls_x509_crt_parse error,  return -0x%x.", -ret);
        return ret;
    }

    LOGD("Loading the CA root certificate success...");

    // Hostname set here should match CN in server certificate
    if (session->host != NULL) {
        ret = mbedtls_ssl_set_hostname(&session->ssl, session->host);
        if (ret != 0) {
            LOGD("mbedtls_ssl_set_hostname error, return -0x%x", -ret);
            return ret;
        }
    }

    ret = mbedtls_ssl_config_defaults(&session->conf,
                                      MBEDTLS_SSL_IS_CLIENT,
                                      MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        LOGD("mbedtls_ssl_config_defaults error, return -0x%x.", -ret);
        return ret;
    }

    // If you want to verify the validity of the server certificate,
    // you need to replace MBEDTLS_SSL_VERIFY_NONE with MBEDTLS_SSL_VERIFY_REQUIRED
    mbedtls_ssl_conf_authmode(&session->conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_ca_chain(&session->conf, &session->cacert, NULL);
    mbedtls_ssl_conf_rng(&session->conf, mbedtls_ctr_drbg_random, &session->ctr_drbg);
    mbedtls_ssl_conf_dbg(&session->conf, SslDebug, NULL);
    ret = mbedtls_ssl_setup(&session->ssl, &session->conf);
    if (ret != 0) {
        LOGD("mbedtls_ssl_setup error, return -0x%x.", -ret);
        return ret;
    }
    LOGD("mbedtls client context init success...");

    return RET_EOK;
}

int MbedtlsClientConnect(MbedTLSSession *session)
{
    if (session == NULL) {
        return -RET_ERROR;
    }
    LOGI("connect: host:%s, port: %s", session->host, session->port);

    int ret = mbedtls_net_connect(&session->server_fd, session->host, session->port, MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        LOGD("mbedtls_net_connect error, return -0x%x.", -ret);
        return ret;
    }
    LOGD("Connected %s:%s fd:%d, success...", session->host, session->port, session->server_fd.fd);

    mbedtls_ssl_set_bio(&session->ssl, &session->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
    LOGD("ssl state=%d", session->ssl.state);

    while ((ret = mbedtls_ssl_handshake(&session->ssl)) != 0) {
        LOGD("mbedtls_ssl_handshake ret=0x%x.", -ret);
        if (RET_EOK != MbedtlsSslCertificateVerify(session)) {
            return -RET_ERROR;
        }
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            LOGD("mbedtls_ssl_handshake error, return -0x%x.", -ret);
            return ret;
        }
    }

    if (RET_EOK != MbedtlsSslCertificateVerify(session)) {
        LOGD("Certificate verified err...");
        return -RET_ERROR;
    }

    LOGD("Certificate verified success...");

    return RET_EOK;
}

int MbedtlsClientRead(MbedTLSSession *session, unsigned char *buf, size_t len)
{
    if (session == NULL || buf == NULL) {
        return -RET_ERROR;
    }
    int ret = mbedtls_ssl_read(&session->ssl, (unsigned char *)buf, len);
    if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        LOGD("MbedtlsClientRead data error, return -0x%x.", -ret);
    }
    return ret;
}

int MbedtlsClientWrite(MbedTLSSession *session, const unsigned char *buf, size_t len)
{
    if (session == NULL || buf == NULL) {
        return -RET_ERROR;
    }
    int ret = mbedtls_ssl_write(&session->ssl, (unsigned char *)buf, len);
    if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        LOGD("MbedtlsClientWrite data error, return -0x%x.", -ret);
    }
    return ret;
}