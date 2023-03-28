#include "spi_msg.h"
#include "hal_trace.h"
#include "spi_dev.h"
#include <string.h>

uint8_t spi_msg_checksum(const struct spi_msg *msg)
{
    uint16_t i = 0;
    uint8_t cs = 0;
    cs = (uint8_t)(msg->type + msg->len);
    for (i = 0; i < msg->len; i++) {
        cs += msg->payload[i];
    }
    return cs;
}

int spi_msg_encode(struct spi_msg *msg, uint8_t type, const void *buf, uint16_t len)
{
    msg->type = (uint8_t)type;
    msg->cs = 0;
    msg->len = len;
    if (buf && len) {
        memcpy(msg->payload, buf, len);
    }
    msg->cs = spi_msg_checksum(msg);
    return SPI_MSG_HDR_SIZE + len;
}

int spi_msg_decode(struct spi_msg *msg)
{
    if (msg->len == 0)
        return -1;

    if (msg->cs != spi_msg_checksum(msg))
        return -1;

    return msg->len;
}

int spi_msg_check_hdr(struct spi_msg *msg)
{
    if (msg->len == 0) {
        if (msg->cs != spi_msg_checksum(msg))
            return -1;

        return 0;
    } else {
        return msg->len;
    }
}

void spi_print_hdr(const char *name, struct spi_msg *msg)
{
    printf("%s msg hdr: type %d, cs %d, len %d\r\n", name, msg->type, msg->cs, msg->len);
}

void spi_print_payload(const char *name, struct spi_msg *msg)
{
    uint16_t i;
    printf("%s msg payload:\r\n", name);
    for (i = 0; i < msg->len; i++) {
        printf("%02X ", msg->payload[i]);
    }
    printf("\r\n");
}