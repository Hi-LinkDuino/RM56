#ifndef __CRC32_H__
#define __CRC32_H__

#include "net_defs.h"
#include "byteorder.h"

extern u32  crc32_le(u32 crc, unsigned char const *p, size_t len);
extern u32  crc32_be(u32 crc, unsigned char const *p, size_t len);

extern u32  __crc32c_le(u32 crc, unsigned char const *p, size_t len);

#define crc32(seed, data, length)  crc32_le(seed, (unsigned char const *)(data), length)


#endif

