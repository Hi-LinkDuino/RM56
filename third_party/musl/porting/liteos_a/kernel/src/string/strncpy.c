#include <string.h>
#ifdef LOSCFG_KERNEL_LMS
__attribute__((no_sanitize_address)) char *__strncpy(char *restrict d, const char *restrict s, size_t n)
#else
char *strncpy(char *restrict d, const char *restrict s, size_t n)
#endif
{
	__stpncpy(d, s, n);
	return d;
}
