
/*******************************************************************************************************************************/
/*********************************************bt controller symbol**************************************************************/

#ifndef  __BT_CONTROLLER_SYMBOL_H__
#define  __BT_CONTROLLER_SYMBOL_H__


#define HCI_FC_ENV_ADDR                                               0XC0007F34
#define LD_ACL_ENV_ADDR                                               0XC00006A4
#define BT_UTIL_BUF_ENV_ADDR                                          0XC000086C
#define BLE_UTIL_BUF_ENV_ADDR                                         0XC0006C50
#define LD_BES_BT_ENV_ADDR                                            0XC0001714
#define DBG_STATE_ADDR                                                0XC0005D7C
#define LC_STATE_ADDR                                                 0XC0000B74
#define LD_SCO_ENV_ADDR                                               0XC00006BC
#define RX_MONITOR_ADDR                                               0XC00064B4
#define LC_ENV_ADDR                                                   0XC0000B64
#define LM_NB_SYNC_ACTIVE_ADDR                                        0XC00003D6
#define LM_ENV_ADDR                                                   0XC00001B4
#define LM_KEY_ENV_ADDR                                               0XC000030C
#define HCI_ENV_ADDR                                                  0XC0007E64
#define LC_SCO_ENV_ADDR                                               0XC00003B8
#define LLM_ENV_ADDR                                                  0XC0007040
#define LD_ENV_ADDR                                                   0XC0000C48
#define RWIP_ENV_ADDR                                                 0XC0005B14
#define BLE_RX_MONITOR_ADDR                                           0XC0006466
#define LLC_ENV_ADDR                                                  0XC0007984
#define RWIP_RF_ADDR                                                  0XC0005A7C
#define LD_ACL_METRICS_ADDR                                           0XC00003F0
#define RF_RX_HWGAIN_TBL_ADDR                                         0XC0004402
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC000437A
#define RF_RX_GAIN_FIXED_TBL_ADDR                                     0XC000645A
#define HCI_DBG_EBQ_TEST_MODE_ADDR                                    0XC0005E5C
#define HOST_REF_CLK_ADDR                                             0XC0005A30
#define DBG_TRC_TL_ENV_ADDR                                           0XC0005F20
#define DBG_TRC_MEM_ENV_ADDR                                          0XC0005F2C
#define DBG_BT_COMMON_SETTING_ADDR                                    0XC0005DF0
#define DBG_BT_SCHE_SETTING_ADDR                                      0XC0005E8C
#define DBG_BT_IBRT_SETTING_ADDR                                      0XC0005CEE
#define DBG_BT_HW_FEAT_SETTING_ADDR                                   0XC0005DB4
#define HCI_DBG_SET_SW_RSSI_ADDR                                      0XC0005EB4
#define LP_CLK_ADDR                                                   0XC0005B10
#define RWIP_PROG_DELAY_ADDR                                          0XC0005B08
#define DATA_BACKUP_CNT_ADDR                                          0XC0004060
#define DATA_BACKUP_ADDR_PTR_ADDR                                     0XC0004064
#define DATA_BACKUP_VAL_PTR_ADDR                                      0XC0004068
#define SCH_MULTI_IBRT_ADJUST_ENV_ADDR                                0XC00082D0
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC000437A
#define RF_RX_GAIN_THS_TBL_LE_ADDR                                    0XC00043E4
#define REPLACE_MOBILE_ADDR_ADDR                                      0XC0000BA0
#define REPLACE_ADDR_VALID_ADDR                                       0XC00000B8
#define PCM_NEED_START_FLAG_ADDR                                      0XC00000AC
#define I2V_VAL_ADDR                                                  0XC000009C
#define I2V_THD_ADDR                                                  0XC0000046
#define RT_SLEEP_FLAG_CLEAR_ADDR                                      0XC0005A64
#define RF_RX_GAIN_THS_TBL_BT_3M_ADDR                                 0XC000643C
#define TESTMODE_3M_FLAG_ADDR                                         0XC0005A54
#define TESTMODE_3M_EN_ADDR                                           0XC0005A53
#define TESTMODE_1M_FLAG_ADDR                                         0XC0005A52
#define TESTMODE_1M_EN_ADDR                                           0XC0005A51
#define NORMAL_IQTAB_ADDR                                             0XC0006484
#define NORMAL_IQTAB_EN_ADDR                                          0XC0005A4C
#define POWER_ADJUST_EN_ADDR                                          0XC0005A50
#define LD_IBRT_ENV_ADDR                                              0XC0001728
#define LLM_LOCAL_LE_FEATS_ADDR                                       0XC000447C
#define ISOOHCI_ENV_ADDR                                              0XC000830C
#define RF_RX_GAIN_THS_TBL_BT_ADDR                                    0XC00043C6
#define LLD_CON_ENV_ADDR                                              0XC0007D84
#define RT_SLEEP_FLAG_CLEAR_ADDR                                      0XC0005A64
#define I2V_VAL_ADDR                                                  0XC000009C
#define RT_SLEEP_FLAG_CLEAR_ADDR                                      0XC0005A64
#define RF_RX_GAIN_THS_TBL_BT_3M_ADDR                                 0XC000643C
#define TESTMODE_3M_FLAG_ADDR                                         0XC0005A54
#define NORMAL_IQTAB_ADDR                                             0XC0006484
#define POWER_ADJUST_EN_ADDR                                          0XC0005A50
#define LLD_ISO_ENV_ADDR                                              0XC0007DB0
#define HCI_DBG_SET_TRANSQ_FLAG_ADDR                                  0XC0005D60

//commit df5f1d0c51bb626ada220e56daa21578531ad5c0
//Author: TianxuWu <tianxuwu@bestechnic.com>
//Date:   Wed Dec 7 14:35:23 2022 +0800
//    Merge 2003_t1 patch modification.
//    
//    1.Not reset ble public address after hci reset
//    2.Always read remote name if lc state in lc_connect.
//    
//    Change-Id: I5fcb36759cbf5218b06be9ae07a7595448ffec03

#ifndef  BT_CONTROLLER_COMMIT_ID
#define  BT_CONTROLLER_COMMIT_ID                    "commit df5f1d0c51bb626ada220e56daa21578531ad5c0"
#endif
#ifndef  BT_CONTROLLER_COMMIT_AUTHOR
#define  BT_CONTROLLER_COMMIT_AUTHOR                "Author: TianxuWu <tianxuwu@bestechnic.com>"
#endif
#ifndef  BT_CONTROLLER_COMMIT_DATE
#define  BT_CONTROLLER_COMMIT_DATE                  "Date:   Wed Dec 7 14:35:23 2022 +0800"
#endif
#ifndef  BT_CONTROLLER_COMMIT_MESSAGE
#define  BT_CONTROLLER_COMMIT_MESSAGE               "    Merge 2003_t1 patch modification.          1.Not reset ble public address after hci reset     2.Always read remote name if lc state in lc_connect.          Change-Id: I5fcb36759cbf5218b06be9ae07a7595448ffec03 "
#endif

#endif
