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

#ifndef DRM_VSYNC_WORKER_H
#define DRM_VSYNC_WORKER_H
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "display_device.h"
#include "hdi_device_common.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
class DrmVsyncWorker {
public:
    DrmVsyncWorker();
    virtual ~DrmVsyncWorker();
    int32_t Init(int fd);
    static DrmVsyncWorker &GetInstance();

    void EnableVsync(bool enable);
    void WorkThread();
    uint64_t WaitNextVBlank(unsigned int &sq);
    bool WaitSignalAndCheckRuning();
    void ReqesterVBlankCb(std::shared_ptr<VsyncCallBack> &cb);

private:
    int mDrmFd = 0;
    std::unique_ptr<std::thread> mThread;
    bool mEnable = false;
    std::mutex mMutex;
    std::condition_variable mCondition;
    std::shared_ptr<VsyncCallBack> mCallBack;
    bool mRunning = false;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // DRM_VSYNC_WORKER_H