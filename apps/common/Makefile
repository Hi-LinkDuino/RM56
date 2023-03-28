cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += \
    -Iplatform/drivers/ana \
    -Iinclude

ifeq ($(IBRT),1)

ifeq ($(IBRT_UI_V1),1)
subdir-ccflags-y += \
    -Iservices/ibrt_core/inc \
    -Iservices/ibrt_ui/inc
endif

ifeq ($(IBRT_CORE_V2),1)
subdir-ccflags-y += \
    -Iservices/ibrt_core_v2/inc
endif
endif

ifeq ($(NO_WDT),1)
CFLAGS_app_watchdog.o += -DNO_WDT
endif

ifeq ($(NO_WDT_TIMER),1)
CFLAGS_app_watchdog.o += -DNO_WDT_TIMER
endif

ifeq ($(CHIP_HAS_RGB_LED),1)
ccflags-y +=  -Iplatform/hal# -Iservices/nv_section/userdata_section
endif
