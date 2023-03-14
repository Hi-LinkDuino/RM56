#ifndef _COMMON_DEF_H
#define _COMMON_DEF_H

#include <unistd.h>
#include <stdlib.h>

#define	__predict_true(exp) __builtin_expect((exp) != 0, 1)
#define	__predict_false(exp) __builtin_expect((exp) != 0, 0)

#endif