/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#include <stdint.h>
#include "cmsis_os.h"
#if defined(__NET_LIBCURL__)
#include "http_media.h"
#include "threading_alt.h"
#endif
#define WIFI_AUDIO_HTTP_URLA     "http://ailabsaicloudservice.alicdn.com/player/resources/fdf1164db06e141eedb13e430da0079f.mp3"
#define WIFI_AUDIO_HTTP_URLB     "http://dumi-audio.cdn.bcebos.com/mp3/5205785_64.mp3?from=dumi&logid=fq2q9-1451d2597000000_2#1_0"
#define WIFI_AUDIO_HTTP_URLC     "http://sp-sycdn.kuwo.cn/resource/n1/56/63/3544533151.mp3"
#define WIFI_AUDIO_HTTP_URLD     "http://1.wp.zp68.com:811/sub/filestores/2019/07/01/93ef046ce0cd0c1dc252b8b0919ee717.mp3?lx=xzwj&k=26779cd1402044e6be7b"
static char *wifi_audio_demo[] =
{
    "http://tmjl-s128.alicdn.com/260/1260/2105392421/1814084334_1572542586080_9687.mp3?auth_key=1616036400-0-0-984ba70632facd4e4d3aef32150f3576",
    "http://other.web.rs01.sycdn.kuwo.cn/resource/n1/85/20/1282621993.mp3",
    "http://other.web.rl01.sycdn.kuwo.cn/resource/n3/51/36/300367912.mp3",
    "http://other.web.rr01.sycdn.kuwo.cn/resource/n2/99/67/2356768425.mp3",
    "http://www.170mv.com/kw/sp-sycdn.kuwo.cn/resource/n1/56/63/3544533151.mp3",
};

/* @ http test */
int bes_wifi_audio_http_test(void)
{
#if defined(__NET_LIBCURL__)

    /* Non-block */
    for (size_t i = 0; i < (sizeof(wifi_audio_demo)/sizeof(char *)); i++)
    {
        hm_url_play(wifi_audio_demo[i]);
        hm_url_stop();
    }

#if 0
    osDelay(20 * 1000);
    hm_url_stop();
    osDelay(3 * 1000); /* Need fix it */

    hm_url_play(WIFI_AUDIO_HTTP_URLB);
    osDelay(20 * 1000);
    hm_url_stop();
    osDelay(3 * 1000); /* Need fix it */

    /* 《你不是真正的快乐.mp3》 */
    hm_url_play(WIFI_AUDIO_HTTP_URLC);
    osDelay(20 * 1000);
    hm_url_stop();
    osDelay(3 * 1000); /* Need fix it */

    /* 《七里香.mp3》 */
    hm_url_play(WIFI_AUDIO_HTTP_URLD);
    osDelay(20 * 1000);
    hm_url_stop();
    osDelay(3 * 1000); /* Need fix it */
#endif
    /* block until misic play finish */
    /* 《你不是真正的快乐.mp3》 */
    // hm_url_play_and_block("http://sp-sycdn.kuwo.cn/resource/n1/56/63/3544533151.mp3");

    /* 《七里香.mp3》 */
    //hm_url_play_and_block(WIFI_AUDIO_HTTP_URLD);
#endif
    return 0;
}



/* This is taken from bes server. */
/* BEGIN FILE string macro TEST_CA_CRT_RSA_SHA256_PEM bes server ca.crt */
#define TEST_CA_CRT_RSA_SHA256_PEM                                         \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIFLTCCAxUCFFqzazYz61BFwFUg1lBtlg9lM6T+MA0GCSqGSIb3DQEBCwUAMFMx\r\n" \
    "CzAJBgNVBAYTAkNOMREwDwYDVQQIDAhzaGFuZ2hhaTEPMA0GA1UEBwwGcHVkb25n\r\n" \
    "MQwwCgYDVQQKDANiZXMxEjAQBgNVBAMMCWJlcyBodHRwczAeFw0yMDAzMDcxMjEy\r\n" \
    "MzVaFw0zMDAzMDUxMjEyMzVaMFMxCzAJBgNVBAYTAkNOMREwDwYDVQQIDAhzaGFu\r\n" \
    "Z2hhaTEPMA0GA1UEBwwGcHVkb25nMQwwCgYDVQQKDANiZXMxEjAQBgNVBAMMCWJl\r\n" \
    "cyBodHRwczCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBANo+P0CkVtXt\r\n" \
    "Q0UZlMkI/m0s/wu4H4ZxfYMVLsKWSN2EOnqEm7xomQrJ9AbOydK66ZjljewUqEnE\r\n" \
    "/eL9XZa7WlwCoJzoEh3vgLnM4XIQGhpHDfRrzj5h9uIn5DNAOlspapg1sGmYPavs\r\n" \
    "CRcu+NQInF0/mD4JmLEX9ZEL1dHultvj1w/DFBw+TRY+aIsLfTFWblOq3yMDq6zL\r\n" \
    "quhc7GSxKFEcQ+6RTgAZhrXNNsNMc9XjGtPs+rR0okb42k37s+oWz048cP3pKeU7\r\n" \
    "d1L6u/Eh3aly11tEOGd8dSfnMKfZppy3Wnuei6Bp037AtsxnUoZRYsOAiQ+eHWoA\r\n" \
    "Yikj8ewjfXU0hAsFoibNKPjKYZM+vb+9LNAd/+O0u49gl4+CXKHt0TeONt3+b+ub\r\n" \
    "tydCB/ooNROHebLjveXsMafoi/w2XFt479EO0eVCrpCGOipCkAF/srVDl8mrKaix\r\n" \
    "AqCZBkC2o/VvS/ThdXz3egCouSzlzY9K2DabOfWfcrZlDZS7nc4n8QO+pMRlF4aX\r\n" \
    "imrOMjw2x75baiugbMpG+o3SRewesf3V3CF4FaPD0UT/Al7UC0d7QSwVzDk9nLwi\r\n" \
    "CH2+SnW3/Mwqap8j+LkinvC6xIBFk+kuRNblnj4hrXD/CUo9thk1ON8khLBH8Cob\r\n" \
    "uxD6aQWyQToo8MPm+lGZWJyuP/PVG0I1AgMBAAEwDQYJKoZIhvcNAQELBQADggIB\r\n" \
    "ABf+j9s1u3BmmvpPJBR7R4n6k/cMZVkvMdmtkekBhXYa+i/UYs5x7syCC5hY5+6f\r\n" \
    "cl+HWLsECmoojfxGRp6xvUI/c0YPfk23VX7n/Sy8CwzuWtvdg/PoNtutv8KMOUMN\r\n" \
    "tcEyBkkDH4zBTFJToYLnpXVVewXhnjQ79zZ8SNKaqahqNxY1XUKf+3wonOAC75KA\r\n" \
    "eom4qXB+7vnRfAbiS8OIC84i0iYTNG3+UOYPumqtfReCQlwJ6XjzQxotZKAf96a9\r\n" \
    "BY0UB+RQurcUohwh84oRVBBfRvWqKWLVCo32m/Uc1vjINrcWmL2tVZSolEendwYQ\r\n" \
    "mS3IYH6WglFg3qCi2Ret4z5beawfAqbbJEbfo9vnLdQBo1Nj+4g7F7I9ylWYW2OA\r\n" \
    "Ut5ybmewDmqmuXNWSCLjF03H3u6aBQ4vNRvXdXU5gvho7QC47a0/gBwsJkAwM44n\r\n" \
    "b/M7HWWbHkTNaCY7jjoYLoNYw0lCFB4VLEZrM3tMdI7u9cRUcocP7C7c2lxtrSlZ\r\n" \
    "gDU5r2znQLVEh47ulJlG7Xuf2ieUrjJyNSwSIHQlrGWmLH+2wVwRf0R48r2d7qTS\r\n" \
    "YDH9JLWTKIDY8tk40eWvKQVdWln1antDKhv9N89OhaTiraDlFOg93PjyCNl+SfNF\r\n" \
    "tnyzY3kGCBcTeFCY06cJG8TRK4o5/LcnsSWVKz8XQTyt\r\n"                     \
    "-----END CERTIFICATE-----\r\n"
/* END FILE */

static const char bes_test_root_ca[] =
    TEST_CA_CRT_RSA_SHA256_PEM
    "";
static const size_t bes_test_root_ca_len = sizeof(bes_test_root_ca);

#define HTTPS_URL       "https://192.168.137.1/test.mp3"

/* @ https test */
int bes_wifi_audio_https_test(void)
{
#if defined(__NET_LIBCURL__) && defined(__NET_MBEDTLS__)
    mbedtls_threading_set_alt(mbedtlsThreading_MutexInit,mbedtlsThreading_MutexFree,mbedtlsThreading_MutexLock,mbedtlsThreading_MutexUnlock);

    //hm_init_root_ca(bes_test_root_ca,bes_test_root_ca_len);
    //hm_url_play(HTTPS_URL);
    osDelay(5000);
    //hm_url_stop();
#endif
    return 0;
}
