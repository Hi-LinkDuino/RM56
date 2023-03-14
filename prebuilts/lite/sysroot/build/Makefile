# Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
# Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other materials
#    provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used
#    to endorse or promote products derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

ARCH = arm
ifneq ($(GCC),)
TARGET =
CROSS_COMPILE = $(GCC:%gcc=%)
CC = $(GCC) $(ARCH_CFLAGS)
MULTILIB = $(patsubst %.,%,$(shell $(CC) -print-multi-directory))
else
TARGET = $(ARCH)-liteos-ohos
CLANG ?= clang
CROSS_COMPILE = $(CLANG:%clang=%llvm-)
CC = $(CLANG) --target=$(TARGET) $(ARCH_CFLAGS)
MULTILIB = $(patsubst $(dir $(shell $(filter-out $(ARCH_CFLAGS),$(CC)) -print-libgcc-file-name))%,/%,$(dir $(shell $(CC) -print-libgcc-file-name)))
endif
MUSLBUILDDIR = build_$(or $(TARGET),$(ARCH))$(subst /,_,$(MULTILIB:%/=%))
HIDE = @
BUILD_DEBUG = false
SED_ARGS = -e '/install-libs:/s/if/and/g'

TOPDIR = $(shell pwd)/../../../..
MUSLDIR = $(TOPDIR)/third_party/musl
LINUXKERNELDIR = $(TOPDIR)/third_party/Linux_Kernel
OPTRTDIR = $(TOPDIR)/third_party/optimized-routines
NUTTXDIR = $(TOPDIR)/third_party/NuttX
SYSROOTDIR = $(TOPDIR)/prebuilts/lite/sysroot
LITEOSADIR = $(TOPDIR)/kernel/liteos_a
LINUXDIR = $(TOPDIR)/kernel/linux/linux-4.19
LINUXHDRDIR = $(PREBUILTLINUXHDRDIR)

TARGETS = $(if $(wildcard $(LITEOSADIR)),liteos_a_user,)
TARGETS += $(if $(wildcard $(LINUXDIR)),linux_user,)

define LINUX_TYPES_H
/* Auto generated file at $(shell date), do NOT edit! */
#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H
#include <stdint.h>
typedef uint32_t __u32, __le32;
#endif
endef
export LINUX_TYPES_H

ifeq ($(ARCH),arm)
ARCH_CFLAGS = -mfloat-abi=softfp -mcpu=cortex-a7 -mfpu=neon-vfpv4
else
$(warning *** warning: ARCH $(ARCH) has not been tested yet, use with cautions!)
ARCH_CFLAGS =
endif

CFLAGS = -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wl,-z,relro,-z,now,-z,noexecstack -Wl,--build-id=sha1

.PHONY: $(TARGETS:%=musl_copy_for_%)
.PHONY: $(TARGETS:%=musl_patch_for_%)
.PHONY: $(TARGETS:%=musl_install_for_%)
.PHONY: $(TARGETS:%=linux_header_install_for_%)
.PHONY: $(TARGETS:%=nuttx_header_install_for_%)
.PHONY: $(TARGETS:%=optimized_routines_install_for_%)
.PHONY: all clean distclean

all: $(TARGETS:%=musl_install_for_%)

$(TARGETS:%=musl_copy_for_%):
	$(HIDE) mkdir -p $@
	$(HIDE) cp -rfu $(MUSLDIR)/[^p]* $@

optimized_routines_install_for_liteos_a_user: musl_copy_for_liteos_a_user
ifneq ($(ARCH),)
	$(HIDE) cp -rfp $(OPTRTDIR)/string/$(ARCH)/* $</src/string/$(ARCH)/
	$(HIDE) cp -rfp $(OPTRTDIR)/string/asmdefs.h $</src/string/asmdefs.h
ifeq ($(ARCH),arm)
	$(HIDE) rm -f $</src/string/arm/memcpy.c
	$(HIDE) rm -f $</src/string/arm/memcpy_le.S
	$(HIDE) rm -f $</src/string/arm/memset.S
	$(HIDE) rm -f $</src/string/arm/strcpy.c
	$(HIDE) true >> $</src/string/arm/strlen.S

CFLAGS += \
	-D__strlen_armv6t2=strlen \
	-D__strcpy_arm=strcpy \
	-D__strcmp_arm=strcmp \
	-D__memcpy_arm=memcpy \
	-D__memchr_arm=memchr

endif
endif

nuttx_header_install_for_liteos_a_user: musl_copy_for_liteos_a_user
	$(HIDE) sed '/^#include/d' $(NUTTXDIR)/include/nuttx/video/fb.h | sed 's,FAR ,,g' | sed 's,LosVmMapRegion,void,g' > $</include/fb.h

linux_header_install_for_liteos_a_user: musl_copy_for_liteos_a_user
	$(HIDE) make -sj headers_install ARCH=$(ARCH) O=$(shell pwd)/$@ -C $(LINUXKERNELDIR)
	$(HIDE) install -p -m 644 -D $@/usr/include/linux/capability.h $</include/linux/capability.h
	$(HIDE) echo "$$LINUX_TYPES_H" > $</include/linux/types.h

musl_patch_for_liteos_a_user: nuttx_header_install_for_liteos_a_user
musl_patch_for_liteos_a_user: linux_header_install_for_liteos_a_user
musl_patch_for_liteos_a_user: optimized_routines_install_for_liteos_a_user
musl_patch_for_liteos_a_user: musl_copy_for_liteos_a_user
	$(HIDE) cp -rfp $(MUSLDIR)/porting/liteos_a/user/* $</

musl_patch_for_liteos_a_user_debug: musl_patch_for_liteos_a_user
musl_patch_for_liteos_a_user_debug: musl_copy_for_liteos_a_user
	$(HIDE) cp -rfp $(MUSLDIR)/porting/liteos_a/user_debug/* $</

ifeq ($(BUILD_DEBUG),true)
ifeq ($(GCC),)
musl_install_for_liteos_a_user: LDFLAGS = $(shell $(CC) -print-file-name=libunwind.a) -Wl,--no-dependent-libraries
musl_install_for_liteos_a_user: SED_ARGS += -e 's,$$(AR) rc $$@ $$(AOBJS)$$,cp $$(shell $$(CC) -print-file-name=libunwind.a) $$@ \&\& &,g'
endif
musl_install_for_liteos_a_user: SED_ARGS += -e 's,CFLAGS_ALL += -DCRT$$,& -fno-unwind-tables -fno-asynchronous-unwind-tables,g'
musl_install_for_liteos_a_user: CFLAGS += -funwind-tables -fasynchronous-unwind-tables -rdynamic -I $(shell $(CC) "-print-file-name=include")
musl_install_for_liteos_a_user: musl_patch_for_liteos_a_user_debug
endif

musl_install_for_liteos_a_user: musl_patch_for_liteos_a_user
	$(HIDE) cd musl_copy_for_liteos_a_user && mkdir -p $(MUSLBUILDDIR) && cd $(MUSLBUILDDIR) && \
	../configure --prefix=$(SYSROOTDIR)/usr --target=$(TARGET) \
		--includedir=$(SYSROOTDIR)/usr/include/$(TARGET) \
		--libdir=$(SYSROOTDIR)/usr/lib/$(TARGET)/$(MULTILIB) \
		$(if $(LDFLAGS),LDFLAGS="$(LDFLAGS)",) \
		CC="$(CC)" CROSS_COMPILE="$(CROSS_COMPILE)" CFLAGS="$(CFLAGS)" >/dev/null && \
	sed $(SED_ARGS) Makefile | make -f- -sj install

ifeq ($(wildcard $(LINUXHDRDIR)),)
LINUXHDRDIR = $(shell pwd)/linux_header_install_for_linux_user/usr/include
linux_header_install_for_linux_user:
	$(HIDE) make -sj headers_install ARCH=$(ARCH) O=$(shell pwd)/$@ -C $(LINUXDIR)
musl_patch_for_linux_user: linux_header_install_for_linux_user
endif

musl_patch_for_linux_user: musl_copy_for_linux_user
	$(HIDE) cp -rfp $(MUSLDIR)/porting/linux/user/* $</

musl_install_for_linux_user: CFLAGS += -I $(LINUXHDRDIR)
ifeq ($(GCC),)
musl_install_for_linux_user: TARGET = $(ARCH)-linux-ohos
endif
musl_install_for_linux_user: musl_patch_for_linux_user
	$(HIDE) cd musl_copy_for_linux_user && mkdir -p $(MUSLBUILDDIR) && cd $(MUSLBUILDDIR) && \
	../configure --prefix=$(SYSROOTDIR)/usr --target=$(TARGET) \
		--includedir=$(SYSROOTDIR)/usr/include/$(TARGET) \
		--libdir=$(SYSROOTDIR)/usr/lib/$(TARGET)/$(MULTILIB) \
		CC="$(CC)" CROSS_COMPILE="$(CROSS_COMPILE)" CFLAGS="$(CFLAGS)" >/dev/null && \
	sed $(SED_ARGS) Makefile | make -f- -sj install
	$(HIDE) cp -rfp $(LINUXHDRDIR)/* $(SYSROOTDIR)/usr/include/$(TARGET)
	$(HIDE) if [ -d $(LINUXHDRDIR)/asm-$(ARCH)/asm ]; then ln -snf asm-$(ARCH)/asm $(SYSROOTDIR)/usr/include/$(TARGET)/; fi

clean:
	$(HIDE) rm -rf musl_copy_for_* linux_header_install_for_*

distclean: clean
	$(HIDE) rm -rf $(SYSROOTDIR)/lib $(SYSROOTDIR)/usr
