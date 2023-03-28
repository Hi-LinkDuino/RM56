cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y +=  -Iplatform/hal \
			  -Iutils/cqueue \
			  -Iapps/audioplayers \
			  -Iservices/bt_profiles_enhanced/inc/ \
			  -Iservices/bt_profiles_enhanced/inc/sys/ \
			  -Iservices/bluetooth_adapter/ \
			  -Iservices/bt_app \
			  -Iservices/resources \
			  -Iservices/osif \
			  -Iutils/heap \
			  -Iservices/bt_if_enhanced/inc \
			  -Iapps/key \
			  -Iapps/audioplayers/a2dp_decoder \
			  -Iservices/bt_app \
			  -Iapps/common \
			  -Iapps/alios/genie/include \
			  -Iservices/bt_if_enhanced/conmgr \
			  -Iservices/multimedia/audio/codec/sbc/inc \
			  -Iservices/multimedia/audio/codec/sbc/src/inc \
			  -Iservices/bt_app/a2dp_codecs/include \
			  -Iplatform/drivers/bt \
			  -Iservices/nv_section/userdata_section \
              -Iservices/nv_section/factory_section
			  
ifeq ($(BLE_V2),1)
ccflags-y += -Iservices/ble_profiles_v2/btadapter/ \
						  -Iservices/ble_profiles_v2/btadapter/client \
						  -Iservices/ble_profiles_v2/btadapter/server \
						  -Iservices/ble_stack_v2/hl/api \
						  -Iservices/ble_app_v2/app_main \
						  -Iservices/ble_stack_v2/modules/common/api \
						  -Iservices/ble_stack_v2/modules/rwip/api \
						  -Iservices/ble_stack_v2/modules/ke/api \
						  -Iservices/ble_stack_v2/ip/ble/hl/inc \
						  -Iservices/ble_stack_v2/ip/ble/hl/api \
						  -Iservices/app_ibrt_v2/inc \
						  -Iservices/ibrt_core_v2/inc \
						  -Iservices/ibrt_middleware/inc \
						  -Iservices/ibrt_ui_v2/inc \
						  -Iservices/ble_profiles_v2/otp \
                          -Iservices/nv_section/factory_section
else
ccflags-y += -Iservices/ble_profiles/btadapter/ \
						  -Iservices/ble_profiles/btadapter/client \
						  -Iservices/ble_profiles/btadapter/server \
						  -Iservices/ble_stack/hl/api \
						  -Iservices/nv_section/factory_section
endif		
