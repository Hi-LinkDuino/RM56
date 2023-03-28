
/*******************************************************************************************************************************/
/*********************************************bt controller symbol**************************************************************/

#ifndef  __BT_CONTROLLER_SYMBOL_H__
#define  __BT_CONTROLLER_SYMBOL_H__


#define HCI_FC_ENV_ADDR                                               0XC0007DDC
#define LD_ACL_ENV_ADDR                                               0XC00006B0
#define BT_UTIL_BUF_ENV_ADDR                                          0XC0000880
#define BLE_UTIL_BUF_ENV_ADDR                                         0XC0006AF8
#define LD_BES_BT_ENV_ADDR                                            0XC0001728
#define DBG_STATE_ADDR                                                0XC0005D64
#define LC_STATE_ADDR                                                 0XC0000B88
#define LD_SCO_ENV_ADDR                                               0XC00006CC
#define RX_MONITOR_ADDR                                               0XC000635C
#define LC_ENV_ADDR                                                   0XC0000B78
#define LM_NB_SYNC_ACTIVE_ADDR                                        0XC00003E2
#define LM_ENV_ADDR                                                   0XC00001C4
#define LM_KEY_ENV_ADDR                                               0XC0000318
#define HCI_ENV_ADDR                                                  0XC0007D0C
#define LC_SCO_ENV_ADDR                                               0XC00003C4
#define LLM_ENV_ADDR                                                  0XC0006EE8
#define LD_ENV_ADDR                                                   0XC0000C5C
#define RWIP_ENV_ADDR                                                 0XC0005AFC
#define BLE_RX_MONITOR_ADDR                                           0XC000630E
#define LLC_ENV_ADDR                                                  0XC000782C
#define RWIP_RF_ADDR                                                  0XC0005A64
#define LD_ACL_METRICS_ADDR                                           0XC00003FC
#define RF_RX_HWGAIN_TBL_ADDR                                         0XC000440A
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC0004382
#define RF_RX_GAIN_FIXED_TBL_ADDR                                     0XC0006302
#define HCI_DBG_EBQ_TEST_MODE_ADDR                                    0XC0005E44
#define HOST_REF_CLK_ADDR                                             0XC0005A18
#define DBG_TRC_TL_ENV_ADDR                                           0XC0005F08
#define DBG_TRC_MEM_ENV_ADDR                                          0XC0005F14
#define DBG_BT_COMMON_SETTING_ADDR                                    0XC0005DD8
#define DBG_BT_SCHE_SETTING_ADDR                                      0XC0005E74
#define DBG_BT_IBRT_SETTING_ADDR                                      0XC0005CD6
#define DBG_BT_HW_FEAT_SETTING_ADDR                                   0XC0005D9C
#define HCI_DBG_SET_SW_RSSI_ADDR                                      0XC0005E9C
#define LP_CLK_ADDR                                                   0XC0005AF8
#define RWIP_PROG_DELAY_ADDR                                          0XC0005AF0
#define DATA_BACKUP_CNT_ADDR                                          0XC0004060
#define DATA_BACKUP_ADDR_PTR_ADDR                                     0XC0004064
#define DATA_BACKUP_VAL_PTR_ADDR                                      0XC0004068
#define SCH_MULTI_IBRT_ADJUST_ENV_ADDR                                0XC0008178
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC0004382
#define RF_RX_GAIN_THS_TBL_LE_ADDR                                    0XC00043EC
#define REPLACE_MOBILE_ADDR_ADDR                                      0XC0000BB4
#define REPLACE_ADDR_VALID_ADDR                                       0XC00000C8
#define PCM_NEED_START_FLAG_ADDR                                      0XC00000B0
#define I2V_VAL_ADDR                                                  0XC000009C
#define I2V_THD_ADDR                                                  0XC0000046
#define RF_RX_GAIN_THS_TBL_BT_3M_ADDR                                 0XC00062E4
#define TESTMODE_3M_FLAG_ADDR                                         0XC0005A3C
#define TESTMODE_3M_EN_ADDR                                           0XC0005A3B
#define TESTMODE_1M_FLAG_ADDR                                         0XC0005A3A
#define TESTMODE_1M_EN_ADDR                                           0XC0005A39
#define NORMAL_IQTAB_ADDR                                             0XC000632C
#define NORMAL_IQTAB_EN_ADDR                                          0XC0005A34
#define POWER_ADJUST_EN_ADDR                                          0XC0005A38
#define LD_IBRT_ENV_ADDR                                              0XC000173C
#define LLM_LOCAL_LE_FEATS_ADDR                                       0XC0004484
#define ISOOHCI_ENV_ADDR                                              0XC00081B4
#define RF_RX_GAIN_THS_TBL_BT_ADDR                                    0XC00043CE
#define LLD_CON_ENV_ADDR                                              0XC0007C2C
#define I2V_VAL_ADDR                                                  0XC000009C
#define RF_RX_GAIN_THS_TBL_BT_3M_ADDR                                 0XC00062E4
#define TESTMODE_3M_FLAG_ADDR                                         0XC0005A3C
#define NORMAL_IQTAB_ADDR                                             0XC000632C
#define POWER_ADJUST_EN_ADDR                                          0XC0005A38
#define LLD_ISO_ENV_ADDR                                              0XC0007C58
#define HCI_DBG_SET_TRANSQ_FLAG_ADDR                                  0XC0005D48

//commit b03bfc753a6997fff25153662bb15bb046bc1566
//Author: TianxuWu <tianxuwu@bestechnic.com>
//Date:   Fri Jul 1 16:23:29 2022 +0800
//    For 2003 chip, only set hwspi reg once.
//    
//    Change-Id: I25f4636a22d3ec42cfb386b383acacdd7fdd843b

#ifndef  BT_CONTROLLER_COMMIT_ID
#define  BT_CONTROLLER_COMMIT_ID                    "commit b03bfc753a6997fff25153662bb15bb046bc1566"
#endif
#ifndef  BT_CONTROLLER_COMMIT_AUTHOR
#define  BT_CONTROLLER_COMMIT_AUTHOR                "Author: TianxuWu <tianxuwu@bestechnic.com>"
#endif
#ifndef  BT_CONTROLLER_COMMIT_DATE
#define  BT_CONTROLLER_COMMIT_DATE                  "Date:   Fri Jul 1 16:23:29 2022 +0800"
#endif
#ifndef  BT_CONTROLLER_COMMIT_MESSAGE
#define  BT_CONTROLLER_COMMIT_MESSAGE               "    For 2003 chip, only set hwspi reg once.          Change-Id: I25f4636a22d3ec42cfb386b383acacdd7fdd843b "
#endif

#endif
