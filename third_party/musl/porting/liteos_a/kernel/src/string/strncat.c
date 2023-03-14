#include <string.h>
#ifdef LOSCFG_KERNEL_LMS
__attribute__((no_sanitize_address)) char *__strncat(char *restrict d, const char *restrict s, size_t n)
#else
char *strncat(char *restrict d, const char *restrict s, size_t n)
#endif
{
	char *a = d;
	d += strlen(d);
	while (n && *s) n--, *d++ = *s++;
	*d++ = 0;
	return a;
}
