#include <stdlib.h>
#include <stdio.h>

#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))

// 33:92:23:7A:FA:28 -> 28fa7a239233
void macToBtAddress(unsigned char * mac, unsigned char * btAddr)
{
    int i = 0;
    char *start = NULL, *end = NULL;

    start = (char *)mac;
    for(i = 5; i >= 0; i --){
        btAddr[i] = start ? strtoul(start, &end, 16) : 0;
        //*end means meet ":"
        if(end) {
            start = (*end) ? end + 1 : end;
        }
    }
}

// 28fa7a239233 -> 33:92:23:7A:FA:28
void btAddressToMac(unsigned char * mac, unsigned char * btAddr)
{
    sprintf((char *)mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                        btAddr[5],btAddr[4],btAddr[3],btAddr[2],btAddr[1],btAddr[0]);
}

static int char2byte(int8_t ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }

    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }

    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' +10;
    }

    return -1;
}

int bt_hexstr2bytes(int8_t *str, uint8_t *bytes)
{
    int len = 0;

    while (isxdigit(*(str + len)) && isxdigit(*(str + len + 1))) {
        *bytes = char2byte(*(str + len)) * 16 + char2byte(*(str + len + 1));
        len ++;
        str ++;
        bytes ++;
    }

    return len;
}

char *bt_bytes2hexstr(char *str, size_t str_size, uint8_t *hex, size_t len)
{
    static char * encode = "0123456789ABCDEF";

    size_t i;

    if ((2*len + 1) > str_size)
        return NULL;

    int j = 0;

    for (i = 0; i < len; i++) {
        unsigned char a = ((unsigned char *) hex)[i];
        str[j++] = encode[(a >> 4) & 0xf];
        str[j++] = encode[a & 0xf];
    }

    str[j] = 0;

    return str;
}

char *bt_bytes2hexstr_reverse(char *str, size_t str_size, uint8_t *hex, size_t len)
{
    static char * encode = "0123456789ABCDEF";

    int i;

    if ((2*len + 1) > str_size)
        return NULL;

    int j = 0;

    for (i = len-1; i >= 0; i--) {
        unsigned char a = ((unsigned char *) hex)[i];
        str[j++] = encode[(a >> 4) & 0xf];
        str[j++] = encode[a & 0xf];
    }

    str[j] = 0;

    return str;
}
