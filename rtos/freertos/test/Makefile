cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)common/test_runner/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)common/posix/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)common/pkcs11/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)unity/extras/fixture/src/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)unity/src/*.c))

obj-y := $(obj-c:.c=.o)

ccflags-y += \
    -Iplatform/cmsis/inc \
    -Iplatform/hal \
    -Irtos/freertos/CMSIS/RTOS2/FreeRTOS/Include \
    -Irtos/freertos/POSIX/include \
    -Irtos/freertos/POSIX/include/private \
    -Irtos/freertos/POSIX/include/portable/bes \
    -Irtos/freertos/test/unity/extras/fixture/src \
    -Irtos/freertos/test/unity/src \
    -Irtos/freertos/test/bes/config_files \
    -Irtos/freertos/test/common/include \
    -Iinclude/rtos/freertos \
    -Inet/mbedtls/include/ \
    -Irtos/freertos/test/common/pkcs11/