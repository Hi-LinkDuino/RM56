#include "usr_lib_define.h"
#include <string.h>

_LIBC_TEXT_SECTION int strcmp(const char *l, const char *r)
{
	for (; *l==*r && *l; l++, r++);
	return *(unsigned char *)l - *(unsigned char *)r;
}
