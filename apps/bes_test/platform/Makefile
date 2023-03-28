cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)

ccflags-y += \
    -Iapps/bes_test/platform/ \
    -Iapps/bes_test/ \
    -Iapps/key/ \
    -Iservices/wifi_app/alsa_adapter/ \
    -Iplatform/hal/ \
    -Iservices/wifi_app/wifi_console/ \
    -Iservices/a7_dsp/common/

