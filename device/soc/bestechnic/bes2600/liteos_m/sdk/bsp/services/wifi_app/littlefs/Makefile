cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += lfs_adapter/
# first run cmd ./tools/mklittlefs -c net/mbedtls/tests/resource/data -d 5 -b 4096 -p 848 -s 3637248 out/littlefs.bin
ifeq ($(LITTLEFS_PARTITION_SUPPORT),1)
obj-y += flash_partition/
endif

subdir-ccflags-y := -Iservices/wifi_app/littlefs/lfs_adapter/ -Iservices/wifi_app/littlefs -Iplatform/drivers/ -Iutils/crc32
