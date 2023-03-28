
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
ifeq ($(NET_LWIP_EXTERN),1)
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bridgeif.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bridgeif_fdb.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ethernet.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lowpan6.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lowpan6_ble.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lowpan6_common.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)slipif.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)zepif.c))
else
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
endif
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ppp/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	
