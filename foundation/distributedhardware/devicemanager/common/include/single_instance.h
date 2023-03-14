/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_DM_SINGLE_INSTANCE_H
#define OHOS_DM_SINGLE_INSTANCE_H

namespace OHOS {
namespace DistributedHardware {
#define DECLARE_SINGLE_INSTANCE_BASE(className)       \
public:                                               \
    static className &GetInstance();                  \
                                                      \
private:                                              \
    className(const className &) = delete;            \
    className &operator=(const className &) = delete; \
    className(className &&) = delete;                 \
    className &operator=(className &&) = delete;

#define DECLARE_SINGLE_INSTANCE(className)  \
    DECLARE_SINGLE_INSTANCE_BASE(className) \
                                            \
private:                                    \
    className() = default;                  \
    ~className() = default;

#define IMPLEMENT_SINGLE_INSTANCE(className)    \
    className &className::GetInstance()         \
    {                                           \
        static auto instance = new className(); \
        return *instance;                       \
    }
}; // namespace DistributedHardware
}; // namespace OHOS
#endif // OHOS_DM_SINGLE_INSTANCE_H
