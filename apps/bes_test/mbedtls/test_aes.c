#define BES_MBEDTLS_AES_TEST

#ifdef __NET_MBEDTLS__
#include "aes.h"
#include "entropy.h"
#endif

int bes_mbedtls_aes_test(void)
{
    int ret = -1;
#ifdef BES_MBEDTLS_AES_TEST
#ifdef EN_MBEDTLS_AES_ALT
    TRACE(0, "mbedtls aes alt en.");
#else
    TRACE(0, "mbedtls aes alt disable.");
#endif

#if defined(MBEDTLS_SELF_TEST) && defined(__NET_MBEDTLS__) && !defined(EN_MBEDTLS_AES_ALT)
    ret = mbedtls_aes_self_test(1);
#endif

    if(0 != ret)
        TRACE(0, "mbedtls aes self test error");
    else
        TRACE(0, "mbedtls aes self test succ");
#endif//BES_MBEDTLS_AES_TEST
    return ret;
}
