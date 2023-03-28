
DSP_TFLITE_MICRO_LIB_NAME := lib-cortex-a7-tensorflow-microlite

MCU_TFLITE_MICRO_LIB_NAME := lib-cortex-m33-tensorflow-microlite

ifeq ($(DSP_TFLM_ENABLE),1)
obj-y += ${DSP_TFLITE_MICRO_LIB_NAME}.a
endif

ifeq ($(MCU_TFLM_ENABLE),1)
obj-y += ${MCU_TFLITE_MICRO_LIB_NAME}.a
endif

