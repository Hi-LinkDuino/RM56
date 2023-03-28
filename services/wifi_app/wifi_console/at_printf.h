#ifdef DEBUG_SPP_LOG

#ifndef __AT_PRINTF_H__
#define __AT_PRINTF_H__

unsigned int SppPrintf(const char *fmt, ...);
unsigned int SppResponseInfo(const char *fmt, ...);
unsigned int SppAtShowCmd(const char *fmt, ...);

#endif

#endif

