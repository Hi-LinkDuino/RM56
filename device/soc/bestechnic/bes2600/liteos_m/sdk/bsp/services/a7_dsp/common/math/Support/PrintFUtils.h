/*
	Copyright (C) 1997-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __PrintFUtils_h__
#define __PrintFUtils_h__

#include "CommonServices.h"
#include "DebugServices.h"

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================
/*!	@group		PrintF
	@abstract	printf implementation with many additional features over the standard printf.
	@discussion
	
	Extra features over the standard C snprintf:
	
	64-bit support for %d (%lld), %i (%lli), %u (%llu), %o (%llo), %x (%llx), and %b (%llb).
	Support for hh (char), j (intmax_t), z (size_t), and t (ptrdiff_t) length modifiers. These are part of C99.
	Support for the thousands separator "'" flag character (e.g. "%'d", 12345 -> "12,345").
	Support for the "?" flag for optional suppression. Arg=Non-zero int to include, zero int to suppress.
	%@		- Cocoa/CoreFoundation object. Param is the object. For containers, field width is the indent level.
			  Note: Non-Mac builds must defined DEBUG_CF_OBJECTS_ENABLED to 1 for this feature to be enabled.
	%.2a	- CEC/HDMI address (2-byte big endian converted to a.b.c.d). Arg=ptr to network address.
	%.4a	- IPv4 address (big endian). Arg=ptr to network address.
	%.6a	- MAC address (6 bytes, AA:BB:CC:DD:EE:FF). Arg=ptr to network address.
	%.8a	- Fibre Channel address (AA:BB:CC:DD:EE:FF:00:11). Arg=ptr to network address.
	%.16a	- IPv6 address (16 bytes). Arg=ptr to network address.
	%#.4a	- IPv4 address in host byte order: Arg=uint32_t * (points to host byte order uint32_t).
	%#.6a	- MAC address from host order uint64_t: Arg=uint64_t * (points to host byte order uint64_t).
	%#a		- IPv4 or IPv6 mDNSAddr. Arg=ptr to mDNSAddr.
	%##a	- IPv4 (if AF_INET defined) or IPv6 (if AF_INET6 defined) sockaddr. Arg=ptr to sockaddr.
	%b		- Binary representation of integer (e.g. 01101011). Modifiers and arg=the same as %d, %x, etc.
	%##b	- Bit numbers of integer (0xA -> "4 2"). 0 bit at right. Modifiers and arg=mostly the same as %d, %x, etc.
	%###b	- Bit numbers of integer (0xA -> "4 2"). 0 bit at left. Modifiers and arg=mostly the same as %d, %x, etc.
			  1 h defaults to 16 bits (%##hb), 2 l defaults to 64 bits (%##llb), otherwise defaults to 32 bits.
			  Precision overrides the number of bits (e.g. %###.8llb means IBM bit 0 is normal bit 7).
	%'c		- Same as %c, but wrapped in single quotes (e.g. %'c, 'a' -> "'a'").
	%C		- Mac-style FourCharCode (e.g. 'APPL'). Arg=32-bit value to print as a Mac-style FourCharCode.
	%'C		- Same as %C, but wrapped in single quotes (e.g. %'C, 'abcd' -> "'abcd'").
	%H		- Hex Dump (e.g. "\x6b\xa7" -> "6B A7"). 1st arg=ptr, 2nd arg=size, 3rd arg=max size.
	%#H		- Hex Dump & ASCII (e.g. "\x41\x62" -> "6B A7 |Ab|"). 1st arg=ptr, 2nd arg=size, 3rd arg=max size.
	%##H	- Raw ASCII dump (e.g. "\x41\x62" -> "|Ab|"). 1st arg=ptr, 2nd arg=size, 3rd arg=max size.
	%.1H	- Multi-line hex dump with ASCII. 1st arg=ptr, 2nd arg=size, 3rd arg=max size. Field width is indent count.
	%.2H	- Hex dump: 1 line <= 16, multi-line > 16. 1st arg=ptr, 2nd arg=size, 3rd arg=max size. Field width is indent count.
	%.3H	- Lowercase hex byte stream (e.g. "00aa11bb"). 1st arg=ptr, 2nd arg=size, 3rd arg=max size.
	%.4H	- Uppercase hex byte stream (e.g. "00AA11BB"). 1st arg=ptr, 2nd arg=size, 3rd arg=max size.
	%m		- Error Message (e.g. 0 -> "kNoErr"). Arg=error code. Size modifiers are the same as %d, %x, etc.
	%#m		- Error and Error Message (e.g. 0 -> "0 kNoErr"). Arg=error code. Size modifiers are the same as %d, %x, etc.
	%##m	- Same as %#m except output is in columns for nicer display of multiple error codes.
	%N		- Now (current Date/Time string) as YYYY-MM-DD HH:MM:SS.ssssss AM/PM. No arg.
	%#N		- Now (current Date/Time string) as YYYY-MM-DD_HH-MM-SS.ssssss-AM/PM. No arg. Suitable for filenames.
	%#s		- Pascal-style length-prefixed string. Arg=ptr to string.
	%##s	- DNS label-sequence name. Arg=ptr to name.
	%###s	- Cleansed function name (i.e. isolate just the [<class>::]<function> part). Arg=ptr to C-string function name.
	%'s		- Same as %s, but wrapped in single quotes (e.g. "%'s, "test" -> "'test'".
	%''s	- Same as %s, but wrapped in double quotes (e.g. "%''s, "test" -> ""test"".
	%S		- UTF-16 string, 0x0000 terminated. Host order if no BOM. Precision is UTF-16 count. Precision includes BOM.
	%#S		- Big Endian UTF-16 string (unless BOM overrides). Otherwise, the same as %S.
	%##S	- Little Endian UTF-16 string (unless BOM overrides). Otherwise, the same as %S.
	%U		- Universally Unique Identifier (UUID) in Microsoft little endian format. Arg=ptr to 16-byte UUID.
			  Little endian: 10 b8 a7 6b ad 9d d1 11 80 b4 00 c0 4f d4 30 c8 -> "6ba7b810-9dad-11d1-80b4-00c04fd430c8".
	%#U		- Universally Unique Identifier (UUID) in big endian format. Arg=ptr to 16-byte UUID.
			  Big endian: 6b a7 b8 10 9d ad 11 d1 80 b4 00 c0 4f d4 30 c8 -> "6ba7b810-9dad-11d1-80b4-00c04fd430c8".
	%V		- Nested PrintF format string and va_list. 1st arg=const char *format, 2nd arg=va_list *args. Note: 2nd is a ptr.
	
	%{<extension>} extensions:
	
	%{text}		Multi-line text. const void *ptr, size_t len. Field width is indent count of each line.
===========================================================================================================================*/

typedef struct PrintFContext PrintFContext;
typedef struct
{
    unsigned leftJustify : 1;
    unsigned forceSign : 1;
    unsigned zeroPad : 1;
    unsigned havePrecision : 1;
    unsigned suppress : 1;
    char hSize;
    char lSize;
    char altForm;
    char sign; // +, -, or space
    unsigned int fieldWidth;
    size_t precision;
    char group;
    char prefix;
    char suffix;

} PrintFFormat;
typedef struct {
    va_list args;
} PrintFVAList;

// Core APIs for use by extensions only.

int PrintFCore(PrintFContext* inContext, const char* inFormat, ...);
int PrintFCoreVAList(PrintFContext* inContext, const char* inFormat, va_list inArgs);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	SNPrintF/VSNPrintF
	@abstract	PrintF to a fixed-size C string buffer.
*/
int SNPrintF(void* inBuf, size_t inMaxLen, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(3, 4);
int VSNPrintF(void* inBuf, size_t inMaxLen, const char* inFormat, va_list inArgs) PRINTF_STYLE_FUNCTION(3, 0);
OSStatus SNPrintF_Add(char** ioPtr, char* inEnd, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(3, 4);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	ASPrintF/VASPrintF
	@abstract	PrintF to a malloc'd C string.
*/
int AppendPrintF(char** ioStr, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(2, 3);
int ASPrintF(char** outStr, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(2, 3);
int VASPrintF(char** outStr, const char* inFormat, va_list inArgs) PRINTF_STYLE_FUNCTION(2, 0);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	CPrintF/VCPrintF
	@abstract	PrintF to a callback function.
*/
typedef int (*PrintFUserCallBack)(const char* inStr, size_t inSize, void* inContext);

int CPrintF(PrintFUserCallBack inCallBack, void* inContext, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(3, 4);
int VCPrintF(PrintFUserCallBack inCallBack, void* inContext, const char* inFormat, va_list inArgs) PRINTF_STYLE_FUNCTION(3, 0);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	FPrintF/VFPrintF
	@abstract	Like fprintf, but supports the PrintFCore extensions and platforms that don't have fprintf.
*/
#if (TARGET_HAS_C_LIB_IO)
int FPrintF(FILE* inFile, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(2, 3);
int VFPrintF(FILE* inFile, const char* inFormat, va_list inArgs) PRINTF_STYLE_FUNCTION(2, 0);
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MemPrintF/VMemPrintF
	@abstract	Like SNPrintF, but it doesn't null terminate. Useful for inserting into existing strings.
*/
int MemPrintF(void* inBuf, size_t inMaxLen, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(3, 4);
int VMemPrintF(void* inBuf, size_t inMaxLen, const char* inFormat, va_list inArgs) PRINTF_STYLE_FUNCTION(3, 0);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	PrintFTest
	@abstract	Unit test.
*/
#if (!EXCLUDE_UNIT_TESTS)
OSStatus PrintFUtils_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __PrintFUtils_h__
