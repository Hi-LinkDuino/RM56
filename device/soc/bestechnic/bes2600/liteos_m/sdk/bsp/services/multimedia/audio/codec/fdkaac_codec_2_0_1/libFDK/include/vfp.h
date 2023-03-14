#if !defined(__VFP_H__)
#define __VFP_H__

#define VFP

#if defined(__arm__)
#include "arm/vfp_arm.h"
#endif

#ifndef FUNCTION_vdiv
#define FUNCTION_vdiv
static inline float vdiv_ff(const float a, const float b) {
    return a / b;
}
#endif

#ifndef FUNCTION_vcvt_f32_s32_31
#define FUNCTION_vcvt_f32_s32_31
static inline int vcvt_f32_s32_31(const float a) {
    return (int)(a * (1u << 31));
}
#endif /* #ifdef FUNCTION_fixmuldiv2_DD */
#if 0
#ifndef FUNCTION_sdiv
#define FUNCTION_sdiv
static inline INT fixsdiv_DD(const INT a, const INT b) {
    return a / b;
}
#endif

#ifndef FUNCTION_udiv
#define FUNCTION_udiv
static inline unsigned fixudiv_UU(const unsigned a, const unsigned b) {
    return a / b;
}
#endif
#endif
#ifndef FUNCTION_vcvt_f32_s32_31
#define FUNCTION_vcvt_f32_s32_31
static inline int vcvt_f32_s32_31(const float a) {
    return (int)(a * (1u << 31));
}
#endif

#ifndef FUNCTION_vcvt_f32_s32_15
#define FUNCTION_vcvt_f32_s32_15
static inline int vcvt_f32_s32_15(const float a) {
    return (int)(a * (1u << 15));
}
#endif

#ifndef FUNCTION_vsqrt
#define FUNCTION_vsqrt
static inline float vsqrt(const float a) {
    return sqrt(a);
}
static inline unsigned vsqrt(unsigned a) {
    return (unsigned)(vsqrt((float)a) * (1 << 15));
}
#endif

#ifndef FUNCTION_vsqrt2
#define FUNCTION_vsqrt2
static inline float vsqrt2(const float a) {
    return vsqrt(vsqrt(a));
}
static inline unsigned vsqrt2(const unsigned a) {
    return (unsigned)(vsqrt2((float)a)*(1<<23));
}
#endif

#ifndef FUNCTION_vsqrt_inv
#define FUNCTION_vsqrt_inv
static inline float vsqrt_inv(const float a) {
    //VSQRT{cond}.F32 Sd, Sm
    //float result;
    //__asm__("vsqrt.f32 %0, %1" : "=r"(result) : "r"(a));
    //return result;
    return 0;
}
static inline unsigned vsqrt_inv(unsigned op) {
    float rst = vsqrt_inv((float)(op));
    return vcvt_f32_s32_31(rst);
}
#endif /* FUNCTION_sqrtFixp */

#ifndef FUNCTION_vabs
#define FUNCTION_vabs
static inline float vabs(const float a) {
    //VABS{cond}.F32 Sd, Sm 
    //float result;
    //__asm__("vabs.f32 %0, %1" : "=r"(result) : "r"(a));
    return a > 0 ? a : -a;
}
#endif

#ifndef FUNCTION_vmaxnm
#define FUNCTION_vmaxnm
static inline float vmaxnm(const float a, const float b) {
    //VMAXNM.F32 Sd, Sn, Sm
    //float result;
    //__asm__("vmaxnm.f32 %0, %1, %2" : "=r"(result) : "r"(a), "r"(b));
    return a > b ? a : b;
}
#endif

#ifndef FUNCTION_vminnm
#define FUNCTION_vminnm
static inline float vminnm(const float a, const float b) {
    //VMINNM.F32 Sd, Sn, Sm
    //float result;
    //__asm__("vminnm.f32 %0, %1, %2" : "=r"(result) : "r"(a), "r"(b));
    return a < b ? a : b;
}
#endif

#ifndef FUNCTION_vsat
#define FUNCTION_vsat
static inline float vsat(float a) {
    a = vmaxnm(a, 1.0f);
    return vminnm(a, 1.0f);
}
#endif

#ifndef FUNCTION_fixnormz_U
#define FUNCTION_fixnormz_U
static inline INT fixnormz_U(unsigned value) {
    return fixnormz_D(*(FIXP_DBL*)&value);
}
#endif

#endif