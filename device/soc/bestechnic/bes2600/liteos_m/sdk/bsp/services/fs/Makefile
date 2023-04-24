cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := fs_test

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
#obj-y := $(obj-y:.c=.o)
#obj-y := $(obj-y:.cpp=.o)
#obj-y := $(obj-y:.S=.o)

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

#obj-y += ChaN/
# ../../platform/drivers/usb/usb_host/
#../../platform/drivers/ana/ ../../utils/hwtimer_list/ 

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
src_obj += ChaN/
$(LIB_NAME)-y := $(src_obj)
obj-y := $(LIB_NAME).a

ccflags-y := -Iservices/fs/ChaN -Iplatform/drivers/usb/usb_host \
	-Iplatform/drivers/ana -Iutils/hwtimer_list
	
subdir-ccflags-y := -Iplatform/drivers/ana