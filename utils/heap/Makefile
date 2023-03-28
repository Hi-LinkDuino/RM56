
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iplatform/cmsis/inc/

ifeq ($(HEAP_DEBUG),1)
ccflags-y += -DHEAP_DEBUG
endif

ifeq ($(MED_HEAP_DEBUG),1)
ccflags-y += -DMED_HEAP_DEBUG
endif


ifdef DEV

LIB_NAME := libheap

ifeq ($(DEV),1)
$(LIB_NAME)-y := $(obj-y)
endif
obj-y := $(LIB_NAME).a

endif  #DEV

