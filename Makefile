
CONFIG_CROSS_COMPILE := arm-none-eabi-
CONFIG_STRICT_CFLAGS ?= y
CONFIG_SAVE_TARGET ?= n
CONFIG_FORCE_WIN_SHELL ?= y

ifneq ($(CONFIG_ARCH_BOARD),)
T := $(CONFIG_ARCH_BOARD)
export NUTTX_BUILD :=1
export KERNEL := NUTTX
export NUTTX_ROOT := $(TOPDIR)
NUTTX_LIBS := $(patsubst %,$(NUTTX_ROOT)/staging/%,$(LINKLIBS))
NUTTX_CFG_FILE := $(NUTTX_ROOT)/.config
else
export NUTTX_ROOT := $(srctree)/thirdparty/incubator-nuttx/nuttx
NUTTX_LIBS := $(filter-out $(NUTTX_ROOT)/staging/libproxies.a ,$(wildcard $(NUTTX_ROOT)/staging/*.a))
NUTTX_CFG_FILE := $(srctree)/config/$(T)/defconfig
endif

export CONFIG_STRICT_CFLAGS CONFIG_SAVE_TARGET CONFIG_FORCE_WIN_SHELL

# filter-out modules
ifeq ($(FORCE_TO_FILTER_MODULE),1)
filter-out-module :=
filter-out-file :=
endif
# ---------------------------------------------------------------------------
# Platform and shell detection

export WIN_PLAT := n
ifeq ($(OS),Windows_NT)
# Detect Cygwin
ifneq ($(findstring /,$(PWD)),/)
# also $(findstring /,$(HOME)) ?
WIN_PLAT := y
endif
endif

export WIN_SHELL := n
ifeq ($(WIN_PLAT),y)
ifeq ($(CONFIG_FORCE_WIN_SHELL),y)
WIN_SHELL := y
else
# Detect Unix-style shell
ifeq ($(shell echo $$0),$$0)
WIN_SHELL := y
endif
endif
endif

BACKSLASH := \ # backslash
BACKSLASH := $(strip $(BACKSLASH))

TO_UNIX_PATH = $(subst $(BACKSLASH),/,$(1))
TO_WIN_PATH = $(subst /,$(BACKSLASH),$(1))
ESC_WIN_PATH = $(subst $(BACKSLASH),$(BACKSLASH)$(BACKSLASH),$(1))

ifeq ($(WIN_PLAT),y)
ifeq ($(WIN_SHELL),y)
# make will choose sh.exe as SHELL if it finds sh.exe in the directories of PATH, regardless of
# the setting in environment or parent (e.g., when git.exe is in the PATH)
SHELL := cmd.exe
SHELL_CMD = $(call TO_WIN_PATH,$(1))
else
SHELL_CMD = $(call ESC_WIN_PATH,$(call TO_WIN_PATH,$(1)))
endif
else
SHELL_CMD = $(1)
endif

# The Unix-style path is recognized by compiler toolchain, GNU utilities and windows redirection
# operators, but not by windows native commands (e.g., mkdir) and applications.

# End of platform and shell detection
# ---------------------------------------------------------------------------

# Do not use make's built-in rules and variables
# (this increases performance and avoids hard-to-debug behaviour);
MAKEFLAGS += -rR

# Avoid funny character set dependencies
unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

# Avoid interference with shell env settings
unexport GREP_OPTIONS

# Check if just to show the help content
ifeq ($(MAKECMDGOALS),help)
ifeq ($(T),)
HELP_TARGET := 1
endif
endif

ifneq ($(HELP_TARGET),1)
# We are using a recursive build, so we need to do a little thinking
# to get the ordering right.
#
# Most importantly: sub-Makefiles should only ever modify files in
# their own directory. If in some directory we have a dependency on
# a file in another dir (which doesn't happen often, but it's often
# unavoidable when linking the built-in.o targets which finally
# turn into elf file), we will call a sub make in that other dir, and
# after that we are sure that everything which is in that other dir
# is now up to date.
#
# The only cases where we need to modify files which have global
# effects are thus separated out and done before the recursive
# descending is started. They are now explicitly listed as the
# prepare rule.

# Beautify output
# ---------------------------------------------------------------------------
#
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# If $(quiet) is empty, the whole command will be printed.
# If it is set to "quiet_", only the short version will be printed.
# If it is set to "silent_", nothing will be printed at all, since
# the variable $(silent_cmd_cc_o_c) doesn't exist.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# If KBUILD_VERBOSE equals 0 then the above command will be hidden.
# If KBUILD_VERBOSE equals 1 then the above command is displayed.
#
# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands

ifeq ("$(origin V)","command line")
  KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
  quiet :=
  Q :=
else
  quiet := quiet_
  Q := @
endif

# If the user is running make -s (silent mode), suppress echoing of
# commands

ifneq ($(filter 4.%,$(MAKE_VERSION)),) # make-4
ifneq ($(filter %s ,$(firstword x$(MAKEFLAGS))),)
  quiet=silent_
endif
else                                   # make-3.8x
ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
  quiet=silent_
endif
endif

export quiet Q KBUILD_VERBOSE

TARGET_CFG_FILE = config/$(T)/target.mk
TARGET_COMMON_FILE = config/common.mk

# To locate output files in a separate directory two syntaxes are supported.
# In both cases the working directory must be the root of the kernel src.
# 1) O=
# Use "make O=dir/to/store/output/files/"
#
# 2) Set KBUILD_OUTPUT
# Set the environment variable KBUILD_OUTPUT to point to the directory
# where the output files shall be placed.
# export KBUILD_OUTPUT=dir/to/store/output/files/
# make
#
# The O= assignment takes precedence over the KBUILD_OUTPUT environment
# variable.

# KBUILD_SRC is set on invocation of make in OBJ directory
# KBUILD_SRC is not intended to be used by the regular user (for now)
ifeq ($(KBUILD_SRC),)

export KBUILD_ROOT := $(CURDIR)

# OK, Make called in directory where kernel src resides
# Do we want to locate output files in a separate directory?

export KBUILD_OUTPUT := $(CURDIR)/out
ifeq ("$(origin O)","command line")
  KBUILD_OUTPUT := $(O)
endif

# Select target
ifeq ($(CONFIG_SAVE_TARGET),y)
ifeq ($(T),)
-include $(KBUILD_OUTPUT)/.config
T := $(strip $(T))
endif
endif
ifeq ($(T),)
$(error Please specify the target in the command line: T=<targetName>)
endif
ifeq ($(wildcard $(TARGET_CFG_FILE)),)
$(error Invalid target: T=$(T))
endif
export T

KBUILD_OUTPUT := $(KBUILD_OUTPUT)/$(T)

# That's our default target when none is given on the command line
PHONY := _all
_all:

# Cancel implicit rules on the config file
$(KBUILD_OUTPUT)/.config: ;

ifneq ($(KBUILD_OUTPUT),)
# Invoke a second make in the output directory, passing relevant variables
# check that the output directory actually exists
saved-output := $(KBUILD_OUTPUT)
ifeq ($(WIN_PLAT),y)
KBUILD_OUTPUT := $(subst /,\,$(KBUILD_OUTPUT))
KBUILD_OUTPUT := $(shell ( if not exist $(KBUILD_OUTPUT)\ mkdir $(KBUILD_OUTPUT) ) \
                         && cd $(KBUILD_OUTPUT) && cd)
KBUILD_OUTPUT := $(subst \,/,$(KBUILD_OUTPUT))
else
KBUILD_OUTPUT := $(shell mkdir -p $(KBUILD_OUTPUT) && cd $(KBUILD_OUTPUT) \
                         && /bin/pwd)
endif

$(if $(KBUILD_OUTPUT),, \
     $(error failed to create output directory "$(saved-output)"))

ifeq ($(CONFIG_SAVE_TARGET),y)
ifeq ($(WIN_PLAT),y)
_dummy := $(shell echo T := $(T)> $(KBUILD_OUTPUT)/../.config)
else
_dummy := $(shell echo "T := $(T)" > $(KBUILD_OUTPUT)/../.config)
endif
endif

PHONY += $(MAKECMDGOALS) sub-make

$(filter-out _all sub-make $(CURDIR)/Makefile, $(MAKECMDGOALS)) _all: sub-make
	@:

include $(CURDIR)/scripts/submods_init.mk

ifneq ($(filter allclean,$(MAKECMDGOALS)),)
ALLCLEAN := 1
export ALLCLEAN
endif

# Look for make include files relative to root of kernel src
MAKEFLAGS += --include-dir=$(CURDIR)

ifeq ($(WIN_PLAT),y)
START_TIME := $(shell echo %time%)
START_DATE_TIME := $(shell echo %date% %time%)
else
START_TIME := $(shell date +"%s.%N")
START_DATE_TIME := $(shell date +"%Y-%m-%d %T.%N")
endif
#$(info MAKECMDGOALS=$(MAKECMDGOALS))
sub-make: FORCE
	@echo MAKE START: $(START_DATE_TIME)
#	@echo $(Q)$(MAKE) -C $(KBUILD_OUTPUT) KBUILD_SRC=$(CURDIR) -f $(CURDIR)/Makefile $(filter-out _all sub-make,$(MAKECMDGOALS))
	$(Q)$(MAKE) -C $(KBUILD_OUTPUT) KBUILD_SRC=$(CURDIR) \
		-f $(CURDIR)/Makefile $(filter-out _all sub-make,$(MAKECMDGOALS))
ifeq ($(WIN_PLAT),y)
	@echo MAKE END: %date% %time%
ifneq ($(wildcard tools/timediff.bat),)
	@tools/timediff.bat "%time%" "$(START_TIME)"
endif
else
	@echo MAKE END: $$(date +"%Y-%m-%d %T.%N")
	@printf "MAKE TIME: %.2f seconds\n" $$(echo "$$(date +%s.%N) - $(START_TIME)" | bc)
endif

# Leave processing to above invocation of make
skip-makefile := 1
endif # ifneq ($(KBUILD_OUTPUT),)
endif # ifeq ($(KBUILD_SRC),)

# We process the rest of the Makefile if this is the final invocation of make
ifneq ($(skip-makefile),1)

# Do not print "Entering directory ...",
# but we want to display it when entering to the output directory
# so that IDEs/editors are able to understand relative filenames.
MAKEFLAGS += --no-print-directory

# If building an external module we do not care about the all: rule
# but instead _all depend on modules
PHONY += all
_all: all

ifeq ($(KBUILD_SRC),)
        # building in the source tree
        srctree := .
else
        ifeq ($(KBUILD_SRC)/,$(dir $(CURDIR)))
                # building in a subdirectory of the source tree
                srctree := ..
        else
                ifeq ($(KBUILD_SRC)/,$(dir $(patsubst %/,%,$(dir $(CURDIR)))))
                        srctree := ../..
                else
                        srctree := $(KBUILD_SRC)
                endif
        endif
endif
objtree		:= .
src		:= $(srctree)
obj		:= $(objtree)

VPATH		:= $(srctree)

export srctree objtree VPATH

BES_LIB_DIR ?= lib/bes
export BES_LIB_DIR

# Git revision
ifeq ($(WIN_PLAT),y)
GIT_REVISION := $(shell (where git >nul 2>&1) && (git rev-parse --short HEAD 2>nul))
else
GIT_REVISION := $(shell (which git >/dev/null 2>&1) && (git rev-parse --short HEAD 2>/dev/null))
endif

ifneq ($(GIT_REVISION),)
ifeq ($(WIN_PLAT),y)
GIT_REVISION := $(GIT_REVISION)$(shell (git diff --quiet && git diff --cached --quiet) >nul 2>&1 || echo -dirty)
else
GIT_REVISION := $(GIT_REVISION)$(shell (git diff --quiet && git diff --cached --quiet) >/dev/null 2>&1 || echo -dirty)
endif
endif


# Cross compiling and selecting different set of gcc/bin-utils
# ---------------------------------------------------------------------------
#
# When performing cross compilation for other architectures ARCH shall be set
# to the target architecture. (See arch/* for the possibilities).
# ARCH can be set during invocation of make:
# make ARCH=ia64
# Another way is to have ARCH set in the environment.
# The default ARCH is the host where make is executed.

# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
# CROSS_COMPILE can be set on the command line
# make CROSS_COMPILE=ia64-linux-
# Alternatively CROSS_COMPILE can be set in the environment.
# A third alternative is to store a setting in .config so that plain
# "make" in the configured kernel build directory always uses that.
# Default value for CROSS_COMPILE is not to prefix executables
# Note: Some architectures assign CROSS_COMPILE in their arch/*/Makefile
ARCH		?= arm
CROSS_COMPILE	?= $(CONFIG_CROSS_COMPILE:"%"=%)

# SHELL used by kbuild
ifneq ($(WIN_PLAT),y)
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
          else if [ -x /bin/bash ]; then echo /bin/bash; \
          else echo sh; fi ; fi)
endif

# Make variables (CC, etc...)
ifeq ($(TOOLCHAIN),armclang)
CC		= armclang --target=arm-arm-none-eabi
CPP		= $(CC) -E
AS		= $(CC)
C++		= $(CC)
LD		= $(CC)
AR		= armar
NM		= fromelf
STRIP	= fromelf
OBJCOPY	= fromelf
OBJDUMP	= fromelf
else
AS		= $(CROSS_COMPILE)as
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
C++		= $(CROSS_COMPILE)g++
LD		= $(CC)
#LD		= $(CROSS_COMPILE)ld
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
endif

AWK		= awk
PERL	= perl
PYTHON	= python

ifeq ($(WIN_PLAT),y)
TO_RCF	= $(PERL) $(subst /,\,$(srctree)/tools/bin2ascii.pl)
else
TO_RCF	= $(srctree)/tools/bin2ascii.pl
endif

KBUILD_CPPFLAGS :=

KBUILD_CFLAGS	:= -fno-common -fmessage-length=0 -Wall \
		   -fno-exceptions -ffunction-sections \
		   -fdata-sections -fomit-frame-pointer

# By default char on ARM platform is unsigned char, but char on x86 platform
# is signed char. To avoid porting issues, force char to be signed char
# on ARM platform.
KBUILD_CFLAGS	+= -fsigned-char

ifneq ($(TOOLCHAIN),armclang)
# 1) Avoid checking out-of-bound array accesses in a loop
#    (and unrolling/peeling/exiting the loop based on the check)
# 2) Avoid detecting paths dereferencing a NULL pointer
#    (and turning the problematic statement into a trap)
KBUILD_CFLAGS	+= -fno-aggressive-loop-optimizations \
		   -fno-isolate-erroneous-paths-dereference
endif

# Treat floating-point constants as float instead of double
ifeq ($(TOOLCHAIN),armclang)
KBUILD_CFLAGS	+= -cl-single-precision-constant -fshort-enums
else
KBUILD_CFLAGS	+= -fsingle-precision-constant
endif
KBUILD_CFLAGS	+= -Wdouble-promotion -Wfloat-conversion

# NOTE:
# In armclang -g == -gdwarf-4, and fromelf cannot interleave source in disassembly file when -gdwarf-4 or -O1/2/3 is specified.
KBUILD_CFLAGS	+= -g

#C_ONLY_FLAGS	:= -std=gnu89
C_ONLY_FLAGS	:= -std=gnu99

C++_ONLY_FLAGS	:= -std=gnu++14 -fno-rtti

KBUILD_AFLAGS   := -D__ASSEMBLY__

export ARCH CROSS_COMPILE AS LD CC
export CPP C++ AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK PERL PYTHON

export KBUILD_CPPFLAGS NOSTDINC_FLAGS OBJCOPYFLAGS LDFLAGS
export KBUILD_CFLAGS
export KBUILD_AFLAGS
export KBUILD_ARFLAGS
export C_ONLY_FLAGS C++_ONLY_FLAGS

# Files to ignore in find ... statements

export RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o    \
			  -name CVS -o -name .pc -o -name .hg -o -name .git \) \
			  -prune -o
export RCS_TAR_IGNORE := --exclude SCCS --exclude BitKeeper --exclude .svn \
			 --exclude CVS --exclude .pc --exclude .hg --exclude .git

# ===========================================================================
# Build targets only.

# Objects we will link into $(IMAGE_FILE) / subdirs we need to visit
init-y		:= init/
core-y		:= main/

LDS_FILE	:= best1000.lds

# Link flags for all LD processes
LINK_CFLAGS	:=
export LINK_CFLAGS

# Link flags for image only
LIB_LDFLAGS		:=
CFLAGS_IMAGE	:= -static
ifeq ($(TOOLCHAIN),armclang)
LDFLAGS_IMAGE	:= --no_locals
else
LDFLAGS_IMAGE	:= -X --no-wchar-size-warning
endif

include $(srctree)/scripts/include.mk

# Include target definitions
include $(srctree)/$(TARGET_CFG_FILE)
include $(srctree)/$(TARGET_COMMON_FILE)
include $(srctree)/config/addrmap.mk

$(srctree)/$(TARGET_CFG_FILE): ;
$(srctree)/$(TARGET_COMMON_FILE): ;

ifneq ($(filter-out %/,$(init-y) $(core-y)),)
$(error The object files cannot be linked at top level: $(filter-out %/,$(init-y) $(core-y)))
endif

# filter-out modules
ifeq ($(FORCE_TO_FILTER_MODULE),1)
filter-module := $(patsubst %/,%,$(filter-out-module))
export filter-module-pattern  := $(addsuffix %,$(filter-module))
core-y := $(filter-out $(filter-module-pattern),$(core-y))
filter-obj := $(patsubst %.c, %.o, $(filter-out-file))
filter-obj += $(patsubst %.S, %.o, $(filter-out-file))
filter-obj += $(patsubst %.cc, %.o, $(filter-out-file))
filter-obj += $(patsubst %.cpp, %.o, $(filter-out-file))
export filter-obj
endif

ifeq ($(TOOLCHAIN),armclang)
# Weak symbols must be put in the last library -- stupid armlink!
core-y += platform/cmsis/weak_sym_armclang/

# Entry objects
ifeq ($(entry-y),)
entry-y		+= utils/boot_struct/boot_struct.o
ifeq ($(ROM_BUILD),1)
entry-y		+= tests/rom/startup_ARMCM.o
ifneq ($(filter tests/rom/,$(core-y)),)
entry-y		+= tests/rom/export_fn_rom.o
endif
else # !ROM_BUILD
entry-y		+= platform/cmsis/retarget_armclang_asm.o
ifeq ($(PROGRAMMER),1)
entry-y		+= tests/programmer/sys_api_programmer.o
else
entry-y		+= platform/main/startup_main.o
endif
endif # !ROM_BUILD
endif
ifeq ($(filter %.o,$(entry-y)),)
$(error Entry objects must be defined in entry-y in target.mk)
endif
BAD_ENTRY_OBJS := $(filter-out %.o,$(entry-y))
ifneq ($(BAD_ENTRY_OBJS),)
$(error Only objects can be defined in entry-y in target.mk: $(BAD_ENTRY_OBJS))
endif
IMAGE_ENTRY := $(entry-y)
USE_ROM_ENTRY := 0
ifneq ($(ROM_IN_FLASH),1)
ifeq ($(ROM_BUILD),1)
USE_ROM_ENTRY := 1
endif
endif
ifeq ($(USE_ROM_ENTRY),1)
CFLAGS_IMAGE	+= -e Reset_Handler
else ifeq ($(PROGRAMMER),1)
CFLAGS_IMAGE	+= -e programmer_start
else
CFLAGS_IMAGE	+= -e __main
endif
endif

ifneq ($(NO_BUILDID),1)
ifneq ($(TOOLCHAIN),armclang)
LDFLAGS_IMAGE	+= --build-id
endif
endif
ifeq ($(CROSS_REF),1)
ifeq ($(TOOLCHAIN),armclang)
LDFLAGS_IMAGE	+= --xref
else
LDFLAGS_IMAGE	+= --cref
endif
endif

REAL_LDS_FILE := $(LDS_FILE)
ifeq ($(TOOLCHAIN),armclang)
SCATTER_LDS_SUFFIX := _scat
ifeq ($(filter %$(SCATTER_LDS_SUFFIX),$(LDS_FILE)),)
REAL_LDS_FILE := $(LDS_FILE)$(SCATTER_LDS_SUFFIX)
endif
endif

# Generate REVISION_INFO (might be defined in target)
ifeq ($(REVISION_INFO),)
ifeq ($(CUST_TGT_INFO),)
REVISION_INFO := $(GIT_REVISION):$(T)
else
REVISION_INFO := $(GIT_REVISION):$(CUST_TGT_INFO)
endif
endif

REVISION_INFO := $(subst $(space),-,$(strip $(REVISION_INFO)))
SOFTWARE_VERSION := $(subst $(space),-,$(strip $(SOFTWARE_VERSION)))

$(info -------------------------------)
$(info REVISION_INFO: $(REVISION_INFO))
$(info -------------------------------)

# Build host and user info
ifeq ($(WIN_PLAT),y)
export BUILD_HOSTNAME := $(COMPUTERNAME)
export BUILD_USERNAME := $(USERNAME)
else
export BUILD_HOSTNAME := $(shell hostname -s)
export BUILD_USERNAME := $(shell id -un)
endif

BUILD_HOSTNAME := $(subst $(space),-,$(strip $(BUILD_HOSTNAME)))
BUILD_USERNAME := $(subst $(space),-,$(strip $(BUILD_USERNAME)))

# Default kernel image to build when no specific target is given.
# IMAGE_FILE may be overruled on the command line or
# set in the environment
IMAGE_FILE ?= $(notdir $(T)).elf

ifneq ($(filter .map .bin .hex .lst,$(suffix $(IMAGE_FILE))),)
$(error Invalid IMAGE_FILE (conflicted suffix): $(IMAGE_FILE))
endif

LST_SECTION_OPTS :=
LST_SECTION_NAME :=
ifneq ($(LST_ONLY_SECTION),)
ifeq ($(TOOLCHAIN),armclang)
LST_SECTION_OPTS += $(foreach m,$(subst $(comma),$(space),$(LST_ONLY_SECTION)),--only=$m )
else
LST_SECTION_OPTS += $(foreach m,$(subst $(comma),$(space),$(LST_ONLY_SECTION)),-j $m )
endif
LST_SECTION_NAME := _$(subst *,+,$(subst !,-,$(LST_ONLY_SECTION)))
endif
ifneq ($(LST_RM_SECTION),)
ifeq ($(TOOLCHAIN),armclang)
LST_SECTION_OPTS += $(foreach m,$(subst $(comma),$(space),$(LST_RM_SECTION)),--ignore_section=$m )
else
LST_SECTION_OPTS += $(foreach m,$(subst $(comma),$(space),$(LST_RM_SECTION)),-R $m )
endif
LST_SECTION_NAME := $(LST_SECTION_NAME)_no_$(subst *,+,$(subst !,-,$(LST_RM_SECTION)))
endif

IMAGE_MAP := $(addsuffix .map,$(basename $(IMAGE_FILE)))
IMAGE_BIN := $(addsuffix .bin,$(basename $(IMAGE_FILE)))
STR_BIN   := $(addsuffix .str,$(basename $(IMAGE_FILE)))
IMAGE_HEX := $(addsuffix .hex,$(basename $(IMAGE_FILE)))
IMAGE_RCF := $(addsuffix .rcf,$(basename $(IMAGE_FILE)))
ifeq ($(LST_SECTION_OPTS),)
IMAGE_LST := $(addsuffix .lst,$(basename $(IMAGE_FILE)))
else
IMAGE_LST := $(addsuffix $(LST_SECTION_NAME).lst,$(basename $(IMAGE_FILE)))
endif
append_lst_sec_name = $(addsuffix $(LST_SECTION_NAME)$(suffix $(1)),$(basename $(1)))

IMAGE_LIB := lib$(addsuffix .a,$(basename $(IMAGE_FILE)))
ifeq ($(LST_SECTION_OPTS),)
IMAGE_LIB_LST := $(addsuffix .lst,$(basename $(IMAGE_LIB)))
else
IMAGE_LIB_LST := $(addsuffix $(LST_SECTION_NAME).lst,$(basename $(IMAGE_LIB)))
endif

IMAGE_CMSE_LIB := $(addsuffix _CMSE_Lib.o,$(basename $(IMAGE_FILE)))

LDS_TARGET := _$(notdir $(REAL_LDS_FILE))

IMAGE_VER  := build_info.o

targets := $(LDS_TARGET) $(IMAGE_FILE) $(IMAGE_BIN) $(STR_BIN) $(IMAGE_RCF) $(IMAGE_LST) $(IMAGE_VER) $(IMAGE_LIB)
cmd_files := $(wildcard $(foreach f,$(targets),$(call get_depfile_name,$(f))))

ifneq ($(cmd_files),)
include $(cmd_files)
$(cmd_files): ;
endif

##
ifdef DEV
BUILD_DEV := 0
GEN_SUBMOD := 0
ifneq ($(KBUILD_SRC),)
BUILD_DEV := 1
ifneq ($(SUBMODS),)
GEN_SUBMOD := 1
endif
endif #ifneq ($(KBUILD_SRC),)
endif #ifdef DEV

# The all: target is the default when no target is given on the
# command line.
# This allow a user to issue only 'make' to build a kernel including modules
# Defaults to $(IMAGE_BIN)

ifeq ($(GEN_SUBMOD),1)
all: $(SUBMODS)
else

ifeq ($(BUILD_DEV),1)

__tempall := buildinfo

ifeq ($(LITTLEFS_ENABLE)_$(LFS_ON_FLASH)_$(T),1_1_best2600w_aiot)
__tempall += fsgen
endif

__tempall += $(IMAGE_BIN)

ifeq ($(TRACE_STR_SECTION),1)
__tempall += $(STR_BIN)
endif


# ifeq ($(T),best2600w_aiot)
__tempall += besfd_dist
# endif


all: $(__tempall) ;


else

ifeq ($(TRACE_STR_SECTION),1)
all: $(IMAGE_BIN) $(STR_BIN) ;
else
all: $(IMAGE_BIN) ;
endif #TRACE_STR_SECTION

endif #BUILD_DEV

endif #GEN_SUBMOD

ifeq ($(TOOLCHAIN),armclang)
      cmd_gen-IMAGE_BIN = $(OBJCOPY) --bincombined -o $@ $<
else
      cmd_gen-IMAGE_BIN = $(OBJCOPY) -R .trc_str -O binary $< $@
endif
quiet_cmd_gen-IMAGE_BIN = GENBIN  $@

$(IMAGE_BIN): $(IMAGE_FILE)
ifneq ($(filter 1,$(COMPILE_ONLY) $(NO_BIN)),)
	@:
else
	+$(call if_changed,gen-IMAGE_BIN)
endif
#	@$(srctree)/tools/dist.sh $(T)


ifeq ($(TOOLCHAIN),armclang)
      cmd_gen-STR_BIN = $(OBJCOPY) --bincombined -o $@ $<
else
      cmd_gen-STR_BIN = $(OBJCOPY) -j .code_start_addr -j .rodata_str  -j .trc_str \
          --change-section-lma .code_start_addr=0x00000000 \
          --change-section-lma .rodata_str=0x00000010 \
          --change-section-lma .trc_str=0x00030000 \
          -O binary $< $@
endif
quiet_cmd_gen-STR_BIN = GENBIN  $@

$(STR_BIN): $(IMAGE_FILE)
ifneq ($(filter 1,$(COMPILE_ONLY) $(NO_BIN)),)
	@:
else
	+$(call if_changed,gen-STR_BIN)
endif

ifneq ($(TOOLCHAIN),armclang)
      cmd_gen-IMAGE_HEX = $(OBJCOPY) -O ihex $< $@
quiet_cmd_gen-IMAGE_HEX = GENHEX  $@

$(IMAGE_HEX): $(IMAGE_FILE)
ifeq ($(COMPILE_ONLY),1)
	@:
else
	+$(call if_changed,gen-IMAGE_HEX)
endif
endif

PHONY += rcf
rcf: $(IMAGE_RCF) ;

ifeq ($(filter 1,$(ROM_BUILD) $(PROGRAMMER)),)
ifeq ($(CHIP_SUBSYS),)
IMAGE_IN_FLASH := 1
endif
endif
ifneq ($(filter 1,$(ROM_IN_FLASH) $(PROGRAMMER_INFLASH) $(BTH_AS_MAIN_MCU)),)
IMAGE_IN_FLASH := 1
endif
ifeq ($(IMAGE_IN_FLASH),1)
ifeq ($(RAM_SIMU_FLASH),1)
RCF_OPTIONS := 4 -flash
else
RCF_OPTIONS := 1 -flash
endif
endif

      cmd_gen-IMAGE_RCF = $(TO_RCF) $< $@ $(RCF_OPTIONS)
quiet_cmd_gen-IMAGE_RCF = GENRCF  $@

$(IMAGE_RCF): $(IMAGE_BIN)
	+$(call if_changed,gen-IMAGE_RCF)

PHONY += lst lst_only
lst lst_only: $(IMAGE_LST) ;

PHONY += lib_lst lib_lst_only
lib_lst lib_lst_only: $(IMAGE_LIB_LST) ;

ifneq ($(filter lst_only lib_lst_only,$(MAKECMDGOALS)),)
NO_COMPILE := 1
endif

ifeq ($(TOOLCHAIN),armclang)
      cmd_gen-IMAGE_LST = $(OBJDUMP) $(LST_SECTION_OPTS) --datasymbols --text -c -d --output=$@ $<
else
ifeq ($(LST_SECTION_OPTS),)
      cmd_gen-IMAGE_LST = $(OBJDUMP) -Sldx $< > $@
else
      cmd_gen-IMAGE_LST = $(OBJCOPY) $(LST_SECTION_OPTS) $< $(call append_lst_sec_name,$<) && $(OBJDUMP) -Sldx $(call append_lst_sec_name,$<) > $@
endif
endif
quiet_cmd_gen-IMAGE_LST = GENLST  $@

$(IMAGE_LST): $(IMAGE_FILE)
	+$(call if_changed,gen-IMAGE_LST)

$(IMAGE_LIB_LST): $(IMAGE_LIB)
	+$(call if_changed,gen-IMAGE_LST)

# Flags

# arch Makefile may override CC so keep this after arch Makefile is included
#ifeq ($(CONFIG_STRICT_CFLAGS),y)
#NOSTDINC_FLAGS += -nostdinc
#endif
#NOSTDINC_FLAGS += -isystem "$(subst \,/,$(shell $(CC) -print-file-name=include))"

ifeq ($(CONFIG_STRICT_CFLAGS),y)
# warn about C99 declaration after statement
#C_ONLY_FLAGS    += -Wdeclaration-after-statement

# disallow errors like 'EXPORT_GPL(foo);' with missing header
C_ONLY_FLAGS   += -Werror=implicit-int

# require functions to have arguments in prototypes, not empty 'int foo()'
#C_ONLY_FLAGS    += -Werror=strict-prototypes

ifneq ($(NUTTX_BUILD),1)
C_ONLY_FLAGS    += -Werror-implicit-function-declaration
endif

# Prohibit date/time macros, which would make the build non-deterministic
#KBUILD_CFLAGS   += $(call cc-option,-Werror=date-time)

ifneq ($(TOOLCHAIN),armclang)
KBUILD_CFLAGS   += $(call cc-option,-Wlogical-op)
KBUILD_CFLAGS   += $(call cc-option,-Wimplicit-fallthrough)
endif

#KBUILD_CFLAGS   += -Wno-address-of-packed-member

KBUILD_CFLAGS	+= -Wno-trigraphs \
		   -fno-strict-aliasing

# Never change loops to C library calls like memcpy/memset
# (otherwise it will be enabled by -O2 in GCC 10)
ifneq ($(TOOLCHAIN),armclang)
KBUILD_CFLAGS	+= -fno-tree-loop-distribute-patterns
endif

#KBUILD_CFLAGS	+= Wundef

# use the deterministic mode of AR if available
KBUILD_ARFLAGS := D

include $(srctree)/scripts/extrawarn.mk
endif # CONFIG_STRICT_CFLAGS

ifeq ($(TOOLCHAIN),armclang)
KBUILD_CFLAGS += -Wno-typedef-redefinition
endif

# Add user supplied CPPFLAGS, AFLAGS and CFLAGS as the last assignments
KBUILD_CPPFLAGS += $(KCPPFLAGS)
KBUILD_AFLAGS += $(KAFLAGS)
KBUILD_CFLAGS += $(KCFLAGS)

IMAGE-dirs	:= $(patsubst %/,%,$(filter %/, $(init-y) $(core-y)))

submodgoals =
ifneq ($(SUBMODS),)
include $(srctree)/scripts/submods.mk

IMAGE-builddirs	:= $(call get_subdirs,$(IMAGE-dirs),$(SUBMODS))
ifeq ($(COMPILE_ONLY),1)
submodgoals = $(call get_submodgoals,$@,$(SUBMODS))
endif
else
IMAGE-builddirs	:= $(IMAGE-dirs)
endif

IMAGE-alldirs	:= $(sort $(IMAGE-dirs) $(patsubst %/,%,$(filter %/, \
			$(init-) $(core-) $(extra-))))

init-y		:= $(patsubst %/, %/built-in$(built_in_suffix), $(init-y))
core-y		:= $(patsubst %/, %/built-in$(built_in_suffix), $(core-y))

IMAGE_INIT := $(init-y)
IMAGE_MAIN := $(core-y)

ifeq ($(ARM_CMNS),1)
ifneq ($(NUTTX_BUILD),1)
IMAGE_MAIN += ../$(ARM_CMSE_TARGET)/$(ARM_CMSE_TARGET)_CMSE_Lib.o
endif
endif

ifeq ($(NO_COMPILE),1)
IMAGE-deps :=
else
IMAGE-deps := $(LDS_TARGET) $(IMAGE_INIT) $(IMAGE_MAIN) $(IMAGE_VER)
endif

BUILD_INFO_FLAGS := \
	-DREVISION_INFO=$(REVISION_INFO) \
	-DFLASH_SIZE=$(FLASH_SIZE) \
	-DOTA_UPGRADE_CRC_LOG_SIZE=$(OTA_UPGRADE_CRC_LOG_SIZE) \
	-DNV_REC_DEV_VER=$(NV_REC_DEV_VER) \
	-I$(srctree)/platform/hal

BUILD_INFO_FLAGS += $(LDS_SECTION_FLAGS)
BUILD_INFO_FLAGS += -DCHIP=$(CHIP)

ifneq ($(HW_VERSION_STRING),)
BUILD_INFO_FLAGS += -DHW_VERSION_STRING=$(HW_VERSION_STRING)
endif

ifneq ($(CHIP_SUBTYPE),)
BUILD_INFO_FLAGS += -DCHIP_SUBTYPE=$(CHIP_SUBTYPE)
endif
ifneq ($(SOFTWARE_VERSION),)
BUILD_INFO_FLAGS += -DSOFTWARE_VERSION=$(SOFTWARE_VERSION)
endif
ifneq ($(OTA_BOOT_SIZE),)
BUILD_INFO_FLAGS += -DOTA_BOOT_OFFSET=$(OTA_BOOT_OFFSET) -DOTA_BOOT_SIZE=$(OTA_BOOT_SIZE)
endif

ifneq ($(OTA_CODE_OFFSET),)
BUILD_INFO_FLAGS += -DOTA_CODE_OFFSET=$(OTA_CODE_OFFSET)
endif
ifneq ($(OTA_REMAP_OFFSET),)
BUILD_INFO_FLAGS += -DOTA_REMAP_OFFSET=$(OTA_REMAP_OFFSET)
endif
ifeq ($(CRC32_OF_IMAGE),1)
BUILD_INFO_FLAGS += -DCRC32_OF_IMAGE
endif
ifeq ($(TRACE_CRLF),1)
BUILD_INFO_FLAGS += -DTRACE_CRLF
endif

BUILD_INFO_FLAGS += -DKERNEL=$(KERNEL)

quiet_cmd_image_ver = CC      $(IMAGE_VER)
      cmd_image_ver = $(CC) $(filter-out -Werror=date-time, \
                             $(call flags,KBUILD_CPPFLAGS) \
                             $(call flags,KBUILD_CFLAGS) \
                             $(call flags,C_ONLY_FLAGS) \
                             $(NOSTDINC_FLAGS)) \
                             $(BUILD_INFO_FLAGS) \
                             -MD -MP -MF $(depfile) -MT $@ \
                             -c -o $@ $<

IMAGE_VER_SRC := $(src)/utils/build_info/build_info.c

$(IMAGE_VER): $(IMAGE_VER_SRC) $(filter-out $(IMAGE_VER),$(IMAGE-deps)) FORCE
	$(call if_changed_dep,image_ver)

# Linker scripts preprocessor (.lds.S -> .lds)
# ---------------------------------------------------------------------------
quiet_cmd_cpp_lds_S = LDS     $@
      cmd_cpp_lds_S = $(CPP) $(call flags,KBUILD_CPPFLAGS) \
                             $(call flags,CPPFLAGS_$(LDS_FILE)) \
                             -MD -MP -MF $(depfile) -MT $@ \
                             $(NOSTDINC_FLAGS) \
                             -P -C -E -x c -o $@ $<

LDS_SRC_STEM := $(src)/scripts/link/$(REAL_LDS_FILE)
LDS_SRC := $(firstword $(wildcard $(LDS_SRC_STEM).S $(LDS_SRC_STEM).sx) $(LDS_SRC_STEM).S)

$(LDS_TARGET): $(LDS_SRC) FORCE
	$(call if_changed_dep,cpp_lds_S)

PHONY += lds
lds: $(LDS_TARGET) ;

ifeq ($(NUTTX_BUILD),1)
KBUILD_CPPFLAGS += -fno-common -DNUTTX_BUILD -D__NUTTX_SUPPORT__ -DFREE_WDATA_IN_HOOK
NUTTX_INCLUDE :=  \
	-I$(NUTTX_ROOT)/include/libcxx \
	-I$(NUTTX_ROOT)/include\

#NUTTX_INCLUDE += -include $(srctree)/platform/hal/hal_trace.h
C++_ONLY_FLAGS += -DCONFIG_WCHAR_BUILTIN $(NUTTX_INCLUDE) -std=c++17 -D__NuttX__ -nostdinc++ -fpermissive
C_ONLY_FLAGS += $(NUTTX_INCLUDE)
CPPFLAGS_$(LDS_FILE) += -I$(NUTTX_ROOT)/include
endif

# Final link of $(IMAGE_FILE)
# ---------------------------------------------------------------------------
#
# 1) Link the archives twice to solve circular references between two or
#    more archives. Otherwise we should use --start-group and --end-group
#    options. Normally, an archive is searched only once in the order that
#    it is specified on the command line.
# 2) Use --whole-archive option to solve weak symbol overriding issue.
#    It tells LD to include every object file in the archive in the link,
#    rather than searching the archive for the required object files.
#    By default the strong symbols defined in the archive will not override
#    any weak symbol, for LD only searches the archive if there is a undefined
#    symbol (and a weak symbol is considered as a defined symbol).
#
ifeq ($(TOOLCHAIN),armclang)

#LDFLAGS_IMAGE += --symbols --list_mapping_symbols
ifeq ($(KBUILD_VERBOSE),1)
LDFLAGS_IMAGE += --verbose
endif
ifeq ($(ARM_CMSE),1)
LDFLAGS_IMAGE += --import-cmse-lib-out=$(IMAGE_CMSE_LIB)
endif

      cmd_link-IMAGE_FILE = $(LD) -o $@ \
	      $(CFLAGS_IMAGE) \
	      -Wl,$(subst $(space),$(comma),$(strip \
	      $(LDFLAGS) $(LDFLAGS_IMAGE) \
	      --scatter=$(LDS_TARGET) \
	      --list=$(IMAGE_MAP) \
	      --info=summarysizes --info=summarystack --info=totals --info=unused \
	      --map --load_addr_map_info \
	      --remove --no_autoat \
	      --emit_debug_overlay_relocs --emit_debug_overlay_section \
	      --diag_style=gnu --diag_suppress=L6314 --diag_suppress=L6329)) \
	      $(IMAGE_ENTRY) $(IMAGE_INIT) $(IMAGE_MAIN) $(IMAGE_VER) \
	      $(LIB_LDFLAGS) $(LIB_LDFLAGS)

else # TOOLCHAIN != armclang

ifeq ($(ARM_CMSE),1)
LDFLAGS_IMAGE += --cmse-implib --out-implib=$(IMAGE_CMSE_LIB)
endif

      cmd_link-IMAGE_FILE = $(LD) -o $@ \
		  $(LD_USE_PATCH_SYMBOL) \
	      -T $(LDS_TARGET) \
	      $(CFLAGS_IMAGE) \
	      -Wl,$(subst $(space),$(comma),$(strip \
	      $(LDFLAGS) $(LDFLAGS_IMAGE) \
	      -Map=$(IMAGE_MAP) \
	      --gc-sections \
	      --whole-archive)) \
	      $(NUTTX_LIBS) $(IMAGE_INIT) $(IMAGE_MAIN) $(IMAGE_VER) \
	      -Wl,--no-whole-archive $(LIB_LDFLAGS) $(LIB_LDFLAGS)

endif # TOOLCHAIN != armclang
quiet_cmd_link-IMAGE_FILE = LINK    $@

# Include targets which we want to
# execute if the rest of the kernel build went well.
$(IMAGE_FILE): $(IMAGE-deps) $(NUTTX_LIBS) FORCE
ifneq ($(filter 1,$(COMPILE_ONLY) $(NO_COMPILE)),)
	@:
else
	+$(call if_changed,link-IMAGE_FILE)
endif

PHONY += lib
lib: $(IMAGE_LIB) ;

      cmd_gen_image_lib = $(call archive-cmd,$(IMAGE_INIT) $(IMAGE_MAIN) $(IMAGE_VER))
quiet_cmd_gen_image_lib = IMGLIB  $(@)

$(IMAGE_LIB): $(IMAGE-deps) $(NUTTX_LIBS) FORCE
ifneq ($(filter 1,$(COMPILE_ONLY) $(NO_COMPILE)),)
	@:
else
	@$(call if_changed,gen_image_lib)
endif

ifneq ($(IMAGE-deps),)
# The actual objects are generated when descending,
# make sure no implicit rule kicks in
$(sort $(filter %/built-in$(built_in_suffix),$(IMAGE-deps))): $(IMAGE-builddirs) ;
endif

# Handle descending into subdirectories listed in $(IMAGE-dirs)
# Preset locale variables to speed up the build process. Limit locale
# tweaks to this spot to avoid wrong language settings when running
# make menuconfig etc.
# Error messages still appears in the original language

PHONY += $(IMAGE-dirs)
$(IMAGE-dirs): scripts
	$(Q)$(MAKE) $(build)=$@ $(submodgoals)

# clean - Delete most, but leave enough to build external modules
#
clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)
ifneq ($(SUBMODS),)
clean-dirs      := $(addprefix _clean_, $(IMAGE-builddirs))
else
clean-dirs      := $(addprefix _clean_, $(IMAGE-alldirs))
endif

PHONY += $(clean-dirs) clean IMAGE-clean distclean subdir_clean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

IMAGE-clean:
	$(Q)$(call CMDRMFILE,$(IMAGE_FILE) $(IMAGE_MAP) \
		$(IMAGE_BIN) $(STR_BIN) $(IMAGE_HEX) $(IMAGE_RCF) $(IMAGE_LST) \
		$(addsuffix *$(suffix $(IMAGE_FILE)),$(basename $(IMAGE_FILE))) \
		$(addsuffix *$(suffix $(IMAGE_LST)),$(basename $(IMAGE_LST))) \
		$(IMAGE_LIB) $(IMAGE_LIB_LST) $(call append_lst_sec_name,$(IMAGE_LIB)) \
		$(addsuffix *$(suffix $(IMAGE_LIB)),$(basename $(IMAGE_LIB))) \
		$(addsuffix *$(suffix $(IMAGE_LIB_LST)),$(basename $(IMAGE_LIB_LST))) \
		$(IMAGE_CMSE_LIB) \
		$(IMAGE_VER) $(LDS_TARGET))

distclean: clean
subdir_clean: clean

clean: IMAGE-clean

clean: $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
ifeq ($(SUBMODS),)
	$(Q)$(call CMDRMFILER,.,*.o *.a *.s *.d)
endif

PHONY += allclean
ifeq ($(KBUILD_OUTPUT),)
allclean: clean ;
else
ifeq ($(SUBMODS),)
quiet_cmd_allclean  = RMDIR   $(KBUILD_OUTPUT)
      cmd_allclean  = $(call CMDRMDIR,$(KBUILD_OUTPUT))

allclean:
	+$(call cmd,allclean)
else
allclean: clean ;
endif
endif

PHONY += libclean
quiet_cmd_libclean	= RMDIR   $(srctree)/$(BES_LIB_DIR)
      cmd_libclean  = $(call CMDRMDIR,$(srctree)/$(BES_LIB_DIR))

libclean:
	+$(call cmd,libclean)

quiet_cmd_predefined-macros = GEN     $@
      cmd_predefined-macros = $(CPP) $(filter-out -I% -D% -include%,$(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) $(C_ONLY_FLAGS)) \
                                     -x c -E -dM -o $@ $(devnull)

PREDEF_MACRO_FILE := predefined-macros.txt

$(PREDEF_MACRO_FILE): FORCE
	$(call cmd,predefined-macros)

PHONY += predefined-macros
predefined-macros: $(PREDEF_MACRO_FILE) ;

# FIXME Should go into a make.lib or something
# ===========================================================================

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = $(if $(wildcard $(rm-dirs)),$(call CMDRMDIR,$(rm-dirs)))

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = $(if $(wildcard $(rm-files)),$(call CMDRMFILE,$(rm-files)))

# Shorthand for $(Q)$(MAKE) -f scripts/clean.mk obj=dir
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean := -f $(srctree)/scripts/clean.mk obj

ifneq ($(WIN_PLAT),y)
# Generate tags for editors
# ---------------------------------------------------------------------------
quiet_cmd_tags = GEN     $@
      cmd_tags = $(CONFIG_SHELL) $(srctree)/tools/tags.sh $@

tags TAGS cscope gtags: FORCE
	$(call cmd,tags)
endif

HELP_TARGET := 2

endif # ifneq ($(skip-makefile),1)
endif # ifneq ($(HELP_TARGET),1)

# Help target
ifneq ($(HELP_TARGET),)
ifeq ($(HELP_TARGET),1)
include scripts/include.mk
endif

help: FORCE
	$(call echo-help,Mandatory options:)
	$(call echo-help,  T=<targetBoard> - Select a target board configuration in config/)
	$(call echo-help,)
	$(call echo-help,Cleaning targets:)
	$(call echo-help,  clean           - Remove most generated files)
	$(call echo-help,  allclean        - Remove all generated files and the output directory if possible)
	$(call echo-help,)
	$(call echo-help,Generic targets:)
	$(call echo-help,  all             - Build all targets marked with [*])
	$(call echo-help,  lst             - Build the mixed source/assembly file of the final image)
	$(call echo-help,  lds             - Build the linker script file)
ifeq ($(HELP_TARGET),2)
	$(call echo-help,* $(IMAGE_FILE))
	$(call echo-help,                  - Build the final image)
endif
	$(call echo-help,  dir/            - Build all files in dir and below)
	$(call echo-help,  dir/file.[oisS] - Build specified target only)
	$(call echo-help,  dir/file.lst    - Build specified mixed source/assembly target only)
	$(call echo-help,                    (requires a recent binutils and recent build (System.map)))
	$(call echo-help,)
	$(call echo-help,  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build)
	$(call echo-help,  make V=2   [targets] 2 => give reason for rebuild of target)
	$(call echo-help,  make O=dir [targets] Locate all output files in "dir", including .config)
	$(call echo-help,  make W=n   [targets] Enable extra gcc checks, n=1,2,3 where)
	$(call echo-help,         1: warnings which may be relevant and do not occur too often)
	$(call echo-help,         2: warnings which occur quite often but may still be relevant)
	$(call echo-help,         3: more obscure warnings, can most likely be ignored)
	$(call echo-help,         Multiple levels can be combined with W=12 or W=123)
	$(call echo-help,)
	$(call echo-help,Execute "make" or "make all" to build all targets marked with [*])

endif # ifneq ($(HELP_TARGET),)

# Cancel implicit rules on top Makefile
ifeq ($(KBUILD_SRC),)
$(CURDIR)/Makefile Makefile: ;
else
$(KBUILD_SRC)/Makefile Makefile: ;
endif

PHONY += FORCE
FORCE: ;

# Debug makefile variables
ifneq ($(skip-makefile),1)

ifeq ($(VAR_VAL_QUOTE),1)
val_quote_l := [
val_quote_r := ]
else
val_quote_l :=
val_quote_r :=
endif

debug_var_list := $(patsubst get_var_%,%,$(filter get_var_%,$(MAKECMDGOALS)))
ifneq ($(debug_var_list),)
# Avoid expanding functions (e.g., shell functions) and recipe automatic variables
$(foreach v,$(debug_var_list),$(if $(filter $(v),$(.VARIABLES)),\
	$(info $(v)=$(val_quote_l)$(value $(v))$(val_quote_r)),\
	$(info $(v) is not set)))
endif

ifneq ($(filter get_all_var,$(MAKECMDGOALS)),)
# Avoid expanding functions (e.g., shell functions) and recipe automatic variables
$(foreach c,$(sort $(.VARIABLES)),$(info $(c)=$(val_quote_l)$(value $(c))$(val_quote_r)))
endif

get_var_%: FORCE ;
get_all_var: FORCE ;
endif

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.

ifeq ($(BUILD_DEV),1)

ifeq ($(GEN_SUBMOD),1)
#PHONY := $(SUBMODS)
# $(info SUBMODS=$(SUBMODS))

$(SUBMODS): FORCE
	@echo Make module: $@
	$(Q)$(MAKE) $(build)=$@ $(submodgoals)
#	@if [ -f $@/Makefile ]; then $(Q)$(MAKE) $(build)=$@ $(submodgoals); else echo Makefile not found in $(KBUILD_SRC)/$@; fi;
#	@$(if $(wildcard $@/Makefile), $(Q)$(MAKE) $(build)=$@ $(submodgoals), echo Makefile not found in $(KBUILD_SRC)/$@)

endif

# $(info KBUILD_CPPFLAGS=$(KBUILD_CPPFLAGS))
# $(info C++_ONLY_FLAGS=$(C++_ONLY_FLAGS))
# $(info KBUILD_CFLAGS=$(KBUILD_CFLAGS))
# $(info C_ONLY_FLAGS=$(C_ONLY_FLAGS))
# $(info KBUILD_AFLAGS=$(KBUILD_AFLAGS))


my_c++flags=$(filter-out -I% -D% -U%,$(C++_ONLY_FLAGS) $(KBUILD_CFLAGS) $(KBUILD_CPPFLAGS))
# $(info my_c++flags=$(my_c++flags))
# $(info --------------------)

my_cflags=$(filter-out -I% -D% -U%,$(C_ONLY_FLAGS) $(KBUILD_CFLAGS) $(KBUILD_CPPFLAGS))
# $(info my_cflags=$(my_cflags))
# $(info --------------------)

my_aflags=$(KBUILD_AFLAGS) $(filter-out -I% -D% -U%,$(KBUILD_CPPFLAGS))
# $(info my_aflags=$(my_aflags))
# $(info --------------------)

genflags: FORCE ;
#	@echo c++flags=$(my_c++flags)
#	@echo ------------------------------
#	@echo cflags=$(my_cflags)
#	@echo ------------------------------
#	@echo aflags=$(my_aflags)
#	@echo ------------------------------
	@echo BESCPPFLAGS=-MD -MP -MF -MT $(my_c++flags) > cppflags
	@echo BESCCFLAGS=-MD -MP -MF -MT $(my_cflags) > cflags
	@echo BESASFLAGS=-MD -MP -MF -MT $(my_aflags) > asflags
	@echo [ok] flags for $(T) generated in out/$(T): cppflags.d/cflags.d/asflags.d


ifeq ($(LITTLEFS_ENABLE)$(LFS_ON_FLASH),11)
fsgen: FORCE
	@echo Generate out/$(T)/fs.bin
	@$(srctree)/tools/mklittlefs -c $(srctree)/fsdata/mcu -d 0 -b 4096 -s $(LFS_FLASH_TOTAL_SIZE) $(srctree)/out/$(T)/fs.bin
endif

out_build_info=.buildinfo
buildinfo: FORCE
	@echo Generating out/$(T)/$(out_build_info)
	@echo CHIP=$(CHIP) > $(out_build_info)
ifdef CHIP_SUBTYPE
	@echo CHIP_SUBTYPE=$(CHIP_SUBTYPE)>> $(out_build_info)
endif
	@echo KERNEL=$(KERNEL)>> $(out_build_info)
ifdef SOFTWARE_VERSION
	@echo SOFTWARE_VERSION=$(SOFTWARE_VERSION)>> $(out_build_info)
endif
ifdef OTA_BOOT_SIZE
	@echo SW_TYPE=OTA>> $(out_build_info)
	@echo OTA_BOOT_OFFSET=$(OTA_BOOT_OFFSET)>> $(out_build_info)
	@echo OTA_BOOT_SIZE=$(OTA_BOOT_SIZE)>> $(out_build_info)
else
	@echo NV_REC_DEV_VER=$(NV_REC_DEV_VER)>> $(out_build_info)

	@echo BOOT1_SIZE=$(BOOT1_SIZE)>> $(out_build_info)
	@echo BOOT2_SIZE=$(BOOT2_SIZE)>> $(out_build_info)
	@echo BOOTINFO_SIZE=$(BOOTINFO_SIZE)>> $(out_build_info)

	@echo COMBO_SECTION_SIZE=$(COMBO_SECTION_SIZE)>> $(out_build_info)
	@echo PROMPT_SECTION_SIZE=$(PROMPT_SECTION_SIZE)>> $(out_build_info)
	@echo HOTWORD_SECTION_SIZE=$(HOTWORD_SECTION_SIZE)>> $(out_build_info)
	@echo CORE_DUMP_SECTION_SIZE=$(CORE_DUMP_SECTION_SIZE)>> $(out_build_info)
	@echo OTA_UPGRADE_LOG_SIZE=$(OTA_UPGRADE_LOG_SIZE)>> $(out_build_info)
	@echo LOG_DUMP_SECTION_SIZE=$(LOG_DUMP_SECTION_SIZE)>> $(out_build_info)
	@echo CRASH_DUMP_SECTION_SIZE=$(CRASH_DUMP_SECTION_SIZE)>> $(out_build_info)
	@echo CUSTOM_PARAMETER_SECTION_SIZE=$(CUSTOM_PARAMETER_SECTION_SIZE)>> $(out_build_info)
	@echo LHDC_LICENSE_SECTION_SIZE=$(LHDC_LICENSE_SECTION_SIZE)>> $(out_build_info)
	@echo USERDATA_SECTION_SIZE=$(USERDATA_SECTION_SIZE)>> $(out_build_info)
	@echo AUD_SECTION_SIZE=$(AUD_SECTION_SIZE)>> $(out_build_info)
	@echo RESERVED_SECTION_SIZE=$(RESERVED_SECTION_SIZE)>> $(out_build_info)
	@echo FACTORY_SECTION_SIZE=$(FACTORY_SECTION_SIZE)>> $(out_build_info)

	@echo COMBO_SECTION_OFFSET=$(COMBO_SECTION_OFFSET)>> $(out_build_info)
	@echo PROMPT_SECTION_OFFSET=$(PROMPT_SECTION_OFFSET)>> $(out_build_info)
	@echo HOTWORD_SECTION_OFFSET=$(HOTWORD_SECTION_OFFSET)>> $(out_build_info)
	@echo CORE_DUMP_SECTION_OFFSET=$(CORE_DUMP_SECTION_OFFSET)>> $(out_build_info)
	@echo OTA_UPGRADE_LOG_OFFSET=$(OTA_UPGRADE_LOG_OFFSET)>> $(out_build_info)
	@echo LOG_DUMP_SECTION_OFFSET=$(LOG_DUMP_SECTION_OFFSET)>> $(out_build_info)
	@echo CRASH_DUMP_SECTION_OFFSET=$(CRASH_DUMP_SECTION_OFFSET)>> $(out_build_info)
	@echo CUSTOM_PARAMETER_SECTION_OFFSET=$(CUSTOM_PARAMETER_SECTION_OFFSET)>> $(out_build_info)
	@echo LHDC_LICENSE_SECTION_OFFSET=$(LHDC_LICENSE_SECTION_OFFSET)>> $(out_build_info)
	@echo USERDATA_SECTION_OFFSET=$(USERDATA_SECTION_OFFSET)>> $(out_build_info)
	@echo AUD_SECTION_OFFSET=$(AUD_SECTION_OFFSET)>> $(out_build_info)
	@echo RESERVED_SECTION_OFFSET=$(RESERVED_SECTION_OFFSET)>> $(out_build_info)
	@echo FACTORY_SECTION_OFFSET=$(FACTORY_SECTION_OFFSET)>> $(out_build_info)

endif
	@echo SECURE_BOOT=$(SECURE_BOOT)>> $(out_build_info)
	@echo FLASH_TYPE=$(FLASH_TYPE)>> $(out_build_info)
	@echo FLASH_BASE=0x2C000000>> $(out_build_info)
	@echo FLASH_NC_BASE=0x28000000>> $(out_build_info)
	@echo FLASH_SIZE=$(FLASH_SIZE)>> $(out_build_info)
ifdef LFS_ON_FLASH
	@echo LFS_ON_FLASH=$(LFS_ON_FLASH)>> $(out_build_info)
ifdef LFS_FLASH_BASE_ADDR
	@echo LFS_FLASH_BASE_ADDR=$(LFS_FLASH_BASE_ADDR)>> $(out_build_info)
endif
ifdef LFS_FLASH_TOTAL_SIZE
	@echo LFS_FLASH_TOTAL_SIZE=$(LFS_FLASH_TOTAL_SIZE)>> $(out_build_info)
endif
endif
ifdef OTA_CODE_OFFSET
	@echo OTA_CODE_OFFSET=$(OTA_CODE_OFFSET)>> $(out_build_info)
endif
ifdef OTA_CODE_SIZE
	@echo OTA_CODE_SIZE=$(OTA_CODE_SIZE)>> $(out_build_info)
endif

ifdef HW_VERSION_STRING
	@echo HW_VERSION=$(HW_VERSION_STRING)>> $(out_build_info)
endif
	@echo REVISION_INFO=$(REVISION_INFO)>> $(out_build_info)


besfd_dist: $(IMAGE_BIN)
	@$(srctree)/tools/dist.sh $(T)

endif # ifeq ($(BUILD_DEV),1)


.PHONY: $(PHONY)

