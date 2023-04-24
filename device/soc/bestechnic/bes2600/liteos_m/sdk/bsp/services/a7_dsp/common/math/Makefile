
TOP := $(shell pwd)
CC ?= gcc
AR ?= ar
INCLUDES = -I./
DEFS :=
SRCS :=
OBJS :=
LIBS :=
A7_USE_MBEDTLS_MATH := 0
#########################  INCLUDE     ##############################
 
 INCLUDES += -Iservices/a7_dsp/common/math/LibTomMath
 INCLUDES += -Iservices/a7_dsp/common/math/SRP
 INCLUDES += -Iservices/a7_dsp/common/math/Support
 INCLUDES += -Iservices/a7_dsp/common/math
 INCLUDES += -Iservices/transq_msg
 INCLUDES += -Iservices/a7_dsp/common/math/mbedtls/include

#########################  DEF         ##############################
ifeq ($(CHIP_SUBSYS), dsp)
 DEFS += -DDEBUG_CF_OBJECTS_ENABLED=1
 DEFS += -DDEBUG_EXPORT_ERROR_STRINGS=1
 DEFS += -DDEBUG=1

 DEFS += -DAPPLE_INTERNAL=0
 DEFS += -DCOMPILER_OBJC=0
 DEFS += -D__STDC_LIMIT_MACROS=1
 DEFS += -D_GNU_SOURCE
 DEFS += -DAES_UTILS_HAS_GLADMAN_GCM=0
 DEFS += -DAES_UTILS_USE_GLADMAN_AES=1
 DEFS += -DUSE_VIA_ACE_IF_PRESENT=0
 DEFS += -DFRAMEWORK_STYLE_INCLUDES=0
 DEFS += -DTARGET_HAS_MD5_UTILS=1
 DEFS += -DTARGET_HAS_SHA_UTILS=1
 DEFS += -DTARGET_HAS_CORECRYPTO_HKDF=0
 DEFS += -DTARGET_HAS_COMMON_CRYPTO_DIGEST=0
 DEFS += -DTARGET_HAS_NATIVE_INT128=0
 DEFS += -DTARGET_HAS_LIBTOMMATH=1
 DEFS += -DTARGET_OS_DARWIN_REAL=0
 DEFS += -DTARGET_NEEDS_NATURAL_ALIGNMENT=1
 DEFS += -DTARGET_NO_OPENSSL=1
 DEFS += -DSNPRINTF_USE_ASSERTS=0
 DEFS += -DTARGET_OS_LINUX

#########################  PLIST       ##############################

 SRCS += LibTomMath/bn_fast_mp_invmod.c
 SRCS += LibTomMath/bn_fast_mp_montgomery_reduce.c
 SRCS += LibTomMath/bn_fast_s_mp_mul_digs.c
 SRCS += LibTomMath/bn_fast_s_mp_mul_high_digs.c
 SRCS += LibTomMath/bn_fast_s_mp_sqr.c
 SRCS += LibTomMath/bn_mp_2expt.c
 SRCS += LibTomMath/bn_mp_abs.c
 SRCS += LibTomMath/bn_mp_add_d.c
 SRCS += LibTomMath/bn_mp_add.c
 SRCS += LibTomMath/bn_mp_clamp.c
 SRCS += LibTomMath/bn_mp_clear_multi.c
 SRCS += LibTomMath/bn_mp_clear.c
 SRCS += LibTomMath/bn_mp_cmp_d.c
 SRCS += LibTomMath/bn_mp_cmp_mag.c
 SRCS += LibTomMath/bn_mp_cmp.c
 SRCS += LibTomMath/bn_mp_cnt_lsb.c
 SRCS += LibTomMath/bn_mp_copy.c
 SRCS += LibTomMath/bn_mp_count_bits.c
 SRCS += LibTomMath/bn_mp_div_2.c
 SRCS += LibTomMath/bn_mp_div_2d.c
 SRCS += LibTomMath/bn_mp_div_3.c
 SRCS += LibTomMath/bn_mp_div_d.c
 SRCS += LibTomMath/bn_mp_div.c
 SRCS += LibTomMath/bn_mp_dr_is_modulus.c
 SRCS += LibTomMath/bn_mp_dr_reduce.c
 SRCS += LibTomMath/bn_mp_dr_setup.c
 SRCS += LibTomMath/bn_mp_exch.c
 SRCS += LibTomMath/bn_mp_exptmod_fast.c
 SRCS += LibTomMath/bn_mp_exptmod.c
 SRCS += LibTomMath/bn_mp_grow.c
 SRCS += LibTomMath/bn_mp_init_copy.c
 SRCS += LibTomMath/bn_mp_init_multi.c
 SRCS += LibTomMath/bn_mp_init_size.c
 SRCS += LibTomMath/bn_mp_init.c
 SRCS += LibTomMath/bn_mp_invmod_slow.c
 SRCS += LibTomMath/bn_mp_invmod.c
 SRCS += LibTomMath/bn_mp_karatsuba_mul.c
 SRCS += LibTomMath/bn_mp_karatsuba_sqr.c
 SRCS += LibTomMath/bn_mp_lshd.c
 SRCS += LibTomMath/bn_mp_mod_2d.c
 SRCS += LibTomMath/bn_mp_mod_d.c
 SRCS += LibTomMath/bn_mp_mod.c
 SRCS += LibTomMath/bn_mp_montgomery_calc_normalization.c
 SRCS += LibTomMath/bn_mp_montgomery_reduce.c
 SRCS += LibTomMath/bn_mp_montgomery_setup.c
 SRCS += LibTomMath/bn_mp_mul_2.c
 SRCS += LibTomMath/bn_mp_mul_2d.c
 SRCS += LibTomMath/bn_mp_mul_d.c
 SRCS += LibTomMath/bn_mp_mul.c
 SRCS += LibTomMath/bn_mp_mulmod.c
 SRCS += LibTomMath/bn_mp_prime_is_divisible.c
 SRCS += LibTomMath/bn_mp_prime_is_prime.c
 SRCS += LibTomMath/bn_mp_prime_miller_rabin.c
 SRCS += LibTomMath/bn_mp_prime_rabin_miller_trials.c
 SRCS += LibTomMath/bn_mp_read_unsigned_bin.c
 SRCS += LibTomMath/bn_mp_reduce_2k_l.c
 SRCS += LibTomMath/bn_mp_reduce_2k_setup_l.c
 SRCS += LibTomMath/bn_mp_reduce_2k_setup.c
 SRCS += LibTomMath/bn_mp_reduce_2k.c
 SRCS += LibTomMath/bn_mp_reduce_is_2k_l.c
 SRCS += LibTomMath/bn_mp_reduce_is_2k.c
 SRCS += LibTomMath/bn_mp_reduce_setup.c
 SRCS += LibTomMath/bn_mp_reduce.c
 SRCS += LibTomMath/bn_mp_rshd.c
 SRCS += LibTomMath/bn_mp_set_int.c
 SRCS += LibTomMath/bn_mp_set.c
 SRCS += LibTomMath/bn_mp_sqr.c
 SRCS += LibTomMath/bn_mp_sqrmod.c
 SRCS += LibTomMath/bn_mp_sub_d.c
 SRCS += LibTomMath/bn_mp_sub.c
 SRCS += LibTomMath/bn_mp_to_unsigned_bin.c
 SRCS += LibTomMath/bn_mp_toom_mul.c
 SRCS += LibTomMath/bn_mp_toom_sqr.c
 SRCS += LibTomMath/bn_mp_unsigned_bin_size.c
 SRCS += LibTomMath/bn_mp_zero.c
 SRCS += LibTomMath/bn_prime_tab.c
 SRCS += LibTomMath/bn_s_mp_add.c
 SRCS += LibTomMath/bn_reverse.c
 SRCS += LibTomMath/bn_s_mp_exptmod.c
 SRCS += LibTomMath/bn_s_mp_mul_digs.c
 SRCS += LibTomMath/bn_s_mp_mul_high_digs.c
 SRCS += LibTomMath/bn_s_mp_sqr.c
 SRCS += LibTomMath/bn_s_mp_sub.c
 SRCS += LibTomMath/bncore.c

 SRCS += test_libtomath.c
 SRCS += tlv.c
 SRCS += a7_math.c
 SRCS += a7_exptmod.c
 SRCS += a7_math_add.c
 SRCS += a7_ModExpMul.c
 SRCS += a7_curve25519.c
 SRCS += a7_ed25519.c
 SRCS += a7_math_function.c
 SRCS += a7_SRPServer_create.c
 SRCS += a7_SRPServer_delete.c
 SRCS += a7_SRPServer_start.c
 SRCS += a7_SRPServer_verify.c

#########################  LIBS        ##############################
obj-c += mbedtls/
obj-c += Curve25519/
obj-c += Ed25519/
obj-c += SRP/
obj-c += Support/

ccflags-y += $(DEFS)
else
SRCS += mcu_math.c
SRCS += tlv.c
SRCS += a7_exptmod.c
SRCS += a7_math_add.c
SRCS += a7_ModExpMul.c
SRCS += a7_curve25519.c
SRCS += a7_ed25519.c
SRCS += a7_math_function.c

SRCS += a7_SRPServer_create.c
SRCS += a7_SRPServer_delete.c
SRCS += a7_SRPServer_start.c
SRCS += a7_SRPServer_verify.c
endif

obj-c += $(SRCS)

LIB_NAME := libmath
src_obj := $(obj-c:.c=.o) $(obj-cpp:.cpp=.o)
$(LIB_NAME)-y := $(src_obj)
obj-y := $(LIB_NAME).a

ccflags-y += $(INCLUDES)
# subdir-ccflags-y += $(DEFS) $(INCLUDES)


