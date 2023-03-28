cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := test_littlefs.o
ifeq ($(LITTLEFS_PARTITION_SUPPORT),1)
obj-y += test_fs_std_api.o
endif


ccflags-y += \
	-Iservices/wifi_app/littlefs/ \
	-Iservices/wifi_app/littlefs/lfs_adapter/ \
	-Iservices/wifi_app/littlefs/flash_partition/

LFS_READ_FILE_AND_PLAY_TEST := 0
ifeq ($(LFS_READ_FILE_AND_PLAY_TEST), 1)
ccflags-y += -DLFS_READ_FILE_AND_PLAY_TEST
endif