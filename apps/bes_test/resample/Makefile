cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)

ccflags-y +=  \
    -Iservices/wifi_app/alsa_adapter/ \
    -Iplatform/hal \
    -Iapps/bes_test/resample/16kHz/ \
    -Iapps/bes_test/resample/17kHz/ \
    -Iapps/bes_test/resample/18kHz/ \
    -Iapps/bes_test/resample/19kHz/ \
    -Iapps/bes_test/resample/20kHz/ \
    -Iapps/bes_test/resample/80kHz/