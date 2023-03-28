cur_makefile := $(lastword $(MAKEFILE_LIST))

# ==========================================================================
# Building
# ==========================================================================

src := $(obj)

PHONY := __build
__build:

$(cur_makefile): ;

# Init all relevant variables used in kbuild files so
# 1) they have correct type
# 2) they do not inherit any value from the environment
obj-y :=
lib-y :=
subdir-y :=
archive-custom :=
archive-custom-valid :=
always :=
targets :=
EXTRA_AFLAGS   :=
EXTRA_CFLAGS   :=
EXTRA_CPPFLAGS :=
EXTRA_LDFLAGS  :=
asflags-y  :=
ccflags-y  :=
cppflags-y :=
ldflags-y  :=

subdir-asflags-y :=
subdir-ccflags-y :=

include scripts/include.mk

# For backward compatibility check that these variables do not change
save-cflags := $(CFLAGS)

kbuild-dir := $(if $(filter /%,$(src)),$(src),$(srctree)/$(src))
kbuild-file := $(kbuild-dir)/Makefile
ifeq ($(wildcard $(kbuild-file)),)
$(error "Makefile not found in $(KBUILD_SRC)/$(obj)")
endif
include $(kbuild-file)

$(kbuild-file): ;

# If the save-* variables changed error out
ifeq ($(KBUILD_NOPEDANTIC),)
        ifneq ("$(save-cflags)","$(CFLAGS)")
                $(error CFLAGS was changed in "$(kbuild-file)". Fix it to use ccflags-y)
        endif
endif

include scripts/lib.mk

ifeq ($(WIN_PLAT),y)
create_dir = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
else
create_dir = [ -d $(1) ] || mkdir -p $(1)
endif

ifneq ($(KBUILD_SRC),)
# Create output directory if not already present
_dummy := $(shell $(call create_dir,$(obj)))

# Create directories for object files if directory does not exist
# Needed when obj-y := dir/file.o syntax is used
_dummy := $(foreach d,$(obj-dirs), $(shell $(call create_dir,$(d))))
endif

ifndef obj
$(warning kbuild: build.mk is included improperly)
endif

# ===========================================================================

ifneq ($(strip $(lib-y) $(lib-m) $(lib-)),)
lib-target := $(obj)/lib.a
endif

ifneq ($(strip $(obj-y) $(obj-m) $(obj-) $(subdir-m) $(lib-target)),)
builtin-target := $(obj)/built-in$(built_in_suffix)
endif

submodgoals =
SUBMODS := $(filter $(obj)/%,$(SUBMODS))

ifneq ($(SUBMODS),)

lst_target := $(filter %.s %.i %.lst, $(SUBMODS))
ifeq ($(lib-target),)
ifneq ($(filter $(obj)/lib.lst,$(lst_target)),)
$(error Cannot make lib.lst: lib-target not exists)
endif
endif
ifeq ($(builtin-target),)
ifneq ($(filter $(obj)/built-in.lst,$(lst_target)),)
$(error Cannot make built-in.lst: builtin-target not exists)
endif
endif

lst_target_obj := $(lst_target)
lst_target_obj := $(filter-out $(obj)/built-in.lst $(obj)/lib.lst,$(lst_target_obj))
lst_target_obj := $(lst_target_obj:.s=.o)
lst_target_obj := $(lst_target_obj:.i=.o)
lst_target_obj := $(lst_target_obj:.lst=.o)
lst_target_obj := $(sort $(lst_target_obj))

cur_submods := $(filter-out $(lst_target),$(SUBMODS))
cur_submods += $(lst_target_obj)

include scripts/submods.mk

buildextra-y := $(filter $(cur_submods),$(extra-y))

ifeq ($(filter $(obj)/built-in.lst,$(lst_target)),)

# Just compile some of the object files
buildsubdir-y := $(call get_subdirs,$(subdir-y),$(cur_submods))
buildobj-y := $(filter $(cur_submods) $(addsuffix /built-in$(built_in_suffix),$(buildsubdir-y)),$(obj-y))
buildobj-y += $(foreach m,$(multi-used-y),$(if $(filter $(cur_submods),$(call get_multi_objs,$m)),$m,))

else

# Compile all the object files
buildsubdir-y := $(subdir-y)
buildobj-y := $(obj-y)

endif

ifeq ($(filter $(obj)/lib.lst,$(lst_target)),)
# Just compile some of the lib files
buildlib-y := $(filter $(cur_submods),$(lib-y))
else
# Compile all the lib files
buildlib-y := $(lib-y)
endif

# Check for invalid targets
errobj-y := $(cur_submods)
errobj-y := $(filter-out $(buildsubdir-y) $(addsuffix /%,$(buildsubdir-y)),$(errobj-y))
errobj-y := $(filter-out $(buildextra-y) $(buildobj-y) $(buildlib-y),$(errobj-y))
errobj-y := $(filter-out $(foreach m,$(multi-used-y),$(call get_multi_objs,$m)),$(errobj-y))
ifneq ($(errobj-y),)
$(warning Unknown targets in [$(obj)]: $(errobj-y))
endif

# Update lst targets
all_buildobj-y := $(buildextra-y) $(buildobj-y) $(filter-out %.a,$(builtin-target))
all_buildlib-y := $(filter %.a,$(builtin-target)) $(lib-target)
lst_target := $(filter $(lst_target), \
		$(all_buildobj-y:.o=.s) \
		$(all_buildobj-y:.o=.i) \
		$(all_buildobj-y:.o=.lst) \
		$(patsubst %.a,%.lst,$(all_buildlib-y)))

# Avoid .o compile error affecting .i generation
lst_i_target := $(filter %.i, $(lst_target))
ifneq ($(lst_i_target),)
$(lst_i_target:.i=.o) : | $(lst_i_target)
endif

ifeq ($(COMPILE_ONLY),1)
submodgoals = $(call get_submodgoals,$@,$(SUBMODS))
endif

else # SUBMODS

buildsubdir-y := $(subdir-y)
buildextra-y := $(extra-y)
buildobj-y := $(obj-y)
buildlib-y := $(lib-y)

buildobj-y	:= $(if $(filter-obj),$(filter-out $(filter-obj),$(buildobj-y)),$(buildobj-y))
endif # SUBMODS

ifeq ($(LIB_BIN_IN_SRC_DIR),1)
archive-bin-path := $(srctree)/$(src)/lib
else ifeq ($(LIB_BIN_IN_TOP_WITH_SRC_DIR),1)
archive-bin-path := $(srctree)/lib/$(src)
else
LIB_BIN_DIR ?= $(BES_LIB_DIR)
archive-bin-path := $(srctree)/$(LIB_BIN_DIR)
endif

archive-src-target :=
archive-custom-target :=
ifeq ($(GEN_LIB),1)
archive-src-target := $(archive-src-y:$(obj)/%=$(archive-bin-path)/%)
archive-src-target += $(archive-custom-valid)
archive-src-target := $(strip $(archive-src-target))
archive-custom-target := $(archive-custom-valid:$(obj)/%=$(archive-bin-path)/%)
endif

ifdef DEV
__build: $(builtin-target) $(lib-target) $(buildextra-y) $(buildsubdir-y) $(lst_target) $(archive-src-target) $(archive-custom-target) $(always) $(archive-src-y)
	@:
else
__build: $(builtin-target) $(lib-target) $(buildextra-y) $(buildsubdir-y) $(lst_target) $(archive-src-target) $(archive-custom-target) $(always)
	@:
endif

# Compile C sources (.c)
# ---------------------------------------------------------------------------

# Default is built-in, unless we know otherwise
quiet_modtag := $(empty)   $(empty)

quiet_cmd_cc_dummy = CC $(quiet_modtag)  $@
cmd_cc_dummy       = $(CC) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -c -x c -o $@ $(devnull)

# For dummy object files. Recipe is: $(call if_changed,cc_dummy)

quiet_cmd_cc_s_c = CC $(quiet_modtag)  $@
cmd_cc_s_c       = $(CC) $(c_flags) $(DISABLE_LTO) -fverbose-asm -S -o $@ $<

$(obj)/%.s: $(src)/%.c FORCE
	$(call if_changed_dep,cc_s_c)

quiet_cmd_cc_i_c = CPP $(quiet_modtag) $@
cmd_cc_i_c       = $(CPP) $(c_flags) -o $@ $<

$(obj)/%.i: $(src)/%.c FORCE
	$(call if_changed_dep,cc_i_c)

# C (.c) files
# The C file is compiled and updated dependency information is generated.
# (See cmd_cc_o_c + relevant part of rule_cc_o_c)

quiet_cmd_cc_o_c = CC $(quiet_modtag)  $@
cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<

define rule_cc_o_c
	$(call echo-cmd,cc_o_c) $(cmd_cc_o_c)
endef

# Built-in and composite module parts
$(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_rule,cc_o_c)

# Compile C++ sources (.cpp)
# ---------------------------------------------------------------------------

quiet_cmd_cc_s_c++ = C++ $(quiet_modtag) $@
cmd_cc_s_c++       = $(CC) $(c++_flags) $(DISABLE_LTO) -fverbose-asm -S -o $@ $<

$(obj)/%.s: $(src)/%.cpp FORCE
	$(call if_changed_dep,cc_s_c++)

$(obj)/%.s: $(src)/%.cc FORCE
	$(call if_changed_dep,cc_s_c++)

quiet_cmd_cc_i_c++ = CPP $(quiet_modtag) $@
cmd_cc_i_c++       = $(CPP) $(c++_flags) -o $@ $<

$(obj)/%.i: $(src)/%.cpp FORCE
	$(call if_changed_dep,cc_i_c++)

$(obj)/%.i: $(src)/%.cc FORCE
	$(call if_changed_dep,cc_i_c++)

# C++ (.cpp) files
# The C++ file is compiled and updated dependency information is generated.
# (See cmd_cc_o_c++ + relevant part of rule_cc_o_c)

quiet_cmd_cc_o_c++ = C++ $(quiet_modtag) $@
cmd_cc_o_c++       = $(C++) $(c++_flags) -c -o $@ $<

define rule_cc_o_c++
	$(call echo-cmd,cc_o_c++) $(cmd_cc_o_c++)
endef

# Built-in and composite module parts
$(obj)/%.o: $(src)/%.cpp FORCE
	$(call if_changed_rule,cc_o_c++)

$(obj)/%.o: $(src)/%.cc FORCE
	$(call if_changed_rule,cc_o_c++)

# Compile assembler sources (.S)
# ---------------------------------------------------------------------------

quiet_cmd_as_s_S = CPP $(quiet_modtag) $@
cmd_as_s_S       = $(CPP) $(a_cpp_flags) -o $@ $<

$(obj)/%.s: $(src)/%.S FORCE
	$(call if_changed_dep,as_s_S)

quiet_cmd_as_o_S = AS $(quiet_modtag)  $@
cmd_as_o_S       = $(CC) $(a_flags) -c -o $@ $<

$(obj)/%.o: $(src)/%.S FORCE
	$(call if_changed_dep,as_o_S)

targets += $(real-objs-y) $(real-objs-m) $(lib-y) $(lst_target)
targets += $(MAKECMDGOALS) $(always)

# Common list command
# ---------------------------------------------------------------------------

quiet_cmd_dump_lst_o = MKLST   $@
ifeq ($(TOOLCHAIN),armclang)
      cmd_dump_lst_o = $(OBJDUMP) --datasymbols --text -c -d --interleave=source --output=$@ $<
else
      cmd_dump_lst_o = $(OBJDUMP) -Sdlxr $< > $@
endif

lst-cmd = @$(call echo-cmd,dump_lst_o) $(cmd_dump_lst_o)

$(obj)/%.lst: $(obj)/%.o
	$(call lst-cmd)

# Build the compiled-in targets
# ---------------------------------------------------------------------------

# To build objects in subdirs, we need to descend into the directories
$(sort $(subdir-obj-y)): $(buildsubdir-y) ;

# Archive check

ifeq ($(CHECK_LIB_SRC),1)
ifneq ($(archive-bin-y),)
$(info )
$(info Library source check OK: $(archive-bin-y))
$(info )
endif

ifneq ($(archive-src-y),)
$(info )
$(foreach m, $(archive-src-y), \
    $(eval $(info $(m) :) \
    $(eval tmp_list=$(call get-archive-src,$(patsubst $(obj)/%,%,$(m)))) \
    $(eval $(foreach k, $(tmp_list), $(info $(empty)    $(k))))))
$(info )
$(error Error: The source files exist for libraries: $(archive-src-y))
endif
endif

# Archive without source files

      cmd_use_lib_file = $(call CMDCPFILE,$<,$@)
quiet_cmd_use_lib_file = USELIB  $(@)

ifneq ($(archive-bin-y),)

ifeq ($(GEN_LIB),1)
$(info )
$(warning WARNING: No source files found for libraries: $(archive-bin-y))
$(info )
endif

$(archive-bin-y): $(obj)/%: $(archive-bin-path)/% FORCE
ifeq ($(FORCE_TO_USE_LIB),1)
	$(call cmd,use_lib_file)
else
	$(call if_changed,use_lib_file)
endif

targets += $(archive-bin-y)

endif # archive-bin-y

# Archive generation (with source files)

ifeq ($(GEN_LIB),1)
ifeq ($(FORCE_TO_USE_LIB),1)
$(error Conflicted options: GEN_LIB and FORCE_TO_USE_LIB)
endif
ifneq ($(archive-src-target),)

      cmd_gen_lib_file = ( $(call create_dir,$(dir $(@))) ) && $(call archive-cmd,$(filter-out $(PHONY),$^))
quiet_cmd_gen_lib_file = GENLIB  $(@)

define archive-src-target-rule
$(1): $(addprefix $(obj)/,$(patsubst %/,%/$(notdir $(builtin-target)),$($(notdir $(1:.a=-y)))))
endef

$(foreach m, $(archive-src-target), $(eval $(call archive-src-target-rule,$(m))))

$(archive-src-target): FORCE
ifeq ($(CHECK_GEN_LIB_DEP),1)
	$(call if_changed,gen_lib_file)
else
# Always pack lib
	$(call cmd,gen_lib_file)
endif

targets += $(archive-src-target)

endif # archive-src-y
endif # GEN_LIB

#
# Rule to compile a set of .o files into one .o/.a built-in file
#
ifdef builtin-target

ifeq ($(BUILT-IN-OBJ),1)

quiet_cmd_link_o_target = LD      $@
# If the list of objects to link is empty, just create an empty built-in.o
ifeq ($(TOOLCHAIN),armclang)
      cmd_link_o_target = $(if $(strip $(obj-y)),\
              $(LD) -Wl$(comma)$(subst $(space),$(comma),$(strip $(ld_flags) --partial)) -nostdlib $(LINK_CFLAGS) -o $@ $(obj-y), \
              $(call CMDRMFILE,$@) && $(AR) --create --debug_symbols $@)
else
      cmd_link_o_target = $(if $(strip $(obj-y)),\
              $(LD) -Wl$(comma)$(subst $(space),$(comma),$(strip $(ld_flags) -r)) -nostdlib -nostartfiles $(LINK_CFLAGS) -o $@ $(obj-y), \
              $(call CMDRMFILE,$@) && $(AR) rcs$(KBUILD_ARFLAGS) $@)
endif

$(builtin-target): $(buildobj-y) FORCE
	$(call if_changed,link_o_target)

else # BUILT-IN-OBJ

quiet_cmd_ar_o_target = AR      $@
cmd_ar_o_target = $(call archive-cmd,$(obj-y))

# ifeq ($(builtin-target),services/built-in.a)
# $(info builtin-target=$(builtin-target))
# $(info cmd_ar_o_target=${cmd_ar_o_target})
# $(info obj-y=${obj-y})
# endif
$(builtin-target): $(buildobj-y) FORCE
	$(call if_changed,ar_o_target)

endif # BUILT-IN-OBJ

targets += $(builtin-target)

$(obj)/built-in.lst: $(builtin-target)
	$(call lst-cmd)

endif # builtin-target

#
# Rule to compile a set of .o files into one .a file
#

# 提供有源码且需要编译成库，则自动编译成库且拷贝到源码目录
ifdef DEV
ifneq ($(archive-src-y),)
#仿照lib-target导致子目录下的built-in.a不展开
#仿照builtin-target则不存在此问题
quiet_cmd_link_l_target = AR      $@
cmd_link_l_target = $(call CMDRMFILE,$@) && $(call archive-cmd,$(obj-y))

$(archive-src-y): $(obj-y) FORCE
	$(call if_changed,link_l_target)
ifeq ($(DEV),1)
	@mkdir -p $(srctree)/$(obj)/lib
#	@echo $(archive-src-y)  $(srctree)/$(obj)/lib/
	@$(call CMDCPFILE, $(archive-src-y), $(srctree)/$(obj)/lib/)
	@echo $(obj) >> $(srctree)/.release
endif

targets += $(archive-src-y)
tmp-debug-libs := libcp_boot lib_a7_dummy
ifneq ($(filter $(LIB_NAME),$(tmp-debug-libs)),)
# $(info archive-src-y=$(archive-src-y))
# $(info obj=$(obj))
# $(info obj-y=$(obj-y))
# $(info lib-y=$(lib-y))
# $(info buildlib-y=$(buildlib-y))
# $(info archive-src-target=$(archive-src-target))
# $(info targets=$(targets))
endif

endif # ifneq ($(archive-src-y),)
endif

ifdef lib-target

quiet_cmd_link_l_target = AR      $@
ifeq ($(TOOLCHAIN),armclang)
      cmd_link_l_target = $(call CMDRMFILE,$@) && $(AR) --create --debug_symbols $@ $(lib-y)
else
      cmd_link_l_target = $(call CMDRMFILE,$@) && $(AR) rcs$(KBUILD_ARFLAGS) $@ $(lib-y)
endif

$(lib-target): $(buildlib-y) FORCE
	$(call if_changed,link_l_target)

targets += $(lib-target)

$(obj)/lib.lst: $(lib-target)
	$(call lst-cmd)

endif

#
# Rule to link composite objects
#
#  Composite objects are specified in kbuild makefile as follows:
#    <composite-object>-objs := <list of .o files>
#  or
#    <composite-object>-y    := <list of .o files>
link_multi_deps =                     \
$(filter $(addprefix $(obj)/,         \
$($(subst $(obj)/,,$(@:.o=-objs)))    \
$($(subst $(obj)/,,$(@:.o=-y)))), $^)

quiet_cmd_link_multi-y = LD      $@
ifeq ($(TOOLCHAIN),armclang)
      cmd_link_multi-y = $(LD) -Wl,$(subst $(space),$(comma),$(strip $(ld_flags) --partial)) -nostdlib $(LINK_CFLAGS) -o $@ $(link_multi_deps)
else
      cmd_link_multi-y = $(LD) -Wl,$(subst $(space),$(comma),$(strip $(ld_flags) -r --whole-archive)) -nostdlib -nostartfiles $(LINK_CFLAGS) -o $@ $(link_multi_deps)
endif

$(multi-used-y): FORCE
	$(call if_changed,link_multi-y)
$(call multi_depend, $(multi-used-y), .o, -objs -y)

targets += $(multi-used-y)

$(multi-used-y:.o=.lst): %.lst : %.o
	$(call lst-cmd)

# Descending
# ---------------------------------------------------------------------------

PHONY += $(subdir-y)

$(subdir-y):
	$(Q)$(MAKE) $(build)=$@ $(submodgoals)

# Add FORCE to the prequisites of a target to force it to be always rebuilt.
# ---------------------------------------------------------------------------

PHONY += FORCE

FORCE:

# Read all saved command lines and dependencies for the $(targets) we
# may be building above, using $(if_changed{,_dep}). As an
# optimization, we don't need to read them if the target does not
# exist, we will rebuild anyway in that case.

targets := $(wildcard $(sort $(targets)))
cmd_files := $(wildcard $(foreach f,$(targets),$(call get_depfile_name,$(f))))

ifneq ($(cmd_files),)
include $(cmd_files)
$(cmd_files): ;
endif

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable se we can use it in if_changed and friends.

.PHONY: $(PHONY)

# Last-resort default rule for unknown targets

%::
	@$(call echo-help,*** Error: No rule is defined for target: $@)
	@exit 1

