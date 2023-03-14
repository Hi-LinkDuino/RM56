/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_FEATURE_ABILITY_MODULE_H
#define OHOS_ACELITE_FEATURE_ABILITY_MODULE_H

#include "acelite_config.h"
#include "non_copyable.h"

#if (FEATURE_FEATURE_ABILITY_MODULE == 1)
#include "ability_kit.h"
#include "presets/preset_module.h"
namespace OHOS {
namespace ACELite {
static constexpr char FEATURE_ABILITY[] = "FeatureAbility";
struct FailCallbackParams : public MemoryHeap {
    void *data;
    uint16_t dataLength;
    uint16_t errorCode;
    FailCallbackParams() : data(nullptr), dataLength(0), errorCode(0) {}
    ACE_DISALLOW_COPY_AND_MOVE(FailCallbackParams);
};
class FeatureAbilityModule final : public PresetModule {
public:
    ACE_DISALLOW_COPY_AND_MOVE(FeatureAbilityModule);
    /**
     * @fn FeatureAbilityModule::FeatureAbilityModule()
     *
     * @brief Constructor.
     */
    FeatureAbilityModule() : PresetModule(FEATURE_ABILITY) {}

    /**
     * @fn FeatureAbilityModule::~FeatureAbilityModule()
     *
     * @brief Constructor.
     */
    ~FeatureAbilityModule() = default;

    void Init() override;

    static void Release();

private:
    /**
     * @fn FeatureAbilityModule::SubscribeMessage
     *
     * @brief subscribe message which comes from other device
     * @param func function object
     * @param context the context of function execution
     * @param args the list of arguments
     * @param length the length of arguments list
     */
    static jerry_value_t SubscribeMessage(const jerry_value_t func,
                                          const jerry_value_t context,
                                          const jerry_value_t args[],
                                          const jerry_length_t length);

    /**
     * @fn FeatureAbilityModule::UnsubscribeMessage
     *
     * @brief unsubscribe message which comes from other device
     * @param func function object
     * @param context the context of function execution
     * @param args the list of arguments
     * @param length the length of arguments list
     */
    static jerry_value_t UnsubscribeMessage(const jerry_value_t func,
                                            const jerry_value_t context,
                                            const jerry_value_t args[],
                                            const jerry_length_t length);

    /**
     * @fn FeatureAbilityModule::SendMsgToPeer
     *
     * @brief send message to phone app
     * @param func function object
     * @param context the context of function execution
     * @param args the list of arguments
     * @param length the length of arguments list
     */
    static jerry_value_t SendMsgToPeer(const jerry_value_t func,
                                       const jerry_value_t context,
                                       const jerry_value_t args[],
                                       const jerry_length_t length);

    static void SyncFailCallback(jerry_value_t &failCb,
                                 const jerry_value_t &context,
                                 const char * const data,
                                 uint16_t errorCode);

    static void SyncSendMsgCallback(const jerry_value_t &arg,
                                    const jerry_value_t &context,
                                    const char *errMsg,
                                    const uint16_t errCode,
                                    bool success);

    /**
     * @fn FeatureAbilityModule::Detect
     *
     * @brief ping to phone app
     * @param func function object
     * @param context the context of function execution
     * @param args the list of arguments
     * @param length the length of arguments list
     */
    static jerry_value_t Detect(const jerry_value_t func,
                                     const jerry_value_t context,
                                     const jerry_value_t args[],
                                     const jerry_length_t length);

    static int32_t MessageSuccessCallback(const void *data);

    static void CopySuccessMessage(const FeatureAbilityDataInfo *origin, FeatureAbilityDataInfo *&target);

    static int32_t MessageFailCallback(const void *data, uint16_t dataLength, uint16_t errorCode);

    static void AsyncSuccessCallback(void *data);

    static void AsyncFailCallback(void *data);

    static void ReleaseJsValues();

    static void ReleaseJsValue(jerry_value_t &value);

    static void ReleaseDetectJsValue();

    static void ReleaseSendMsgJsValue();

    /**
     * the execute context of callback
     */
    static jerry_value_t callbackContext_;

    /**
     * success callback
     */
    static jerry_value_t successCallback_;

    /**
     * fail callback
     */
    static jerry_value_t failCallback_;

    /**
     * register callback whether of not
     */
    static bool registed;

    /**
     * the execute context of callback for sendMsg
     */
    static jerry_value_t sendMsgCallbackContext_;

    /**
     * success callback for sendMsg
     */
    static jerry_value_t sendMsgSuccessCallback_;

    /**
     * fail callback for sendMsg
     */
    static jerry_value_t sendMsgFailCallback_;

    /**
     * the execute context of callback for detect
     */
    static jerry_value_t detectCallbackContext_;

    /**
     * success callback for detect
     */
    static jerry_value_t detectSuccessCallback_;

    /**
     * fail callback for detect
     */
    static jerry_value_t detectFailCallback_;
};
} // namespace ACELite
} // namespace OHOS
#endif
namespace OHOS {
namespace ACELite {
class FeaAbilityModule final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(FeaAbilityModule);
    FeaAbilityModule() = default;
    ~FeaAbilityModule() = default;
    static void Load()
    {
#if (FEATURE_FEATURE_ABILITY_MODULE == 1)
        FeatureAbilityModule featureAbilityModule;
        featureAbilityModule.Init();
#endif
    }
    static void Release()
    {
#if (FEATURE_FEATURE_ABILITY_MODULE == 1)
        FeatureAbilityModule::Release();
#endif
    }
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_FEATURE_ABILITY_MODULE_H
