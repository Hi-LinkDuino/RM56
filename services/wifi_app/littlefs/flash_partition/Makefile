cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += dirent.o

ccflags-y += \
    -Iplatform/drivers/ \
    -Iservices/wifi_app/littlefs/flash_partition