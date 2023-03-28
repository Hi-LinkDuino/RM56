cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)

ifeq ($(BIS_MASTER),1)
ccflags-y += -D_BIS_MASTER_TEST_
endif

ifeq ($(BIS_SLAVE),1)
ccflags-y += -D_BIS_SLAVE_TEST_
endif

ccflags-y += -Iapps/bes_test/ -Iplatform/hal/ -Iservice/audioflinger/ -Iutils/cqueue/
