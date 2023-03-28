cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
#obj-y := $(obj-y:.c=.o)
#obj-y := $(obj-y:.cpp=.o)
#obj-y := $(obj-y:.S=.o)
obj-y := a7_dsp_os.o




ifeq ($(MODULE_KERNEL_STUB),1)
# LITEOS_A_ROOTPATH ?= rtos/liteos/
OHROOT_PATH ?= ../../../../../../../
LITEOS_A_ROOTPATH ?= $(OHROOT_PATH)kernel/

ccflags-y += \
    -I$(LITEOS_A_ROOTPATH)liteos_a/kernel/include \
    -I$(LITEOS_A_ROOTPATH)liteos_a/kernel/common \
    -I$(LITEOS_A_ROOTPATH)liteos_a/arch/arm \
    -I$(LITEOS_A_ROOTPATH)liteos_a/kernel/base/include \
    -I$(LITEOS_A_ROOTPATH)liteos_a/platform/include \
    -I$(LITEOS_A_ROOTPATH)liteos_a/arch/arm/include \
    -I$(LITEOS_A_ROOTPATH)liteos_a/arch/arm/arm/include \
    -I$(LITEOS_A_ROOTPATH)liteos_a/bounds_checking_function/include \
    -I$(LITEOS_A_ROOTPATH)liteos_a/kernel/extended/include \
    -Irtos/liteos/liteos_a/config/board \
    -Irtos/liteos/liteos_a/platform/include/ \
    -I$(LITEOS_A_ROOTPATH)liteos_a/ \
    -I$(OHROOT_PATH)third_party/bounds_checking_function/include/

ccflags-y += -I$(LITEOS_A_ROOTPATH)liteos_a/kernel/extended/cpup/
ccflags-y += -I$(OHROOT_PATH)third_party/musl/porting/liteos_a/kernel/include/
ccflags-y += -I$(OHROOT_PATH)kernel/liteos_a/lib/libscrew/include/
ccflags-y += -I$(OHROOT_PATH)kernel/liteos_a/fs/rootfs/
ccflags-y += -I$(OHROOT_PATH)kernel/liteos_a/arch/arm/arm/src/include/
ccflags-y += -I$(OHROOT_PATH)kernel/liteos_a/kernel/extended/liteipc/
ccflags-y += -Irtos/liteos/kal/cmsis


else
ccflags-y += -Irtos/liteos/liteos_a/kernel/include \
			 -Irtos/liteos/liteos_a/kernel/common \
			 -Irtos/liteos/liteos_a/arch/arm \
			 -Irtos/liteos/liteos_a/kernel/base/include \
			 -Irtos/liteos/liteos_a/config/board \
			 -Irtos/liteos/liteos_a/platform/include \
			 -Irtos/liteos/liteos_a/arch/arm/include \
			 -Irtos/liteos/liteos_a/arch/arm/arm/include \
			 -Irtos/liteos/liteos_a/bounds_checking_function/include \
			 -Irtos/liteos/liteos_a/kernel/extended/include \
			 -Irtos/liteos/liteos_a/kernel/extended/cpup\
			 -Irtos/liteos/liteos_a/lib/libscrew/include\
			 -Irtos/liteos/liteos_a/lib/musl/kernel/include
endif