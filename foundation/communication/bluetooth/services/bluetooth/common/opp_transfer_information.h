/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OPP_TRANSFER_INFORMATION_H
#define OPP_TRANSFER_INFORMATION_H

#include <cstdint>
#include <string>
#include "iosfwd"

namespace OHOS {
namespace bluetooth {
class IOppTransferInformation {
public:
    IOppTransferInformation() = default;
    virtual ~IOppTransferInformation() = default;

    int GetId() const;
    void SetId(int id);
    std::string GetFileName() const;
    void SetFileName(std::string fileName);
    std::string GetFilePath() const;
    void SetFilePath(std::string filePath);
    std::string GetFileType() const;
    void SetFileType(std::string fileType);
    std::string GetDeviceName() const;
    void SetDeviceName(std::string deviceName);
    std::string GetDeviceAddress() const;
    void SetDeviceAddress(std::string deviceAddress);
    int GetDirection() const;
    void SetDirection(int direction);
    int GetStatus() const;
    void SetStatus(int status);
    int GetFailedReason() const;
    void SetFailedReason(int failedReason);
    uint64_t GetTimeStamp() const;
    void SetTimeStamp(uint64_t timeStamp);
    uint64_t GetCurrentBytes() const;
    void SetCurrentBytes(uint64_t currentBytes);
    uint64_t GetTotalBytes() const;
    void SetTotalBytes(uint64_t totalBytes);

private:
    int id_ = -1;
    std::string fileName_;
    std::string filePath_;
    std::string fileType_;
    std::string deviceName_;
    std::string deviceAddress_;
    int direction_ = 0;
    int status_ = 0;
    int failedReason_ = 0;
    uint64_t timeStamp_ = 0;
    uint64_t currentBytes_ = 0;
    uint64_t totalBytes_ = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // OPP_TRANSFER_INFORMATION_H
