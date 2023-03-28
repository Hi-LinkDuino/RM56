#ifndef __HW_CRC_H__
#define __HW_CRC_H__

void hw_checksum_init(void);
uint16 hw_checksum(void *data, int16 len, uint32_t src, uint32_t dest, uint8_t proto);

void hw_crc_init(void);
uint32_t hw_crc(void *data, int16 len, uint32_t init, uint32_t poly, uint8_t flag);

#endif

