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

#ifndef HDI_NETLINK_NONITOR_H
#define HDI_NETLINK_NONITOR_H
#include <thread>

namespace OHOS {
namespace HDI {
namespace DISPLAY {
class HdiNetLinkMonitor {
public:
    HdiNetLinkMonitor();
    int Init();
    virtual ~HdiNetLinkMonitor();

private:
    Void MonitorThread();
    volatile bool mRunning = false;
    int mScoketFd = -1;
    std::unique_ptr<std::thread> mThread;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // HDI_NETLINK_NONITOR_H