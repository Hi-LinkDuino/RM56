/**
 ****************************************************************************************
 *
 * @file gaf_cfg.h
 *
 * @brief Generic Audio Framework - Configuration
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef GAF_CFG_H_
#define GAF_CFG_H_

/**
 ****************************************************************************************
 * @addtogroup GAF - Generic Audio Framework
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"            // RW IP Configuration
#include <stdint.h>                 // Standard Integer Definitions
#include <stdbool.h>                // Standard Boolean definitions
#include "co_math.h"                // Common Math Functions Definitions

/*
 * COMPILATION FLAGS
 ****************************************************************************************
 */
#if BLE_AUDIO_ENABLED
#define CFG_GAF_API_MSG
#define CFG_GAF_DBG
#define CFG_GAF_VS

// IAP supported
#define CFG_IAP_INTF_NB (2)

// ARC supported
#define CFG_ARC_VCS
#define CFG_ARC_MICS
#define CFG_ARC_VOCS
#define CFG_ARC_AICS

// ACC supported
#define CFG_ACC_MCS
#define CFG_ACC_TBS
#define CFG_ACC_OTS

// ATC supported
//#define CFG_ATC_RAAS
#define CFG_ATC_CSISM
#define CFG_CAPS

// BAP supported
#define CFG_BAP_UC_SRV
#define CFG_BAP_CAPA_SRV
#define CFG_BAP_BC_SRC
#define CFG_BAP_BC_SCAN
#define CFG_BAP_BC_DELEG
#define CFG_BAP_BC_ASSIST

#if 1
// ARC supported
#define CFG_ARC_VCC
#define CFG_ARC_MICC
#define CFG_ARC_VOCC
#define CFG_ARC_AICC

// ACC supported
#define CFG_ACC_MCC
#define CFG_ACC_TBC
#define CFG_ACC_OTC

// ATC supported
//#define CFG_ATC_RAAC
#define CFG_ATC_CSISC
#define CFG_CAPC

// BAP supported
#define CFG_BAP_UC_CLI
#define CFG_BAP_CAPA_CLI
#define CFG_BAP_BC_SINK
#endif
#else
#define CFG_IAP_INTF_NB (0)
#endif


#if (BLE_EMB_PRESENT)
/// Number of streams that can be created using IAP
#define IAP_NB_STREAMS           (BLE_ISO_CHANNEL_MAX)
#else //(BLE_EMB_PRESENT)
/// Number of streams that can be created using IAP
#define IAP_NB_STREAMS           (3)
#endif //(BLE_EMB_PRESENT)

/// Unicast mode supported
#if defined(CFG_CIS)
#define GAF_UNICAST_SUPP         (1)
#else //(defined(CFG_CIS))
#define GAF_UNICAST_SUPP         (0)
#endif //(defined(CFG_CIS))

/// Broadcast mode supported
#if defined(CFG_BIS)
#define GAF_BROADCAST_SUPP       (1)
#if (BLE_OBSERVER)
#define GAF_BROADCAST_SLV_SUPP   (1)
#else
#define GAF_BROADCAST_SLV_SUPP   (0)
#endif //(BLE_OBSERVER)
#if (BLE_BROADCASTER)
#define GAF_BROADCAST_MST_SUPP   (1)
#else
#define GAF_BROADCAST_MST_SUPP   (0)
#endif //(BLE_OBSERVER)
#else //(defined(CFG_BIS))
#define GAF_BROADCAST_SUPP       (0)
#endif //(defined(CFG_BIS))

/// IAP supported
#if (CFG_IAP_INTF_NB != 0)
#define GAF_IAP                  (1)
#else //(CFG_IAP_INTF_NB != 0)
#define GAF_IAP                  (0)
#endif //(CFG_IAP_INTF_NB != 0)

/// ARC supported
#if defined(CFG_ARC_VCS) || defined(CFG_ARC_VCC) || defined(CFG_ARC_MICS) || defined(CFG_ARC_MICC) || \
    defined(CFG_ARC_AICS) || defined(CFG_ARC_AICC) || defined(CFG_ARC_VOCS) || defined(CFG_ARC_VOCC)
#define GAF_ARC                  (1)

#if defined(CFG_ARC_VCS)
#define GAF_ARC_VCS              (1)
#else //(defined(CFG_ARC_VCS))
#define GAF_ARC_VCS              (0)
#endif //(defined(CFG_ARC_VCS))

#if defined(CFG_ARC_VCC)
#define GAF_ARC_VCC              (1)
#else //(defined(CFG_ARC_VCC))
#define GAF_ARC_VCC              (0)
#endif //(defined(CFG_ARC_VCC))

#if defined(CFG_ARC_MICS)
#define GAF_ARC_MICS             (1)
#else //(defined(CFG_ARC_MICS))
#define GAF_ARC_MICS             (0)
#endif //(defined(CFG_ARC_MICS))

#if defined(CFG_ARC_MICC)
#define GAF_ARC_MICC             (1)
#else //(defined(CFG_ARC_MICC))
#define GAF_ARC_MICC             (0)
#endif //(defined(CFG_ARC_MICC))

#if defined(CFG_ARC_AICS)
#define GAF_ARC_AICS             (1)
#else //(defined(CFG_ARC_AICS))
#define GAF_ARC_AICS             (0)
#endif //(defined(CFG_ARC_AICS))

#if defined(CFG_ARC_AICC)
#define GAF_ARC_AICC             (1)
#else //(defined(CFG_ARC_AICC))
#define GAF_ARC_AICC             (0)
#endif //(defined(CFG_ARC_AICC))

#if defined(CFG_ARC_VOCS)
#define GAF_ARC_VOCS             (1)
#else //(defined(CFG_ARC_VOCS))
#define GAF_ARC_VOCS             (0)
#endif //(defined(CFG_ARC_VOCS))

#if defined(CFG_ARC_VOCC)
#define GAF_ARC_VOCC             (1)
#else //(defined(CFG_ARC_VOCC))
#define GAF_ARC_VOCC             (0)
#endif //(defined(CFG_ARC_VOCC))

#if (GAF_ARC_VCS || GAF_ARC_MICS || GAF_ARC_AICS || GAF_ARC_VOCS)
#define GAF_ARC_SRV              (1)
#else //(GAF_ARC_VCS || GAF_ARC_MICS || GAF_ARC_AICS || GAF_ARC_VOCS)
#define GAF_ARC_SRV              (0)
#endif //(GAF_ARC_VCS || GAF_ARC_MICS || GAF_ARC_AICS || GAF_ARC_VOCS)

#if (GAF_ARC_VCC || GAF_ARC_MICC || GAF_ARC_AICC || GAF_ARC_VOCC)
#define GAF_ARC_CLI              (1)
#else //(GAF_ARC_VCC || GAF_ARC_MICC || GAF_ARC_AICC || GAF_ARC_VOCC)
#define GAF_ARC_CLI              (0)
#endif //(GAF_ARC_VCC || GAF_ARC_MICC || GAF_ARC_AICC || GAF_ARC_VOCC)
#else //(defined(CFG_ARC_VCS) || defined(CFG_ARC_VCC) ...)
#define GAF_ARC                  (0)
#define GAF_ARC_SRV              (0)
#define GAF_ARC_CLI              (0)
#define GAF_ARC_VCS              (0)
#define GAF_ARC_VCC              (0)
#define GAF_ARC_MICS             (0)
#define GAF_ARC_MICC             (0)
#define GAF_ARC_AICS             (0)
#define GAF_ARC_AICC             (0)
#define GAF_ARC_VOCS             (0)
#define GAF_ARC_VOCC             (0)
#endif //(defined(CFG_ARC_VCS) || defined(CFG_ARC_VCC) ...)

/// ACC supported
#if defined(CFG_ACC_TBS) || defined(CFG_ACC_TBC) || defined(CFG_ACC_MCS) || defined(CFG_ACC_MCC)
#define GAF_ACC                  (1)

#if defined(CFG_ACC_TBS)
#define GAF_ACC_TBS              (1)
#else //(defined(CFG_ACC_TBS))
#define GAF_ACC_TBS              (0)
#endif //(defined(CFG_ACC_TBS))

#if defined(CFG_ACC_TBC)
#define GAF_ACC_TBC              (1)
#else //(defined(CFG_ACC_TBC))
#define GAF_ACC_TBC              (0)
#endif //(defined(CFG_ACC_TBC))

#if defined(CFG_ACC_MCS)
#define GAF_ACC_MCS              (1)
#else //(defined(CFG_ACC_MCS))
#define GAF_ACC_MCS              (0)
#endif //(defined(CFG_ACC_MCS))

#if defined(CFG_ACC_MCC)
#define GAF_ACC_MCC              (1)
#else //(defined(CFG_ACC_MCC))
#define GAF_ACC_MCC              (0)
#endif //(defined(CFG_ACC_MCC))

#if (GAF_ACC_MCS)
#if defined(CFG_ACC_OTS)
#define GAF_ACC_OTS              (1)
#else //(defined(CFG_ACC_OTS))
#define GAF_ACC_OTS              (0)
#endif //(defined(CFG_ACC_OTS))
#else //(GAF_ACC_MCS)
#define GAF_ACC_OTS              (0)
#endif //(GAF_ACC_MCS)

#if (GAF_ACC_MCC)
#if defined(CFG_ACC_OTC)
#define GAF_ACC_OTC              (1)
#else //(defined(CFG_ACC_OTC))
#define GAF_ACC_OTC              (0)
#endif //(defined(CFG_ACC_OTC))
#else //(GAF_ACC_MCC)
#define GAF_ACC_OTC              (0)
#endif //(GAF_ACC_MCC)

#if (GAF_ACC_TBS || GAF_ACC_MCS || GAF_ACC_OTS)
#define GAF_ACC_SRV              (1)
#else //(GAF_ACC_TBS || GAF_ACC_MCS || GAF_ACC_OTS)
#define GAF_ACC_SRV              (0)
#endif //(GAF_ACC_TBS || GAF_ACC_MCS || GAF_ACC_OTS)

#if (GAF_ACC_TBC || GAF_ACC_MCC || GAF_ACC_OTC)
#define GAF_ACC_CLI              (1)
#else //(GAF_ACC_TBC || GAF_ACC_MCC || GAF_ACC_OTC)
#define GAF_ACC_CLI              (0)
#endif //(GAF_ACC_TBC || GAF_ACC_MCC || GAF_ACC_OTC)
#else //(defined(CFG_ACC_TBS) || defined(CFG_ACC_TBC) ...)
#define GAF_ACC                  (0)
#define GAF_ACC_SRV              (0)
#define GAF_ACC_CLI              (0)
#define GAF_ACC_TBS              (0)
#define GAF_ACC_TBC              (0)
#define GAF_ACC_MCS              (0)
#define GAF_ACC_MCC              (0)
#define GAF_ACC_OTS              (0)
#define GAF_ACC_OTC              (0)
#endif //(defined(CFG_ACC_TBS) || defined(CFG_ACC_TBC) ...)

/// ATC supported
#if defined(CFG_ATC_RAAS) || defined(CFG_ATC_RAAC) || defined(CFG_ATC_CSISM) || defined(CFG_ATC_CSISC)
#define GAF_ATC                  (1)

#if defined(CFG_ATC_RAAS)
#define GAF_ATC_RAAS             (1)
#else //(defined(CFG_ATC_RAAS))
#define GAF_ATC_RAAS             (0)
#endif //(defined(CFG_ATC_RAAS))

#if defined(CFG_ATC_RAAC)
#define GAF_ATC_RAAC             (1)
#else //(defined(CFG_ATC_RAAC))
#define GAF_ATC_RAAC             (0)
#endif //(defined(CFG_ATC_RAAC))

#if defined(CFG_ATC_CSISM)
#define GAF_ATC_CSISM            (1)
#else //(defined(CFG_ATC_CSISM))
#define GAF_ATC_CSISM            (0)

#if defined(CFG_CAPS)
#undef CFG_CAPS
#endif //(defined(CFG_CAPS))
#endif //(defined(CFG_ATC_CSISM))

#if defined(CFG_ATC_CSISC)
#define GAF_ATC_CSISC            (1)
#else //(defined(CFG_ATC_CSISC))
#define GAF_ATC_CSISC            (0)

#if defined(CFG_CAPC)
#undef CFG_CAPC
#endif //(defined(CFG_CAPC))
#endif //(defined(CFG_ATC_CSISC))

#if (GAF_ATC_RAAS || GAF_ATC_CSISM)
#define GAF_ATC_SRV              (1)
#else //(GAF_ATC_RAAS || GAF_ATC_CSISM)
#define GAF_ATC_SRV              (0)
#endif //(GAF_ATC_RAAS || GAF_ATC_CSISM)

#if (GAF_ATC_RAAC || GAF_ATC_CSISC)
#define GAF_ATC_CLI              (1)
#else //(GAF_ATC_RAAC || GAF_ATC_CSISC)
#define GAF_ATC_CLI              (0)
#endif //(GAF_ATC_RAAC || GAF_ATC_CSISC)
#else //(defined(CFG_ATC_RAAS) || defined(CFG_ATC_RAAS) ...)
#define GAF_ATC                  (0)
#define GAF_ATC_SRV              (0)
#define GAF_ATC_CLI              (0)
#define GAF_ATC_RAAS             (0)
#define GAF_ATC_RAAC             (0)
#define GAF_ATC_CSISM            (0)
#define GAF_ATC_CSISC            (0)
#endif //(defined(CFG_ATC_RAAS) || defined(CFG_ATC_RAAS) ...)

#if defined(CFG_CAPS) || defined(CFG_CAPC)
#define GAF_CAP                  (1)

#if defined(CFG_CAPS)
#define GAF_CAPS                 (1)
#else
#define GAF_CAPS                 (0)
#endif //(defined(CFG_CAPS))

#if defined(CFG_CAPC)
#define GAF_CAPC                 (1)
#else
#define GAF_CAPC                 (0)
#endif //(defined(CFG_CAPS))

#else
#define GAF_CAP                  (0)
#define GAF_CAPS                 (0)
#define GAF_CAPC                 (0)
#endif //(defined(CFG_CAPS) || defined(CFG_CAPC))

#if (defined(CFG_BAP_UC_SRV) || defined(CFG_BAP_UC_CLI) || defined(CFG_BAP_CAPA_SRV) || defined(CFG_BAP_CAPA_CLI) || \
     defined(CFG_BAP_BC_SRC) || defined(CFG_BAP_BC_SINK) || defined(CFG_BAP_BC_SCAN) || defined(CFG_BAP_BC_DELEG) || \
     defined(CFG_BAP_BC_ASSIST))
#define GAF_BAP                  (1)

#if defined(CFG_BAP_UC_SRV)
#define GAF_BAP_UC_SRV           (1)
#else
#define GAF_BAP_UC_SRV           (0)
#endif //(defined(CFG_BAP_UC_SRV))

#if defined(CFG_BAP_UC_CLI)
#define GAF_BAP_UC_CLI           (1)
#else
#define GAF_BAP_UC_CLI           (0)
#endif //(defined(CFG_BAP_UC_CLI))

#if defined(CFG_BAP_CAPA_SRV)
#define GAF_BAP_CAPA_SRV         (1)
#else
#define GAF_BAP_CAPA_SRV         (0)
#endif //(defined(CFG_BAP_CAPA_SRV))

#if defined(CFG_BAP_CAPA_CLI)
#define GAF_BAP_CAPA_CLI         (1)
#else
#define GAF_BAP_CAPA_CLI         (0)
#endif //(defined(CFG_BAP_CAPA_CLI))

#if (defined(CFG_BAP_BC_SRC) && GAF_BROADCAST_SUPP)
#define GAF_BAP_BC_SRC           (1)
#else
#define GAF_BAP_BC_SRC           (0)
#endif //(defined(CFG_BAP_BC_SRC) && GAF_BROADCAST_SUPP)

#if (defined(CFG_BAP_BC_ASSIST) && GAF_BROADCAST_SUPP)
#define GAF_BAP_BC_ASSIST         (1)
#else
#define GAF_BAP_BC_ASSIST         (0)
#endif //(defined(GAF_BAP_BC_ASSIST) && GAF_BROADCAST_SUPP)

#if (defined(CFG_BAP_BC_DELEG) && GAF_BROADCAST_SUPP)
#define GAF_BAP_BC_DELEG          (1)
#else
#define GAF_BAP_BC_DELEG          (0)
#endif //(defined(GAF_BAP_BC_DELEG) && GAF_BROADCAST_SUPP)

#if ((defined(CFG_BAP_BC_SINK) || defined(CFG_BAP_BC_DELEG)) && GAF_BROADCAST_SUPP)
#define GAF_BAP_BC_SINK           (1)
#else
#define GAF_BAP_BC_SINK           (0)
#endif //((defined(CFG_BAP_BC_SINK) || CFG_BAP_BC_DELEG) && GAF_BROADCAST_SUPP)

#if ((defined(CFG_BAP_BC_SCAN) || GAF_BAP_BC_SINK || defined(CFG_BAP_BC_ASSIST) || defined(CFG_BAP_BC_DELEG)) && GAF_BROADCAST_SUPP)
#define GAF_BAP_BC_SCAN           (1)
#else
#define GAF_BAP_BC_SCAN           (0)
#endif //((defined(CFG_BAP_BC_SCAN) || GAF_BAP_BC_SINK || CFG_BAP_BC_ASSIST || CFG_BAP_BC_DELEG) && GAF_BROADCAST_SUPP)


#else
#define GAF_BAP                  (0)
#define GAF_BAP_UC_SRV           (0)
#define GAF_BAP_UC_CLI           (0)
#define GAF_BAP_CAPA_SRV         (0)
#define GAF_BAP_CAPA_CLI         (0)
#define GAF_BAP_BC_SRC           (0)
#define GAF_BAP_BC_SINK          (0)
#define GAF_BAP_BC_SCAN          (0)
#define GAF_BAP_BC_ASSIST        (0)
#define GAF_BAP_BC_DELEG         (0)
#endif //(defined(CFG_BAP_UC_SRV) || defined(CFG_BAP_UC_CLI) || defined(CFG_BAP_CAPA_SRV) ...)

#if defined(CFG_GAF_API_MSG)
#define GAF_API_MSG              (1)
#else
#define GAF_API_MSG              (0)
#endif //(defined(CFG_GAF_API_MSG))

// Support of Debug functions
#if defined(CFG_GAF_DBG)
#define GAF_DBG                  (1)
#else
#define GAF_DBG                  (0)
#endif //(defined(CFG_GAF_DBG))

// Support of Vendor Specific functions
#if defined(CFG_GAF_VS)
#define GAF_VS                   (1)
#else
#define GAF_VS                   (0)
#endif //(defined(CFG_GAF_VS))

// GAF Adaptation layer
#if (GAF_BAP_BC_SRC)
#define GAF_AL_PER_ADV           (1)
#else
#define GAF_AL_PER_ADV           (0)
#endif //(GAF_BAP_BC_SRC)

#if (GAF_BAP_BC_SINK || GAF_BAP_BC_ASSIST)
#define GAF_AL_PER_SYNC          (1)
#else
#define GAF_AL_PER_SYNC          (0)
#endif //(GAF_BAP_BC_SINK || GAF_BAP_BC_ASSIST)

#if (GAF_AL_PER_SYNC && GAF_BAP_BC_ASSIST)
#define GAF_AL_PAST              (1)
#else
#define GAF_AL_PAST              (0)
#endif //(GAF_AL_PER_SYNC && GAF_BAP_BC_ASSIST)

#if (GAF_BAP_BC_SINK || GAF_BAP_BC_SCAN || GAF_BAP_BC_ASSIST || GAF_AL_PER_SYNC)
#define GAF_AL_SCAN              (1)
#else
#define GAF_AL_SCAN              (0)
#endif //(GAF_BAP_BC_SINK || GAF_BAP_BC_SCAN || GAF_BAP_BC_ASSIST || GAF_AL_PER_SYNC)

#if(GAF_BAP_BC_DELEG)
#define GAF_AL_ADV               (1)
#else
#define GAF_AL_ADV               (0)
#endif //(GAF_BAP_BC_DELEG)


#if (GAF_AL_PER_ADV || GAF_AL_PER_SYNC || GAF_AL_SCAN || GAF_AL_ADV)
#define GAF_AL                   (1)
#define GAF_AL_ACT_MAX           (BLE_ACTIVITY_MAX)
#else
#define GAF_AL                   (0)
#endif //(GAF_AL_PER_ADV)

/// @} GAF

#endif // GAF_CFG_H_
