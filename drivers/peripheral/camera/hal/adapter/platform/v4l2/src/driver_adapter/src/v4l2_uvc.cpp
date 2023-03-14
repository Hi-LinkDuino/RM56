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

#include "v4l2_uvc.h"
#include "securec.h"
#include "v4l2_control.h"
#include "v4l2_fileformat.h"
#include "v4l2_dev.h"

namespace OHOS::Camera {
HosV4L2UVC::HosV4L2UVC() {}
HosV4L2UVC::~HosV4L2UVC() {}

void HosV4L2UVC::V4L2UvcSearchCapability(const std::string devName, const std::string v4l2Device, bool inOut)
{
    if (devName.length() == 0 || v4l2Device.length() == 0) {
        CAMERA_LOGE("UVC:V4L2UvcSearchCapability devName or v4l2Device is null");
    }

    std::vector<DeviceControl>().swap(control_);
    std::vector<DeviceFormat>().swap(format_);

    if (inOut) {
        char *name = nullptr;
        char absPath[PATH_MAX] = {0};

        name = realpath(v4l2Device.c_str(), absPath);
        if (name == nullptr) {
            CAMERA_LOGE("UVC:V4L2UvcMatchDev realpath error\n");
            return;
        }
        int fd = open(name, O_RDWR | O_NONBLOCK, 0);
        free(name);
        if (fd < 0) {
            CAMERA_LOGE("UVC:V4L2UvcSearchCapability open %s name %s error\n", v4l2Device.c_str(), devName.c_str());
        } else {
            std::shared_ptr<HosFileFormat> fileFormat = nullptr;
            fileFormat = std::make_shared<HosFileFormat>();
            if (fileFormat == nullptr) {
                CAMERA_LOGE("UVC:V4L2UvcMatchDev fileFormat make_shared is NULL\n");
            } else {
                fileFormat->V4L2GetFmtDescs(fd, format_);
            }

            std::shared_ptr<HosV4L2Control> control = nullptr;
            control = std::make_shared<HosV4L2Control>();
            if (control == nullptr) {
                CAMERA_LOGE("UVC:V4L2UvcMatchDev control make_shared is NULL\n");
            } else {
                control->V4L2GetControls(fd, control_);
            }
            close(fd);
        }
    }
}

void HosV4L2UVC::V4L2UvcMatchDev(const std::string name, const std::string v4l2Device, bool inOut)
{
    std::pair<std::map<std::string, std::string>::iterator, bool> iter;
    constexpr uint32_t nameSize = 16;
    int i = 0;
    char devName[nameSize] = {0};

    CAMERA_LOGD("UVC:V4L2UvcMatchDev name %{public}s v4l2Device %{public}s inOut = %{public}d\n",
        name.c_str(), v4l2Device.c_str(), inOut);
    if ((sprintf_s(devName, sizeof(devName), "%s", name.c_str())) < 0) {
        CAMERA_LOGE("%s: sprintf devName failed", __func__);
        return;
    }
    if (inOut) {
        {
            std::lock_guard<std::mutex> l(HosV4L2Dev::deviceFdLock_);
            iter = HosV4L2Dev::deviceMatch.insert(std::make_pair(std::string(devName), v4l2Device));
        }
        if (!iter.second) {
            for (i = 1; i < MAXUVCNODE; i++) {
                if ((sprintf_s(devName, sizeof(devName), "%s%d", devName, i)) < 0) {
                    CAMERA_LOGE("%{public}s: sprintf devName failed", __func__);
                    return;
                }
                {
                    std::lock_guard<std::mutex> l(HosV4L2Dev::deviceFdLock_);
                    iter = HosV4L2Dev::deviceMatch.insert(std::make_pair(std::string(devName), v4l2Device));
                }
                if (iter.second) {
                    CAMERA_LOGD("UVC:V4L2UvcMatchDev::deviceMatch.insert: %{public}s devName %{public}s i %{public}d\n",
                        v4l2Device.c_str(), devName, i);
                    break;
                }
            }
        }
    } else {
        CAMERA_LOGD("UVC: HosV4L2Dev::deviceMatch.erase: %{public}s devName %{public}s\n",
            v4l2Device.c_str(), devName);
        std::lock_guard<std::mutex> l(HosV4L2Dev::deviceFdLock_);
        HosV4L2Dev::deviceMatch.erase(std::string(devName));
    }

    V4L2UvcSearchCapability(std::string(devName), v4l2Device, inOut);

    uvcCallbackFun_(std::string(devName), control_, format_, inOut);
}

RetCode HosV4L2UVC::V4L2UvcGetCap(const std::string v4l2Device, struct v4l2_capability& cap)
{
    int fd, rc;
    char *devName = nullptr;
    char absPath[PATH_MAX] = {0};

    devName = realpath(v4l2Device.c_str(), absPath);
    if (devName == nullptr) {
        CAMERA_LOGE("UVC:V4L2UvcGetCap realpath error v4l2Device == %{public}s\n", v4l2Device.c_str());
        return RC_ERROR;
    }

    fd = open(devName, O_RDWR | O_NONBLOCK, 0);
    free(devName);
    if (fd < 0) {
        CAMERA_LOGE("UVC:ERROR opening V4L2 interface for %{public}s\n", v4l2Device.c_str());
        return RC_ERROR;
    }

    rc = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (rc < 0) {
        CAMERA_LOGE("UVC:%{public}s V4L2EnmeDevices VIDIOC_QUERYCAP erro\n", v4l2Device.c_str());
        close(fd);
        return RC_ERROR;
    }
    close(fd);

    if (!((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) && (cap.capabilities & V4L2_CAP_STREAMING))) {
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode HosV4L2UVC::V4L2UVCGetCapability(int fd, const std::string devName, std::string& cameraId)
{
    struct v4l2_capability capability = {};
    constexpr uint32_t removeCount = 5;

    int rc = ioctl(fd, VIDIOC_QUERYCAP, &capability);
    if (rc < 0) {
        return RC_ERROR;
    }

    if (!((capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) && (capability.capabilities & V4L2_CAP_STREAMING))) {
        return RC_ERROR;
    }

    if (cameraId != std::string((char*)capability.driver)) {
        return RC_ERROR;
    }
    V4L2UvcMatchDev(std::string((char*)capability.driver), devName, true);

    CAMERA_LOGD("UVC:v4l2 driver name = %{public}s\n", capability.driver);
    CAMERA_LOGD("UVC:v4l2 capabilities = 0x{public}%x\n", capability.capabilities);
    CAMERA_LOGD("UVC:v4l2 card: %{public}s\n", capability.card);
    CAMERA_LOGD("UVC:v4l2 bus info: %{public}s\n", capability.bus_info);

    return RC_OK;
}

void HosV4L2UVC::V4L2UvcEnmeDevices()
{
    struct stat sta = {};
    std::string name = DEVICENAMEX;
    char devName[16] = {0};
    std::string cameraId = "uvcvideo";
    int rc = 0;
    int fd = 0;

    for (int j = 0; j < MAXVIDEODEVICE; ++j) {
        if ((sprintf_s(devName, sizeof(devName), "%s%d", name.c_str(), j)) < 0) {
            CAMERA_LOGE("%{public}s: sprintf devName failed", __func__);
            return;
        }

        if (stat(devName, &sta) != 0) {
            continue;
        }

        if (!S_ISCHR(sta.st_mode)) {
            continue;
        }

        fd = open(devName, O_RDWR | O_NONBLOCK, 0);
        if (fd == -1) {
            continue;
        }

        rc = V4L2UVCGetCapability(fd, devName, cameraId);
        if (rc == RC_ERROR) {
            close(fd);
            continue;
        }

        close(fd);
        break;
    }
}

const char* HosV4L2UVC::V4L2GetUsbValue(const char* key, const char* str, int len)
{
    if (key == nullptr || str == nullptr || len <= 0 || strlen(key) > len) {
        return nullptr;
    }

    const char* pos = strstr(str, key);
    if (pos == nullptr) {
        return nullptr;
    }

    if (pos + strlen(key) - str > len) {
        return nullptr;
    }
    return pos + strlen(key);
}

void HosV4L2UVC::V4L2GetUsbString(std::string& action, std::string& subsystem,
    std::string& devnode, char* buf, unsigned int len)
{
    int lineLen;
    int pos = 0;
    const char* retVal;

    CAMERA_LOGD("UVC:V4L2GetUsbString enter\n");

    lineLen = strlen(buf);
    while (pos + lineLen < len && lineLen) {
        if (action == "") {
            retVal = V4L2GetUsbValue("ACTION=", buf + pos, lineLen);
            if (retVal == nullptr) {
                action = "";
            } else {
                action = std::string(retVal);
                CAMERA_LOGD("UVC:V4L2GetUsbString action %{public}s\n", action.c_str());
            }
        }

        if (subsystem == "") {
            retVal = V4L2GetUsbValue("SUBSYSTEM=", buf + pos, lineLen);
            if (retVal == nullptr) {
                subsystem = "";
            } else {
                subsystem = std::string(retVal);
                CAMERA_LOGD("UVC:V4L2GetUsbString subsystem %{public}s\n", subsystem.c_str());
            }
        }

        if (devnode == "") {
            retVal = V4L2GetUsbValue("DEVNAME=", buf + pos, lineLen);
            if (retVal == nullptr) {
                devnode = "";
            } else {
                devnode = std::string(retVal);
                CAMERA_LOGD("UVC:V4L2GetUsbString devnode %{public}s\n", devnode.c_str());
            }
        }

        pos += lineLen + 1;
        lineLen = strlen(buf + pos);
    }

    CAMERA_LOGD("UVC:V4L2GetUsbString exit\n");
}

void HosV4L2UVC::loopUvcDevice()
{
    fd_set fds;
    int rc;
    constexpr uint32_t delayTime = 200000;
    CAMERA_LOGD("UVC:loopUVCDevice fd = %{public}d getuid() = %{public}d\n", uDevFd_, getuid());
    V4L2UvcEnmeDevices();

    FD_ZERO(&fds);
    FD_SET(uDevFd_, &fds);
    FD_SET(eventFd_, &fds);
    while (uvcDetectEnable_) {
        rc = select(((uDevFd_ > eventFd_) ? uDevFd_ : eventFd_) + 1, &fds, &fds, NULL, NULL);
        if (rc > 0 && FD_ISSET(uDevFd_, &fds)) {
            usleep(delayTime);
            constexpr uint32_t buffSize = 4096;
            char buf[buffSize] = {};
            unsigned int len = recv(uDevFd_, buf, sizeof(buf), 0);
            if (len > 0 && (strstr(buf, "video4linux") != nullptr)) {
                std::string action = "";
                std::string subsystem = "";
                std::string devnode = "";
                V4L2GetUsbString(action, subsystem, devnode, buf, len);
                if (subsystem == "video4linux") {
                    CAMERA_LOGD("UVC:ACTION = %{public}s, SUBSYSTEM = %{public}s, DEVNAME = %{public}s\n",
                                action.c_str(), subsystem.c_str(), devnode.c_str());
                    if (action == "remove") {
                        for (auto &itr : HosV4L2Dev::deviceMatch) {
                            std::string devName = {};
                            devName = "/dev/" + devnode;
                            if (itr.second == devName) {
                                CAMERA_LOGD("UVC:loop HosV4L2Dev::deviceMatch %{public}s\n", action.c_str());
                                V4L2UvcMatchDev(itr.first, devName, false);
                                break;
                            }
                        }
                    } else {
                        struct v4l2_capability cap = {};
                        std::string devName = {};
                        devName = "/dev/" + devnode;
                        rc = V4L2UvcGetCap(devName, cap);
                        if (rc == RC_ERROR) {
                            CAMERA_LOGE("UVC:lop V4L2UvcGetCap err rc %d devnode = %{public}s\n", rc, devnode.c_str());
                            continue;
                        }
                        CAMERA_LOGD("UVC:loop HosV4L2Dev::deviceMatch %{public}s\n", action.c_str());
                        V4L2UvcMatchDev(std::string((char*)cap.driver), devName, true);
                    }
                }
            }
        } else
            CAMERA_LOGD("UVC:No Device from udev_monitor_receive_device() or exit uvcDetectEnable_ = %{public}d\n",
                uvcDetectEnable_);
    }
}

void HosV4L2UVC::V4L2UvcDetectUnInit()
{
    int rc;
    constexpr uint32_t delayTime = 300000;

    uvcDetectEnable_ = 0;

    CAMERA_LOGD("UVC:loop V4L2UvcDetectUnInit\n");

    uint64_t one = 1;
    rc = write(eventFd_, &one, sizeof(one));
    if (rc < 0) {
        usleep(delayTime);
        rc = write(eventFd_, &one, sizeof(one));
    }

    uvcDetectThread_->join();
    close(uDevFd_);
    close(eventFd_);

    delete uvcDetectThread_;
    uvcDetectThread_ = nullptr;
}

RetCode HosV4L2UVC::V4L2UvcDetectInit(UvcCallback cb)
{
    int rc;
    struct sockaddr_nl nls;

    CAMERA_LOGD("UVC:V4L2Detect enter\n");

    if (cb == nullptr || uvcDetectEnable_) {
        CAMERA_LOGE("UVC:V4L2Detect is on or UvcCallback is NULL\n");
        return RC_ERROR;
    }
    // set callback
    uvcCallbackFun_ = cb;

    uDevFd_ = socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (uDevFd_ < 0) {
        CAMERA_LOGE("UVC:V4L2Detect socket() error\n");
        return RC_ERROR;
    }

    memset_s(&nls, sizeof(nls), 0, sizeof(nls));
    nls.nl_family = AF_NETLINK;
    nls.nl_pid = getpid();
    nls.nl_groups = 1;
    rc = bind(uDevFd_, (struct sockaddr *)&nls, sizeof(nls));
    if (rc < 0) {
        CAMERA_LOGE("UVC:V4L2Detect bind() error\n");
        goto error;
    }

    eventFd_ = eventfd(0, 0);
    if (eventFd_ < 0) {
        CAMERA_LOGE("UVC:V4L2Detect eventfd error\n");
        goto error;
    }

    uvcDetectEnable_ = 1;
    uvcDetectThread_ = new (std::nothrow) std::thread(&HosV4L2UVC::loopUvcDevice, this);
    if (uvcDetectThread_ == nullptr) {
        uvcDetectEnable_ = 0;
        CAMERA_LOGE("UVC:V4L2Detect creat loopUVCDevice thread error\n");
        goto error1;
    }

    return RC_OK;

error1:
    close (eventFd_);
    uvcCallbackFun_ = nullptr;
error:
    close (uDevFd_);

    return RC_ERROR;
}
} // namespace OHOS::Camera
