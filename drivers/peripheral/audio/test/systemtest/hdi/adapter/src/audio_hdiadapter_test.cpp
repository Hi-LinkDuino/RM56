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

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Test audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver adapter.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_hdi_common.h
 *
 * @brief Declares APIs for operations related to the audio adapter.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_hdi_common.h"
#include "audio_hdiadapter_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";


class AudioHdiAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static TestAudioManager *(*GetAudioManager)();
    static void *handleSo;
#ifdef AUDIO_MPI_SO
    static int32_t (*SdkInit)();
    static void (*SdkExit)();
    static void *sdkSo;
#endif
};

TestAudioManager *(*AudioHdiAdapterTest::GetAudioManager)() = nullptr;
void *AudioHdiAdapterTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiAdapterTest::SdkInit)() = nullptr;
    void (*AudioHdiAdapterTest::SdkExit)() = nullptr;
    void *AudioHdiAdapterTest::sdkSo = nullptr;
#endif

void AudioHdiAdapterTest::SetUpTestCase(void)
{
#ifdef AUDIO_MPI_SO
    char sdkResolvedPath[] = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_render");
    sdkSo = dlopen(sdkResolvedPath, RTLD_LAZY);
    if (sdkSo == nullptr) {
        return;
    }
    SdkInit = (int32_t (*)())(dlsym(sdkSo, "MpiSdkInit"));
    if (SdkInit == nullptr) {
        return;
    }
    SdkExit = (void (*)())(dlsym(sdkSo, "MpiSdkExit"));
    if (SdkExit == nullptr) {
        return;
    }
    SdkInit();
#endif
    char absPath[PATH_MAX] = {0};
    if (realpath(RESOLVED_PATH.c_str(), absPath) == nullptr) {
        return;
    }
    handleSo = dlopen(absPath, RTLD_LAZY);
    if (handleSo == nullptr) {
        return;
    }
    GetAudioManager = (TestAudioManager *(*)())(dlsym(handleSo, FUNCTION_NAME.c_str()));
    if (GetAudioManager == nullptr) {
        return;
    }
}

void AudioHdiAdapterTest::TearDownTestCase(void)
{
#ifdef AUDIO_MPI_SO
    SdkExit();
    if (sdkSo != nullptr) {
        dlclose(sdkSo);
        sdkSo = nullptr;
    }
    if (SdkInit != nullptr) {
        SdkInit = nullptr;
    }
    if (SdkExit != nullptr) {
        SdkExit = nullptr;
    }
#endif
    if (handleSo != nullptr) {
        dlclose(handleSo);
        handleSo = nullptr;
    }
    if (GetAudioManager != nullptr) {
        GetAudioManager = nullptr;
    }
}

void AudioHdiAdapterTest::SetUp(void) {}

void AudioHdiAdapterTest::TearDown(void) {}

/**
* @tc.name  Test GetAllAdapters API via legal input
* @tc.number  SUB_Audio_HDI_GetAllAdapters_0001
* @tc.desc  test GetAllAdapters interface，Returns 0 if the list is obtained successfully
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_GetAllAdapters_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    ASSERT_NE(nullptr, manager);
    ret = manager->GetAllAdapters(manager, &descs, &size);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(AUDIO_ADAPTER_MAX_NUM, size);
}

/**
* @tc.name  Test GetAllAdapters API via setting the incoming parameter manager is nullptr
* @tc.number  SUB_Audio_HDI_GetAllAdapters_0002
* @tc.desc  test GetAllAdapters interface，Returns -1 if the incoming parameter manager is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_GetAllAdapters_0002, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;
    TestAudioManager *manager1 = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    ASSERT_NE(nullptr, manager);
    ret = manager->GetAllAdapters(manager1, &descs, &size);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

/**
* @tc.name  Test GetAllAdapters API via setting the incoming parameter descs is nullptr
* @tc.number  SUB_Audio_HDI_GetAllAdapters_0003
* @tc.desc  test GetAllAdapters interface，Returns -1 if the incoming parameter descs is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_GetAllAdapters_0003, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor **descs = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    ASSERT_NE(nullptr, manager);
    ret = manager->GetAllAdapters(manager, descs, &size);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

/**
* @tc.name  Test GetAllAdapters API via setting the incoming parameter size is nullptr
* @tc.number  SUB_Audio_HDI_GetAllAdapters_0004
* @tc.desc  test GetAllAdapters interface，Returns -1 if the incoming parameter size is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_GetAllAdapters_0004, TestSize.Level1)
{
    int32_t ret = -1;
    int *size = nullptr;
    struct AudioAdapterDescriptor *descs = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    ASSERT_NE(nullptr, manager);
    ret = manager->GetAllAdapters(manager, &descs, size);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

/**
* @tc.name  Test LoadAdapter API via legal input
* @tc.number  SUB_Audio_HDI_LoadAdapter_0001
* @tc.desc  test LoadAdapter interface，Returns 0 if the driver is loaded successfully
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_LoadAdapter_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetAdapters(manager, &descs, size);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    struct AudioAdapterDescriptor *desc = &descs[0];
    ASSERT_TRUE(desc != nullptr);
    struct AudioAdapter *adapter = nullptr;
    ret = manager->LoadAdapter(manager, desc, &adapter);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = -1;
    if (adapter != nullptr) {
        if (adapter->InitAllPorts != nullptr && adapter->CreateRender != nullptr &&
            adapter->DestroyRender != nullptr && adapter->CreateCapture != nullptr &&
            adapter->DestroyCapture != nullptr && adapter->GetPortCapability != nullptr &&
            adapter->SetPassthroughMode != nullptr && adapter->GetPassthroughMode != nullptr) {
            ret = 0;
        }
    }
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test LoadAdapter API via changing the adapterName of incoming parameter desc
* @tc.number  SUB_Audio_HDI_LoadAdapter_0002
* @tc.desc  test LoadAdapter interface，Returns -1 if the adapterName of incoming parameter desc is not support
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_LoadAdapter_0002, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetAdapters(manager, &descs, size);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    struct AudioAdapterDescriptor *desc = &descs[0];
    desc->adapterName = "illegal";
    ASSERT_TRUE(desc != nullptr);
    struct AudioAdapter *adapter = nullptr;

    ret = manager->LoadAdapter(manager, desc, &adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    desc->adapterName = "internal";
    ret = manager->LoadAdapter(manager, desc, &adapter);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test LoadAdapter API via setting the adapterName of incoming parameter desc  is illegal
* @tc.number  SUB_Audio_HDI_LoadAdapter_0003
* @tc.desc  test LoadAdapter interface，Returns -1 if the adapterName of incoming parameter desc is illegal
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_LoadAdapter_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapterDescriptor desc = {
        .adapterName = "illegal",
        .portNum = 2,
        .ports = nullptr,
    };

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    ASSERT_NE(nullptr, manager);
    ret = manager->LoadAdapter(manager, &desc, &adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test LoadAdapter API via setting the adapterName of incoming parameter manager is nullptr
* @tc.number  SUB_Audio_HDI_LoadAdapter_0004
* @tc.desc  test LoadAdapter interface，Returns -1 if the incoming parameter manager is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_LoadAdapter_0004, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;
    TestAudioManager *managerNull = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetAdapters(manager, &descs, size);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    struct AudioAdapterDescriptor *desc = &descs[0];
    ASSERT_TRUE(desc != nullptr);
    struct AudioAdapter *adapter = nullptr;

    ret = manager->LoadAdapter(managerNull, desc, &adapter);
    ASSERT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test LoadAdapter API via setting the adapterName of incoming parameter desc is nullptr
* @tc.number  SUB_Audio_HDI_LoadAdapter_0005
* @tc.desc  test LoadAdapter interface，Returns -1 if the incoming parameter desc is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_LoadAdapter_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapterDescriptor *desc = nullptr;
    struct AudioAdapter *adapter = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    ASSERT_NE(nullptr, manager);
    ret = manager->LoadAdapter(manager, desc, &adapter);
    ASSERT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test LoadAdapter API via setting the adapterName of incoming parameter adapter is nullptr
* @tc.number  SUB_Audio_HDI_LoadAdapter_0006
* @tc.desc  test LoadAdapter interface，Returns -1 if the incoming parameter adapter is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_LoadAdapter_0006, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;
    struct AudioAdapter **adapter = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetAdapters(manager, &descs, size);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    struct AudioAdapterDescriptor *desc = &descs[0];
    ASSERT_TRUE(desc != nullptr);

    ret = manager->LoadAdapter(manager, desc, adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

/**
* @tc.name  Test AudioAdapterInitAllPorts API via legal input.
* @tc.number  SUB_Audio_HDI_AdapterInitAllPorts_0001
* @tc.desc  Test AudioAdapterInitAllPorts interface, return 0 if the ports is initialize successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterInitAllPorts_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioPort* renderPort = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioAdapterInitAllPorts API when loads two adapters.
* @tc.number  SUB_Audio_HDI_AdapterInitAllPorts_0002
* @tc.desc  Test AudioAdapterInitAllPorts interface, return 0 if loads two adapters successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterInitAllPorts_0002, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t ret2 = -1;
    struct AudioPort* renderPort = nullptr;
    struct AudioPort* renderPortUsb = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapter1 = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_INTERNAL, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret2 = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter1, renderPortUsb);
    if (ret2 < 0 || adapter1 == nullptr) {
        manager->UnloadAdapter(manager, adapter);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret2);
    }
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = adapter->InitAllPorts(adapter1);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret2);

    manager->UnloadAdapter(manager, adapter);
    manager->UnloadAdapter(manager, adapter1);
}

/**
* @tc.name  Test AudioAdapterInitAllPorts API when the parameter adapter is empty.
* @tc.number  SUB_Audio_HDI_AdapterInitAllPorts_0003
* @tc.desc  Test AudioAdapterInitAllPorts API, return -1 if the parameter adapter is empty.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterInitAllPorts_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapterNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapterNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioAdapterGetPortCapability API when the PortType is PORT_OUT.
* @tc.number  SUB_Audio_HDI_AdapterGetPortCapability_0001
* @tc.desc  Test AudioAdapterGetPortCapability,return 0 if PortType is PORT_OUT.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPortCapability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    struct AudioPortCapability capability = {};

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPortCapability(adapter, audioPort, &capability);
    if (ret < 0 || capability.formats == nullptr || capability.subPorts == nullptr) {
        manager->UnloadAdapter(manager, adapter);
        ASSERT_NE(AUDIO_HAL_SUCCESS, ret);
        ASSERT_NE(nullptr, capability.formats);
        ASSERT_NE(nullptr, capability.subPorts);
    }
    if (capability.subPorts->desc == nullptr) {
        manager->UnloadAdapter(manager, adapter);
        ASSERT_NE(nullptr, capability.subPorts->desc);
    }
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioAdapterGetPortCapability API when the PortType is PORT_IN.
* @tc.number  SUB_Audio_HDI_AdapterGetPortCapability_0002
* @tc.desc  Test AudioAdapterGetPortCapability,return 0 if PortType is PORT_IN.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPortCapability_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    struct AudioPortCapability capability = {};

    ret = GetLoadAdapter(manager, PORT_IN, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPortCapability(adapter, audioPort, &capability);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioAdapterGetPortCapability API when the PortType is PORT_OUT_IN.
* @tc.number  SUB_Audio_HDI_AdapterGetPortCapability_0003
* @tc.desc  Test AudioAdapterGetPortCapability,return 0 if PortType is PORT_OUT_IN.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPortCapability_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    struct AudioPortCapability capability = {};

    ret = GetLoadAdapter(manager, PORT_OUT_IN, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPortCapability(adapter, audioPort, &capability);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioAdapterGetPortCapability API, when the parameter adapter is empty.
* @tc.number  SUB_Audio_HDI_AdapterGetPortCapability_0004
* @tc.desc  Test AudioAdapterGetPortCapability, return -1 if the parameter adapter is empty.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPortCapability_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapterNull = nullptr;
    struct AudioPortCapability capability = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_INTERNAL, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    ret = adapter->GetPortCapability(adapterNull, audioPort, &capability);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioAdapterGetPortCapability API when the audioPort is nullptr or not supported.
* @tc.number  SUB_Audio_HDI_AdapterGetPortCapability_0005
* @tc.desc  Test AudioAdapterGetPortCapability, return -1 if the audioPort is nullptr or not supported.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPortCapability_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort *audioPortNull = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioPortCapability capability = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    struct AudioPort* audioPort = nullptr;
    struct AudioPort audioPortError = { .dir = PORT_OUT, .portId = 9, .portName = "AIP" };

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_INTERNAL, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPortCapability(adapter, audioPortNull, &capability);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = adapter->GetPortCapability(adapter, &audioPortError, &capability);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioAdapterGetPortCapability API when the capability is nullptr.
* @tc.number  SUB_Audio_HDI_AdapterGetPortCapability_0006
* @tc.desc  Test AudioAdapterGetPortCapability, return -1 if capability is nullptr.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPortCapability_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioPortCapability *capabilityNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_INTERNAL, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPortCapability(adapter, audioPort, capabilityNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AdapterSetPassthroughMode API when the PortType is PORT_OUT.
* @tc.number  SUB_Audio_HDI_AdapterSetPassthroughMode_0001
* @tc.desc  test AdapterSetPassthroughMode interface, return 0 if PortType is PORT_OUT.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterSetPassthroughMode_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    enum AudioPortPassthroughMode modeLpcm = PORT_PASSTHROUGH_AUTO;

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->SetPassthroughMode(adapter, audioPort, PORT_PASSTHROUGH_LPCM);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPassthroughMode(adapter, audioPort, &modeLpcm);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(PORT_PASSTHROUGH_LPCM, modeLpcm);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name Test AdapterSetPassthroughMode API when the PortType is PORT_IN.
* @tc.number  SUB_Audio_HDI_AdapterSetPassthroughMode_0002
* @tc.desc  test AdapterSetPassthroughMode interface, return -1 if PortType is PORT_IN.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterSetPassthroughMode_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioPort* audioPort = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_IN, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->SetPassthroughMode(adapter, audioPort, PORT_PASSTHROUGH_LPCM);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AdapterSetPassthroughMode API when the parameter adapter is nullptr.
* @tc.number  SUB_Audio_HDI_AdapterSetPassthroughMode_0003
* @tc.desc  test AdapterSetPassthroughMode interface, return -1 the parameter adapter is empty.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterSetPassthroughMode_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapterNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_INTERNAL, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->SetPassthroughMode(adapterNull, audioPort, PORT_PASSTHROUGH_LPCM);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AdapterSetPassthroughMode API when the parameter audioPort is nullptr or not supported.
* @tc.number  SUB_Audio_HDI_AdapterSetPassthroughMode_0004
* @tc.desc  test AdapterSetPassthroughMode interface, return -1 if the audioPort is nullptr or not supported.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterSetPassthroughMode_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioPort *audioPortNull = nullptr;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    struct AudioPort audioPortError = { .dir = PORT_OUT, .portId = 8, .portName = "AIP1" };
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->SetPassthroughMode(adapter, audioPortNull, mode);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = adapter->SetPassthroughMode(adapter, &audioPortError, mode);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AdapterSetPassthroughMode API when the not supported mode.
* @tc.number  SUB_Audio_HDI_AdapterSetPassthroughMode_0005
* @tc.desc  test AdapterSetPassthroughMode interface, return -1 if the not supported mode.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterSetPassthroughMode_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->SetPassthroughMode(adapter, audioPort, PORT_PASSTHROUGH_RAW);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AdapterGetPassthroughMode API via legal input
* @tc.number  SUB_Audio_HDI_AdapterGetPassthroughMode_0001
* @tc.desc  test AdapterGetPassthroughMode interface, return 0 if is get successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPassthroughMode_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_AUTO;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = adapter->SetPassthroughMode(adapter, audioPort, PORT_PASSTHROUGH_LPCM);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = adapter->GetPassthroughMode(adapter, audioPort, &mode);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(PORT_PASSTHROUGH_LPCM, mode);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AdapterGetPassthroughMode API  when the parameter adapter is nullptr.
* @tc.number  SUB_Audio_HDI_AdapterGetPassthroughMode_0002
* @tc.desc  test AdapterGetPassthroughMode interface, return -1 if the parameter adapter is empty..
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPassthroughMode_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapterNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPassthroughMode(adapterNull, audioPort, &mode);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name   Test AdapterGetPassthroughMode API when the parameter audioPort is nullptr or not supported.
* @tc.number  SUB_Audio_HDI_AdapterGetPassthroughMode_0003
* @tc.desc  test AdapterGetPassthroughMode interface, return -1 if the audioPort is nullptr or not supported.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPassthroughMode_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    struct AudioPort *audioPortNull = nullptr;
    enum AudioPortPassthroughMode mode = PORT_PASSTHROUGH_LPCM;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    struct AudioPort audioPortError = { .dir = PORT_OUT, .portId = 8, .portName = "AIP" };
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPassthroughMode(adapter, audioPortNull, &mode);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = adapter->GetPassthroughMode(adapter, &audioPortError, &mode);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AdapterGetPassthroughMode API  when the parameter mode is nullptr.
* @tc.number  SUB_Audio_HDI_AdapterGetPassthroughMode_0004
* @tc.desc  test AdapterGetPassthroughMode interface, return -1 if the parameter mode is empty.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiAdapterTest, SUB_Audio_HDI_AdapterGetPassthroughMode_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* audioPort = nullptr;
    enum AudioPortPassthroughMode *modeNull = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ASSERT_NE(nullptr, adapter);
    ret = adapter->InitAllPorts(adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->GetPassthroughMode(adapter, audioPort, modeNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}
}
