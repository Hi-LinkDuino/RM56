#ifndef __RF_COMMAND__
#define __RF_COMMAND__


//define the RF test mode
/*
注：此寄存器bit7:0为配置需要执行的测试方式，
  0x02：TX;
  0x03: RX;
  0xF2: TX tone;//单载波
  0xF3: TX continue;
  0xF4: iq calib;
  0xF5: two TX tone;

*/
#define RF_TX_MODE              0x02
#define RF_TX_TONE_MODE         0xF2
#define RF_TX_DC_TONE_MODE      0xFD
#define RF_DPD_CALI_MODE        0XF6
#define RF_TX_CONTINUE_MODE     0xF3
#define RF_TX_TWO_TONE_MODE     0xF5
#define RF_RX_MODE              0x03
#define RF_IQ_CALIB_MODE        0xF4
#define RF_MEM_DUMP_MODE        0xF7
#define RF_TX_STOP              0xFF
#define RF_TX_11B_CNT           0xE0
#define RF_TX_11G_CNT           0xE1
#define RF_TX_11N_CNT           0xE2
#define RF_TX_STOP_CNT          0xE3
//#define BAND_2G4 				0
//#define BAND_5G  				1

/*
    function: the first cmd that enter into rf test mode
*/
void wifi_rf_start(void);

/*
    function:parse cmd
*/
extern int rf_parse_command(char *argstr);

extern int wifi_rf_test_enable(char* band);

/*
    fuction:select diferent mode, tx, or rx etc
    testType =0x02：TX;
    testType =0x03: RX;
*/
extern int wifi_rf_test_mode(int testType);

/*
    function:stop tx
*/
extern void wifi_rf_set_tx_stop(void);

/*
    function:set channel
    2.4g :1~14
    5g:36 ~165
*/
extern int wifi_rf_test_set_channel(int channel);

/*
    function:get currnet work channel
*/
extern int wifi_rf_test_get_channel( void );

/*
   function:set tx interval,defual 8ms
*/
extern int wifi_rf_test_set_tx_interval(int interval);

/*
    fuction:get current tx interval
*/
extern int wifi_rf_test_get_tx_interval();

/*
    function:set tx packet length,500~4096,default:1024
*/
extern int wifi_rf_test_set_frame_length(int frameLength);

/*
    function:get current tx packet lenght
*/
extern int wifi_rf_test_get_frame_length( );

/*
    function:set different mode,rateIndx 0~21
    rateIndx = 3 ~11b
    rateIndx = 13 ~11g
    rateIndx = 21 ~11n
*/
extern int wifi_rf_test_set_rate(int rateIndx);

/*
    fuction:get current mode
    3~11b
   13~11g
   21~11n
*/
extern int wifi_rf_test_get_rate( void );

/*
    fuction:set tx powerlevel 0~0x3fff,default 0x1400
*/
extern int wifi_rf_test_calibrate_txPower(int powerlevel);

/*
    function:get tx powerlevel
*/
extern int wifi_rf_test_get_txPower( void );


/*
    function:set current reg powerlevel into flash
*/
extern int wifi_rf_test_power_save();


/*
    function:get powerlevel form flash
*/
extern unsigned short* wifi_rf_test_get_power_table_from_flash();

/*
    function:adjust Clockfreq
*/
extern int wifi_rf_test_calibrate_ClockfreqOffset(  unsigned short freqOffset);

/*
    function:get currnet clock freq
*/
extern int wifi_rf_test_get_ClockfreqOffset( void );

/*
    function: save reg clock freq into flash
*/
extern int wifi_rf_test_save_clock_freq();

/*
    function:get save clock freq form flash
*/
extern int wifi_rf_test_get_save_clock_freq();

/*
    function：get currnt temperature
*/
extern int wifi_rf_test_get_temp();

/*
    function:get chip receive packet count
*/
extern unsigned int wifi_rf_rx_ack_statistic(void);

/*
    function:clear reg for receiving packet count
*/
extern void wifi_rf_set_rx_ack_statistic(void);

/*
    function:open temperature calibration
*/
extern int wifi_rf_temp_calibration_open();

/*
    function:set caliration finish flags
*/
extern int wifi_rf_tx_power_calib_finish();


/*
    function:from wifi rf switch to bt rf test
*/
extern void wifi_rf_to_bt();

/*
    function:stop rx
*/
extern int wifi_rf_rx_stop();

/*
    function:read reg
*/
extern void wifi_rf_mem_read(char** argv);

/*
    function:write reg
*/
extern int wifi_rf_mem_write(char** argv);

/*
    function:save calibration temperature into flash
*/
extern int wifi_rf_test_save_temp();

/*
    function:get calibration termperature form flash
*/
extern int wifi_rf_test_get_save_temp();

/*
    function: set the power into default value
*/
extern int wifi_rf_set_default_power();

extern void wifi_set_rx_not_filter(void);
extern void wifi_set_rx_filter(void);

/*
    set rx filter mac addess
    type:00:xx:xx:xx:xx:xx
*/
extern unsigned int wifi_rf_test_get_rx_macaddr();
extern int wifi_rf_test_set_rx_macaddr(char* macParam);


unsigned char* bt_get_device_btmac_from_factory( void );
int bt_set_device_btmac_to_factory(char *argv);

/*
    set mac addess to flash for device
    type:00:xx:xx:xx:xx:xx
*/
extern int wifi_set_device_mac_to_factory(char *argv);
extern unsigned char* wifi_get_device_mac_from_factory( void );

extern int wifi_rf_test_get_all_ch_power_table_from_flash();
extern int wifi_rf_test_power_all_ch_save();
extern void wifi_rf_power_list();
extern void wifi_rf_reg_dump(void);
extern void wifi_rf_green_field_mode(unsigned char value);
extern int wifi_rf_temp_calibration_open();
extern int wifi_rf_temp_calibration_close();
extern int wifi_rf_set_tx_power_config(int* tx_power);
//extern int wifi_rf_rssi_task(void);
extern int wifi_rf_reg_status_debug_throughput(void);
extern int wifi_rf_agc_select(unsigned char id,unsigned char bandWidth_limit);
extern void wifi_rf_printf_calib_param();

extern int wifi_rf_test_save_all_power_toflash_2g4(unsigned short* powertable , unsigned char len);
extern int wifi_rf_test_save_all_power_toflash_5g(unsigned short * powertable, unsigned char len);

#endif //__RF_COMMAND__



