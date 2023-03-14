#include <dlfcn.h>
#include "dynlink.h"

static int dummy(void *p)
{
	return __dl_invalid_handle(p);
}
weak_alias(dummy, __dlclose);

int dlclose(void *p)
{
	return __dlclose(p);
}

