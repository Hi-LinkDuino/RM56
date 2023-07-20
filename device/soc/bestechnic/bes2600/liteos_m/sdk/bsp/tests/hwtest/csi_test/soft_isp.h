/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __SOFT_ISP_H__
#define __SOFT_ISP_H__

#ifdef __cpulsplus
extern "C" {
#endif

typedef void* aecCoreHandleType;

// extern aecCoreHandleType aec_handle;

/// Define ISP function return type
typedef enum isp_ret_e
{
    ISP_RET_SUCCESS          = 0,         /// 0 Return Success
    ISP_RET_ERROR            = 0x01,      /// 1 Common Error, stop and restart need
    ISP_RET_INGORED_FAILURE  = 0x02,      /// 2 Calculator failure, ignore return value
    ISP_RET_INVALID          = 0x04,      /// 3 Invalid input parameters
    ISP_RET_NOMEM            = 0x08,      /// 4 Memory allocate and relative error
    ISP_RET_OVERFLOW         = 0x10,      /// 5 Array or memory or type overflow
    ISP_RET_UNINITIALIZED    = 0x20,      /// 6 Uninitialized error
    ISP_RET_TIMEOUT          = 0x40,      /// 7 Timeout error
} isp_ret_t;

typedef enum ae_state_e
{
    AE_STATE_WAIT,
    AE_STATE_SET_WEIGHT_MESH,
    AE_STATE_STATISTIC_READY,
    AE_STATE_UPDATE_COMPENSATE_PARAMETER,
    AE_STATE_DISABLE,
    AE_STATE_UPDATE_CURRENT_EXPOSURE,
    AE_STATE_UPDATE_EXPOSURE_PARAMETER,
} ae_state_t;

typedef enum aec_mode_e
{
    AEC_MODE_OFF = 0,
    AEC_MODE_AUTO,
} aec_mode_t;

typedef enum hist_y_type_e
{
    RAW_NON_LINEAR_Y     = 0,
    RAW_LINEAR_Y,
    YUV_NON_LINEAR_Y,
    YUV_LINEAR_Y
} hist_y_type_t;

// HDR exposure time ratio denominator
typedef enum hdr_exp_base_e
{
    HDR_SHORT_BASE     = 0,
    HDR_LONG_BASE,
} hdr_exp_base_t;

typedef enum exp_meterring_mode_e
{
    EXP_METERRING_MODE_CENTER           = 0,
    EXP_METERRING_MODE_AVERAGE,
    EXP_METERRING_MODE_UP_TO_DOWN,
    EXP_METERRING_MODE_DOWN_TO_UP,
    EXP_METERRING_MODE_LEFT_TO_RIGHT,
    EXP_METERRING_MODE_RIGHT_TO_LEFT,
    EXP_METERRING_MODE_MAX,
} exp_meterring_mode_t;

typedef struct
{
    // control the speed of a single exposure adjustment
    // use aeConvergeStep times to reach the final target
    uint32_t  aeConvergeStep;         /// default 6,[3:32] AE convergence speed
    uint32_t  aeMaxConvSpeed;         /// default 60,[30 : 200] 100 base percent
    uint32_t  aeStableInRange;        /// default 4,[2:255] AE convergence stability range in
    uint32_t  aeStableOutRange;       /// default 32,[5:512] AE convergence stability range out
    uint32_t  aeSaturationThr;        /// default 960,[0:1023] AE Saturation Threshold, use to calculation linear Y ---
    uint32_t  aeStatsYIdx;            /// default 3,[0,3] which stats Y use to judge ae stable out
    uint32_t  aeChgFrameCnt;          /// default 15,[0, 4096] AE change time out frames lumaAve in the range aeStableOutRange
}ae_converge_param_t;

typedef struct
{
    uint32_t  aeTarget;               /// default 250,[0:1023] base brightness target
    uint32_t  aeHighContrTarget;      /// default 60,[0:100] high dynamic range target
    uint32_t  aeLowLevel;             /// default 15000,[0:100000] dark area percent 100000 is 100%
    uint32_t  aeHighLevel;            /// default 95000,[0:100000] bright area percent 100000 is 100%
    uint32_t  aeLowContrast;          /// default 15,[0:255] dynamic range lower bound
    uint32_t  aeHighContrast;         /// default 25,[0:255] dynamic range upper bound
    uint32_t  aeHighCutMin;
    uint32_t  aeLongTarget;           /// default 40,[0:100] HDR Long Target for DOL2/3/4
    uint32_t  aeLmRatioMax;           /// default 32,[1:64] HDR Max L/M ratio
    uint32_t  aeMidTarget;            /// default 40,[0:100] HDR Mid Target for DOL3/4
    uint32_t  aeMsRatioMax;           /// default 32,[1:64] HDR Max M/S ratio
    uint32_t  aeShortTarget;          /// default 40,[0:100] HDR Short Target for DOL4
    uint32_t  aeSvsRatioMax;          /// default 32,[1:64] HDR Max S/VS ratio
    uint32_t  aeSaturationMaxPercent; /// default 10240, [0:102400]AE Saturation Max Pcent use for Long base DOL, 1024 is %1
} ae_target_param_t;

typedef struct _ae_param_t
{
    uint32_t aeConvergeStep;
    uint32_t stableRange;
    uint32_t aeTarget;
    uint32_t highLevel;
    uint32_t lowLevel;
    uint32_t highContrTarget;
    uint32_t highContrast;
    uint32_t lowContrast;
    uint32_t midTarget;
    uint32_t maxmsRatio;
    uint32_t longTarget;
    uint32_t maxlmRatio;
    uint32_t outdoorProb;
    uint32_t hdrSatThr;
    uint32_t hdrSatMaxPcent;

    uint32_t longRatioMaxMulti;
    uint32_t specialEnable;
    uint32_t specialContrast;
} ae_param_t;

typedef struct
{
    // max_exp_time (lines) = max_fps/setting_min_fps * frm_lines -Vblank
    uint32_t  maxIntegrationTime;    /// Maximun of integration time (lines) --- Maximun of exposure time
    uint32_t  minIntegrationTime;    /// Minimun of integration time (lines) --- Minimun of exposure time
    uint32_t  maxAnalogGain;         ///
    uint32_t  minAnalogGain;         /// 1000x
    uint32_t  lineTime;              /// 1us: 4096
    uint32_t  limitShortMid;         /// S + M limitation for DOL3
    uint32_t  limitLong;             /// Long limitation
    uint32_t  anaGain;
    uint32_t  expTime;
} drv_sensor_exp_param_t;

typedef struct ae_ctrl_t_s
{
    ae_state_t              eAEState;
    aec_mode_t              eAECMode;

    ae_converge_param_t     aeConvParam;            //convergence parameter，independent of luxidx
    ae_target_param_t       aeTargetParam;          //light meter. to calculate final target using luxidx.
    drv_sensor_exp_param_t  sensorExpParam;

    uint32_t            aeCalInterval;
    uint8_t             aeStatsInterruptCounter;
    uint8_t             aeConvergeDir;              //converge direction：0-stable 1-under2-over
    uint8_t             aeStable;
    uint16_t            stableCount;                //count of frames stable but unconverged
    uint16_t            stableCountForChg;          //count of frames stable lumaAve

    hist_y_type_t       histYType;
    uint32_t            *pAEHist;               // histogram of channel in raw domain for algorithm
    uint32_t            *pAEHistY;              // histogram of Y in YUV domain for algorithm
    uint32_t            lumaAveLinear;          // lumaAve in linear area of histogram, raw domain
    uint32_t            lumaAveNonLinear;
    uint32_t            hightCutMin;
    uint64_t            ullHistSum;
    uint32_t            finalTarget;
    uint32_t            hightCut;
    uint32_t            lowCut;
    uint32_t            contrast;               // hight_cut/low_cut * 1000X

    //ration convergence method, calculate ratio of targetNextFrame.
    //ratio = ( targetNextFrame +-  stepVal )/lumaAveNonLinear
    uint64_t            expoCalcAdjPara;        //ratio of lumaAve adjustment
    uint32_t            acturalIspGain;

    uint32_t            imgWidth;
    uint32_t            imgHeight;
    uint32_t            imgAddr;
    uint32_t            frameNum;
} ae_ctrl_t;

#define AEC_API_TYPE_INIT       (0)
#define AEC_API_TYPE_PROC       (1)
#define AEC_API_TYPE_RELEASE    (2)

void aec_api_handler(uint8_t type, uint32_t img);

int soft_isp_aec_open(void);

int soft_isp_aec_process_data(uint32_t img, uint32_t img_size);

int soft_isp_aec_close(void);

#ifdef __cpulsplus
}
#endif

#endif
