
/*******************************************************************************************************************************/
/*********************************************bt controller symbol**************************************************************/

#ifndef  __BT_CONTROLLER_SYMBOL_H__
#define  __BT_CONTROLLER_SYMBOL_H__


#define HCI_FC_ENV_ADDR                                               0XC000803C
#define LD_ACL_ENV_ADDR                                               0XC000069C
#define BT_UTIL_BUF_ENV_ADDR                                          0XC0000864
#define BLE_UTIL_BUF_ENV_ADDR                                         0XC0006D58
#define LD_BES_BT_ENV_ADDR                                            0XC000170C
#define DBG_STATE_ADDR                                                0XC0005D54
#define LC_STATE_ADDR                                                 0XC0000B6C
#define LD_SCO_ENV_ADDR                                               0XC00006B4
#define RX_MONITOR_ADDR                                               0XC00065BC
#define LC_ENV_ADDR                                                   0XC0000B5C
#define LM_NB_SYNC_ACTIVE_ADDR                                        0XC00003CE
#define LM_ENV_ADDR                                                   0XC00001AC
#define LM_KEY_ENV_ADDR                                               0XC0000304
#define HCI_ENV_ADDR                                                  0XC0007F6C
#define LC_SCO_ENV_ADDR                                               0XC00003B0
#define LLM_ENV_ADDR                                                  0XC0007148
#define LD_ENV_ADDR                                                   0XC0000C40
#define RWIP_ENV_ADDR                                                 0XC0005AEC
#define BLE_RX_MONITOR_ADDR                                           0XC000656E
#define LLC_ENV_ADDR                                                  0XC0007A8C
#define RWIP_RF_ADDR                                                  0XC0005A54
#define LD_ACL_METRICS_ADDR                                           0XC00003E8
#define RF_RX_HWGAIN_TBL_ADDR                                         0XC00043FE
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC0004376
#define RF_RX_GAIN_FIXED_TBL_ADDR                                     0XC0006562
#define HCI_DBG_EBQ_TEST_MODE_ADDR                                    0XC0005E24
#define HOST_REF_CLK_ADDR                                             0XC0005A08
#define DBG_TRC_TL_ENV_ADDR                                           0XC0005EE8
#define DBG_TRC_MEM_ENV_ADDR                                          0XC0005EF4
#define DBG_BT_COMMON_SETTING_ADDR                                    0XC0005DB8
#define DBG_BT_SCHE_SETTING_ADDR                                      0XC0005E54
#define DBG_BT_IBRT_SETTING_ADDR                                      0XC0005CC6
#define DBG_BT_HW_FEAT_SETTING_ADDR                                   0XC0005D84
#define HCI_DBG_SET_SW_RSSI_ADDR                                      0XC0005E7C
#define LP_CLK_ADDR                                                   0XC0005AE8
#define RWIP_PROG_DELAY_ADDR                                          0XC0005AE0
#define DATA_BACKUP_CNT_ADDR                                          0XC0004060
#define DATA_BACKUP_ADDR_PTR_ADDR                                     0XC0004064
#define DATA_BACKUP_VAL_PTR_ADDR                                      0XC0004068
#define SCH_MULTI_IBRT_ADJUST_ENV_ADDR                                0XC00083D8
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC0004376
#define RF_RX_GAIN_THS_TBL_LE_ADDR                                    0XC00043E0
#define REPLACE_MOBILE_ADDR_ADDR                                      0XC0000B98
#define REPLACE_ADDR_VALID_ADDR                                       0XC00000B8
#define PCM_NEED_START_FLAG_ADDR                                      0XC00000AC
#define I2V_VAL_ADDR                                                  0XC000009C
#define I2V_THD_ADDR                                                  0XC0000046
#define RT_SLEEP_FLAG_CLEAR_ADDR                                      0XC0005A3C
#define RF_RX_GAIN_THS_TBL_BT_3M_ADDR                                 0XC0006544
#define TESTMODE_3M_FLAG_ADDR                                         0XC0005A2C
#define TESTMODE_3M_EN_ADDR                                           0XC0005A2B
#define TESTMODE_1M_FLAG_ADDR                                         0XC0005A2A
#define TESTMODE_1M_EN_ADDR                                           0XC0005A29
#define NORMAL_IQTAB_ADDR                                             0XC000658C
#define NORMAL_IQTAB_EN_ADDR                                          0XC0005A24
#define POWER_ADJUST_EN_ADDR                                          0XC0005A28
#define LD_IBRT_ENV_ADDR                                              0XC0001720
#define LLM_LOCAL_LE_FEATS_ADDR                                       0XC0004478
#define ISOOHCI_ENV_ADDR                                              0XC0008414
#define RF_RX_GAIN_THS_TBL_BT_ADDR                                    0XC00043C2
#define LLD_CON_ENV_ADDR                                              0XC0007E8C
#define RT_SLEEP_FLAG_CLEAR_ADDR                                      0XC0005A3C
#define I2V_VAL_ADDR                                                  0XC000009C
#define RT_SLEEP_FLAG_CLEAR_ADDR                                      0XC0005A3C
#define RF_RX_GAIN_THS_TBL_BT_3M_ADDR                                 0XC0006544
#define TESTMODE_3M_FLAG_ADDR                                         0XC0005A2C
#define NORMAL_IQTAB_ADDR                                             0XC000658C
#define POWER_ADJUST_EN_ADDR                                          0XC0005A28
#define LLD_ISO_ENV_ADDR                                              0XC0007EB8
#define HCI_DBG_SET_TRANSQ_FLAG_ADDR                                  0XC0005D38

//commit 756d5b525866e899f62c96bae7545790373d1519
//Author: qiuyimei <yimeiqiu@bestechnic.com>
//Date:   Tue Apr 11 11:31:59 2023 +0800
//    optimize page schedule(part II)
//    
//    Change-Id: I13867c97835bd1a8812da0fdf288f36b30588336

#ifndef  BT_CONTROLLER_COMMIT_ID
#define  BT_CONTROLLER_COMMIT_ID                    "commit 756d5b525866e899f62c96bae7545790373d1519"
#endif
#ifndef  BT_CONTROLLER_COMMIT_AUTHOR
#define  BT_CONTROLLER_COMMIT_AUTHOR                "Author: qiuyimei <yimeiqiu@bestechnic.com>"
#endif
#ifndef  BT_CONTROLLER_COMMIT_DATE
#define  BT_CONTROLLER_COMMIT_DATE                  "Date:   Tue Apr 11 11:31:59 2023 +0800"
#endif
#ifndef  BT_CONTROLLER_COMMIT_MESSAGE
#define  BT_CONTROLLER_COMMIT_MESSAGE               "    optimize page schedule(part II)          Change-Id: I13867c97835bd1a8812da0fdf288f36b30588336 "
#endif

#endif
