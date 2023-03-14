/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_PLATFORM_BASE_H
#define AUDIO_PLATFORM_BASE_H

#include "audio_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MIN_PERIOD_SIZE (2 * 1024)
#define BITSTOBYTE 8
#define MAX_BUFF_SIZE (128 * 1024)

enum DataBitWidth {
    DATA_BIT_WIDTH8  =  8,      /* 8 bit witdth */
    DATA_BIT_WIDTH16 =  16,     /* 16 bit witdth */
    DATA_BIT_WIDTH18 =  18,     /* 18 bit witdth */
    DATA_BIT_WIDTH20 =  20,     /* 20 bit witdth */
    DATA_BIT_WIDTH24 =  24,     /* 24 bit witdth */
    DATA_BIT_WIDTH32 =  32,     /* 32 bit witdth */
};

enum PcmStatus {
    PCM_STOP = 0, /* pcm stream stop flag */
    PCM_PAUSE,    /* pcm stream pause flag */
    PCM_START,    /* pcm stream start flag */
};

struct CircleBufInfo {
    uint32_t cirBufSize;        /* Current DMA cache size */
    uint32_t trafBufSize;       /* PCM data per transmission default size */
    uint32_t period;            /* not used */
    uint32_t periodSize;        /* DMA cache period size */
    uint32_t periodCount;       /* Number of DMA cache periods */
    unsigned long phyAddr;      /* DMA cache physical address */
    uint32_t *virtAddr;         /* DMA cache virtual address */
    uint32_t wbufOffSet;        /* DMA cache write address offset */
    uint32_t wptrOffSet;        /* DMA cache write pointer offset */
    uint32_t rbufOffSet;        /* DMA cache read address offset */
    uint32_t rptrOffSet;        /* DMA cache read pointer offset */
    enum PcmStatus runStatus;   /* PCM status */
    uint32_t chnId;             /* Channel ID */
    uint32_t enable;            /* Enable identification */
    struct OsalMutex buffMutex; /* mutex */
    uint32_t framesPosition;    /* Frame position */
    uint32_t pointer;           /* Read/write pointer position during playback and recording */
    uint32_t periodsMax;        /* Maximum number of periods */
    uint32_t periodsMin;        /* Minimum number of periods */
    uint32_t cirBufMax;         /* Size of DMA cache requested */
    uint32_t curTrafSize;       /* The size of each actual transmission of PCM data */
};

/* head file: pnp_message_report */
enum PnpReportType {
    DEVICE_PULG,
    EVENT_REPORT,
    REPORT_TYPE_INVALID
};

enum PnpEventID {
    THRESHOLD_REPORT,
    LOAD_ADAPTER,
    SERVICE_STATUS,
    EVENT_ID_INVALID
};

enum PnpDeviceType {
    PRIMARY_DEVICE,
    USB_DEVICE,
    A2DP_DEVICE,
    DEVICE_TYPE_INVALID
};

struct PnpReportDevPlugMsg {
    uint8_t eventType;
    uint8_t state;
    uint8_t deviceType;
    uint8_t deviceCap;
    uint8_t id;
};

struct PnpReportEventMsg {
    uint8_t eventType;
    uint8_t eventId;
    uint8_t eventValue;
    uint8_t deviceType;
    uint8_t reserve;      /* Reserved fields are not used for the time being */
};

struct PnpReportMsg {
    enum PnpReportType reportType;
    union {
        struct PnpReportDevPlugMsg devPlugMsg;
        struct PnpReportEventMsg eventMsg;
    };
};

unsigned int SysReadl(unsigned long addr);
void SysWritel(unsigned long addr, unsigned int value);

struct PlatformData *PlatformDataFromCard(const struct AudioCard *card);
uint32_t AudioBytesToFrames(uint32_t frameBits, uint32_t size);
int32_t AudioDataBigEndianChange(char *srcData, uint32_t audioLen, enum DataBitWidth bitWidth);
int32_t AudioFormatToBitWidth(enum AudioFormat format, unsigned int *bitWidth);
int32_t AudioSetPcmInfo(struct PlatformData *platformData, const struct AudioPcmHwParams *param);
int32_t AudioSetRenderBufInfo(struct PlatformData *data, const struct AudioPcmHwParams *param);
int32_t AudioSetCaptureBufInfo(struct PlatformData *data, const struct AudioPcmHwParams *param);
int32_t AudioPcmWrite(const struct AudioCard *card, struct AudioTxData *txData);
int32_t AudioPcmRead(const struct AudioCard *card, struct AudioRxData *rxData);
int32_t AudioPcmMmapWrite(const struct AudioCard *card, const struct AudioMmapData *txMmapData);
int32_t AudioPcmMmapRead(const struct AudioCard *card, const struct AudioMmapData *rxMmapData);
int32_t AudioRenderOpen(const struct AudioCard *card);
int32_t AudioCaptureOpen(const struct AudioCard *card);
int32_t AudioRenderClose(const struct AudioCard *card);
int32_t AudioPcmPointer(const struct AudioCard *card, uint32_t *pointer, enum AudioStreamType streamType);
int32_t AudioCaptureClose(const struct AudioCard *card);
int32_t AudioHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param);
int32_t AudioRenderPrepare(const struct AudioCard *card);
int32_t AudioCapturePrepare(const struct AudioCard *card);
int32_t AudioRenderTrigger(struct AudioCard *card, int cmd);
int32_t AudioCaptureTrigger(struct AudioCard *card, int cmd);
int32_t AudioCapSilenceThresholdEvent(struct HdfDeviceObject *device, const struct PnpReportMsg *reportMsg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* CODEC_CORE_H */
