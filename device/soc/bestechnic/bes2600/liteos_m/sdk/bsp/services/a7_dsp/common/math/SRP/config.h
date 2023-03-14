/*
	Portions Copyright (C) 2007-2014 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#include "CommonServices.h"
#include "RandomNumberUtils.h"
#include SHA_HEADER

/* Define if you have the ANSI C header files.  */
#if( !TARGET_KERNEL )
	#define STDC_HEADERS	1
#endif
#if 0
#if( TARGET_HAS_COMMON_CRYPTO )
	#define COMMONCRYPTO	1
#elif( !defined( OPENSSL ) && !TARGET_NO_OPENSSL )
	#define OPENSSL						1
	#if( !defined( OPENSSL_ENGINE ) )
		#if( !TARGET_KERNEL )
			#define OPENSSL_ENGINE		1
		#else
			#define OPENSSL_ENGINE		0
		#endif
	#endif
#endif
#endif
/* #undef GNU_MP */

/* #undef GCRYPT */

/* #undef MPI */

/* #undef TOMCRYPT */

/* #undef TOMMATH */
#if( !defined( TOMMATH ) && defined( TARGET_HAS_LIBTOMMATH ) && TARGET_HAS_LIBTOMMATH )
	#define TOMMATH		1
#endif
#undef TOMMATH

#define MBEDTLS		1
/* #undef CRYPTOLIB */

/* #undef ENABLE_YP */

/* Name of package */
#define PACKAGE "libsrp"

/* Version number of package */
#define VERSION "2.1.2"

#define _CDECL

#if !defined(SRP_TESTS)
	#define SRP_TESTS		0
#endif

#ifndef P
	#if defined(__STDC__) || defined(__cplusplus)
		#define P(x) x
	#else
		#define P(x) ()
	#endif
#endif

#define _TYPE(a)	a

#if STDC_HEADERS
	#include <stdlib.h>
	#include <string.h>
#endif
