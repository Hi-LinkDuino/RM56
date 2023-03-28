cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)/source/*.c))

obj-y := $(obj-c:.c=.o)

ccflags-y += -Iplatform/cmsis/inc -Iplatform/hal \
    -Irtos/freertos/CMSIS/RTOS2/FreeRTOS/Include \
    -Irtos/freertos/POSIX/include \
    -Irtos/freertos/POSIX/include/private \
    -Irtos/freertos/POSIX/include/portable/bes \
    -Irtos/freertos/POSIX/unity/extras/fixture/src \
    -Irtos/freertos/POSIX/unity/src
