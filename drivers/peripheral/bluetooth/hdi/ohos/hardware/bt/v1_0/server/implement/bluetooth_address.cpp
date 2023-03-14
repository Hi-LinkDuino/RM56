/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "bluetooth_address.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <hdf_log.h>
#include <unistd.h>
#include "securec.h"

namespace OHOS {
namespace HDI {
namespace BT {
namespace V1_0 {
namespace {
constexpr int ADDRESS_STR_LEN = 17;
constexpr int ADDRESS_SIZE = 6;
}  // namespace

BluetoothAddress::BluetoothAddress()
{
    address_.resize(ADDRESS_SIZE);
}

int BluetoothAddress::ParseAddressFromString(const std::string &string)
{
    size_t offset = 0;
    int bytesIndex = 0;
    int readCount = 0;
    for (bytesIndex = 0; bytesIndex < ADDRESS_SIZE && offset < string.size(); bytesIndex++) {
        readCount = 0;
        if (sscanf_s(&string[offset], "%02hhx:%n", &address_[bytesIndex], &readCount) > 0) {
            if (readCount == 0 && bytesIndex != ADDRESS_SIZE - 1) {
                return bytesIndex;
            }
            offset += readCount;
        } else {
            break;
        }
    }

    return bytesIndex;
}

std::shared_ptr<BluetoothAddress> BluetoothAddress::GetDeviceAddress(const std::string &path)
{
    int addr_fd = open(path.c_str(), O_RDONLY);
    if (addr_fd < 0) {
        HDF_LOGI("open %{public}s failed err:%{public}s.", path.c_str(), strerror(errno));
        char addressStr[ADDRESS_STR_LEN + 1] = {"00:11:22:33:44:55"};
        auto ptr = std::make_shared<BluetoothAddress>();
        if (ptr->ParseAddressFromString(addressStr) != ADDRESS_SIZE) {
            return nullptr;
        }
        return ptr;
    }

    char addressStr[ADDRESS_STR_LEN + 1] = {0};
    if (read(addr_fd, addressStr, ADDRESS_STR_LEN) != ADDRESS_STR_LEN) {
        HDF_LOGE("read %s failed.", path.c_str());
        close(addr_fd);
        return nullptr;
    }
    close(addr_fd);

    auto ptr = std::make_shared<BluetoothAddress>();
    if (ptr->ParseAddressFromString(addressStr) != ADDRESS_SIZE) {
        return nullptr;
    }

    return ptr;
}

std::shared_ptr<BluetoothAddress> BluetoothAddress::GenerateDeviceAddress(const std::string &prefix)
{
    auto ptr = std::make_shared<BluetoothAddress>();
    int prefixCount = ptr->ParseAddressFromString(prefix);
    if (prefixCount < ADDRESS_SIZE) {
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd < 0) {
            HDF_LOGE("open /dev/urandom failed err:%s.", strerror(errno));
            return nullptr;
        }

        if (read(fd, &ptr->address_[prefixCount], ADDRESS_SIZE - prefixCount) != ADDRESS_SIZE - prefixCount) {
            HDF_LOGE("read /dev/urandom failed.");
            close(fd);
            return nullptr;
        }
        close(fd);
    }

    return ptr;
}

void BluetoothAddress::ReadAddress(std::vector<uint8_t> &address) const
{
    address = address_;
}

void BluetoothAddress::ReadAddress(std::string &address) const
{
    address.resize(ADDRESS_STR_LEN + 1);

    int offset = 0;
    for (int ii = 0; ii < ADDRESS_SIZE; ii++) {
        int ret = snprintf_s(
            &address[offset], (ADDRESS_STR_LEN + 1) - offset, ADDRESS_STR_LEN - offset, "%02x:", address_[ii]);
        if (ret < 0) {
            break;
        }
        offset += ret;
    }
}
}  // namespace V1_0
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS