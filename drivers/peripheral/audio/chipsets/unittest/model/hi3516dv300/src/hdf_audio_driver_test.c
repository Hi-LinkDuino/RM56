/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_audio_driver_test.h"
#include "hdf_log.h"
#include "hi3516_aiao_impl_test.h"
#include "hi3516_codec_impl_test.h"
#include "hi3516_codec_ops_test.h"
#include "hi3516_dai_ops_test.h"
#include "hi3516_dma_ops_test.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

static HdfTestCaseList g_hdfHi3516DirverTestCaseList[] = {
    {AUDIO_DRIVER_TESTDAIDEVICEINIT, TestDaiDeviceInit},
    {AUDIO_DRIVER_TESTDAISTARTUP, TestDaiStartup},
    {AUDIO_DRIVER_TESTDAIHWPARAMS, TestDaiHwParams},
    {AUDIO_DRIVER_TESTDAI_TRIGGER, TestDaiTrigger},

    {AUDIO_DRIVER_TESTCODECHALSYSINIT, TestCodecHalSysInit},
    {AUDIO_DRIVER_TESTACODECREGBITSREAD, TestCodecRegBitsRead},
    {AUDIO_DRIVER_TESTCODECREGBITSUPDATE, TestCodecRegBitsUpdate},
    {AUDIO_DRIVER_TESTCODECREGDEFAULTINIT, TestCodecRegDefaultInit},
    {AUDIO_DRIVER_TESTCODECSETADCTUNEENABLE, TestCodecSetAdcTuneEnable},
    {AUDIO_DRIVER_TESTCODECDAIPARAMSUPDATE, TestCodecDaiParamsUpdate},
    {AUDIO_DRIVER_TESTAUDIOCODECAIAOSETCTRLOPS, TestAudioCodecAiaoSetCtrlOps},
    {AUDIO_DRIVER_TESTAUDIOCODECAIAOGETCTRLOPS, TestAudioCodecAiaoGetCtrlOps},

    {AUDIO_DRIVER_TESTCODECDEVICEINIT, TestCodecDeviceInit},
    {AUDIO_DRIVER_TESTCODECDAIDEVICEINIT, TestCodecDaiDeviceInit},
    {AUDIO_DRIVER_TESTCODECDAIHWPARAMS, TestCodecDaiHwParams},
    {AUDIO_DRIVER_TESTCODECDAISTARTUP, TestCodecDaiStartup},

    {AUDIO_DRIVER_TESTAIOPREGCFG, TestAiopRegCfg},
    {AUDIO_DRIVER_TESTAIAOHALSYSINIT, TestAiaoHalSysInit},
    {AUDIO_DRIVER_TESTAIAOCLOCKRESET, TestAiaoClockReset},
    {AUDIO_DRIVER_TESTAIAOHALREADREG, TestAiaoHalReadReg},
    {AUDIO_DRIVER_TESTAOPHALSETBUFFRPTR, TestAopHalSetBuffRptr},
    {AUDIO_DRIVER_TESTAOPHALSETBUFFWPTR, TestAopHalSetBuffWptr},
    {AUDIO_DRIVER_TESTAIPHALSETBUFFWPTR, TestAipHalSetBuffWptr},
    {AUDIO_DRIVER_TESTAIPHALSETBUFFRPTR, TestAipHalSetBuffRptr},
    {AUDIO_DRIVER_TESTAOPHALSETTXSTART, TestAopHalSetTxStart},
    {AUDIO_DRIVER_TESTAIPHALSETRXSTART, TestAipHalSetRxStart},
    {AUDIO_DRIVER_TESTAOPHALDEVENABLE, TestAopHalDevEnable},
    {AUDIO_DRIVER_TESTAIAODEVICEINIT, TestAiaoDeviceInit},
    {AUDIO_DRIVER_TESTAUDIOAOINIT, TestAudioAoInit},
    {AUDIO_DRIVER_TESTAUDIOAIINIT, TestAudioAiInit},
    {AUDIO_DRIVER_TESTAIAOSYSPINMUX, TestAiaoSysPinMux},
    {AUDIO_DRIVER_TESTAIAOGETMCLK, TestAiaoGetMclk},
    {AUDIO_DRIVER_TESTAIAOSETSYSCTLREGVALUE, TestAiaoSetSysCtlRegValue},

    {AUDIO_DRIVER_TESTAUDIODMADEVICEINIT, TestAudioDmaDeviceInit},
    {AUDIO_DRIVER_TESTHI3516DMABUFALLOC, TestHi3516DmaBufAlloc},
    {AUDIO_DRIVER_TESTHI3516DMABUFFREE, TestHi3516DmaBufFree},
    {AUDIO_DRIVER_TESTHI3516DMAREQUESTCHANNEL, TestHi3516DmaRequestChannel},
    {AUDIO_DRIVER_TESTHI3516DMACONFIGCHANNEL, TestHi3516DmaConfigChannel},
    {AUDIO_DRIVER_TESTHI3516DMAPREP, TestHi3516DmaPrep},
    {AUDIO_DRIVER_TESTHI3516DMASUBMIT, TestHi3516DmaSubmit},
    {AUDIO_DRIVER_TESTHI3516DMAPENDING, TestHi3516DmaPending},
    {AUDIO_DRIVER_TESTHI3516DMAPAUSE, TestHi3516DmaPause},
    {AUDIO_DRIVER_TESTHI3516DMARESUME, TestHi3516DmaResume},
    {AUDIO_DRIVER_TESTHI3516DMAPOINTER, TestHi3516DmaPointer},
};

int32_t HdfAudioDriverEntry(HdfTestMsg *msg)
{
    int32_t result;
    uint32_t i;

    if (msg == NULL) {
        HDF_LOGE("%s is fail: HdfTestMsg is NULL!", __func__);
        return HDF_SUCCESS;
    }

    for (i = 0; i < sizeof(g_hdfHi3516DirverTestCaseList) / sizeof(g_hdfHi3516DirverTestCaseList[0]); ++i) {
        if ((msg->subCmd == g_hdfHi3516DirverTestCaseList[i].subCmd) &&
            (g_hdfHi3516DirverTestCaseList[i].testFunc != NULL)) {
            result = g_hdfHi3516DirverTestCaseList[i].testFunc();
            HDF_LOGE("HdfAudioDriverEntry::hi3516 driver test result[%s-%u]",
                     ((result == 0) ? "pass" : "fail"), msg->subCmd);
            msg->result = (result == 0) ? HDF_SUCCESS : HDF_FAILURE;
            return HDF_SUCCESS;
        }
    }

    return HDF_SUCCESS;
}
