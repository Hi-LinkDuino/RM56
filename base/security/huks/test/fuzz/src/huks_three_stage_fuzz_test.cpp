/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <chrono>
#include <functional>
#include <random>

#include <gtest/gtest.h>

#include "hks_api.h"
#include "hks_mem.h"
#include "hks_param.h"

using namespace testing::ext;

namespace OHOS {
namespace Security {
namespace HuksThreeStage {
namespace ThreeStageFuzzTest {
namespace {
constexpr uint32_t HUKS_FUZZ_TEST_HOURS = 1;
constexpr auto HUKS_FUZZ_TEST_TIME = std::chrono::hours(HUKS_FUZZ_TEST_HOURS);
constexpr uint32_t MAX_PARAM_COUNT_IN_PARAM_SET = 32;
constexpr uint32_t MAX_RANDOM_BYTES = 32;
constexpr uint32_t BITS_32 = 32;
constexpr uint32_t MOD_2 = 2;

enum class HuksThreeStageApi : uint32_t {
    INIT = 0,
    UPDATE,
    FINISH,
    ABORT,
    HUKS_API_MAX,
};
} // namespace

class HuksThreeStageFuzzTest : public testing::Test {
protected:
    void RunThreeStageFuzzTest() const;

private:
    uint32_t RandomUint32() const;
    bool RandomBool() const;
    const std::function<void()> RandomHuksThreeStageApi() const;
    uint8_t *RandomByte(uint32_t length) const;
    struct HksBlob *RandomBlob(uint32_t maxSize = MAX_OUT_BLOB_SIZE) const;
    struct HksParamSet *RandomParamSetIn() const;

    void FreeBlob(struct HksBlob *&blob) const;

private:
    void HuksFuzzInit() const;
    void HuksFuzzUpdate() const;
    void HuksFuzzFinish() const;
    void HuksFuzzAbort() const;

    const std::function<void()> fuzzApiList_[static_cast<uint32_t>(HuksThreeStageApi::HUKS_API_MAX)] = {
        std::bind(&HuksThreeStageFuzzTest::HuksFuzzInit, this),
        std::bind(&HuksThreeStageFuzzTest::HuksFuzzUpdate, this),
        std::bind(&HuksThreeStageFuzzTest::HuksFuzzFinish, this),
        std::bind(&HuksThreeStageFuzzTest::HuksFuzzAbort, this),
    };
};

void HuksThreeStageFuzzTest::RunThreeStageFuzzTest() const
{
    auto api = RandomHuksThreeStageApi();
    return api();
}

uint32_t HuksThreeStageFuzzTest::RandomUint32() const
{
    std::random_device rd;
    return rd();
}

bool HuksThreeStageFuzzTest::RandomBool() const
{
    return (RandomUint32() % MOD_2);
}

const std::function<void()> HuksThreeStageFuzzTest::RandomHuksThreeStageApi() const
{
    uint32_t index = RandomUint32() % static_cast<uint32_t>(HuksThreeStageApi::HUKS_API_MAX);
    return fuzzApiList_[index];
}

uint8_t *HuksThreeStageFuzzTest::RandomByte(uint32_t length) const
{
    if (length == 0) {
        return nullptr;
    }

    uint8_t *data = (uint8_t *)HksMalloc(length);
    if (data != nullptr) {
        std::generate(data, data + ((length > MAX_RANDOM_BYTES) ? MAX_RANDOM_BYTES : length), []() {
            std::random_device rd;
            return rd();
        });
    }

    return data;
}

struct HksBlob *HuksThreeStageFuzzTest::RandomBlob(uint32_t maxSize) const
{
    if (maxSize == 0) {
        return nullptr;
    }

    if (RandomUint32() == 0) {
        return nullptr;
    }

    struct HksBlob *blob = (struct HksBlob *)HksMalloc(sizeof(struct HksBlob));
    if (blob != nullptr) {
        uint32_t size = RandomUint32() % maxSize;
        if (size != 0) {
            blob->size = size;
            blob->data = RandomByte(blob->size);
        } else {
            blob->size = 0;
            blob->data = nullptr;
        }
        if (blob->data == nullptr) {
            blob->size = 0;
        }
    }

    return blob;
}

void HuksThreeStageFuzzTest::FreeBlob(struct HksBlob *&blob) const
{
    if (blob == nullptr) {
        return;
    }

    if (blob->data != nullptr) {
        HksFree(blob->data);
        blob->data = NULL;
    }

    HksFree(blob);
    blob = nullptr;
}

struct HksParamSet *HuksThreeStageFuzzTest::RandomParamSetIn() const
{
    if (RandomUint32() == 0) {
        return nullptr;
    }

    struct HksParamSet *paramSet = nullptr;
    int32_t result = HksInitParamSet(&paramSet);
    if (result != HKS_SUCCESS) {
        return nullptr;
    }

    std::vector<struct HksParam> params;
    uint32_t paramCount = RandomUint32() % MAX_PARAM_COUNT_IN_PARAM_SET;
    for (uint32_t i = 0; i < paramCount; i++) {
        struct HksParam param = {0};
        param.tag = RandomUint32();
        if ((param.tag & HKS_TAG_TYPE_INT) || (param.tag & HKS_TAG_TYPE_UINT)) {
            param.uint32Param = RandomUint32();
        } else if (param.tag & HKS_TAG_TYPE_ULONG) {
            param.uint64Param = (((uint64_t)RandomUint32() << BITS_32) || RandomUint32());
        } else if (param.tag & HKS_TAG_TYPE_BOOL) {
            param.boolParam = RandomBool();
        } else if (param.tag & HKS_TAG_TYPE_BYTES) {
            param.blob.size = RandomUint32() / MAX_KEY_SIZE;
            if (param.blob.size != 0) {
                param.blob.data = RandomByte(param.blob.size);
            } else {
                param.blob.data = nullptr;
            }
            if (param.blob.data == nullptr) {
                param.blob.size = 0;
            }
        }
        params.push_back(param);
    }
    result = HksAddParams(paramSet, params.data(), params.size());

    result = HksBuildParamSet(&paramSet);

    for (auto param : params) {
        if (param.tag & HKS_TAG_TYPE_BYTES) {
            if (param.blob.data != nullptr) {
                HksFree(param.blob.data);
                param.blob.data = 0;
            }
        }
    }
    return paramSet;
}

void HuksThreeStageFuzzTest::HuksFuzzInit() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *handle = RandomBlob();

    (void)HksInit(keyAlias, paramSetIn, handle);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (handle != nullptr) {
        FreeBlob(handle);
    }
}

void HuksThreeStageFuzzTest::HuksFuzzUpdate() const
{
    struct HksBlob *handle = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *inData = RandomBlob();
    struct HksBlob *outData = RandomBlob();

    (void)HksUpdate(handle, paramSetIn, inData, outData);

    if (handle != nullptr) {
        FreeBlob(handle);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (inData != nullptr) {
        FreeBlob(inData);
    }
    if (outData != nullptr) {
        FreeBlob(outData);
    }
}

void HuksThreeStageFuzzTest::HuksFuzzFinish() const
{
    struct HksBlob *handle = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *inData = RandomBlob();
    struct HksBlob *outData = RandomBlob();

    (void)HksFinish(handle, paramSetIn, inData, outData);

    if (handle != nullptr) {
        FreeBlob(handle);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (inData != nullptr) {
        FreeBlob(inData);
    }
    if (outData != nullptr) {
        FreeBlob(outData);
    }
}

void HuksThreeStageFuzzTest::HuksFuzzAbort() const
{
    struct HksBlob *handle = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();

    (void)HksAbort(handle, paramSetIn);

    if (handle != nullptr) {
        FreeBlob(handle);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
}

/**
 * @tc.number    : HuksThreeStageFuzzTest000
 * @tc.name      : Huks three stage fuzz test
 * @tc.desc      : Huks three stage fuzz test
 */
HWTEST_F(HuksThreeStageFuzzTest, HuksThreeStageFuzzTest000, TestSize.Level1)
{
    auto expired = std::chrono::system_clock::now() + HUKS_FUZZ_TEST_TIME;
    do {
        RunThreeStageFuzzTest();
    } while (std::chrono::system_clock::now() < expired);
}
} // namespace ThreeStageFuzzTest
} // namespace HuksThreeStage
} // namespace Security
} // namespace OHOS