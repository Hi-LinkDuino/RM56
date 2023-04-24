make_file := $(abspath $(lastword $(MAKEFILE_LIST)))
os_path   := $(dir $(make_file))

#LITEOS_SRCS += $(wildcard $(os_path)components/bounds_check/*.c)
LITEOS_SRCS += $(wildcard $(os_path)kal/cmsis/*.c)

ifeq ($(MODULE_KERNEL_STUB),1)
subdir-ccflags-y += -DMODULE_KERNEL_STUB
else
ifeq ($(OS_HAVE_POSIX),1)
LITEOS_SRCS += $(os_path)kal/posix/src/errno.c
LITEOS_SRCS += $(os_path)kal/posix/src/libc.c
LITEOS_SRCS += $(os_path)kal/posix/src/mqueue.c
#LITEOS_SRCS += $(os_path)kal/posix/src/poll.c
LITEOS_SRCS += $(os_path)kal/posix/src/pthread_attr.c
LITEOS_SRCS += $(os_path)kal/posix/src/pthread_cond.c
LITEOS_SRCS += $(os_path)kal/posix/src/pthread_mutex.c
LITEOS_SRCS += $(os_path)kal/posix/src/pthread.c
LITEOS_SRCS += $(os_path)kal/posix/src/semaphore.c
LITEOS_SRCS += $(os_path)kal/posix/src/signal.c
LITEOS_SRCS += $(os_path)kal/posix/src/time.c

LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/__ctype_get_mb_cur_max.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/isalnum.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/isascii.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/isdigit.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/islower.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/isprint.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/isspace.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/isupper.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/isxdigit.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/tolower.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/ctype/toupper.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/errno/strerror.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/exit/abort.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/exit/assert.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/exit/atexit.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/internal/floatscan.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/internal/intscan.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/internal/shgetc.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/locale/__lctrans.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/locale/c_locale.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/locale/langinfo.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/misc/dirname.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/misc/realpath.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/multibyte/internal.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/multibyte/mbtowc.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/multibyte/wcrtomb.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/multibyte/wctomb.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/network/h_errno.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/network/htonl.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/network/htons.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/network/ntohl.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/network/ntohs.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/prng/rand.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/prng/random.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/regex/regcomp.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/regex/regexec.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/regex/tre-mem.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__fdopen.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__fmodeflags.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__lockfile.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__overflow.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__stdio_close.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__stdio_read.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__stdio_seek.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__stdio_write.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__stdout_write.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__toread.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__towrite.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/__uflow.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/clearerr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fclose.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/feof.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fflush.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fgets.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fileno.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fopen.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fputc.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fputs.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fread.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fseek.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/ftell.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/fwrite.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/ofl.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/ofl_add.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/perror.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/remove.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/rewind.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/snprintf.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/stderr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/stdin.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/stdout.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/vfprintf.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/vsnprintf.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/vsprintf.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/abs.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/atof.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/atoi.c

#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/strtol.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/locale/iconv.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdio/ungetc.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/atol.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/atoll.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/bsearch.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/llabs.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/stdlib/strtod.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/memchr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/memcmp.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/memcpy.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/memmove.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/memrchr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/memset.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/stpcpy.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strcasecmp.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strcat.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strchr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strchrnul.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strcmp.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strcspn.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strdup.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strlen.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strncasecmp.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strncmp.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strncpy.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strnlen.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strrchr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strspn.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strstr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/strtok.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/wcschr.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/string/wcslen.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/__month_to_secs.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/__secs_to_tm.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/__tm_to_secs.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/__tz.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/__year_to_secs.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/asctime_r.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/asctime.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/ctime.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/strftime.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/time/strptime.c

LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/__fpclassify.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/__fpclassifyl.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/__signbit.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/__signbitl.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/exp_data.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/frexp.c
#LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/frexpl.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/log.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/log_data.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/pow.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/pow_data.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/round.c
LITEOS_SRCS += $(os_path)kal/posix/musl/src/math/sqrt.c

subdir-ccflags-y += -D__USE_NEWLIB__
subdir-ccflags-y += -D_BSD_SOURCE
endif
LITEOS_SRCS += $(wildcard $(os_path)utils/*.c)
LITEOS_SRCS += $(wildcard $(os_path)components/bounds_checking_function/src/*.c)
LITEOS_SRCS += $(wildcard $(os_path)components/signal/*.c)
LITEOS_SRCS += $(wildcard $(os_path)components/exchook/*.c)
LITEOS_SRCS += $(wildcard $(os_path)components/cpup/*.c)
LITEOS_SRCS += $(wildcard $(os_path)components/backtrace/*.c)
LITEOS_SRCS += $(wildcard $(os_path)components/power/*.c)
LITEOS_SRCS += $(wildcard $(os_path)kernel/src/*.c)
LITEOS_SRCS += $(wildcard $(os_path)kernel/src/mm/*.c)

# trustzone build
ifeq ($(ARM_CMNS),1)
LITEOS_SRCS += $(wildcard $(os_path)arch/arm/cortex-m33/gcc/TZ/non_secure/*.c)
LITEOS_AS   += $(wildcard $(os_path)arch/arm/cortex-m33/gcc/TZ/non_secure/*.S)
LITEOS_SRCS += $(wildcard $(os_path)arch/arm/cortex-m33/gcc/TZ/secure/*.c)
LITEOS_AS   += $(wildcard $(os_path)arch/arm/cortex-m33/gcc/TZ/secure/*.S)
else
LITEOS_SRCS += $(wildcard $(os_path)arch/arm/cortex-m33/gcc/NTZ/*.c)
LITEOS_AS   += $(wildcard $(os_path)arch/arm/cortex-m33/gcc/NTZ/*.S)
endif
STRIP_SRCS_AS = $(subst $(os_path),,$(LITEOS_AS))
obj-y += $(patsubst %.S, %.o, $(STRIP_SRCS_AS))
endif

STRIP_SRCS = $(subst $(os_path),,$(LITEOS_SRCS))

obj-y += $(patsubst %.c, %.o, $(STRIP_SRCS))
obj-y += board.o

# include extern kernel header
ifeq ($(MODULE_KERNEL_STUB_INC),1)
EXTERN_ROOT_PATH ?= ./../../../../../
${info "EXTERN_ROOT_PATH:$(EXTERN_ROOT_PATH)"}
INC_DIR  = -I$(EXTERN_ROOT_PATH)kernel/liteos_m/kernel/include/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/utils/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/kernel/arch/include/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/arch/include/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/kernel/arch/arm/cortex-m33/gcc/NTZ/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/arch/arm/cortex-m33/gcc/NTZ/
INC_DIR  += -I$(EXTERN_ROOT_PATH)third_party/bounds_checking_function/include/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/components/exchook/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/components/backtrace/
INC_DIR  += -I$(EXTERN_ROOT_PATH)kernel/liteos_m/components/power/
ccflags-y += $(INC_DIR)
else
INC_DIR  = kernel/include
# trustzone build
ifeq ($(ARM_CMNS),1)
INC_DIR += arch/arm/cortex-m33/gcc/TZ/non_secure/
INC_DIR += arch/arm/cortex-m33/gcc/TZ/secure/
else
INC_DIR += arch/arm/cortex-m33/gcc/NTZ/
endif


INC_DIR += arch/include
INC_DIR += kal
INC_DIR += kal/cmsis
INC_DIR += utils
INC_DIR += components/bounds_checking_function/include
INC_DIR += components/signal/
INC_DIR += components/exchook
ifeq ($(OS_HAVE_POSIX),1)
INC_DIR += kal/posix/include/
INC_DIR += kal/posix/musl/src/include/
INC_DIR += kal/posix/musl/src/internal/
INC_DIR += kal/posix/musl/include/
INC_DIR += kernel/arch/include/
INC_DIR += kernel/include/
INC_DIR += utils/
INC_DIR += ../../../include/rtos/liteos/liteos_m/kal/posix/include
endif
INC_DIR += components/cpup
INC_DIR += components/backtrace
INC_DIR += components/power
ccflags-y += $(addprefix -I${os_path},${INC_DIR})
endif

ccflags-y += -I${os_path}../../../platform/hal/$(CHIP)

AFLAGS_los_dispatch.o += -D__FPU_PRESENT=1 -D__FPU_USED=1
AFLAGS_los_exc.o += -D__FPU_PRESENT=1 -D__FPU_USED=1
