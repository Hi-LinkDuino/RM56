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

#ifndef OHOS_ACELITE_DFT_IMPL_H
#define OHOS_ACELITE_DFT_IMPL_H

#include "acelite_config.h"

#if ((OHOS_ACELITE_PRODUCT_WATCH == 1) || (FEATURE_CUSTOM_ENTRY_PAGE == 1))
#include "memory_heap.h"

namespace OHOS {
namespace ACELite {
typedef void (*PageReplacedCallback)(const char *path, int state);

struct Param {
    const char *routerPath = nullptr;
    const char *routerParam = nullptr;
    const char *path = nullptr;
};

typedef void (*PageInfoFunc)(const Param param);

class DftImpl final : public MemoryHeap {
public:
    static DftImpl *GetInstance();

    /**
     * @fn DftImpl::RegisterPageReplaced()
     *
     * @brief Register page replace event
     */
    void RegisterPageReplaced(PageReplacedCallback handler);

    void RegisterPageChangedCallback(PageInfoFunc pageInfoFunc);

    /**
     * @fn DftImpl::CallbackPageReplaced()
     *
     * @brief Call the path handler if it's set
     */
    void CallbackPageReplaced(int state, const char *param = nullptr);

private:
    DftImpl() : pageReplacedCallback_(nullptr), pageInfoFunc_(nullptr) {}

    ~DftImpl() = default;

    /**
     * @fn DftImpl::GetPagePath()
     *
     * @brief Return current page path
     */
    char *GetPagePath();

    PageReplacedCallback pageReplacedCallback_;
    PageInfoFunc pageInfoFunc_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_PRODUCT_WATCH
#endif // OHOS_ACELITE_DFT_IMPL_H
