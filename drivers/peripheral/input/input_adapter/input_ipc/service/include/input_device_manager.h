/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef INPUT_DEVICE_MANAGER_H
#define INPUT_DEVICE_MANAGER_H

#include <vector>
#include <sys/epoll.h>
#include <thread>
#include <pthread.h>
#include <map>
#include <list>
#include <unistd.h>
#include <functional>
#include <mutex>
#include <linux/input.h>
#include "iinput_host_callbck.h"
#include "hdf_log.h"

namespace OHOS {
namespace Input {
using namespace std;
#define EPOLL_WAIT_TIMEOUT (-1)
#define DEVICE_INFO_SIZE (80)
#define EPOLL_MAX_EVENTS    (32)
#define BUFFER_SIZE     (1024)
#define ARRAY_LENGTH    (128)
#define NAME_LENGTH     (128)
#define EVENT_BUFFER_SIZE (512)
#define MS_THOUSAND (1000)

enum InputDeviceStatus {
    INPUT_DEVICE_STATIS_OPENED = 1,
    INPUT_DEVICE_STATUS_CLOSED,
    INPUT_DEVICE_STATUS_UNKNOWN
};

// manager the device node list
typedef struct {
    uint32_t   index;
    uint32_t   status;
    int32_t    fd;
    char       devPathNode[DEV_NAME_LEN];
    DeviceInfo detailInfo;
} InputDevListNode;

class InputDeviceManager {
public:
    InputDeviceManager() = default;
    virtual ~InputDeviceManager()
    {
        inputDevList_.clear();
        reportEventPkgCallback_.clear();
    };
    InputDeviceManager(const InputDeviceManager &other) = delete;
    InputDeviceManager(InputDeviceManager &&other) = delete;
    InputDeviceManager &operator=(const InputDeviceManager &other) = delete;
    InputDeviceManager &operator=(InputDeviceManager &&other) = delete;
    void Init(void);
    vector<string> GetFiles(string path);
    void DoRead(int32_t fd, struct input_event* event, size_t size);
    int32_t OpenInputDevice(string devPath);
    RetStatus CloseInputDevice(string devPath);
    int32_t GetInputDeviceInfo(int32_t fd, DeviceInfo* detailInfo);
    void GetInputDeviceInfoList(int32_t epollFd = 0);
    int32_t DoInputDeviceAction(void);
    int32_t InotifyEventHandler(int32_t epollFd, int32_t notifyFd);
    void RemoveEpoll(int32_t epollFd, int32_t fileFd);
    int32_t AddToEpoll(int32_t epollFd, int32_t fileFd);
    void WorkerThread();
    int32_t FindIndexFromFd(int32_t& fd, uint32_t* index);
    int32_t FindIndexFromDevName(string devName, uint32_t* index);
    void DoWithEventDeviceAdd(int32_t& epollFd, int32_t& fd, string devPath);
    void SendHotPlugEvent(uint32_t& type, uint32_t& index, uint32_t status);
    void DoWithEventDeviceDel(int32_t& epollFd, uint32_t& index);
    // InputManager
    RetStatus ScanDevice(DevDesc *staArr, uint32_t arrLen);
    RetStatus OpenDevice(uint32_t deviceIndex);
    RetStatus CloseDevice(uint32_t deviceIndex);
    int32_t GetDevice(int32_t deviceIndex, DeviceInfo **devInfo);
    int32_t GetDeviceList(uint32_t *devNum, DeviceInfo **deviceList, uint32_t size);
    // InputController
    RetStatus SetPowerStatus(uint32_t devIndex, uint32_t status);
    RetStatus GetPowerStatus(uint32_t devIndex, uint32_t *status);
    RetStatus GetDeviceType(uint32_t devIndex, uint32_t *deviceType);
    RetStatus GetChipInfo(uint32_t devIndex, char *chipInfo, uint32_t length);
    RetStatus GetVendorName(uint32_t devIndex, char *vendorName, uint32_t length);
    RetStatus GetChipName(uint32_t devIndex, char *chipName, uint32_t length);
    RetStatus SetGestureMode(uint32_t devIndex, uint32_t gestureMode);
    RetStatus RunCapacitanceTest(uint32_t devIndex, uint32_t testType, char *result, uint32_t length);
    RetStatus RunExtraCommand(uint32_t devIndex, InputExtraCmd *cmd);
    // InputReporter
    RetStatus RegisterReportCallback(uint32_t devIndex, OHOS::sptr<InputReportEventCb> callback);
    RetStatus UnregisterReportCallback(uint32_t devIndex);
    RetStatus RegisterHotPlugCallback(OHOS::sptr<InputReportHostCb> callback);
    RetStatus UnregisterHotPlugCallback(void);
    void dumpInfoList(InputDevListNode in)
    {
        HDF_LOGD("%{public}s index: %{public}u state:%{public}u fd:%{public}d devPathNode:%{public}s", __func__,
                 in.index, in.status, in.fd, in.devPathNode);
        HDF_LOGD("devIndex: %{public}u devType:%{public}u chipInfo:%{public}s "
                 "vendorName: %{public}s chipName: %{public}s attrSet.devName: %s "
                 "attrSet.id.busType: %{public}u attrSet.id.vendor: %{public}u "
                 "attrSet.id.product: %{public}u attrSet.id.version: %{public}u ",
                 in.detailInfo.devIndex, in.detailInfo.devType,
                 in.detailInfo.chipInfo, in.detailInfo.vendorName,
                 in.detailInfo.chipName, in.detailInfo.attrSet.devName,
                 in.detailInfo.attrSet.id.busType, in.detailInfo.attrSet.id.vendor,
                 in.detailInfo.attrSet.id.product, in.detailInfo.attrSet.id.version);
        for (int32_t i = 0; i < ABS_CNT; i++) {
            HDF_LOGD("attrSet.axisInfo.axis: %{public}d attrSet.axisInfo.min: %{public}d "
                     "attrSet.axisInfo.max: %{public}d attrSet.axisInfo.fuzz: %{public}d "
                     "attrSet.axisInfo.flat: %{public}d attrSet.axisInfo.range: %{public}d ",
                     in.detailInfo.attrSet.axisInfo[i].axis, in.detailInfo.attrSet.axisInfo[i].flat,
                     in.detailInfo.attrSet.axisInfo[i].fuzz, in.detailInfo.attrSet.axisInfo[i].max,
                     in.detailInfo.attrSet.axisInfo[i].min, in.detailInfo.attrSet.axisInfo[i].range);
        }
    }
private:
    mutable std::mutex lock_;
    std::map<uint32_t, InputDevListNode> inputDevList_;
    struct epoll_event epollEventList_[EPOLL_MAX_EVENTS] {};
    std::string devPath_ {"/dev/input"};
    uint32_t devIndex_ {1};
    std::map<int32_t, OHOS::sptr<InputReportEventCb>> reportEventPkgCallback_;
    OHOS::sptr<InputReportHostCb> reportHotPlugEventCallback_ {nullptr};
    std::thread thread_ {};
    int32_t mEpollId_ {0};
    int32_t mInotifyId_ {0};
};
}
}
#endif // INPUT_DEVICE_MANAGER_H
