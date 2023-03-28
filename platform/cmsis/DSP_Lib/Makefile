cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += BasicMathFunctions/
obj-y += CommonTables/
obj-y += FilteringFunctions/
obj-y += MatrixFunctions/
obj-y += SupportFunctions/
obj-y += StatisticsFunctions/
obj-y += TransformFunctions/

subdir-ccflags-y := -Iinclude/cmsis_dsp -Iinclude/cmsis_dsp/PrivateInclude
subdir-ccflags-y += -DARM_MATH_LOOPUNROLL

ifeq ($(SCO_CP_ACCEL),1)
cmsis_dsp_lib-y := $(obj-y)
obj-y := cmsis_dsp_lib.o
endif

