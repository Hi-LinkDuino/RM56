cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(NUTTX_BUILD),1)
ifneq ($(DUAL_BAND),1)
WIFI_LIB_NAME := libwifidrv
else
WIFI_LIB_NAME := libwifidrv-$(RF_TX_CONTROL_IO)
endif
else
WIFI_LIB_NAME := libwifidrv
endif

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
src_obj += $(CHIP)/

$(WIFI_LIB_NAME)-y := $(src_obj)

obj-y := $(WIFI_LIB_NAME).a

subdir-ccflags-y += -Iservices/nvrecord \
		    -Iplatform/drivers/wifi \
		    -Iplatform/drivers/wifi/$(CHIP) \
		    -Inet/

ifneq ($(DEBUG_PORT),)
subdir-ccflags-y += -DDEBUG_PORT=$(DEBUG_PORT)
endif
			
ifeq ($(BT_WIFI_COEXISTENCE),1)
subdir-ccflags-y += -DBT_WIFI_COEXISTENCE
endif

ifeq ($(USE_FEM),1)
subdir-ccflags-y += -DUSE_FEM
endif
