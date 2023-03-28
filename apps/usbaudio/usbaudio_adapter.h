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
#ifndef __USBAUDIO_ADAPTER_H__
#define __USBAUDIO_ADAPTER_H__

#ifdef USB_AUDIO_ADAPTER


#include "cmsis_os.h"
#include "app_utils.h"
#include "kfifo.h"
#include "alsa.h"

typedef struct{
    struct kfifo fifo;
    unsigned int (*read)(unsigned char *buffer, unsigned int size);
    unsigned int (*write)(unsigned char *buffer, unsigned int size);
    void (*clean)(void);
} UsbAudioPcmOut;

typedef struct{
    struct kfifo fifo;
    unsigned int (*read)(unsigned char *buffer, unsigned int size);
    unsigned int (*write)(unsigned char *buffer, unsigned int size);
    void (*clean)(void);
} UsbAudioCaptureIn;

unsigned int UsbAudioPlayRead(unsigned char *buffer, unsigned int size);
unsigned int UsbAudioPlayWrite(unsigned char *buffer, unsigned int size);
void UsbAudioBufClean(void);
alsa_handle_t * usb_audio_play_open(void);
int AppUsbAudioStop(void);
int AppUsbAudioClose(void);
int AppUsbAudioPlay(unsigned char *buf, unsigned int len);
int AppUsbVolSet(unsigned char vol);
int AppUsbMuteSet(void);
int AppUsbMuteCancel(void);

#endif

#endif
