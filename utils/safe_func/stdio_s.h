#ifndef STDIO_S_H
#define STDIO_S_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"
#include "stdarg.h"
#include "libc_rom_s_errno.h"

/* Standard sprintf() function. Work as the libc one. */
int sprintf_s(char * buf, size_t dstMax,const char *fmt, ...);
/* Standard snprintf() function from BSD, more secure... */
int snprintf_s(char * buf, size_t dstMax,size_t size, const char *fmt, ...);
/* Standard sscanf() function. Work as the libc one. */
int sscanf_s(const char * buf,const char * fmt, ...);
/* If you need to code your own printf... */
int vsprintf_s(char *buf, size_t dstMax,const char *fmt, va_list ap);
int vsnprintf_s(char *buf, size_t dstMax,size_t size, const char *fmt, va_list ap);
int vsscanf_s(const char *fp, const char *fmt0, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* STDIO_S_H */



