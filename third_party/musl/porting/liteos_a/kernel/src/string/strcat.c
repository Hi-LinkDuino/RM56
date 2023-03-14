#include <string.h>
#ifdef LOSCFG_KERNEL_LMS
__attribute__((no_sanitize_address)) char *__strcat(char *restrict dest, const char *restrict src)
#else
char *strcat(char *restrict dest, const char *restrict src)
#endif
{
	strcpy(dest + strlen(dest), src);
	return dest;
}
