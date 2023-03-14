/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "audio_adapter_test.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "audio_common_test.h"

using namespace std;
using namespace comfun;
using namespace testing::ext;
namespace {
class AudioAdapterTest : public testing::Test {
public:
    struct AudioManager *managerFuncs = nullptr;
    struct AudioAdapterDescriptor *descs = nullptr;
    struct AudioAdapterDescriptor *desc = nullptr;
    struct AudioAdapter *adapter = nullptr;
    virtual void SetUp();
    virtual void TearDown();
};

void AudioAdapterTest::SetUp()
{
    managerFuncs = GetAudioManagerFuncs();
    ASSERT_NE(managerFuncs, nullptr);
    int32_t size = 0;
    ASSERT_EQ(HDF_SUCCESS, managerFuncs->GetAllAdapters(managerFuncs, &descs, &size));
    desc = &descs[0];
    ASSERT_EQ(HDF_SUCCESS, managerFuncs->LoadAdapter(managerFuncs, desc, &adapter));
}

void AudioAdapterTest::TearDown()
{
    managerFuncs->UnloadAdapter(managerFuncs, adapter);
    adapter = nullptr;
}

HWTEST_F(AudioAdapterTest, GetAudioRenderFuncWhenHwRenderIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = nullptr;
    EXPECT_EQ(HDF_FAILURE, GetAudioRenderFunc(hwRender));
}

HWTEST_F(AudioAdapterTest, GetAudioRenderFuncWhenParamIsVaild, TestSize.Level1)
{
    struct AudioHwRender hwRender;
    EXPECT_EQ(HDF_SUCCESS, GetAudioRenderFunc(&hwRender));
}

HWTEST_F(AudioAdapterTest, CheckParaDescWhenDescIsNull, TestSize.Level1)
{
    const struct AudioDeviceDescriptor *desc = nullptr;
    const char *type = "Render";
    EXPECT_EQ(HDF_FAILURE, CheckParaDesc(desc, type));
}

HWTEST_F(AudioAdapterTest, CheckParaDescWhenTypeIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    const char *type = nullptr;
    EXPECT_EQ(HDF_FAILURE, CheckParaDesc(&desc, type));
}

HWTEST_F(AudioAdapterTest, CheckParaDescWhenPortIdLessThanZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    const char *type = "Render";
    desc.portId = AUDIO_HAL_ERR_NOT_SUPPORT;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, CheckParaDesc(&desc, type));
}

HWTEST_F(AudioAdapterTest, CheckParaDescWhenPinsIsPinNone, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    const char *type = "Render";
    desc.pins = PIN_NONE;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, CheckParaDesc(&desc, type));
}

HWTEST_F(AudioAdapterTest, CheckParaDescWhenTypeIsError, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    const char *type = "123";
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, CheckParaDesc(&desc, type));
}

HWTEST_F(AudioAdapterTest, CheckParaDescWhenParamIsVaild, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    const char *type = "Render";
    EXPECT_EQ(HDF_SUCCESS, CheckParaDesc(&desc, type));
}

HWTEST_F(AudioAdapterTest, CheckParaAttrWhenAttrsIsNull, TestSize.Level1)
{
    const struct AudioSampleAttributes *attrs = nullptr;
    EXPECT_EQ(HDF_FAILURE, CheckParaAttr(attrs));
}

HWTEST_F(AudioAdapterTest, CheckParaAttrWhenPeriodLessThanZero, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.period = AUDIO_HAL_ERR_NOT_SUPPORT;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, CheckParaAttr(&attrs));
}

HWTEST_F(AudioAdapterTest, CheckParaAttrWhenTypeIsNotSupport, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.type = (enum AudioCategory)AUDIO_HAL_ERR_NOT_SUPPORT;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, CheckParaAttr(&attrs));
}

HWTEST_F(AudioAdapterTest, CheckParaAttrWhenFormatIsNotSupport, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.format = AUDIO_FORMAT_G726;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, CheckParaAttr(&attrs));
}

HWTEST_F(AudioAdapterTest, CheckParaAttrWhenParamIsVaild, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS,  InitAttrs(attrs));
    EXPECT_EQ(HDF_SUCCESS, CheckParaAttr(&attrs));
}

HWTEST_F(AudioAdapterTest, AttrFormatToBitWhenAttrsIsNull, TestSize.Level1)
{
    const struct AudioSampleAttributes *attrs = nullptr;
    int32_t format = -1;
    EXPECT_EQ(HDF_FAILURE, AttrFormatToBit(attrs, &format));
}

HWTEST_F(AudioAdapterTest, AttrFormatToBitWhenFormatIsNull, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    int32_t *format = nullptr;
    EXPECT_EQ(HDF_FAILURE, AttrFormatToBit(&attrs, format));
}

HWTEST_F(AudioAdapterTest, AttrFormatToBitWhenAttrsIsNotSupport, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.format = AUDIO_FORMAT_AAC_MAIN;
    int32_t format = -1;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AttrFormatToBit(&attrs, &format));
}

HWTEST_F(AudioAdapterTest, AttrFormatToBitWhenParamIsVaild, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    int32_t format = -1;
    EXPECT_EQ(HDF_SUCCESS, AttrFormatToBit(&attrs, &format));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenHwRenderIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_FAILURE, InitHwRenderParam(nullptr, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenDescIsNull, TestSize.Level1)
{
    struct AudioHwRender hwRender;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_FAILURE, InitHwRenderParam(&hwRender, nullptr, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenAttrsIsNull, TestSize.Level1)
{
    struct AudioHwRender hwRender;
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_FAILURE, InitHwRenderParam(&hwRender, &desc, nullptr));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenPortIdLessThanZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    desc.portId = AUDIO_HAL_ERR_NOT_SUPPORT;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioHwRender hwRender;
    EXPECT_EQ(HDF_SUCCESS, InitHwRender(hwRender, desc, attrs));
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, InitHwRenderParam(&hwRender, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenPeriodLessThanZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.period = AUDIO_HAL_ERR_NOT_SUPPORT;
    struct AudioHwRender hwRender;
    EXPECT_EQ(HDF_SUCCESS, InitHwRender(hwRender, desc, attrs));
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, InitHwRenderParam(&hwRender, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenFormatIsNotSupport, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.format = AUDIO_FORMAT_AAC_MAIN;
    struct AudioHwRender hwRender;
    EXPECT_EQ(HDF_SUCCESS, InitHwRender(hwRender, desc, attrs));
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, InitHwRenderParam(&hwRender, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenChannelCountIsZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.channelCount = 0;
    struct AudioHwRender hwRender;
    EXPECT_EQ(HDF_SUCCESS, InitHwRender(hwRender, desc, attrs));
    EXPECT_EQ(HDF_FAILURE, InitHwRenderParam(&hwRender, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwRenderParamWhenParamIsVaild, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioHwRender hwRender;
    EXPECT_EQ(HDF_SUCCESS, InitHwRender(hwRender, desc, attrs));
    EXPECT_EQ(HDF_SUCCESS, InitHwRenderParam(&hwRender, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitForGetPortCapabilityWhenCapabilityIndexIsNull, TestSize.Level1)
{
    struct AudioPort portIndex;
    EXPECT_EQ(HDF_SUCCESS, InitPort(portIndex));
    struct AudioPortCapability *capabilityIndex = nullptr;
    EXPECT_EQ(HDF_FAILURE, InitForGetPortCapability(portIndex, capabilityIndex));
}

HWTEST_F(AudioAdapterTest, InitForGetPortCapabilityWhenDirIsPortIn, TestSize.Level1)
{
    struct AudioPort portIndex;
    EXPECT_EQ(HDF_SUCCESS, InitPort(portIndex));
    portIndex.dir = PORT_IN;
    struct AudioPortCapability capabilityIndex;
    EXPECT_EQ(HDF_SUCCESS, InitForGetPortCapability(portIndex, &capabilityIndex));
}

void TestInitForGetPortCapability(uint32_t portId)
{
    struct AudioPort portIndex;
    EXPECT_EQ(HDF_SUCCESS, InitPort(portIndex));
    portIndex.portId = portId;
    struct AudioPortCapability capabilityIndex;
    ASSERT_EQ(HDF_SUCCESS, InitForGetPortCapability(portIndex, &capabilityIndex));

    EXPECT_NE(capabilityIndex.subPorts, nullptr);
    if (capabilityIndex.subPorts != nullptr) {
        EXPECT_EQ(capabilityIndex.subPorts->portId, portIndex.portId);
        EXPECT_EQ(capabilityIndex.subPorts->desc, portIndex.portName);
        EXPECT_EQ(capabilityIndex.subPorts->mask, PORT_PASSTHROUGH_LPCM);
    }
    if (capabilityIndex.subPorts != nullptr) {
        free(capabilityIndex.subPorts);
        capabilityIndex.subPorts = nullptr;
    }
}

HWTEST_F(AudioAdapterTest, InitForGetPortCapabilityWhenPortIdIsZero, TestSize.Level1)
{
    TestInitForGetPortCapability(0);
}

HWTEST_F(AudioAdapterTest, InitForGetPortCapabilityWhenPortIdIsOne, TestSize.Level1)
{
    struct AudioPort portIndex;
    EXPECT_EQ(HDF_SUCCESS, InitPort(portIndex));
    portIndex.portId = 1;
    struct AudioPortCapability capabilityIndex;
    EXPECT_EQ(HDF_SUCCESS, InitForGetPortCapability(portIndex, &capabilityIndex));
}

HWTEST_F(AudioAdapterTest, InitForGetPortCapabilityWhenPortIdIsHdmiPortId, TestSize.Level1)
{
    TestInitForGetPortCapability(HDMI_PORT_ID);
}

HWTEST_F(AudioAdapterTest, InitForGetPortCapabilityWhenPortIdIsTwo, TestSize.Level1)
{
    struct AudioPort portIndex;
    EXPECT_EQ(HDF_SUCCESS, InitPort(portIndex));
    portIndex.dir = PORT_OUT;
    portIndex.portId = AUDIO_PORT_ID;
    struct AudioPortCapability capabilityIndex;
    EXPECT_EQ(HDF_FAILURE, InitForGetPortCapability(portIndex, &capabilityIndex));
}

HWTEST_F(AudioAdapterTest, AudioAdapterReleaseCapSubPortsWhenPortCapabilitysIsNull, TestSize.Level1)
{
    const struct AudioPortAndCapability *portCapabilitys = nullptr;
    int32_t num = PORTNUM;
    AudioAdapterReleaseCapSubPorts(portCapabilitys, num);
    EXPECT_EQ(nullptr, portCapabilitys);
}

HWTEST_F(AudioAdapterTest, AudioAdapterReleaseCapSubPortsWhenParamIsVaild, TestSize.Level1)
{
    struct AudioPortAndCapability portCapabilitys;
    struct AudioSubPortCapability *subPorts =
        (struct AudioSubPortCapability *)calloc(1, sizeof(struct AudioSubPortCapability));
    ASSERT_NE(subPorts, nullptr);
    portCapabilitys.capability.subPorts = subPorts;
    int32_t num = PORTNUM;
    AudioAdapterReleaseCapSubPorts(&portCapabilitys, num);
    EXPECT_EQ(nullptr, portCapabilitys.capability.subPorts);
}

HWTEST_F(AudioAdapterTest, AudioAdapterInitAllPortsWhenAdapterIsNull, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterInitAllPorts(nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterInitAllPortsWhenPortCapabilitysIsNotNull, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    struct AudioPortAndCapability capability;
    struct AudioPortAndCapability *pCapability = hwAdapter->portCapabilitys;
    hwAdapter->portCapabilitys = &capability;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts((struct AudioAdapter *)hwAdapter));
    hwAdapter->portCapabilitys = pCapability;
}

HWTEST_F(AudioAdapterTest, AudioAdapterInitAllPortsWhenPortsIsNull, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    struct AudioPort *pPorts = hwAdapter->adapterDescriptor.ports;
    hwAdapter->adapterDescriptor.ports = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterInitAllPorts((struct AudioAdapter *)hwAdapter));
    hwAdapter->adapterDescriptor.ports = pPorts;
}

HWTEST_F(AudioAdapterTest, AudioAdapterInitAllPortsWhenPortNumIsZero, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->adapterDescriptor.portNum = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterInitAllPorts((struct AudioAdapter *)hwAdapter));
}

HWTEST_F(AudioAdapterTest, AudioAdapterInitAllPortsWhenParamIsVaild, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
}

HWTEST_F(AudioAdapterTest, AudioReleaseRenderHandleWhenHwRenderIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = nullptr;
    AudioReleaseRenderHandle(hwRender);
    EXPECT_EQ(nullptr, hwRender);
}

HWTEST_F(AudioAdapterTest, AudioSetAcodeModeRenderWhenHwRenderIsNull, TestSize.Level1)
{
    InterfaceLibModeRenderSo pInterfaceLibModeRender;
    EXPECT_EQ(HDF_FAILURE, AudioSetAcodeModeRender(nullptr, &pInterfaceLibModeRender));
}

HWTEST_F(AudioAdapterTest, AudioSetAcodeModeRenderWhenpInterfaceLibModeRenderIsNull, TestSize.Level1)
{
    struct AudioHwRender hwRender;
    EXPECT_EQ(HDF_FAILURE, AudioSetAcodeModeRender(&hwRender, nullptr));
}

HWTEST_F(AudioAdapterTest, AudioSetAcodeModeRenderWhenpDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender hwRender;
    InterfaceLibModeRenderSo pInterfaceLibModeRender;
    hwRender.devCtlHandle = nullptr;
    EXPECT_EQ(HDF_FAILURE, AudioSetAcodeModeRender(&hwRender, &pInterfaceLibModeRender));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderPreWhenHwRenderIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = nullptr;
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    EXPECT_EQ(HDF_FAILURE, AudioAdapterCreateRenderPre(hwRender, &desc, &attrs, hwAdapter));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderPreWhenDescIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    ASSERT_NE(hwRender, nullptr);
    struct AudioDeviceDescriptor *desc = nullptr;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    int32_t ret = AudioAdapterCreateRenderPre(hwRender, desc, &attrs, hwAdapter);
    EXPECT_EQ(HDF_FAILURE, ret);
    free(hwRender);
    hwRender = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderPreWhenAttrsIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    ASSERT_NE(hwRender, nullptr);
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    struct AudioSampleAttributes *attrs = nullptr;
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    int32_t ret = AudioAdapterCreateRenderPre(hwRender, &desc, attrs, hwAdapter);
    EXPECT_EQ(HDF_FAILURE, ret);
    free(hwRender);
    hwRender = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderPreWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    ASSERT_NE(hwRender, nullptr);
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioHwAdapter *hwAdapter = nullptr;
    int32_t ret = AudioAdapterCreateRenderPre(hwRender, &desc, &attrs, hwAdapter);
    EXPECT_EQ(HDF_FAILURE, ret);
    free(hwRender);
    hwRender = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderPreWhenPortIdLessThanZero, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    ASSERT_NE(hwRender, nullptr);
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    devDesc.portId = AUDIO_HAL_ERR_NOT_SUPPORT;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    EXPECT_EQ(HDF_FAILURE, AudioAdapterCreateRenderPre(hwRender, &devDesc, &attrs, (struct AudioHwAdapter *)adapter));
    free(hwRender);
    hwRender = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderPreWhenAdapterNameIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    ASSERT_NE(hwRender, nullptr);
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioHwAdapter *hwAdapter = reinterpret_cast<AudioHwAdapter *>(adapter);
    hwAdapter->adapterDescriptor.adapterName = nullptr;
    EXPECT_EQ(HDF_FAILURE, AudioAdapterCreateRenderPre(hwRender, &devDesc, &attrs, hwAdapter));
    free(hwRender);
    hwRender = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderPreWhenParamIsVaild, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    ASSERT_NE(hwRender, nullptr);
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioHwAdapter *hwAdapter = reinterpret_cast<AudioHwAdapter *>(adapter);
    EXPECT_EQ(HDF_SUCCESS, AudioAdapterCreateRenderPre(hwRender, &devDesc, &attrs, hwAdapter));
    free(hwRender);
    hwRender = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterBindServiceRenderWhenHwRenderIsNull, TestSize.Level1)
{
    EXPECT_EQ(HDF_FAILURE, AudioAdapterBindServiceRender(nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    struct AudioSampleAttributes attrs;
    struct AudioRender *render;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateRender(nullptr, &desc, &attrs, &render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderWhenDescIsNull, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    struct AudioRender *render;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateRender(adapter, nullptr, &attrs, &render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderWhenAttrsIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    struct AudioRender *render;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateRender(adapter, &devDesc, nullptr, &render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderWhenRenderIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateRender(adapter, &devDesc, &attrs, nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderWhenAdapterMgrRenderFlagIsGreaterThanOne, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->adapterMgrRenderFlag = 1;
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioRender *render;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL,
        AudioAdapterCreateRender((struct AudioAdapter *)hwAdapter, &devDesc, &attrs, &render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderWhenPeriodIsLessThanZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    attrs.period = -1;
    struct AudioRender *render;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterCreateRender(adapter, &devDesc, &attrs, &render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateRenderWhenParamIsVaild, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioRender *render;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterCreateRender(adapter, &devDesc, &attrs, &render));
    EXPECT_EQ(HDF_SUCCESS, AudioAdapterDestroyRender(adapter, render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterDestroyRenderWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioRender render;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterDestroyRender(nullptr, &render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterDestroyRenderWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *render = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterDestroyRender(adapter, render));
}

HWTEST_F(AudioAdapterTest, AudioAdapterDestroyRenderWhenBufferIsNotNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    struct AudioRender *render;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterCreateRender(adapter, &devDesc, &attrs, &render));

    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterDestroyRender(adapter, render));
}

HWTEST_F(AudioAdapterTest, GetAudioCaptureFuncWhenHwCaptureIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = nullptr;
    EXPECT_EQ(HDF_FAILURE, GetAudioCaptureFunc(hwCapture));
}

HWTEST_F(AudioAdapterTest, GetAudioCaptureFuncWhenParamIsVaild, TestSize.Level1)
{
    struct AudioHwCapture hwCapture;
    EXPECT_EQ(HDF_SUCCESS, GetAudioCaptureFunc(&hwCapture));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenHwCaptureIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_FAILURE, InitHwCaptureParam(nullptr, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenDescIsNull, TestSize.Level1)
{
    struct AudioHwCapture hwCapture;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_FAILURE, InitHwCaptureParam(&hwCapture, nullptr, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenAttrsIsNull, TestSize.Level1)
{
    struct AudioHwCapture hwCapture;
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_FAILURE, InitHwCaptureParam(&hwCapture, &desc, nullptr));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenPortIdLessThanZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    desc.portId = AUDIO_HAL_ERR_NOT_SUPPORT;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioHwCapture hwCapture;
    EXPECT_EQ(HDF_SUCCESS, InitHwCapture(hwCapture, desc, attrs));
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, InitHwCaptureParam(&hwCapture, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenPeriodLessThanZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    attrs.period = AUDIO_HAL_ERR_NOT_SUPPORT;
    struct AudioHwCapture hwCapture;
    EXPECT_EQ(HDF_SUCCESS, InitHwCapture(hwCapture, desc, attrs));
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, InitHwCaptureParam(&hwCapture, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenFormatIsNotSupport, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    attrs.format = AUDIO_FORMAT_AAC_MAIN;
    struct AudioHwCapture hwCapture;
    EXPECT_EQ(HDF_SUCCESS, InitHwCapture(hwCapture, desc, attrs));
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, InitHwCaptureParam(&hwCapture, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenChannelCountIsZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    attrs.channelCount = 0;
    struct AudioHwCapture hwCapture;
    EXPECT_EQ(HDF_SUCCESS, InitHwCapture(hwCapture, desc, attrs));
    EXPECT_EQ(HDF_FAILURE, InitHwCaptureParam(&hwCapture, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, InitHwCaptureParamWhenParamIsVaild, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioHwCapture hwCapture;
    EXPECT_EQ(HDF_SUCCESS, InitHwCapture(hwCapture, desc, attrs));
    EXPECT_EQ(HDF_SUCCESS, InitHwCaptureParam(&hwCapture, &desc, &attrs));
}

HWTEST_F(AudioAdapterTest, AudioReleaseCaptureHandleWhenHwCaptureIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = nullptr;
    AudioReleaseCaptureHandle(hwCapture);
    EXPECT_EQ(nullptr, hwCapture);
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCapturePreWhenHwCaptureIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = nullptr;
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    EXPECT_EQ(HDF_FAILURE, AudioAdapterCreateCapturePre(hwCapture, &desc, &attrs, hwAdapter));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCapturePreWhenDescIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)calloc(1, sizeof(*hwCapture));
    ASSERT_NE(hwCapture, nullptr);
    struct AudioDeviceDescriptor *desc = nullptr;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    EXPECT_EQ(HDF_FAILURE, AudioAdapterCreateCapturePre(hwCapture, desc, &attrs, hwAdapter));
    free(hwCapture);
    hwCapture = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCapturePreWhenAttrsIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)calloc(1, sizeof(*hwCapture));
    ASSERT_NE(hwCapture, nullptr);
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    struct AudioSampleAttributes *attrs = nullptr;
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    EXPECT_EQ(HDF_FAILURE, AudioAdapterCreateCapturePre(hwCapture, &desc, attrs, hwAdapter));
    free(hwCapture);
    hwCapture = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCapturePreWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)calloc(1, sizeof(*hwCapture));
    ASSERT_NE(hwCapture, nullptr);
    struct AudioDeviceDescriptor desc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(desc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioHwAdapter *hwAdapter = nullptr;
    EXPECT_EQ(HDF_FAILURE, AudioAdapterCreateCapturePre(hwCapture, &desc, &attrs, hwAdapter));
    free(hwCapture);
    hwCapture = nullptr;
}

HWTEST_F(AudioAdapterTest, AudioAdapterInterfaceLibModeCaptureWhenHwCaptureIsNull, TestSize.Level1)
{
    EXPECT_EQ(HDF_FAILURE, AudioAdapterInterfaceLibModeCapture(nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCaptureWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor desc;
    struct AudioSampleAttributes attrs;
    struct AudioCapture *capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateCapture(nullptr, &desc, &attrs, &capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCaptureWhenDescIsNull, TestSize.Level1)
{
    struct AudioSampleAttributes attrs;
    struct AudioCapture *capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateCapture(adapter, nullptr, &attrs, &capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCaptureWhenAttrsIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    struct AudioCapture *capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateCapture(adapter, &devDesc, nullptr, &capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCaptureWhenCaptureIsNull, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterCreateCapture(adapter, &devDesc, &attrs, nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCaptureWhenAdapterMgrCaptureFlagIsGreaterThanOne, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->adapterMgrCaptureFlag = 1;
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioCapture *capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL,
        AudioAdapterCreateCapture((struct AudioAdapter *)adapter, &devDesc, &attrs, &capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCaptureWhenPeriodIsLessThanZero, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    attrs.period = -1;
    struct AudioCapture *capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterCreateCapture(adapter, &devDesc, &attrs, &capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterCreateCaptureWhenParamIsVaild, TestSize.Level1)
{
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioCapture *capture;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterCreateCapture(adapter, &devDesc, &attrs, &capture));
    EXPECT_EQ(HDF_SUCCESS, AudioAdapterDestroyCapture(adapter, capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterDestroyCaptureWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioCapture capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterDestroyCapture(nullptr, &capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterDestroyCaptureWhenCaptureIsNull, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterDestroyCapture(adapter, nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterDestroyCaptureWhenBufferIsNotNull, TestSize.Level1)
{ 
    struct AudioDeviceDescriptor devDesc;
    EXPECT_EQ(HDF_SUCCESS, InitDevDescCapture(devDesc));
    struct AudioSampleAttributes attrs;
    EXPECT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    struct AudioCapture *capture;
    /* to support different products */
    int32_t ret = AudioAdapterCreateCapture(adapter, &devDesc, &attrs, &capture);
    if ((ret == AUDIO_HAL_SUCCESS) || (ret == AUDIO_HAL_ERR_INTERNAL)) {
        EXPECT_TRUE(true);
    }

    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterDestroyCapture(adapter, capture));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioPort port;
    struct AudioPortCapability capability;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPortCapability(nullptr, &port, &capability));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenPortIsNull, TestSize.Level1)
{
    struct AudioPortCapability capability;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPortCapability(adapter, nullptr, &capability));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenCapabilityIsNull, TestSize.Level1)
{
    struct AudioPort *port = desc->ports;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPortCapability(adapter, port, nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenPortNameIsNull, TestSize.Level1)
{
    struct AudioPort port;
    port.portName = nullptr;
    struct AudioPortCapability capability;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPortCapability(adapter, &port, &capability));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenPortCapabilitysIsNull, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->portCapabilitys = nullptr;
    struct AudioPort *port = desc->ports;
    struct AudioPortCapability capability;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL,
        AudioAdapterGetPortCapability((struct AudioAdapter *)hwAdapter, port, &capability));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenPortNumIsZero, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->adapterDescriptor.portNum = 0;
    struct AudioPort *port = desc->ports;
    struct AudioPortCapability capability;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL,
        AudioAdapterGetPortCapability((struct AudioAdapter *)hwAdapter, port, &capability));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenParamIsVaild, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioPort *port = desc->ports;
    struct AudioPortCapability capability;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterGetPortCapability(adapter, port, &capability));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPortCapabilityWhenPortIdIsError, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioPort *port = desc->ports;
    uint32_t portId = port->portId;
    port->portId = 1;
    struct AudioPortCapability capability;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterGetPortCapability(adapter, port, &capability));
    port->portId = portId;
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioPort port;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterSetPassthroughMode(nullptr, &port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenPortIsNull, TestSize.Level1)
{
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterSetPassthroughMode(adapter, nullptr, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenPortNameIsNull, TestSize.Level1)
{
    struct AudioPort port;
    port.portName = nullptr;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterSetPassthroughMode(adapter, &port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenPortDirIsPortIn, TestSize.Level1)
{
    struct AudioPort port;
    port.portName = "usb";
    port.dir = PORT_IN;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterSetPassthroughMode(adapter, &port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenPortCapabilitysIsNull, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->portCapabilitys = nullptr;
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterSetPassthroughMode((struct AudioAdapter *)hwAdapter, port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenPortIdIsError, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioPort *port = desc->ports;
    uint32_t portId = port->portId;
    port->portId = 1;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterSetPassthroughMode(adapter, port, mode));
    port->portId = portId;
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenPortNumIsZero, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->adapterDescriptor.portNum = 0;
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterSetPassthroughMode((struct AudioAdapter *)hwAdapter, port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenSubPortsIsNull, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    free(hwAdapter->portCapabilitys->capability.subPorts);
    hwAdapter->portCapabilitys->capability.subPorts = nullptr;
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterSetPassthroughMode((struct AudioAdapter *)hwAdapter, port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenParamIsVaild, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterSetPassthroughMode(adapter, port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterSetPassthroughModeWhenModeIsError, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_RAW;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterSetPassthroughMode(adapter, port, mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioPort port;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPassthroughMode(nullptr, &port, &mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenPortIsNull, TestSize.Level1)
{
    struct AudioPort *port = nullptr;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPassthroughMode(adapter, port, &mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenModeIsNull, TestSize.Level1)
{
    struct AudioPort *port = desc->ports;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPassthroughMode(adapter, port, nullptr));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenPortNameIsNull, TestSize.Level1)
{
    struct AudioPort port;
    port.portName = nullptr;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioAdapterGetPassthroughMode(adapter, &port, &mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenPortDirIsPortIn, TestSize.Level1)
{
    struct AudioPort port;
    port.portName = "usb";
    port.dir = PORT_IN;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterGetPassthroughMode(adapter, &port, &mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenPortCapabilitysIsNull, TestSize.Level1)
{
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->portCapabilitys = nullptr;
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterGetPassthroughMode((AudioAdapter *)hwAdapter, port, &mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenPortNumIsZero, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->adapterDescriptor.portNum = 0;
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterGetPassthroughMode((AudioAdapter *)hwAdapter, port, &mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenParamIsVaild, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioPort *port = desc->ports;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterGetPassthroughMode(adapter, port, &mode));
}

HWTEST_F(AudioAdapterTest, AudioAdapterGetPassthroughModeWhenPortIdIsError, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterInitAllPorts(adapter));
    struct AudioPort *port = desc->ports;
    uint32_t portId = port->portId;
    port->portId = 1;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioAdapterGetPassthroughMode(adapter, port, &mode));
    port->portId = portId;
}
}