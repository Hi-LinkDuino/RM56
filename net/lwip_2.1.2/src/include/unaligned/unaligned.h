#ifndef __ASM_GENERIC_UNALIGNED_H
#define __ASM_GENERIC_UNALIGNED_H

/*
 * This is the most generic implementation of unaligned accesses
 * and should work almost anywhere.
 */
#include "byteorder.h"

/* Set by the arch if it can handle unaligned accesses in hardware. */
#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#include "access_ok.h"
#endif

#if defined(__LITTLE_ENDIAN)
#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#include "le_struct.h"
#include "be_byteshift.h"
#endif

#elif defined(__BIG_ENDIAN)
#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#include "be_struct.h"
#include "le_byteshift.h"
#endif

#else
#error need to define endianess
#endif

#endif /* __ASM_GENERIC_UNALIGNED_H */
