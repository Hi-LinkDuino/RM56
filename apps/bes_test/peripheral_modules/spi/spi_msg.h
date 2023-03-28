#ifndef __SPI_MSG_H__
#define __SPI_MSG_H__

#include <stdint.h>

/** 
 * spi_msg_type:
 * read:    00 xx 00 00
 * write:   01 xx xx xx ...
 */
#define SPI_READ 0
#define SPI_WRITE 1

struct spi_msg {
    uint8_t type;
    uint8_t cs;
    uint16_t len;
    uint8_t payload[];
};

#define SPI_MSG_HDR_SIZE (sizeof(struct spi_msg))

/* spi slave states */
enum spi_states {
    SPI_RX_HDR,
    SPI_RX_DATA,
    SPI_TX_DATA
};

/* spi slave events */
enum spi_events {
    SPI_RX_HDR_WITHOUT_DATA_DONE,
    SPI_RX_HDR_WITH_DATA_DONE,
    SPI_RX_DATA_DONE,
    SPI_TX_DONE,
    SPI_ERR
};

uint8_t spi_msg_checksum(const struct spi_msg *msg);
int spi_msg_encode(struct spi_msg *msg, uint8_t type, const void *buf, uint16_t len);
int spi_msg_decode(struct spi_msg *msg);
int spi_msg_check_hdr(struct spi_msg *msg);
void spi_print_hdr(const char *name, struct spi_msg *msg);
void spi_print_payload(const char *name, struct spi_msg *msg);

#endif
