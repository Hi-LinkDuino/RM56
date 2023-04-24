cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := cmsis_nvic.o patch.o patch_armv7m.o retarget_armclang_asm.o retarget_armclang.o \
        system_ARMCM.o system_cp.o system_subsys.o system_utils.o \
		stack_protector.o dwt.o

ifneq ($(NUTTX_BUILD),1)
obj-y += retarget_gcc.o
endif

ifeq ($(CPU),m4)
ifeq ($(ARCH_ENHANCE),1)
obj-y += mpu_armv8m.o
else
obj-y += mpu_armv7m.o
endif
else
obj-y += mpu_armv8m.o
endif

ifeq ($(DSP_LIB),1)
obj-y += DSP_Lib/
endif

ifeq ($(NN_LIB),1)
obj-y += NN_Lib/
endif

ifneq ($(filter a%,$(CPU)),)
obj-y += ca/
endif

ccflags-y := -I$(obj)/inc -Iplatform/drivers/ana

FAULT_FLAGS :=
USAGE_FAULT ?= 1
ifeq ($(USAGE_FAULT),1)
FAULT_FLAGS += -DUSAGE_FAULT
endif
BUS_FAULT ?= 1
ifeq ($(BUS_FAULT),1)
FAULT_FLAGS += -DBUS_FAULT
endif
MEM_FAULT ?= 1
ifeq ($(MEM_FAULT),1)
FAULT_FLAGS += -DMEM_FAULT
endif
MONITOR_FAULT ?= 1
ifeq ($(MONITOR_FAULT),1)
FAULT_FLAGS += -DMONITOR_FAULT
endif
CFLAGS_system_ARMCM.o += $(FAULT_FLAGS)
CFLAGS_system_cp.o += $(FAULT_FLAGS)

ifeq ($(ACCURATE_DB_TO_FLOAT),1)
CFLAGS_system_utils.o += -DACCURATE_DB_TO_FLOAT
endif
ifeq ($(BOOT_CODE_IN_RAM),1)
CFLAGS_system_utils.o += -DBOOT_CODE_IN_RAM
endif

ifeq ($(CHIP_HAS_PSRAM),1)
CFLAGS_system_utils.o += -DCHIP_HAS_PSRAM
endif
ifeq ($(PSRAM_ENABLE),1)
CFLAGS_system_utils.o += -DPSRAM_ENABLE
endif
ifeq ($(CHIP_HAS_PSRAMUHS),1)
CFLAGS_system_utils.o += -DCHIP_HAS_PSRAMUHS
endif
ifeq ($(PSRAMUHS_ENABLE),1)
CFLAGS_system_utils.o += -DPSRAMUHS_ENABLE
endif

ifeq ($(INTSRAM_RUN),1)
CFLAGS_system_utils.o += -DINTSRAM_RUN
endif

ifeq ($(PSRAM_INIT_CHECK),1)
CFLAGS_system_utils.o += -DPSRAM_INIT_CHECK
endif
