cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

ifeq ($(RSA_PKCS1_PSS_PADDING),1)
ccflags-y  += -DRSA_PKCS1_PSS_PADDING
endif

ifeq ($(SEC_ENG_ROM_ONLY),1)
ccflags-y += -DSEC_ENG_ROM_ONLY
endif

CFLAGS_rsa.o += -O2
#ccflags-y += -O2
