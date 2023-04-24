cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y +=  -Iplatform/hal \
			  -Iutils/cqueue \
			  -Iapps/audioplayers \
			  -Iservices/bt_profiles/inc/ \
	      -Iservices/bt_profiles/inc/sys/ \
	      -Iservices/bluetooth_adapter/ \
			  -Iservices/bt_app \
	      -Iservices/resources \
			  -Iutils/heap \
			  -Iservices/bt_if/inc \
			  
ifeq ($(BLE_V2),1)
ccflags-y += -Iservices/ble_profiles_v2/btadapter/ \
						  -Iservices/ble_profiles_v2/btadapter/client \
						  -Iservices/ble_profiles_v2/btadapter/server \
						  -Iservices/ble_stack_v2/hl/api \
						  -Iservices/ble_app_v2/app_main
else
ccflags-y += -Iservices/ble_profiles/btadapter/ \
						  -Iservices/ble_profiles/btadapter/client \
						  -Iservices/ble_profiles/btadapter/server \
						  -Iservices/ble_stack/hl/api
endif		





