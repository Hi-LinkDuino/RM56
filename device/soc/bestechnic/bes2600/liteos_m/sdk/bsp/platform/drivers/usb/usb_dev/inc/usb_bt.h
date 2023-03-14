#ifndef __USB_BT_H__
#define __USB_BT_H__
#include "hal_usb.h"
#include "stdint.h"
int usbbt_open(void);
bool usbbt_bulk_send_compl_handler(const uint8_t *data, uint32_t length, enum XFER_COMPL_STATE state);
bool usbbt_bulk_recv_compl_handler(const uint8_t *data, uint32_t length, enum XFER_COMPL_STATE state);
#endif