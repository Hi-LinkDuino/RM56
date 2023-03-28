
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c)) 
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sntp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)altcp_tls/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)http/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lwiperf/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)mdns/*.c))
ifneq ($(APP_TUYA_SUPPORT),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)mqtt/*.c))
endif
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)netbiosns/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)smtp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)snmp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)tftp/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := -Iplatform/hal/	\

