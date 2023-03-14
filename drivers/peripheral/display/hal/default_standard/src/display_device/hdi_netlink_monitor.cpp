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

#include "hdi_netlink_monitor.h"
#include <sys/socket.h>
#include <errno.h>

namespace OHOS {
namespace HDI {
namespace DISPLAY {
HdiNetLinkMonitor::HdiNetLinkMonitor()
{
    DISPLAY_LOGD();
}

int HdiNetLinkMonitor::Init()
{
    DISPLAY_LOGD();
    int fd;
    struct sockaddr_nl snl = { 0 };
    int ret;
    const int32_t bufferSize = 1024;
    DISPLAY_CHK_RETURN((mScoketFd < 0), DISPLAY_FAILURE, DISPLAY_LOGE("the socket has initial"));
    fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    DISPLAY_CHK_RETURN((fd < 0), DISPLAY_FAILURE, DISPLAY_LOGE("scoket create failed"));
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    ret = bind(fd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    DISPLAY_CHK_RETURN((ret < 0), DISPLAY_FAILURE, DISPLAY_LOGE("bind failed errno : %{public}d", errno));

    mThread = std::make_unique<std::thread>([this]() {
        mRunning = true;
        MonitorThread();
    });
    mScoketFd = fd;
    return DISPLAY_SUCCESS;
}

HdiNetLinkMonitor::~HdiNetLinkMonitor()
{
    DISPLAY_LOGD();
    if (mScoketFd >= 0) {
        close(mScoketFd);
    }
}

Void HdiNetLinkMonitor::MonitorThread()
{
    DISPLAY_LOGD();
    constexpr int BUFFER_SIZE = UEVENT_BUFFER_SIZE * 2;
    while (mRunning) {
        char buf[BUFFER_SIZE] = { 0 };
        recv(mScoketFd, &buf, sizeof(buf), 0);
    }
}
} // DISPLAY
} // HDI
} // OHOS

#endif // HDI_NETLINK_NONITOR_H