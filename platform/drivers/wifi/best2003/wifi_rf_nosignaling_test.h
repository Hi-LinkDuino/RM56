
#ifndef __WIFI_RF_NOSIGNALING_TEST_H__
#define __WIFI_RF_NOSIGNALING_TEST_H__

#include <stdbool.h>
#include "wifi_test.h"

bool rf_cmd_nosignaling_test_handler(uint32_t usb_recv_buf[]);

void rf_nosignaling_test_start_up(void);

#endif
