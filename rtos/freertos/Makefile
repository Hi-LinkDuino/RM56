obj-y := CMSIS/RTOS2/FreeRTOS/ Source/

subdir-ccflags-y += -Iplatform/cmsis/inc -Iplatform/hal

ifeq ($(OSTICK_USE_FAST_TIMER),1)
subdir-ccflags-y += -DOSTICK_USE_FAST_TIMER
endif