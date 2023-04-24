CHIP		?= best2300a

DEBUG		?= 1

NOSTD		?= 1

LIBC_ROM	?= 1

PROGRAMMER	:= 1

init-y		:=
core-y		:= tests/ramrun_test/ platform/cmsis/ platform/hal/ platform/drivers/ana/

LDS_FILE	:= programmer.lds

KBUILD_CPPFLAGS += -Iplatform/cmsis/inc -Iplatform/hal -Iplatform/drivers/ana

KBUILD_CFLAGS +=

LIB_LDFLAGS +=

CFLAGS_IMAGE +=

LDFLAGS_IMAGE +=

