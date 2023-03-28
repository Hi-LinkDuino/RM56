cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)



ifdef DEV

LIB_NAME := liboverlay

ifeq ($(DEV),1)
$(LIB_NAME)-y := $(obj-y)
endif
obj-y := $(LIB_NAME).a

endif  #DEV
