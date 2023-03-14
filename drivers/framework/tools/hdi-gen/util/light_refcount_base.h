/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_LIGHTREFCOUNTBASE_H
#define OHOS_HDI_LIGHTREFCOUNTBASE_H

#include <atomic>

namespace OHOS {
namespace HDI {
class LightRefCountBase {
public:
    inline LightRefCountBase() : refCount_(0) {}

    int AddRef();

    int Release();

    inline int GetRefCount() const
    {
        return refCount_.load(std::memory_order_relaxed);
    }

protected:
    virtual ~LightRefCountBase() = default;

private:
    std::atomic<int> refCount_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_LIGHTREFCOUNTBASE_H