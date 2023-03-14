#include <string.h>
#ifdef LOSCFG_KERNEL_LMS
__attribute__((no_sanitize_address)) char *__strcpy(char *restrict dest, const char *restrict src)
#else
char *strcpy(char *restrict dest, const char *restrict src)
#endif
{
	__stpcpy(dest, src);
	return dest;
}
