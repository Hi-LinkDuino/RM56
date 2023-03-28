#if !defined(M533_ARM_H)
#define M533_ARM_H

#define FUNCTION_smmul

#ifdef FUNCTION_smmul
static inline int smmul(const int a, const int b) {
	//return (int)(((long long)a * b) >> 31);
	int result;
	__asm__("SMMUL %0, %1, %2" : "=r"(result) : "r"(a), "r"(b));
	return result;
}
#endif 

#endif