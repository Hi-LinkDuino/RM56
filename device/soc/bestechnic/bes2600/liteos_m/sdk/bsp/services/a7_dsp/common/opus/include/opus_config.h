#ifndef OPUS_CONFIG_H
#define OPUS_CONFIG_H

/* general stuff */
#define OPUS_BUILD

/* alloc stuff */
#if defined(OPUS_ENCODE_IN_CP) || defined(OPUS_DECODE_IN_CP)
#define VAR_ARRAYS
#endif
//#define NORM_ALIASING_HACK

#define NONTHREADSAFE_PSEUDOSTACK
// #define OVERRIDE_OPUS_ALLOC_SCRATCH /*mabin modify*/

#define CUT 1

//#define OVERRIDE_OPUS_ALLOC
//#define OVERRIDE_OPUS_FREE
//#define OVERRIDE_OPUS_ALLOC_SCRATCH

//#define opus_alloc          _ogg_malloc
//#define opus_free           _ogg_free
//#define opus_alloc_scratch  _ogg_malloc

/* lrint */
//#define HAVE_LRINTF 0
//#define HAVE_LRINT  0

/* for A7 define */
// #define OPUS_IN_A7_SUPPORT
#ifdef OPUS_IN_A7_SUPPORT

/* embedded stuff */
#define FIXED_POINT		1
#define DISABLE_FLOAT_API

/* Float approximations */
//#define FLOAT_APPROX

/* Make use of ARM asm optimization */
#define OPUS_ARM_ASM 1
/* Use generic ARMv4 inline asm optimizations */
#define  OPUS_ARM_INLINE_ASM
/* Use ARMv5E inline asm optimizations */
#define  OPUS_ARM_INLINE_EDSP

//#define OPUS_HAVE_RTCD 1
/* Use ARM NEON inline asm optimizations */
#define OPUS_ARM_INLINE_NEON 1
/* Define if compiler supports NEON instructions */
#define OPUS_ARM_MAY_HAVE_NEON 1
/* Compiler supports ARMv7/Aarch64 Neon Intrinsics */
#define OPUS_ARM_MAY_HAVE_NEON_INTR 1

/* Use ARMv6 inline asm optimizations */
#define OPUS_ARM_INLINE_MEDIA

/* Define if assembler supports ARMv6 media instructions */
/* #undef OPUS_ARM_MAY_HAVE_MEDIA */

/* Define if binary requires Aarch64 Neon Intrinsics */
/* #undef OPUS_ARM_PRESUME_AARCH64_NEON_INTR */

/* Define if binary requires EDSP instruction support */
/* #undef OPUS_ARM_PRESUME_EDSP */

/* Define if binary requires ARMv6 media instruction support */
/* #undef OPUS_ARM_PRESUME_MEDIA */

/* Define if binary requires NEON instruction support */
/* #undef OPUS_ARM_PRESUME_NEON */

/* Define if binary requires NEON intrinsics support */
/* #undef OPUS_ARM_PRESUME_NEON_INTR */

#else

/* embedded stuff */
#define FIXED_POINT		1
#define DISABLE_FLOAT_API
//#define EMBEDDED_ARM 1
/* undefinitions */
#ifdef ABS
#undef ABS
#endif
#ifdef MIN
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif

#define OPUS_ARM_ASM 1
#define OPUS_ARM_INLINE_ASM 1
#define OPUS_ARM_INLINE_EDSP 1
//#define OPUS_ARM_INLINE_EDSP_CM4 1
#endif




#endif /* OPUS_CONFIG_H */

