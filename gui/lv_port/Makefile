cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += indev/
ifeq ($(DISPLAY_V2), 1)
obj-y += display_v2/
else
obj-y += display/
endif
