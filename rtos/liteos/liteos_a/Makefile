cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(MODULE_KERNEL_STUB),1)
obj-y :=
obj-y += config/board/os_adapt/os_adapt.o
obj-y += config/board/platform.o
obj-y += kal/cmsis/cmsis_liteos.o
obj-y += kal/cmsis/cmsis_liteos2.o
obj-y += config/board/drv/drv_uart.o

ccflags-y += \
    -DSECUREC_HAVE_STDIO_H=0 \
    -DSECUREC_IN_KERNEL=0 \
    -DSECUREC_ENABLE_FSCANF=0 \
    -DSECUREC_ENABLE_VFSCANF=0 \
    -DLOSCFG_BASE_IPC_MUX=1 \
    -DLOSCFG_BASE_IPC_SEM=1 \
    -DLOSCFG_BASE_IPC_QUEUE=1 \
    -DLOSCFG_KERNEL_SMP_CORE_NUM=2

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
LITEOSTOPDIR := rtos/liteos/liteos_a
-include $(LITEOSTOPDIR)/.config

LITEOS_AS :=
LITEOS_SRCS :=
LITEOS_INC :=

# build arch
ARCH_SRCS := $(cur_dir)arch/arm
LITEOS_AS += $(wildcard $(ARCH_SRCS)/arm/src/*.S)
LITEOS_SRCS += $(wildcard $(ARCH_SRCS)/arm/src/*.c)
LITEOS_INC += \
	-I$(LITEOSTOPDIR)/kernel/include \
	-I$(LITEOSTOPDIR)/kernel/common \
	-I$(LITEOSTOPDIR)/kernel/base/include \
	-I$(LITEOSTOPDIR)/kernel/extended/include \
	-I$(LITEOSTOPDIR)/kernel/extended/cpup \
	-I$(LITEOSTOPDIR)/kernel/extended/power \
	-I$(LITEOSTOPDIR)/kernel/extended/dynload/include \
	-I$(LITEOSTOPDIR)/arch/arm/ \
	-I$(LITEOSTOPDIR)/arch/arm/include \
	-I$(LITEOSTOPDIR)/arch/arm/arm/include \
	-I$(LITEOSTOPDIR)/arch/arm/arm/src/include \
	-I$(LITEOSTOPDIR)/config/board \
	-I$(LITEOSTOPDIR)/bounds_checking_function/include \
	-I$(LITEOSTOPDIR)/platform/include \
	-I$(LITEOSTOPDIR)/lib/libscrew/include \
	-I$(LITEOSTOPDIR)/lib/musl/kernel/include \
	-I$(LITEOSTOPDIR)/fs/rootfs \
	-I$(LITEOSTOPDIR)/bsd/compat/linuxkpi/include \

#ifeq ($(LITEOS_ARM_ARCH), -march=armv7-a)
LITEOS_AS += $(wildcard $(ARCH_SRCS)/arm/src/armv7a/*.S)
#endif

ifneq ($(LOSCFG_KERNEL_VM), y)
#LITEOS_SRCS := $(filter-out $(ARCH_SRCS)/arm/src/strncpy_from_user.c, $(LITEOS_SRCS))
LITEOS_SRCS := $(filter-out $(ARCH_SRCS)/arm/src/strnlen_user.c, $(LITEOS_SRCS))
LITEOS_SRCS := $(filter-out $(ARCH_SRCS)/arm/src/user_copy.c, $(LITEOS_SRCS))
endif

ifeq ($(LOSCFG_KERNEL_SMP), y)
LITEOS_AS += $(ARCH_SRCS)/arm/src/startup/reset_vector_mp.S
else
LITEOS_AS += $(ARCH_SRCS)/arm/src/startup/reset_vector_up.S
endif

ifeq ($(LOSCFG_ARCH_GIC_V2), y)
LITEOS_SRCS +=  $(ARCH_SRCS)/gic/gic_v2.c
else ifeq ($(LOSCFG_ARCH_GIC_V3), y)
LITEOS_SRCS +=  $(ARCH_SRCS)/gic/gic_v3.c
endif

# build platform
LITEOS_SRCS += $(wildcard $(cur_dir)platform/main.c)
#LITEOS_INC += -I$(LITEOSTOPDIR)/compat/posix/src \
                 -I$(LITEOSTOPDIR)/bsd/dev/random

LITEOS_SRCS += $(wildcard $(cur_dir)bounds_checking_function/src/*.c)
LITEOS_SRCS += $(wildcard $(cur_dir)kernel/common/*.c)
LITEOS_SRCS += $(wildcard $(cur_dir)kernel/base/core/*.c)
LITEOS_SRCS += $(wildcard $(cur_dir)kernel/base/mp/*.c)
LITEOS_SRCS += $(wildcard $(cur_dir)kernel/base/om/*.c)
LITEOS_SRCS += $(wildcard $(cur_dir)kernel/base/ipc/*.c)
LITEOS_SRCS += $(wildcard $(cur_dir)kernel/base/sched/sched_sq/*.c)
LITEOS_SRCS += $(cur_dir)kernel/base/mem/common/los_memstat.c
LITEOS_SRCS += $(cur_dir)kernel/base/mem/tlsf/los_memory.c
LITEOS_SRCS += $(cur_dir)kernel/base/mem/membox/los_membox.c
LITEOS_SRCS += $(cur_dir)kernel/base/misc/los_stackinfo.c
LITEOS_SRCS += $(cur_dir)kernel/base/misc/los_misc.c
LITEOS_SRCS += $(cur_dir)kernel/base/misc/mempt_shellcmd.c
LITEOS_SRCS += $(cur_dir)kernel/base/misc/task_shellcmd.c

LITEOS_SRCS += $(cur_dir)kernel/base/vm/los_vm_boot.c
ifeq ($(LOSCFG_KERNEL_CPPSUPPORT), y)
LITEOS_SRCS += $(cur_dir)kernel/extended/cppsupport/los_cppsupport.c
endif
ifeq ($(LOSCFG_KERNEL_CPUP), y)
LITEOS_SRCS += $(cur_dir)kernel/extended/cpup/los_cpup.c
endif

LITEOS_SRCS := $(filter-out $(cur_dir)kernel/common/los_rootfs.c, $(LITEOS_SRCS))
LITEOS_SRCS := $(filter-out $(cur_dir)kernel/common/los_seq_buf.c, $(LITEOS_SRCS))
ifneq ($(LOSCFG_FS_VFS), y)
LITEOS_SRCS := $(filter-out $(cur_dir)kernel/common/console.c $(cur_dir)kernel/common/virtual_serial.c, $(LITEOS_SRCS))
endif
ifneq ($(LOSCFG_BASE_CORE_HILOG), y)
LITEOS_SRCS := $(filter-out $(cur_dir)kernel/common/los_hilog.c, $(LITEOS_SRCS))
endif

ifeq ($(LOSCFG_KERNEL_VDSO), y)
LITEOS_INC += -I$(LITEOSTOPDIR)/kernel/extended/vdso/include
endif

# build config/board
UART_SRC    := $(cur_dir)config/board/drv
LITEOS_SRCS += $(wildcard $(UART_SRC)/*.c)
LITEOS_SRCS +=  $(cur_dir)config/board/platform.c
LITEOS_SRCS +=  $(cur_dir)config/board/os_adapt/os_adapt.c

# build kal
LITEOS_SRCS +=  $(cur_dir)kal/cmsis/cmsis_liteos.c
LITEOS_SRCS +=  $(cur_dir)kal/cmsis/cmsis_liteos2.c
LITEOS_INC += -I$(LITEOSTOPDIR)/kal/cmsis

STRIP_SRCS_AS = $(subst $(cur_dir),,$(LITEOS_AS))
obj-y += $(patsubst %.S, %.o, $(STRIP_SRCS_AS))
STRIP_SRCS = $(subst $(cur_dir),,$(LITEOS_SRCS))
obj-y += $(patsubst %.c, %.o, $(STRIP_SRCS))

ccflags-y += $(LITEOS_INC)
ccflags-y += -DLOSCFG_LIB_LIBC=1

KBUILD_AFLAGS += \
	-I$(LITEOSTOPDIR)/platform/include \
	-I$(LITEOSTOPDIR)/kernel/include \
	-I$(LITEOSTOPDIR)/kernel/common \
	-I$(LITEOSTOPDIR)/kernel/base/include \
	-I$(LITEOSTOPDIR)/arch/arm \
	-I$(LITEOSTOPDIR)/arch/arm/arm/include \
	-I$(LITEOSTOPDIR)/arch/arm/arm/src/include \
	-I$(LITEOSTOPDIR)/config/board \
	-imacros los_config.h
endif