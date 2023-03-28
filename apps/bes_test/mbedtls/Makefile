cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=
obj-y +=  test_mbedtls_ssl_client.o
obj-y +=  test_aes.o
ifeq ($(NET_MBEDTLS_TESTS_SUPPORT),1)
obj-y +=  test_all_alg.o
endif

ccflags-y += \
    -Inet/mbedtls/include/ \
    -Inet/mbedtls/include/mbedtls \
    -Iservices/wifi_app/littlefs/lfs_adapter/ \
    -Iservices/wifi_app/littlefs/flash_partition/ \
    -Inet/mbedtls/include/