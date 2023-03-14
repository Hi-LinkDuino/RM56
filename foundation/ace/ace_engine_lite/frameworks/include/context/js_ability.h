/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_JS_ABILITY_H
#define OHOS_ACELITE_JS_ABILITY_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "memory_heap.h"

namespace OHOS {
namespace ACELite {
class JSAbility final : public MemoryHeap {
public:
    JSAbility(const JSAbility &) = delete;
    JSAbility &operator=(const JSAbility &) = delete;
    JSAbility(JSAbility &&) = delete;
    JSAbility &operator=(JSAbility &&) = delete;

    /**
     * @fn JSAbility::JSAbility()
     *
     * @brief Default constructor.
     */
    JSAbility() : jsAbilityImpl_(nullptr) {}

    /**
     * @fn virtual JSAbility::~JSAbility()
     *
     * @brief Destructor.
     */
    virtual ~JSAbility();

    /**
     * @fn JSAbility::Launch()
     *
     * @brief Call this function to create the JS runtime environment, and it will init
     *        the JS framework and eval abilityPath/src/index.js as well
     * @param [in] the ability path to bundle name and token of this ability
     */
    void Launch(const char *abilityPath, const char *bundleName, uint16_t token, const char *pageInfo = nullptr);

    /**
     * @fn JSAbility::Show()
     *
     * @brief Call this function to active the application UI
     */
    void Show();

    /**
     * @fn JSAbility::Hide()
     *
     * @brief Call this function to move the current JS application to background
     */
    void Hide();

    /**
     * @fn JSAbility::TransferToDestroy()
     *
     * @brief Tear down JS runtime, release JS engine, and invoke user's onDestroy callback
     */
    void TransferToDestroy();

    /**
     * @fn JSAbility::BackPressed()
     *
     * @brief Called by AbilityHost to notify back key has been pressed, JS ability will tear down
     */
    void BackPressed();

    /**
     * @fn JSAbility::GetPackageName()
     *
     * @brief Return current package name
     */
    static const char *GetPackageName();

    /**
     * @fn JSAbility::ForceDestroy()
     *
     * @brief Force to clean up the whole application
     */
    void ForceDestroy();

    /**
     * @fn JSAbility::IsRecycled()
     *
     * @brief Used for checking if the current ability is already teardown
     */
    bool IsRecycled();

    /**
     * @fn JSAbility::HandleRenderTick()
     *
     * @brief Call the render tick if it's set
     */
    void HandleRenderTick();

    /**
     * @fn JSAbility::LazyLoadHandleRenderTick()
     *
     * @brief Call the lazy load tick if it's set
     */
    static void LazyLoadHandleRenderTick(void *data);

private:
    // the holder of JS runtime and user' JS related environment
    void *jsAbilityImpl_ = nullptr;
    // tracking the error render tick count
    uint32_t errorTickCount_ = 0;
    // to avoid tracing too frequently, output every 5 ticks
    static constexpr uint8_t ERR_TICK_COUNT_TRACE_CTRL = 5;
    bool isActived_ = false;
}; // class JSAbility
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_JS_ABILITY_H
