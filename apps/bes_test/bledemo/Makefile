cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y +=  -Iplatform/hal \
			  -Iservices/bt_profiles/inc/ \
	      -Iservices/bluetooth_adapter/ \
			  -Iservices/bt_if/inc \
			  -Iservices/ble_profiles_v2/btadapter/ \
				-Iservices/ble_stack_v2/hl/api

