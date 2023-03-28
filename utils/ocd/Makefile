cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))
OCD_LIB := libocd

ifneq ($(SDK), 1)
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
$(OCD_LIB)-y := $(src_obj)
endif  #end ifneq ($(SDK), 1)
obj-y := $(OCD_LIB).a

ccflags-y := -Iplatform/hal/
ccflags-y += -Iservices/wifi_app/wifi_console/
