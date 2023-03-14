#ifndef __FACTORY_EFUSE_H_
#define __FACTORY_EFUSE_H_

#if defined(CHIP_BEST2002) || defined (CHIP_BEST2003)

#include "plat_types.h"

enum RF_EFUSE_DATA_TYPE {
    RF_EFUSE_2G_CALIB_FLAGS = 1,
    RF_EFUSE_5G_CALIB_FLAGS = 2,
    RF_EFUSE_FREQ_CALIB = 3,
    RF_EFUSE_FREQ_CALIB_FLAGS = 4,
    RF_EFUSE_BDR_POWER = 5,
    RF_EFUSE_EDR_POWER = 6,
    RF_EFUSE_BT_CALIB_FLAGS = 7,
    RF_EFUSE_WIFI_ADDR_CALIB_FLAGS = 8,
    RF_EFUSE_DATA_TYPE_NUM,
};

enum RF_EFUSE_POWER_TYPE {
    RF_EFUSE_2G_POWER = 0,
    RF_EFUSE_5G_POWER = 1,
    RF_EFUSE_POWER_NUM,
};

enum RF_EFUSE_PAGE_T {
    RF_EFUSE_PAGE_NULL           = -1,
    RF_EFUSE_PAGE_WIFI_ADDR3_4  = 0,
    RF_EFUSE_PAGE_WIFI_BT_ADDR5_3 = 2,
    RF_EFUSE_PAGE_BT_ADDR4_5    = 3,
    RF_EFUSE_PAGE_BT_PWR        = 4,
    RF_EFUSE_PAGE_WIFI_ADDR_FLG = 5,
    RF_EFUSE_PAGE_CH1_13        = 9,
    RF_EFUSE_PAGE_24G_FLG       = 9,
    RF_EFUSE_PAGE_CH36_56       = 10,
    RF_EFUSE_PAGE_5G_FLG        = 10,
    RF_EFUSE_PAGE_CH60_112      = 11,
    RF_EFUSE_PAGE_FREQ_FLG      = 11,
    RF_EFUSE_PAGE_CH116_136     = 12,
    RF_EFUSE_PAGE_BT_PWR_FLG    = 12,
    RF_EFUSE_PAGE_CH140_161     = 13,
    RF_EFUSE_PAGE_CH1165_169    = 14,
    RF_EFUSE_PAGE_FREQ_VAL      = 14,
};

enum POWER_LEVEL {
    RF_POWER_LEVEL_NULL = -1,
    RF_POWER_LEVEL_0x1000 = 0,
    RF_POWER_LEVEL_0x1100 = 1,
    RF_POWER_LEVEL_0x1200 = 2,
    RF_POWER_LEVEL_0x1300 = 3,
    RF_POWER_LEVEL_0x1400 = 4,
    RF_POWER_LEVEL_0x1500 = 5,
    RF_POWER_LEVEL_0x1600 = 6,
    RF_POWER_LEVEL_0x1700 = 7,
    RF_POWER_LEVEL_0x1800 = 8,
    RF_POWER_LEVEL_0x1900 = 9,
    RF_POWER_LEVEL_0x1A00 = 10,
    RF_POWER_LEVEL_0x1B00 = 11,
    RF_POWER_LEVEL_0x1C00 = 12,
    RF_POWER_LEVEL_0x1D00 = 13,
    RF_POWER_LEVEL_0x1E00 = 14,
    RF_POWER_LEVEL_0x1F00 = 15,
    RF_POWER_LEVEL_0x2000 = 16,
    RF_POWER_LEVEL_0x2100 = 17,
    RF_POWER_LEVEL_0x2200 = 18,
    RF_POWER_LEVEL_0x2300 = 19,
    RF_POWER_LEVEL_0x2400 = 20,
    RF_POWER_LEVEL_0x2500 = 21,
    RF_POWER_LEVEL_0x2600 = 22,
    RF_POWER_LEVEL_0x2700 = 23,
    RF_POWER_LEVEL_0x2800 = 24,
    RF_POWER_LEVEL_0x2900 = 25,
    RF_POWER_LEVEL_0x2A00 = 26,
    RF_POWER_LEVEL_0x2B00 = 27,
    RF_POWER_LEVEL_0x2C00 = 28,
    RF_POWER_LEVEL_0x2D00 = 29,
    RF_POWER_LEVEL_0x2E00 = 30,
    RF_POWER_LEVEL_0x2F00 = 31,
    RF_POWER_LEVEL_NUM,
};


int factory_set_power_into_efuse(unsigned short power,int ch);
int factory_set_calib_data_to_efuse(enum RF_EFUSE_DATA_TYPE data_type, uint16_t value);
int factory_get_calib_data_from_efuse(enum RF_EFUSE_DATA_TYPE data_type, uint16_t* value);

int factory_set_efuse_unit_test(int param, uint16_t freqcalib);
int factory_get_efuse_unit_test(uint8_t clear_flgs);

uint16_t factory_check_calib_status(enum RF_EFUSE_DATA_TYPE data_type);
uint16_t* factory_get_power_from_efuse(enum RF_EFUSE_POWER_TYPE pwr_type);

int factory_set_wifi_addr_into_efuse(uint16_t addr,int pos);
int factory_set_bt_addr_into_efuse(uint16_t addr,int pos);
int factory_set_wifi_addr_efuse_test(uint8_t *addr);
int factory_set_bt_addr_efuse_test(uint8_t *addr);
int factory_get_wifi_addr_efuse_test(uint8_t clear_flgs);
int factory_get_bt_addr_efuse_test(uint8_t clear_flgs);
#endif //CHIP_BEST2002 || CHIP_BEST2003

#endif
