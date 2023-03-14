/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MIPI_CSI_CORE_H
#define MIPI_CSI_CORE_H

#include "hdf_base.h"
#include "hdf_device.h"
#include "hdf_device_desc.h"
#include "hdf_object.h"
#include "mipi_csi_if.h"
#include "osal_mutex.h"
#include "osal_spinlock.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define MAX_CNTLR_CNT         2
#define COMBO_DEV_MAX_NUM     2
#define MIPI_RX_MAX_DEV_NUM   2

/**
 * @brief Image size.
 *
 * @since 1.0
 */
typedef struct {
    /** Image width */
    unsigned int width;
    /** Image height */
    unsigned int height;
} ImgSize;

/**
 * @brief LP, HS and escape mode switching timeout error.
 *
 * @since 1.0
 */
typedef struct {
    /** Clock1 Lane switching from LP to HS timeout */
    unsigned int clk1FsmTimeoutErrCnt;
    /** Clock0 Lane switching from LP to HS timeout */
    unsigned int clk0FsmTimeoutErrCnt;
    /** Data lane0 switching from LP to HS timeout */
    unsigned int d0FsmTimeoutErrCnt;
    /** Data lane1 switching from LP to HS timeout */
    unsigned int d1FsmTimeoutErrCnt;
    /** Data lane2 switching from LP to HS timeout */
    unsigned int d2FsmTimeoutErrCnt;
    /** Data lane3 switching from LP to HS timeout */
    unsigned int d3FsmTimeoutErrCnt;

    /** Clock1 Lane switching to escape mode timed out */
    unsigned int clk1FsmEscapeErrCnt;
    /** Clock0 Lane switching to escape mode timed out */
    unsigned int clk0FsmEscapeErrCnt;
    /** Data lane0 switching to escape mode timed out */
    unsigned int d0FsmEscapeErrCnt;
    /** Data lane1 switching to escape mode timeout */
    unsigned int d1FsmEscapeErrCnt;
    /** Data lane2 switching to escape mode timeout */
    unsigned int d2FsmEscapeErrCnt;
    /** Data lane3 switching to escape mode timeout */
    unsigned int d3FsmEscapeErrCnt;
} PhyErrIntCnt;

/**
 * @brief Errors generated during Mipi communication, including data packets,
 * data frames and read-write control.
 *
 * @since 1.0
 */
typedef struct {
    /** Packet err.
        Header has at least 2 errors and ECC cannot correct them */
    unsigned int errEccDoubleCnt;
    /** CRC redundancy check error count for vc3 channel data */
    unsigned int vc3ErrCrcCnt;
    /** CRC redundancy check error count for vc2 channel data */
    unsigned int vc2ErrCrcCnt;
    /** CRC redundancy check error count for Vc1 channel data */
    unsigned int vc1ErrCrcCnt;
    /** CRC redundancy check error count for vc0 channel data */
    unsigned int vc0ErrCrcCnt;
    /** ECC corrected error count for vc3 channel header */
    unsigned int vc3ErrEccCorrectedCnt;
    /** ECC corrected error count for vc2 channel header */
    unsigned int vc2ErrEccCorrectedCnt;
    /** ECC corrected error count for Vc1 channel header */
    unsigned int vc1ErrEccCorrectedCnt;
    /** ECC corrected error count for vc0 channel header */
    unsigned int vc0ErrEccCorrectedCnt;
    /** Frame sequence error count of vc3 */
    unsigned int errIdVc3Cnt;
    /** Frame sequence error count of vc2 */
    unsigned int errIdVc2Cnt;
    /** Frame sequence error count of Vc1 */
    unsigned int errIdVc1Cnt;
    /** Frame sequence error count or vc0 */
    unsigned int errIdVc0Cnt;

    /** Frame err.
        Count of data types not supported by vc3 channel */
    unsigned int errFrameDataVc3Cnt;
    /** Count of data types not supported by vc2 channel */
    unsigned int errFrameDataVc2Cnt;
    /** Count of data types not supported by Vc1 channel */
    unsigned int errFrameDataVc1Cnt;
    /** Count of data types not supported by vc0 channel */
    unsigned int errFrameDataVc0Cnt;
    /** Frame sequence error count of vc3 */
    unsigned int errFSeqVc3Cnt;
    /** Frame sequence error count of vc2 */
    unsigned int errFSeqVc2Cnt;
    /** Frame sequence error count of Vc1 */
    unsigned int errFSeqVc1Cnt;
    /** Frame sequence error count or vc0 */
    unsigned int errFSeqVc0Cnt;
    /** Mismatch count of frame start and frame end short packets of vc3 channel */
    unsigned int errFBndryMatchVc3Cnt;
    /** Mismatch count of frame start and frame end short packets of vc2 channel */
    unsigned int errFBndryMatchVc2Cnt;
    /** Mismatch count of frame start and frame end short packets of Vc1 channel */
    unsigned int errFBndryMatchVc1Cnt;
    /** Count of mismatch between frame start and frame end short packets of vc0 channel */
    unsigned int errFBndryMatchVc0Cnt;

    /** Ctrl err.
        Mipi read data FIFO raw interrupt count */
    unsigned int dataFifoRderrCnt;
    /** Mipi read command FIFO raw interrupt count */
    unsigned int cmdFifoRderrCnt;
    /** Mipi write data FIFO raw interrupt count */
    unsigned int dataFifoWrerrCnt;
    /** Mipi write command FIFO raw interrupt count */
    unsigned int cmdFifoWrerrCnt;
} MipiErrIntCnt;

/**
 * @brief LVDS internal read or write interrupt error.
 *
 * @since 1.0
 */
typedef struct {
    /** CMD_ FIFO register read error interrupt count */
    unsigned int cmdRdErrCnt;
    /** CMD_ FIFO register write error interrupt count */
    unsigned int cmdWrErrCnt;
    /** Read line_ BUF error interrupt count */
    unsigned int popErrCnt;
    /** Synchronization error interrupt count of each lane during LVDS */
    unsigned int lvdsStateErrCnt;
    /** Link0 read FIFO error interrupt count */
    unsigned int link0RdErrCnt;
    /** Link0 write FIFO error interrupt count */
    unsigned int link0WrErrCnt;
} LvdsErrIntCnt;

/**
 * @brief Internal FIFO alignment error.
 *
 * @since 1.0
 */
typedef struct {
    /** Lane3 FIFO overflow */
    unsigned int lane3AlignErrCnt;
    /** Lane2 FIFO overflow */
    unsigned int lane2AlignErrCnt;
    /** Lane1 FIFO overflow */
    unsigned int lane1AlignErrCnt;
    /** Lane0 FIFO overflow */
    unsigned int lane0AlignErrCnt;
    /** FIFO overflow */
    unsigned int fifoFullErrCnt;
} AlignErrIntCnt;

/**
 * @brief Controller context parameter variable.
 *
 * @since 1.0
 */
typedef struct {
    /** Set by {@link MipiCsiSetHsMode} function */
    LaneDivideMode laneDivideMode;
    bool hsModeCfged;
    /** The properties of different types of devices are set through the {@link MipiCsiSetComboDevAttr} function */
    ComboDevAttr comboDevAttr[COMBO_DEV_MAX_NUM];
    bool devValid[COMBO_DEV_MAX_NUM];
    /** Whether the {@link MipiCsiSetComboDevAttr} function is called for parameter setting */
    bool devCfged[COMBO_DEV_MAX_NUM];
    unsigned int laneBitmap[COMBO_DEV_MAX_NUM];
} MipiDevCtx;

/**
 * @brief Mipi CSI controller attribute and method definition.
 *
 * @since 1.0
 */
struct MipiCsiCntlr {
    /** The services provided by this controller are sent out when the driver is bound to the HDF framework */
    struct IDeviceIoService service;
    /** The device side pointer is passed in when the driver binds to the HDF frame */
    struct HdfDeviceObject *device;
    /** device number */
    unsigned int devNo;
    /** All interfaces provided by the controller */
    struct MipiCsiCntlrMethod *ops;
    /** For all interfaces debugged by the controller, null is required when the driver is not implemented */
    struct MipiCsiCntlrDebugMethod *debugs;
    /** Controller context parameter variable */
    MipiDevCtx ctx;
    /** Lock when accessing controller context parameter variables */
    OsalSpinlock ctxLock;
    /** Lock when operating controller method */
    struct OsalMutex lock;
    /** Anonymous data pointer, used to store struct mipi_csi_device */
    void *priv;
};

/**
 * @brief All interfaces provided by the MIPI-CSI controller.
 *
 * Refer to the corresponding interface declaration for function pointer description.
 *
 * @since 1.0
 */
struct MipiCsiCntlrMethod {
    int32_t (*setComboDevAttr)(struct MipiCsiCntlr *cntlr, ComboDevAttr *pAttr);
    int32_t (*setPhyCmvmode)(struct MipiCsiCntlr *cntlr, uint8_t devno, PhyCmvMode cmvMode);
    int32_t (*setExtDataType)(struct MipiCsiCntlr *cntlr, ExtDataType* dataType);
    int32_t (*setHsMode)(struct MipiCsiCntlr *cntlr, LaneDivideMode laneDivideMode);
    int32_t (*enableClock)(struct MipiCsiCntlr *cntlr, uint8_t comboDev);
    int32_t (*disableClock)(struct MipiCsiCntlr *cntlr, uint8_t comboDev);
    int32_t (*resetRx)(struct MipiCsiCntlr *cntlr, uint8_t comboDev);
    int32_t (*unresetRx)(struct MipiCsiCntlr *cntlr, uint8_t comboDev);
    int32_t (*enableSensorClock)(struct MipiCsiCntlr *cntlr, uint8_t snsClkSource);
    int32_t (*disableSensorClock)(struct MipiCsiCntlr *cntlr, uint8_t snsClkSource);
    int32_t (*resetSensor)(struct MipiCsiCntlr *cntlr, uint8_t snsResetSource);
    int32_t (*unresetSensor)(struct MipiCsiCntlr *cntlr, uint8_t snsResetSource);
    int32_t (*setDrvData)(struct MipiCsiCntlr *cntlr, void *drvData);
};

/**
 * @brief For all interfaces debugged by the MIPI-CSI controller, <b>NULL</b> is required when
 * the driver is not implemented.
 *
 * Refer to the corresponding interface declaration for function pointer description.
 *
 * @since 1.0
 */
struct MipiCsiCntlrDebugMethod {
    void (*getMipiDevCtx)(struct MipiCsiCntlr *cntlr, MipiDevCtx *ctx);
    void (*getPhyErrIntCnt)(struct MipiCsiCntlr *cntlr, unsigned int phyId, PhyErrIntCnt *errInfo);
    void (*getMipiErrInt)(struct MipiCsiCntlr *cntlr, unsigned int phyId, MipiErrIntCnt *errInfo);
    void (*getLvdsErrIntCnt)(struct MipiCsiCntlr *cntlr, unsigned int phyId, LvdsErrIntCnt *errInfo);
    void (*getAlignErrIntCnt)(struct MipiCsiCntlr *cntlr, unsigned int phyId, AlignErrIntCnt *errInfo);
    void (*getPhyData)(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData);
    void (*getPhyMipiLinkData)(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData);
    void (*getPhyLvdsLinkData)(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData);
    void (*getMipiImgsizeStatis)(struct MipiCsiCntlr *cntlr, uint8_t devno, short vc, ImgSize *pSize);
    void (*getLvdsImgsizeStatis)(struct MipiCsiCntlr *cntlr, uint8_t devno, short vc, ImgSize *pSize);
    void (*getLvdsLaneImgsizeStatis)(struct MipiCsiCntlr *cntlr, uint8_t devno, short lane, ImgSize *pSize);
};

int32_t MipiCsiRegisterCntlr(struct MipiCsiCntlr *cntlr, struct HdfDeviceObject *device);
void MipiCsiUnregisterCntlr(struct MipiCsiCntlr *cntlr);

/**
  * @brief Turn HdfDeviceObject to an MipiCsiCntlr.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the MipiCsiCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
struct MipiCsiCntlr *MipiCsiCntlrFromDevice(const struct HdfDeviceObject *device);

/**
 * @brief Obtains the MIPI CSI device handle with a specified channel ID.
 *
 * @param id Indicates the MIPI CSI channel ID.
 *
 * @return Returns the MIPI CSI device if the operation is successful; returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
struct MipiCsiCntlr *MipiCsiCntlrGet(uint8_t id);

/**
 * @brief Releases the MIPI CSI device handle.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 *
 * @since 1.0
 */
void MipiCsiCntlrPut(const struct MipiCsiCntlr *cntlr);

/**
 * @brief Set the parameters of Mipi, CMOS or LVDS camera to the controller.
 *
 * The parameters including working mode, image area, image depth, data rate and physical channel.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param pAttr Indicates the pointer to the attribute.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrSetComboDevAttr(struct MipiCsiCntlr *cntlr, ComboDevAttr *pAttr);

/**
 * @brief Set common mode voltage mode.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param devno There are 2 device numbers in total, pointing to 0 or 1.
 * @param cmvMode Common mode voltage mode parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrSetPhyCmvmode(struct MipiCsiCntlr *cntlr, uint8_t devno, PhyCmvMode cmvMode);

/**
 * @brief Reset sensor.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param snsResetSource The reset signal line number of sensor is called the reset source of sensor in software.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrResetSensor(struct MipiCsiCntlr *cntlr, uint8_t snsResetSource);

/**
 * @brief Unreset sensor.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param snsResetSource The reset signal line number of sensor is called the reset source of sensor in software.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrUnresetSensor(struct MipiCsiCntlr *cntlr, uint8_t snsResetSource);

/**
 * @brief Reset Mipi Rx.
 *
 * Different s32WorkingViNum have different enSnsType.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrResetRx(struct MipiCsiCntlr *cntlr, uint8_t comboDev);

/**
 * @brief Uneset MIPI RX.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrUnresetRx(struct MipiCsiCntlr *cntlr, uint8_t comboDev);

/**
 * @brief Set the lane distribution of Mipi Rx.
 *
 * Select the specific mode according to the form of hardware connection.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param laneDivideMode Lane division mode parameters.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrSetHsMode(struct MipiCsiCntlr *cntlr, LaneDivideMode laneDivideMode);

/**
 * @brief Enable Mipi clock.
 *
 * Decide whether to use Mipi or LVDS according to the ensnstype parameter
 * passed by the upper layer function.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrEnableClock(struct MipiCsiCntlr *cntlr, uint8_t comboDev);

/**
 * @brief Disable the clock of Mipi device.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrDisableClock(struct MipiCsiCntlr *cntlr, uint8_t comboDev);

/**
 * @brief Enable the sensor clock on Mipi.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param snsClkSource The clock signal line number of sensor, which is called the clock source of sensor in software.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrEnableSensorClock(struct MipiCsiCntlr *cntlr, uint8_t snsClkSource);

/**
 * @brief Disable the sensor clock.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param snsClkSource The clock signal line number of sensor, which is called the clock source of sensor in software.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrDisableSensorClock(struct MipiCsiCntlr *cntlr, uint8_t snsClkSource);

/**
 * @brief Set YUV and RAW data format and bit depth.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param dataType Pointer to image data format.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrSetExtDataType(struct MipiCsiCntlr *cntlr, ExtDataType* dataType);

/**
 * @brief Sets additional parameters for a MIPI CSI device.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param drvData Indicates the pointer to the additional parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiCntlrSetDrvData(struct MipiCsiCntlr *cntlr, void *drvData);

/**
 * @brief Get controller context parameter variable for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param ctx Controller context parameter variable.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetMipiDevCtx(struct MipiCsiCntlr *cntlr, MipiDevCtx *ctx);

/**
 * @brief Get mode switching timeout error for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param phyId Indicates PHY index.
 * @param errInfo Indicates mode switching timeout error.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetPhyErrIntCnt(struct MipiCsiCntlr *cntlr, unsigned int phyId, PhyErrIntCnt *errInfo);

/**
 * @brief Get errors generated during Mipi communication for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param phyId Indicates PHY index.
 * @param errInfo Indicates Mipi communication error.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetMipiErrInt(struct MipiCsiCntlr *cntlr, unsigned int phyId, MipiErrIntCnt *errInfo);

/**
 * @brief Get LVDS internal read or write interrupt error for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param phyId Indicates PHY index.
 * @param errInfo Indicates LVDS read or write interrupt error.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetLvdsErrIntCnt(struct MipiCsiCntlr *cntlr, unsigned int phyId, LvdsErrIntCnt *errInfo);

/**
 * @brief Get internal FIFO alignment error for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param phyId Indicates PHY index.
 * @param errInfo Indicates FIFO alignment error.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetAlignErrIntCnt(struct MipiCsiCntlr *cntlr, unsigned int phyId, AlignErrIntCnt *errInfo);

/**
 * @brief Get PHY data for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param phyId Indicates PHY index.
 * @param laneId Indicates lane index, and the value range is 0 to <b>COMBO_MAX_LANE_NUM-1</b>.
 * @param laneData Indicates a single PHY data.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetPhyData(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData);

/**
 * @brief Get MIPI data for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param phyId Indicates PHY index.
 * @param laneId Indicates lane index, and the value range is 0 to <b>COMBO_MAX_LANE_NUM-1</b>.
 * @param laneData Indicates a single MIPI data.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetPhyMipiLinkData(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData);

/**
 * @brief Get LVDS data for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param phyId Indicates PHY index.
 * @param laneId Indicates lane index, and the value range is 0 to <b>COMBO_MAX_LANE_NUM-1</b>.
 * @param laneData Indicates a single LVDS data.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetPhyLvdsLinkData(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData);

/**
 * @brief Get image size of MIPI for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param devno There are 2 device numbers in total, pointing to 0 or 1.
 * @param vc Indicates virtual channel subscript.
 * @param pSize Pointer to image size.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetMipiImgsizeStatis(struct MipiCsiCntlr *cntlr, uint8_t devno, short vc, ImgSize *pSize);

/**
 * @brief Get image size of LVDS for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param devno There are 2 device numbers in total, pointing to 0 or 1.
 * @param vc Indicates virtual channel subscript.
 * @param pSize Pointer to image size.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetLvdsImgsizeStatis(struct MipiCsiCntlr *cntlr, uint8_t devno, short vc, ImgSize *pSize);

/**
 * @brief Get image size of LVDS-Lane for debugging.
 *
 * @param cntlr Indicates the MIPI CSI device obtained via {@link MipiCsiOpen}.
 * @param devno There are 2 device numbers in total, pointing to 0 or 1.
 * @param lane Indicates lane index, and the value range is 0 to <b>COMBO_MAX_LANE_NUM-1</b>.
 * @param pSize Pointer to image size.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDebugGetLvdsLaneImgsizeStatis(struct MipiCsiCntlr *cntlr, uint8_t devno, short lane, ImgSize *pSize);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
