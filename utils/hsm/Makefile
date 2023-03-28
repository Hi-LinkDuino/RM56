include $(srctree)/scripts/common.mk

HSM_LIB_NAME := libhsm

ifeq ($(USE_TRACE_ID), 1)
HSM_LIB_NAME := $(HSM_LIB_NAME)_log_compressed
endif

ifeq ($(DEV), 1)

$(HSM_LIB_NAME)-y := $(src_obj)

endif  #end ifeq ($(DEV), 1)

obj-y := $(HSM_LIB_NAME).a

