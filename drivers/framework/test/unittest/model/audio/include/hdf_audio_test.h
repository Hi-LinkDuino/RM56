/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_AUDIO_TEST_H
#define HDF_AUDIO_TEST_H

#include "hdf_main_test.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    AUDIO_ADM_TEST_AUDIOHOSTCREATEANDBIND = 1,       // audio ADM audio_host
    AUDIO_ADM_TEST_GETCARDINSTANCE = 2,               // audio ADM audio_host

    AUDIO_ADM_TEST_AUDIOFILLCONFIGDATA = 3,           // audio ADM audio_parse
    AUDIO_ADM_TEST_CODECGETREGCONFIG = 4,             // audio ADM audio_parse

    AUDIO_ADM_TEST_AUDIODEVICEREADREG = 5,            // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIODEVICEWRITEREG,               // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSOCREGISTERPLATFORM,          // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSOCREGISTERDAI,               // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOREGISTERDSP,                  // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOREGISTERCODEC,                // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOREGISTERACCESSORY,            // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOBINDDAILINK,                  // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOUPDATECODECREGBITS,           // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOUPDATEACCESSORYREGBITS,       // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOUPDATEDAIREGBITS,             // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOKCONTROLGETCPUDAI,            // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOKCONTROLGETCODEC,             // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOKCONTROLGETACCESSORY,         // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOADDCONTROLS,                  // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOADDCONTROL,                   // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOGETCTRLOPSRREG,               // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOGETCTRLOPSREG,                // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSETCTRLOPSREG,                // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSETCTRLOPSRREG,               // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIODAIREADREG,                   // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIODAIWRITEREG,                  // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECREADREG,                 // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECWRITEREG,                // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOACCESSORYREADREG,             // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOACCESSORYWRITEREG,            // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOINFOCTRLOPS,                  // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECGETCTRLOPS,              // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECSETCTRLOPS,              // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOACCESSORYGETCTRLOPS,          // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOACCESSORYSETCTRLOPS,          // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCPUDAISETCTRLOPS,             // audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCPUDAIGETCTRLOPS = 37,        // audio ADM audio_core

    AUDIO_ADM_TEST_AUDIOSAPMNEWCOMPONENTS = 38,       // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAPMADDROUTES,                // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAPMNEWCONTROLS,              // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAPMSLEEP,                    // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAMPPOWERUP,                  // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAMPSETPOWERMONITOR,          // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOCODECSAPMSETCTRLOPS,          // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOCODECSAPMGETCTRLOPS,          // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOACCESSORYSAPMSETCTRLOPS,      // audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOACCESSORYSAPMGETCTRLOPS = 47, // audio ADM audio_sapm

    AUDIO_ADM_TEST_I2CREADWRITE,                      // audio ADM audio_accessory_base
    AUDIO_ADM_TEST_REGBITSREAD,
    AUDIO_ADM_TEST_REGBITSUPDATE,
    AUDIO_ADM_TEST_DEVICEFREQUENCYPARSE,
    AUDIO_ADM_TEST_DAIPARAMSUPDATE,
    AUDIO_ADM_TEST_DEVICECFGGET,
    AUDIO_ADM_TEST_DEVICECTRLREGINIT,
    AUDIO_ADM_TEST_DEVICEREGREAD,
    AUDIO_ADM_TEST_DEVICEREGWRITE,
    AUDIO_ADM_TEST_AGETCONFIGINFO,

    AUDIO_ADM_TEST_GETSERVICENAME,                    // audio ADM audio_codec_base
    AUDIO_ADM_TEST_GETDAINAME,
    AUDIO_ADM_TEST_GETCONFIGINFO,
    AUDIO_ADM_TEST_SETCONFIGINFO,
    AUDIO_ADM_TEST_SETCTLFUNC,

    AUDIO_ADM_TEST_DAIDATAFROMCARD,                   // audio ADM audio_dai_base
    AUDIO_ADM_TEST_DAIGETCONFIGINFO,
    AUDIO_ADM_TEST_DAICHECKSAMPLERATE,
    AUDIO_ADM_TEST_DAISETCONFIGINFO,

    AUDIO_ADM_TEST_DMABUFALLOC,                       // audio ADM audio_dma_base
    AUDIO_ADM_TEST_DMABUFFREE,
    AUDIO_ADM_TEST_DMAREQUESTCHANNEL,
    AUDIO_ADM_TEST_DMACONFIGCHANNEL,
    AUDIO_ADM_TEST_DMAPREP,
    AUDIO_ADM_TEST_DMASUBMIT,
    AUDIO_ADM_TEST_DMAPENDING,
    AUDIO_ADM_TEST_DMAPAUSE,
    AUDIO_ADM_TEST_DMARESUME,
    AUDIO_ADM_TEST_DMAPOINTER,

    AUDIO_ADM_TEST_DSPGETSERVICENAME,                 // audio ADM audio_dsp_base
    AUDIO_ADM_TEST_DSPGETDAINAME,

    AUDIO_ADM_TEST_DATAFROMCARD,                      // audio ADM audio_platform_base
    AUDIO_ADM_TEST_BYTESTOFRAMES,
    AUDIO_ADM_TEST_DATABIGENDIANCHANGE,
    AUDIO_ADM_TEST_FRAMATTOBITWIDTH,
    AUDIO_ADM_TEST_SETPCMINFO,
    AUDIO_ADM_TEST_SETRENDERBUFINFO,
    AUDIO_ADM_TEST_SETCAPTUREBUFINFO,
    AUDIO_ADM_TEST_PCMWRITE,
    AUDIO_ADM_TEST_PCMREAD,
    AUDIO_ADM_TEST_PCMMMAPWRITE,
    AUDIO_ADM_TEST_PCMMMAPREAD,
    AUDIO_ADM_TEST_RENDEROPEN,
    AUDIO_ADM_TEST_CAPTUREOPEN,
    AUDIO_ADM_TEST_RENDERCLOSE,
    AUDIO_ADM_TEST_PCMPOINTER,
    AUDIO_ADM_TEST_CAPTURECLOSE,
    AUDIO_ADM_TEST_HWPARAMS,
    AUDIO_ADM_TEST_RENDERPREPARE,
    AUDIO_ADM_TEST_CAPTUREPREPARE,
    AUDIO_ADM_TEST_RENDERTRIGGER,
    AUDIO_ADM_TEST_CAPTURETRIGGER,
} HdfAudioTestCaseCmd;

int32_t HdfAudioEntry(HdfTestMsg *msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDF_AUDIO_TEST_H */
