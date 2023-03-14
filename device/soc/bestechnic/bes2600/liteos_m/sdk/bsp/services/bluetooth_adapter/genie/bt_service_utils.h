#ifndef __BT_SERVICE_UTILS_H__
#define __BT_SERVICE_UTILS_H__
void macToBtAddress(unsigned char * mac, unsigned char * btAddr);
void btAddressToMac(unsigned char * mac, unsigned char * btAddr);
int bt_hexstr2bytes(int8_t *str, uint8_t *bytes);
char *bt_bytes2hexstr(char *str, size_t str_size, uint8_t *hex, size_t len);
char *bt_bytes2hexstr_reverse(char *str, size_t str_size, uint8_t *hex, size_t len);
#endif
