obj-y := aud_section/
obj-y += customparam_section/
obj-y += log_section/
obj-y += factory_section/
obj-y += factory_efuse/
obj-y += userdata_section/


ifdef DEV

LIB_NAME := libnv_section

ifeq ($(DEV),1)
$(LIB_NAME)-y := $(obj-y)
endif

obj-y := $(LIB_NAME).a

endif  #DEV