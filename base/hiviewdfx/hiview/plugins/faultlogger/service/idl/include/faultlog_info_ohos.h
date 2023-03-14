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
#ifndef FAULTLOG_INFO_OHOS_H
#define FAULTLOG_INFO_OHOS_H

#include <cstdint>
#include <map>
#include <string>

#include "parcel.h"

namespace OHOS {
namespace HiviewDFX {
class FaultLogInfoOhos : public Parcelable {
public:
    int64_t time {0};
    int32_t uid {-1};
    int32_t pid {-1};
    int32_t faultLogType {0};
    // the fd from service will be closed in the application class
    // be careful about it
    int32_t fd {-1};
    std::string module;
    std::string reason;
    std::string summary;
    std::string logPath;
    std::map<std::string, std::string> sectionMaps;

    bool Marshalling(Parcel &parcel) const override;
    static FaultLogInfoOhos* Unmarshalling(Parcel &parcel);

    FaultLogInfoOhos(){};
    virtual ~FaultLogInfoOhos() {};
};
}
} // namespace OHOS
#endif // FAULTLOG_INFO_OHOS_H
