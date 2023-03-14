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

#ifndef HDI_SHARED_FD
#define HDI_SHARED_FD
#include <fcntl.h>
#include <memory>
#include "display_common.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
class HdiFd {
public:
    HdiFd()
    {
        DISPLAY_LOGD();
    }
    explicit HdiFd(int fd) : mFd(fd)
    {
        DISPLAY_LOGD("mFd %{public}d", mFd);
    }
    int GetFd() const
    {
        return mFd;
    };

    HdiFd &operator = (int fd)
    {
        if (mFd >= 0) {
            close(mFd);
        }
        mFd = fd;
        return *this;
    }

    virtual ~HdiFd()
    {
        if (mFd >= 0) {
            close(mFd);
        }
    }

private:
    int mFd = -1;
};

using FdPtr = std::shared_ptr<HdiFd>;
} // OHOS
} // HDIO
} // DISPLAY

#endif // HDI_SHARED_FD