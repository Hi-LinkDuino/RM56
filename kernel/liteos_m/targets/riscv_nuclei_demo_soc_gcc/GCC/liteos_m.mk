LITEOSTOPDIR := ../../../
LITEOSTOPDIR := $(realpath $(LITEOSTOPDIR))

# Common
C_SOURCES     += $(wildcard $(LITEOSTOPDIR)/kernel/src/*.c) \
                 $(wildcard $(LITEOSTOPDIR)/kernel/src/mm/*.c) \
                 $(wildcard $(LITEOSTOPDIR)/components/cpup/*.c) \
                 $(wildcard $(LITEOSTOPDIR)/components/los_backtrace/*.c) \
                 $(wildcard $(LITEOSTOPDIR)/components/power/*.c) \
                 $(wildcard $(LITEOSTOPDIR)/utils/*.c)

C_INCLUDES    += -I$(LITEOSTOPDIR)/utils \
                 -I$(LITEOSTOPDIR)/kernel/include \
                 -I$(LITEOSTOPDIR)/components/cpup \
                 -I$(LITEOSTOPDIR)/components/los_backtrace \
                 -I$(LITEOSTOPDIR)/components/power

#third party related
C_INCLUDES    += -I$(LITEOSTOPDIR)/../../third_party/bounds_checking_function/include \
                 -I$(LITEOSTOPDIR)/../../third_party/bounds_checking_function/src

C_SOURCES     += $(wildcard $(LITEOSTOPDIR)/../../third_party/bounds_checking_function/src/*.c)

# NMSIS related
C_INCLUDES    += -I$(LITEOSTOPDIR)/arch/risc-v/nuclei/gcc/nmsis/Core/Include \
                 -I$(LITEOSTOPDIR)/arch/risc-v/nuclei/gcc/nmsis/DSP/Include \
                 -I$(LITEOSTOPDIR)/arch/risc-v/nuclei/gcc/nmsis/NN/Include

ASM_SOURCES   += $(wildcard $(LITEOSTOPDIR)/arch/risc-v/nuclei/gcc/*.s)

ASMS_SOURCES  += $(wildcard $(LITEOSTOPDIR)/arch/risc-v/nuclei/gcc/*.S)

C_SOURCES     += $(wildcard $(LITEOSTOPDIR)/arch/risc-v/nuclei/gcc/*.c)

C_INCLUDES    += -I. \
                 -I$(LITEOSTOPDIR)/arch/include \
                 -I$(LITEOSTOPDIR)/arch/risc-v/nuclei/gcc

ASFLAGS       += -imacros $(LITEOSTOPDIR)/kernel/include/los_config.h -DCLZ=CLZ

# list of ASM .S program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMS_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMS_SOURCES)))

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(ASFLAGS) $< -o $@
