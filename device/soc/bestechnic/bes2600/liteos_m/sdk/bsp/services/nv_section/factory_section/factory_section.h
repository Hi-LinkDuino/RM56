/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __FACTORY_SECTIONS_H__
#define __FACTORY_SECTIONS_H__

#define ALIGN4 __attribute__((aligned(4)))
#define nvrec_mini_version      1
#define nvrec_dev_magic         0xba80
#define nvrec_current_version   2
#define FACTORY_SECTOR_SIZE     4096

typedef struct{
    unsigned short magic;
    unsigned short version;
    unsigned int crc ;
    unsigned int reserved0;
    unsigned int reserved1;
}section_head_t;

typedef struct {
    unsigned int  rev2_reserved0;
    unsigned int  rev2_reserved1;
    unsigned int  rev2_bt_name[63];
    unsigned int  rev2_bt_addr[2];
    unsigned int  rev2_ble_addr[2];
    unsigned int  rev2_dongle_addr[2];
    unsigned int  rev2_xtal_fcap;
    unsigned int  rev2_ble_name[8];
} factory_rev2_data_t;

typedef struct{
    unsigned char device_name[248+1] ALIGN4;
    unsigned char bt_address[8] ALIGN4;
    unsigned char ble_address[8] ALIGN4;
    unsigned char tester_address[8] ALIGN4;
    unsigned int  xtal_fcap ALIGN4;
    unsigned int  rev1_data_len;

    unsigned int  rev2_data_len;
    unsigned int  rev2_crc;
    unsigned int  rev2_reserved0;
    unsigned int  rev2_reserved1;
    unsigned int  rev2_bt_name[63];
    unsigned int  rev2_bt_addr[2];
    unsigned int  rev2_ble_addr[2];
    unsigned int  rev2_dongle_addr[2];
    unsigned int  rev2_xtal_fcap;
    unsigned int  rev2_ble_name[8];
}factory_section_data_t;

typedef struct{
    section_head_t head;
    factory_section_data_t data;
}factory_section_t;

typedef struct
{
    unsigned short magic;
    unsigned short version;
    unsigned int crc;
} section_wifi_head_t;

typedef struct
{
    unsigned char mac_addr[6];
    unsigned short freq_cal;
    unsigned char ble_power_control;
    unsigned char wifi_table_select;
    unsigned char reserve2;
    unsigned char reserve3;

    unsigned int iQ_offset;
/*
	tx_power_type 0 - save bgn 1,7,13 power
	tx_power_type 1 - save bgn 1-13 power
	tx_power_type 0xff - not calibration
*/
    unsigned char tx_power_type;

/*
	freq_cal_flags 0 - chip not calibrated
	freq_cal_flags 1 - chip has been calibrated
*/
    unsigned char freq_cal_flags;

/*
	 when freq_cal_flags value is 0
	 index 0-8
			0-2:11b ch1, 11b ch7, 11b ch13
			3-5:11g ch1, 11g ch7, 11g ch13
			6-8:11n ch1, 11n ch7, 11n ch13

	  when freq_cal_flags value is 1
     index 0-41
            0-13:11b ch1, 11b ch2 ......11b ch14
           14-27:11g ch1, 11g ch2 ......11g ch14
           28-41:11n ch1, 11n ch2 ......11n ch14
*/
    unsigned short tx_power_ch[9];
    unsigned short rf_dc_cali_data_one[33];//0-5:2g dc; 6-32:5g dc

    unsigned short temperature;;
    unsigned char band; //0:2.4 1:5 2:dual band
    unsigned char country[3];
    unsigned int bt_tx_power[4];

    /*
        5G power calibration 0-12 save the 5G calib power,13-15 reserve
        5G power calibration 0-12 save the 5G calib power,13-15 reserve
        11g
        0   36~40��1    44~48��2    52~56��3    60~64;
        4   100~104��5    108~112��6    116~120;
        7   124~128��8    132~136��9    140~144
        11    149~153; 12   157~161��13   165~169

        11n
        0   36~40��1    44~48��2    52~56��3    60~64;
        4   100~104��5    108~112��6    116~120;
        7   124~128��8    132~136��9    140~144
        11    149~153; 12   157~161��13   165~169

    */
#if 1 //def __DUAL_BAND__
        unsigned short tx_power_ch_5G[29];
        unsigned short rf_dc_cali_data_two;//5g dc
        unsigned long  rf_dc_data_crc;

    /*
        0- it means that power not calib
        1- it means that power have clibrated
    */
        unsigned short tx_power_flags_5G;
    /*
        The temperature after 5G clibrating.
    */
        unsigned short temperature_5G;
#endif
} factory_section_wifi_data_t;

/* offset 2k */
#define FACTORY_SECTION_WIFI_OFFSET 0x800
typedef struct{
    section_wifi_head_t head;
    factory_section_wifi_data_t data;
}factory_section_wifi_t;

#ifdef __cplusplus
extern "C" {
#endif

void factory_section_init(void);
int factory_section_open(void);
void factory_section_original_btaddr_get(uint8_t *btAddr);
int factory_section_set_bt_name(const char *name,int len);
int factory_section_set_ble_name(const char *name,int len);
int factory_section_xtal_fcap_get(unsigned int *xtal_fcap);
int factory_section_xtal_fcap_set(unsigned int xtal_fcap);
uint8_t* factory_section_get_bt_address(void);
int factory_section_set_bt_address(uint8_t* btAddr);
uint8_t* factory_section_get_ble_address(void);
uint8_t* factory_section_get_bt_name(void);
uint8_t* factory_section_get_ble_name(void);
uint32_t factory_section_get_version(void);
uint8_t *factory_section_get_wifi_address(void);
int factory_section_set_wifi_address(uint8_t *wifi_addr);
int factory_section_set_wifi_tx_power_ch(int power,int index);
factory_section_wifi_t* factory_section_get_wifi_data();
uint8_t* factory_section_get_wifi_tx_power(void);
uint8_t *factory_section_get_wifi_tx_power_5G(void);
int  factory_section_set_wifi_temp(int temp);
int  factory_section_set_wifi_xtal(unsigned short xtal);
int  factory_section_set_bt_tx_power(uint8_t index, uint32_t div_value, uint32_t txPower);

int  factory_section_set_wifi_tx_power_all_ch(int power,int index);

int  factory_section_set_wifi_power_type(unsigned short powerType);

int factory_section_wifi_band_status(int bandType);
char *factory_section_get_country_code(void);
int  factory_section_set_country_code(char* counryCode);

unsigned char factory_section_get_ble_power_control();
int  factory_section_set_ble_power_control(unsigned char ble_power_control);
int  factory_section_select_power_table_for_tpc(unsigned char table_index);
unsigned char factory_section_get_wifi_power_table_idx();
int  factory_section_save_dc_cali_data(uint16_t* dcCalibValue2g, uint16_t* dcCalibValue5g);


#ifdef __cplusplus
}
#endif
#endif
