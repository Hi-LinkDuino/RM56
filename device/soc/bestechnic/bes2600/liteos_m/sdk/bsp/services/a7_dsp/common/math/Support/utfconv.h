/*
	Copyright (C) 2006-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef _SYS_UTFCONV_H_
#define _SYS_UTFCONV_H_

#if (defined(KERNEL) || defined(_KERNEL))
#include <sys/param.h>
#else
#include "CommonServices.h"
#endif

#if (defined(KERNEL) || defined(_KERNEL))
#define utfmalloc(SIZE) malloc((SIZE), M_TEMP, M_WAITOK)
#define utffree(PTR) free((PTR), M_TEMP)
#else
#define utfmalloc(SIZE) malloc((SIZE))
#define utffree(PTR) free((PTR))
#endif

/*
 * UTF-8 encode/decode flags
 */
#define UTF_REVERSE_ENDIAN 0x0001 /* reverse UCS-2 byte order */
#define UTF_NO_NULL_TERM 0x0002 /* do not add null termination */
#define UTF_DECOMPOSED 0x0004 /* generate fully decomposed UCS-2 */
#define UTF_PRECOMPOSED 0x0008 /* generate precomposed UCS-2 */
#define UTF_ESCAPE_ILLEGAL 0x0010 /* escape illegal UTF-8 */
#define UTF_SFM_CONVERSIONS 0x0020 /* Use SFM mappings for illegal NTFS chars */
#define UTF_STRICT 0x0040 /* Fail on bad bytes, isolated surrogates, etc. */

#define UTF_BIG_ENDIAN \
    (TARGET_RT_BIG_ENDIAN ? 0 : UTF_REVERSE_ENDIAN)

#define UTF_LITTLE_ENDIAN \
    (TARGET_RT_LITTLE_ENDIAN ? 0 : UTF_REVERSE_ENDIAN)

#ifdef __cplusplus
extern "C" {
#endif

/*
 * utf8_encodelen - Calculate the UTF-8 encoding length
 *
 * This function takes an Unicode input string, ucsp, of ucslen bytes
 * and calculates the size of the UTF-8 output in bytes (not including
 * a NULL termination byte). The string must reside in kernel memory.
 *
 * FLAGS
 *    UTF_REVERSE_ENDIAN:  Unicode byte order is opposite current runtime
 *
 *    UTF_BIG_ENDIAN:  Unicode byte order is always big endian
 *
 *    UTF_LITTLE_ENDIAN:  Unicode byte order is always little endian
 *
 *    UTF_DECOMPOSED:  assume fully decomposed output
 *
 * ERRORS
 *    None
 */
size_t
utf8_encodelen(const uint16_t* ucsp, size_t ucslen, uint16_t altslash,
    int flags);

/*
 * utf8_encodestr - Encodes a Unicode string into UTF-8
 *
 * This function takes an Unicode input string, ucsp, of ucslen bytes
 * and produces the UTF-8 output into a buffer of buflen bytes pointed
 * to by utf8p. The size of the output in bytes (not including a NULL
 * termination byte) is returned in utf8len. The UTF-8 string output
 * is NULL terminated. Both buffers must reside in kernel memory.
 *
 * If '/' chars are possible in the Unicode input then an alternate
 * (replacement) char must be provided in altslash.
 *
 * FLAGS
 *    UTF_REVERSE_ENDIAN:  Unicode byte order is opposite current runtime
 *
 *    UTF_BIG_ENDIAN:  Unicode byte order is always big endian
 *
 *    UTF_LITTLE_ENDIAN:  Unicode byte order is always little endian
 *
 *    UTF_NO_NULL_TERM:  do not add null termination to output string
 *
 *    UTF_DECOMPOSED:  generate fully decomposed output
 *
 * ERRORS
 *    ENAMETOOLONG:  output did not fit; only utf8len bytes were encoded
 *
 *    EINVAL:  illegal Unicode char encountered
 */
int utf8_encodestr(const uint16_t* ucsp, size_t ucslen, uint8_t* utf8p,
    size_t* utf8len, size_t buflen, uint16_t altslash, int flags);

/*
 * utf8_decodestr - Decodes a UTF-8 string into Unicode
 *
 * This function takes an UTF-8 input string, utf8p, of utf8len bytes
 * and produces the Unicode output into a buffer of buflen bytes pointed
 * to by ucsp. The size of the output in bytes (not including a NULL
 * termination byte) is returned in ucslen. Both buffers must reside
 * in kernel memory.
 *
 * If '/' chars are allowed in the Unicode output then an alternate
 * (replacement) char must be provided in altslash.
 *
 * FLAGS
 *    UTF_REV_ENDIAN:  Unicode byte order is opposite current runtime
 *
 *    UTF_BIG_ENDIAN:  Unicode byte order is always big endian
 *
 *    UTF_LITTLE_ENDIAN:  Unicode byte order is always little endian
 *
 *    UTF_DECOMPOSED:  generate fully decomposed output (NFD)
 *
 *    UTF_PRECOMPOSED:  generate precomposed output (NFC)
 *
 *    UTF_ESCAPE_ILLEGAL:  percent escape any illegal UTF-8 input
 *
 * ERRORS
 *    ENAMETOOLONG:  output did not fit; only ucslen bytes were decoded.
 *
 *    EINVAL:  illegal UTF-8 sequence encountered.
 */
int utf8_decodestr(const uint8_t* utf8p, size_t utf8len, uint16_t* ucsp,
    size_t* ucslen, size_t buflen, uint16_t altslash, int flags);

/*
 * utf8_normalizestr - Normalize a UTF-8 string (NFC or NFD)
 *
 * This function takes an UTF-8 input string, instr, of inlen bytes
 * and produces normalized UTF-8 output into a buffer of buflen bytes
 * pointed to by outstr. The size of the output in bytes (not including
 * a NULL termination byte) is returned in outlen. In-place conversions
 * are not supported (i.e. instr != outstr).  Both buffers must reside
 * in kernel memory.
 *
 * FLAGS
 *    UTF_DECOMPOSED:  output string will be fully decomposed (NFD)
 *
 *    UTF_PRECOMPOSED:  output string will be precomposed (NFC)
 *
 *    UTF_NO_NULL_TERM:  do not add null termination to output string
 *
 *    UTF_ESCAPE_ILLEGAL:  percent escape any illegal UTF-8 input
 *
 * ERRORS
 *    ENAMETOOLONG:  output did not fit or input exceeded MAXPATHLEN bytes
 *
 *    EINVAL:  illegal UTF-8 sequence encountered or invalid flags
 */
int utf8_normalizestr(const uint8_t* instr, size_t inlen, uint8_t* outstr,
    size_t* outlen, size_t buflen, int flags);

/*
 * utf8_validatestr - validates a UTF-8 string
 *
 * This function takes an UTF-8 input string, utf8p, of utf8len bytes
 * and determines if its valid UTF-8.  The string must reside in kernel
 * memory.
 *
 * ERRORS
 *    EINVAL:  illegal UTF-8 sequence encountered.
 */
int utf8_validatestr(const uint8_t* utf8p, size_t utf8len);

/*
 * utf32_to_utf16 - Converts a UTF-16 string to UTF-32.
 *
 * This function takes a UTF-16 input string, inUTF16, of inUTF16ByteLen 
 * bytes and produces the UTF-32 output into a buffer of inUTF32BufByteLen 
 * bytes pointed to by inUTF32Buf. The size of the output in bytes is 
 * returned in outUTF32ByteLen. The UTF-32 string is *not* null terminated.
 *
 * NOTES:
 *    The resulting UTF-32 string is not NULL terminated.
 *
 * input flags:
 *    UTF_STRICT: Fail on irregular sequences and isolated surrogates.
 *
 * result:
 *    ENAMETOOLONG: Name didn't fit; only buflen bytes were encoded
 *
 *    EINVAL: Illegal char found.
 */
int utf16_to_utf32(const uint16_t* inUTF16, size_t inUTF16ByteLen,
    uint32_t* inUTF32Buf, size_t* outUTF32ByteLen, size_t inUTF32BufByteLen,
    int inFlags);

/*
 * utf8_encodestr_copy - Encodes a UTF-16 string into UTF-8 into a malloc'd buffer.
 *
 * NOTES:
 *    On success, the caller must use utffree to free the UTF-8 buffer when no longer needed.
 *    Otherwise, the same notes as utf8_encodestr.
 *
 * FLAGS:
 *    Same flags as utf8_encodestr.
 *
 * ERRORS:
 *    Same errors as utf8_encodestr.
 */
int utf8_encodestr_copy(const uint16_t* inUTF16, size_t inUTF16Bytes,
    void* outUTF8, size_t* outUTF8Bytes,
    uint16_t altslash, int flags);

/*
 * utf8_decodestr_copy - Decodes a UTF-8 string into UTF-16 into a malloc'd buffer.
 *
 * NOTES:
 *    On success, the caller must use utffree to free the UTF-16 buffer when no longer needed.
 *    Otherwise, the same notes as utf8_decodestr.
 *
 * FLAGS:
 *    Same flags as utf8_decodestr.
 *
 * ERRORS:
 *    Same errors as utf8_decodestr.
 */
int utf8_decodestr_copy(
    const void* inUTF8Ptr, size_t inUTF8Bytes,
    void* outUTF16, size_t* outUTF16Bytes,
    uint16_t inAltslash, int inFlags);

/*
 * latin1_to_utf8 - Converts Windows Latin-1 to UTF-8 into a malloc'd buffer.
 *
 * NOTES:
 *    On success, the caller must use utffree to free the UTF-8 buffer when no longer needed.
 *    Otherwise, the same notes as utf8_encodestr.
 *
 * FLAGS:
 *    Same flags as utf8_encodestr.
 *
 * ERRORS:
 *    Same errors as utf8_encodestr.
 */
int latin1_to_utf8_copy(
    const void* inLatin1Ptr,
    size_t inLatin1Len,
    void* outUTF8Ptr,
    size_t* outUTF8Len,
    uint16_t inAltSlash,
    int inFlags);

/*
	macroman_to_utf8 - Converts MacRoman to UTF-8 into a malloc'd buffer.
	
	NOTES:
		On success, the caller must use utffree to free the UTF-8 buffer when no longer needed.
		Otherwise, the same notes as utf8_encodestr.
	
	FLAGS:
		Same flags as utf8_encodestr.
	
	ERRORS:
		Same errors as utf8_encodestr.
*/
int macroman_to_utf8_copy(
    const void* inMacRomanPtr,
    size_t inMacRomanLen,
    void* outUTF8Ptr,
    size_t* outUTF8Len,
    uint16_t inAltSlash,
    int inFlags);

// Debugging

#if (!EXCLUDE_UNIT_TESTS)
OSStatus utfconv_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* !_SYS_UTFCONV_H_ */
