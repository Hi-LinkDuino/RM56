/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "util/light_refcount_base.h"

namespace OHOS {
namespace HDI {
int LightRefCountBase::AddRef()
{
    const int beforeCount = refCount_.fetch_add(1, std::memory_order_relaxed);
    return beforeCount + 1;
}

int LightRefCountBase::Release()
{
    const int beforeCount = refCount_.fetch_sub(1, std::memory_order_release);
    if (beforeCount - 1 == 0) {
        delete this;
    }
    return beforeCount - 1;
}
} // namespace HDI
} // namespace OHOS