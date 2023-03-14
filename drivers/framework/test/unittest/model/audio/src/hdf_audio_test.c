/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "audio_host_test.h"
#include "audio_core_test.h"
#include "audio_parse_test.h"
#include "audio_sapm_test.h"
#include "audio_dai_base_test.h"
#include "audio_dma_base_test.h"
#include "audio_dsp_base_test.h"
#include "audio_codec_base_test.h"
#include "audio_platform_base_test.h"
#include "audio_accessory_base_test.h"
#include "hdf_audio_test.h"

#define HDF_LOG_TAG hdf_audio_test

// add test case entry
static HdfTestCaseList g_hdfAudioTestCaseList[] = {
    {AUDIO_ADM_TEST_AUDIOHOSTCREATEANDBIND, AudioHostCreateAndBindTest},           // audio ADM audio_host
    {AUDIO_ADM_TEST_GETCARDINSTANCE, GetCardInstanceTest},                           // audio ADM audio_host

    {AUDIO_ADM_TEST_AUDIOFILLCONFIGDATA, AudioFillConfigDataTest},                   // audio ADM audio_parse
    {AUDIO_ADM_TEST_CODECGETREGCONFIG, CodecGetRegConfigTest},                      // audio ADM audio_parse

    {AUDIO_ADM_TEST_AUDIODEVICEREADREG, AudioDeviceReadRegTest},                     // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIODEVICEWRITEREG, AudioDeviceWriteRegTest},                   // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOSOCREGISTERPLATFORM, AudioSocRegisterPlatformTest},         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOSOCREGISTERDAI, AudioSocRegisterDaiTest},                   // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOREGISTERDSP, AudioRegisterDspTest},                         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOREGISTERCODEC, AudioRegisterCodecTest},                     // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOREGISTERACCESSORY, AudioRegisterAccessoryTest},             // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOBINDDAILINK, AudioBindDaiLinkTest},                         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOUPDATECODECREGBITS, AudioUpdateCodecRegBitsTest},           // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOUPDATEACCESSORYREGBITS, AudioUpdateAccessoryRegBitsTest},   // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOUPDATEDAIREGBITS, AudioUpdateDaiRegBitsTest},               // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOKCONTROLGETCPUDAI, AudioKcontrolGetCpuDaiTest},             // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOKCONTROLGETCODEC, AudioKcontrolGetCodecTest},               // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOKCONTROLGETACCESSORY, AudioKcontrolGetAccessoryTest},       // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOADDCONTROLS, AudioAddControlsTest},                         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOADDCONTROL, AudioAddControlTest},                           // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOGETCTRLOPSRREG, AudioGetCtrlOpsRRegTest},                   // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOGETCTRLOPSREG, AudioGetCtrlOpsRegTest},                     // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOSETCTRLOPSREG, AudioSetCtrlOpsRegTest},                     // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOSETCTRLOPSRREG, AudioSetCtrlOpsRRegTest},                   // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIODAIREADREG, AudioDaiReadRegTest},                           // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIODAIWRITEREG, AudioDaiWriteRegTest},                         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOCODECREADREG, AudioCodecReadRegTest},                       // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOCODECWRITEREG, AudioCodecWriteRegTest},                     // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOACCESSORYREADREG, AudioAccessoryReadRegTest},               // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOACCESSORYWRITEREG, AudioAccessoryWriteRegTest},             // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOINFOCTRLOPS, AudioInfoCtrlOpsTest},                         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOCODECGETCTRLOPS, AudioCodecGetCtrlOpsTest},                 // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOCODECSETCTRLOPS, AudioCodecSetCtrlOpsTest},                 // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOACCESSORYGETCTRLOPS, AudioAccessoryGetCtrlOpsTest},         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOACCESSORYSETCTRLOPS, AudioAccessorySetCtrlOpsTset},         // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOCPUDAISETCTRLOPS, AudioCpuDaiSetCtrlOpsTest},               // audio ADM audio_core
    {AUDIO_ADM_TEST_AUDIOCPUDAIGETCTRLOPS, AudioCpuDaiGetCtrlOpsTest},               // audio ADM audio_core

    {AUDIO_ADM_TEST_AUDIOSAPMNEWCOMPONENTS, AudioSapmNewComponentsTest},             // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOSAPMADDROUTES, AudioSapmAddRoutesTest},                     // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOSAPMNEWCONTROLS, AudioSapmNewControlsTest},                 // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOSAPMSLEEP, AudioSapmSleepTest},                             // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOSAMPPOWERUP, AudioSampPowerUpTest},                         // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOSAMPSETPOWERMONITOR, AudioSampSetPowerMonitorTest},         // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOCODECSAPMSETCTRLOPS, AudioCodecSapmSetCtrlOpsTest},         // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOCODECSAPMGETCTRLOPS, AudioCodecSapmGetCtrlOpsTest},         // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOACCESSORYSAPMSETCTRLOPS, AudioAccessorySapmSetCtrlOpsTest}, // audio ADM audio_sapm
    {AUDIO_ADM_TEST_AUDIOACCESSORYSAPMGETCTRLOPS, AudioAccessorySapmGetCtrlOpsTest}, // audio ADM audio_sapm

    {AUDIO_ADM_TEST_I2CREADWRITE, AccessoryI2cReadWriteTest},
    {AUDIO_ADM_TEST_REGBITSREAD, AccessoryRegBitsReadTest},
    {AUDIO_ADM_TEST_REGBITSUPDATE, AccessoryRegBitsUpdateTest},
    {AUDIO_ADM_TEST_DEVICEFREQUENCYPARSE, AccessoryDeviceFrequencyParseTest},
    {AUDIO_ADM_TEST_DAIPARAMSUPDATE, AccessoryDaiParamsUpdateTest},
    {AUDIO_ADM_TEST_DEVICECFGGET, AccessoryDeviceCfgGetTest},
    {AUDIO_ADM_TEST_DEVICECTRLREGINIT, AccessoryDeviceCtrlRegInitTest},
    {AUDIO_ADM_TEST_DEVICEREGREAD, AccessoryDeviceRegReadTest},
    {AUDIO_ADM_TEST_DEVICEREGWRITE, AccessoryDeviceRegWriteTest},
    {AUDIO_ADM_TEST_AGETCONFIGINFO, AccessoryGetConfigInfoTest},

    {AUDIO_ADM_TEST_GETSERVICENAME, CodecGetServiceNameTest},
    {AUDIO_ADM_TEST_GETDAINAME, CodecGetDaiNameTest},
    {AUDIO_ADM_TEST_GETCONFIGINFO, CodecGetConfigInfoTest},
    {AUDIO_ADM_TEST_SETCONFIGINFO, CodecSetConfigInfoTest},
    {AUDIO_ADM_TEST_SETCTLFUNC, CodecSetCtlFuncTest},

    {AUDIO_ADM_TEST_DAIDATAFROMCARD, DaiDataFromCardTest},
    {AUDIO_ADM_TEST_DAIGETCONFIGINFO, DaiGetConfigInfoTest},
    {AUDIO_ADM_TEST_DAICHECKSAMPLERATE, DaiCheckSampleRateTest},
    {AUDIO_ADM_TEST_DAISETCONFIGINFO, DaiSetConfigInfoTest},

    {AUDIO_ADM_TEST_DMABUFALLOC, AudioDmaBufAllocTest},
    {AUDIO_ADM_TEST_DMABUFFREE, AudioDmaBufFreeTest},
    {AUDIO_ADM_TEST_DMAREQUESTCHANNEL, AudioDmaRequestChannelTest},
    {AUDIO_ADM_TEST_DMACONFIGCHANNEL, AudioDmaConfigChannelTest},
    {AUDIO_ADM_TEST_DMAPREP, AudioDmaPrepTest},
    {AUDIO_ADM_TEST_DMASUBMIT, AudioDmaSubmitTest},
    {AUDIO_ADM_TEST_DMAPENDING, AudioDmaPendingTest},
    {AUDIO_ADM_TEST_DMAPAUSE, AudioDmaPauseTest},
    {AUDIO_ADM_TEST_DMARESUME, AudioDmaResumeTest},
    {AUDIO_ADM_TEST_DMAPOINTER, AudioDmaPointerTest},

    {AUDIO_ADM_TEST_DSPGETSERVICENAME, DspGetServiceNameTest},
    {AUDIO_ADM_TEST_DSPGETDAINAME, DspGetDaiNameTest},

    {AUDIO_ADM_TEST_DATAFROMCARD, PlatformDataFromCardTest},
    {AUDIO_ADM_TEST_BYTESTOFRAMES, AudioBytesToFramesTest},
    {AUDIO_ADM_TEST_DATABIGENDIANCHANGE, AudioDataBigEndianChangeTest},
    {AUDIO_ADM_TEST_FRAMATTOBITWIDTH, AudioFramatToBitWidthTest},
    {AUDIO_ADM_TEST_SETPCMINFO, AudioSetPcmInfoTest},
    {AUDIO_ADM_TEST_SETRENDERBUFINFO, AudioSetRenderBufInfoTest},
    {AUDIO_ADM_TEST_SETCAPTUREBUFINFO, AudioSetCaptureBufInfoTest},
    {AUDIO_ADM_TEST_PCMWRITE, AudioPcmWriteTest},
    {AUDIO_ADM_TEST_PCMREAD, AudioPcmReadTest},
    {AUDIO_ADM_TEST_PCMMMAPWRITE, AudioPcmMmapWriteTest},
    {AUDIO_ADM_TEST_PCMMMAPREAD, AudioPcmMmapReadTest},
    {AUDIO_ADM_TEST_RENDEROPEN, AudioRenderOpenTest},
    {AUDIO_ADM_TEST_CAPTUREOPEN, AudioCaptureOpenTest},
    {AUDIO_ADM_TEST_RENDERCLOSE, AudioRenderCloseTest},
    {AUDIO_ADM_TEST_PCMPOINTER, AudioPcmPointerTest},
    {AUDIO_ADM_TEST_CAPTURECLOSE, AudioCaptureCloseTest},
    {AUDIO_ADM_TEST_HWPARAMS, AudioHwParamsTest},
    {AUDIO_ADM_TEST_RENDERPREPARE, AudioRenderPrepareTest},
    {AUDIO_ADM_TEST_CAPTUREPREPARE, AudioCapturePrepareTest},
    {AUDIO_ADM_TEST_RENDERTRIGGER, AudioRenderTriggerTest},
    {AUDIO_ADM_TEST_CAPTURETRIGGER, AudioCaptureTriggerTest}
};

int32_t HdfAudioEntry(HdfTestMsg *msg)
{
    int32_t result, i;

    if (msg == NULL) {
        HDF_LOGE("%s is fail: HdfTestMsg is NULL!", __func__);
        return HDF_SUCCESS;
    }

    for (i = 0; i < sizeof(g_hdfAudioTestCaseList) / sizeof(g_hdfAudioTestCaseList[0]); ++i) {
        if ((msg->subCmd == g_hdfAudioTestCaseList[i].subCmd) && (g_hdfAudioTestCaseList[i].testFunc != NULL)) {
            result = g_hdfAudioTestCaseList[i].testFunc();
            HDF_LOGE("HdfTest:Audio test result[%s-%u]", ((result == 0) ? "pass" : "fail"), msg->subCmd);
            msg->result = (result == 0) ? HDF_SUCCESS : HDF_FAILURE;
            return HDF_SUCCESS;
        }
    }
    return HDF_SUCCESS;
}
