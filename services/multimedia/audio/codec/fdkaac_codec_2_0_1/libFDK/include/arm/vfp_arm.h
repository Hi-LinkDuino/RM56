#if !defined(VFP_ARM_H)
#define VFP_ARM_H

#if defined(__arm__)

#if defined(__GNUC__) && defined(__arm__)

#define FUNCTION_vsqrt
#define FUNCTION_vsqrt2
#define FUNCTION_vsqrt_inv
#define FUNCTION_vabs
#define FUNCTION_vdiv
#define FUNCTION_vcvt_f32_s32_31
#define FUNCTION_vcvt_f32_s32_15
#define FUNCTION_vmaxnm
#define FUNCTION_vminnm


#ifdef FUNCTION_vdiv
static inline float vdiv_ff(const float a, const float b) {
	float result;
	__asm__ ("vdiv.f32 %0, %1, %2" : "=w"(result) : "w"(a), "w"(b));
	return result;
}
#endif 

#ifdef FUNCTION_vcvt_f32_s32_31
static inline int vcvt_f32_s32_31(const float a) {
	int result;
	__asm__ ("vcvt.s32.f32 %0, %1, #31" : "=w"(result) : "w"(a));
	return (int)result;
}
#endif

#ifdef FUNCTION_vcvt_f32_s32_15
static inline int vcvt_f32_s32_15(const float a) {
	int result;
	__asm__("vcvt.s32.f32 %0, %1, #15" : "=w"(result) : "w"(a));
	return (int)result;
}
#endif

#ifdef FUNCTION_vsqrt
static inline float vsqrt(const float a) {
	//VSQRT{cond}.F32 Sd, Sm
	float result;
	__asm__ ("vsqrt.f32 %0, %1" : "=w"(result) : "w"(a));
	return result;
}
#endif

#ifdef FUNCTION_vsqrt
static inline unsigned vsqrt(unsigned a) {
	//VSQRT{cond}.F32 Sd, Sm
	unsigned result;
	__asm__(
		"vcvt.f32.u32 %0, %1;\n"
		"vsqrt.f32 %0, %0;\n"
		"vcvt.u32.f32 %0, %0, #15"
		: "+w"(result)
		: "w"(a)
	);
	return result;
}
#endif

#ifdef FUNCTION_vsqrt2
static inline float vsqrt2(const float a) {
	//VSQRT{cond}.F32 Sd, Sm
	float result;
	__asm__("vsqrt.f32 %0, %1;"
		"vsqrt.f32 %0, %0;"
		: "=w"(result) 
		: "w"(a));
	return result;
}
static inline unsigned vsqrt2(unsigned a) {
	//VSQRT{cond}.F32 Sd, Sm
	unsigned result=0;
	__asm__(
		"vcvt.f32.u32 %0, %1;"
		"vsqrt.f32 %0, %0;"
		"vsqrt.f32 %0, %0;"
		"vcvt.u32.f32 %0, %0, #23"
		: "+w"(result)
		: "w"(a)
	);
	return result;
}
#endif

#ifdef FUNCTION_vsqrt_inv
static inline unsigned vsqrt_inv(unsigned a) {
	//VSQRT{cond}.F32 Sd, Sm
	float num = 1u<<31;
	unsigned result;
	__asm__ (
		"vcvt.f32.u32 %0, %1;\n"
		"vsqrt.f32 %0, %0;\n"
		"vdiv.f32 %0, %2, %0;\n"
		"vcvt.u32.f32 %0, %0, #15"
		: "+w"(result)
		: "w"(a), "w"(num)
		);
	return (int)result;
}
static inline unsigned vsqrt_inv(unsigned a,int*result_e) {
	//VSQRT{cond}.F32 Sd, Sm
	float num = 1u << 31;
	int shf;
	unsigned result;
	__asm__(
		"vcvt.f32.u32 s13, %1;"
		"vsqrt.f32 s13, s13;"
		"vdiv.f32 s13, %2, s13;"
		"vcvt.u32.f32 r1, s13;"
		"clz r2, r1"
		: "+w"(result)
		: "w"(a), "w"(num)
	);
	return (int)result;
}
#endif

#ifdef FUNCTION_vabs
static inline float vabs(const float a) {
	//VABS{cond}.F32 Sd, Sm 
	float result;
	__asm__ ("vabs.f32 %0, %1" : "=w"(result) : "w"(a));
	return result;
}
#endif

#ifdef FUNCTION_vmaxnm
static inline float vmaxnm(const float a,const float b) {
	//VMAXNM.F32 Sd, Sn, Sm
	float result;
	__asm__ ("vmaxnm.f32 %0, %1, %2" : "=w"(result) : "w"(a), "w"(b));
	return result;
}
#endif

#ifdef FUNCTION_vminnm
static inline float vminnm(const float a, const float b) {
	//VMINNM.F32 Sd, Sn, Sm
	float result;
	__asm__ ("vminnm.f32 %0, %1, %2" : "=w"(result) : "w"(a), "w"(b));
	return result;
}
#endif

#endif
#endif
#endif