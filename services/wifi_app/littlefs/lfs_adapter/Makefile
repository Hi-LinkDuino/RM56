cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)


ifeq ($(LFS_ON_FLASH), 1)
KBUILD_CPPFLAGS += -DLFS_ON_FLASH
endif

ifeq ($(LFS_ON_SD_CARD), 1)
KBUILD_CPPFLAGS += -DLFS_ON_SD_CARD
endif

ifeq ($(LFS_ON_RAM), 1)
KBUILD_CPPFLAGS += -DLFS_ON_RAM=1
endif
ccflags-y := -Iservices/wifi_app/littlefs/ \
			 -Iservices/wifi_app/littlefs/lfs_adapter/ \
			 -Iinclude/ \
			 -Iplatform/drivers/ \
			 -Iplatform/drivers/ana

