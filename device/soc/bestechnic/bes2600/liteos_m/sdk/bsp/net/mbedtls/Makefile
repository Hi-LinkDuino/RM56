
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-c :=
ifneq ($(ARM_CMSE),1)
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)/configs/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)/library/*.c))
else

obj-c += library/ecp.c
obj-c += library/ecp_curves.c
obj-c += library/ecdh.c
obj-c += library/ecdsa.c
obj-c += library/asn1write.c
obj-c += library/asn1parse.c
obj-c += library/aes.c
obj-c += library/cipher.c
obj-c += library/cipher_wrap.c
obj-c += library/ccm.c
obj-c += library/ctr_drbg.c
obj-c += library/entropy.c
obj-c += library/gcm.c
obj-c += library/bignum.c
obj-c += library/md.c
obj-c += library/md5.c
obj-c += library/md_wrap.c
obj-c += library/ripemd160.c
obj-c += library/sha1.c
obj-c += library/pkcs5.c
obj-c += library/des.c
obj-c += library/camellia.c
obj-c += library/blowfish.c
obj-c += library/sha256.c
obj-c += library/sha512.c
obj-c += library/hmac_drbg.c
obj-c += library/hkdf.c
obj-c += library/platform_util.c
obj-c += library/pk.c
obj-c += library/pk_wrap.c
obj-c += library/pkparse.c
obj-c += library/oid.c
obj-c += library/pem.c
obj-c += library/base64.c

endif

ifneq ($(MBEDTLS_CONFIG_FILE),)
subdir-ccflags-y += -DMBEDTLS_CONFIG_FILE='"$(MBEDTLS_CONFIG_FILE)"'
endif

ifeq ($(NET_MBEDTLS_TESTS_SUPPORT),1)
obj-c += tests/test_suite_aes.cbc.c
obj-c += tests/test_suite_aes.cfb.c
obj-c += tests/test_suite_aes.ecb.c
obj-c += tests/test_suite_aes.ofb.c
obj-c += tests/test_suite_aes.rest.c
obj-c += tests/test_suite_aes.xts.c
obj-c += tests/test_suite_arc4.c
obj-c += tests/test_suite_aria.c
obj-c += tests/test_suite_asn1write.c
obj-c += tests/test_suite_base64.c
obj-c += tests/test_suite_blowfish.c
obj-c += tests/test_suite_camellia.c
obj-c += tests/test_suite_ccm.c
obj-c += tests/test_suite_chacha20.c
obj-c += tests/test_suite_chachapoly.c
obj-c += tests/test_suite_cipher.aes.c
obj-c += tests/test_suite_cipher.arc4.c
obj-c += tests/test_suite_cipher.blowfish.c
obj-c += tests/test_suite_cipher.camellia.c
obj-c += tests/test_suite_cipher.ccm.c
obj-c += tests/test_suite_cipher.chacha20.c
obj-c += tests/test_suite_cipher.chachapoly.c
obj-c += tests/test_suite_cipher.des.c
obj-c += tests/test_suite_cipher.gcm.c
obj-c += tests/test_suite_cipher.misc.c
obj-c += tests/test_suite_cipher.null.c
obj-c += tests/test_suite_cipher.padding.c
obj-c += tests/test_suite_cmac.c
obj-c += tests/test_suite_ctr_drbg.c
obj-c += tests/test_suite_debug.c
obj-c += tests/test_suite_des.c
obj-c += tests/test_suite_dhm.c
obj-c += tests/test_suite_ecdh.c
obj-c += tests/test_suite_ecdsa.c
obj-c += tests/test_suite_ecjpake.c
obj-c += tests/test_suite_ecp.c
obj-c += tests/test_suite_entropy.c
obj-c += tests/test_suite_error.c
obj-c += tests/test_suite_gcm.aes128_de.c
obj-c += tests/test_suite_gcm.aes128_en.c
obj-c += tests/test_suite_gcm.aes192_de.c
obj-c += tests/test_suite_gcm.aes192_en.c
obj-c += tests/test_suite_gcm.aes256_de.c
obj-c += tests/test_suite_gcm.aes256_en.c
obj-c += tests/test_suite_gcm.camellia.c
obj-c += tests/test_suite_gcm.misc.c
obj-c += tests/test_suite_hkdf.c
obj-c += tests/test_suite_hmac_drbg.misc.c
obj-c += tests/test_suite_hmac_drbg.no_reseed.c
obj-c += tests/test_suite_hmac_drbg.nopr.c
obj-c += tests/test_suite_hmac_drbg.pr.c
obj-c += tests/test_suite_md.c
obj-c += tests/test_suite_mdx.c
obj-c += tests/test_suite_memory_buffer_alloc.c
obj-c += tests/test_suite_mpi.c
obj-c += tests/test_suite_nist_kw.c
obj-c += tests/test_suite_pem.c
obj-c += tests/test_suite_pk.c
obj-c += tests/test_suite_pkcs1_v15.c
obj-c += tests/test_suite_pkcs1_v21.c
obj-c += tests/test_suite_pkcs5.c
obj-c += tests/test_suite_pkparse.c
obj-c += tests/test_suite_pkwrite.c
obj-c += tests/test_suite_poly1305.c
obj-c += tests/test_suite_rsa.c
obj-c += tests/test_suite_shax.c
obj-c += tests/test_suite_ssl.c
obj-c += tests/test_suite_timing.c
obj-c += tests/test_suite_version.c
obj-c += tests/test_suite_x509parse.c
obj-c += tests/test_suite_x509write.c
obj-c += tests/test_suite_xtea.c
endif

ccflags-y += \
    -Iservices/wifi_app/littlefs/flash_partition/ \
    -Iservices/wifi_app/littlefs/flash_partition/sys \
    -Inet/mbedtls/include/mbedtls


obj-y := $(obj-c:.c=.o)



ifeq ($(NET_MBEDTLS_AES_ALT),1)
ccflags-y += \
        -Iplatform/drivers/security_engine/
endif

ccflags-y += \
    -Inet/mbedtls/include/ \
    -Inet/mbedtls/include/mbedtls/ \
    -Inet/pkcs11-helper/include/
