/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

/**
 * @file a2dp_snk_service.h
 *
 * @brief Declares the class of the a2dp profile, including attributes, and methods.
 *
 * @since 6
 */

#ifndef A2DP_SNK_SERVICE_H
#define A2DP_SNK_SERVICE_H

#include <cstdint>
#include <cstring>

#include "gavdp/a2dp_service.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a2dp sink functions called by Framework API.
 *
 * @since 6.0
 */
class A2dpSnkService : public A2dpService {
public:
    /**
     * @brief A constructor used to create an a2dp sink service instance.
     *
     * @since 6.0
     */
    A2dpSnkService();

    /**
     * @brief A destructor used to delete the a2dp sink service instance.
     *
     * @since 6.0
     */
    ~A2dpSnkService(){};

    /**
     * @brief Get a2dp sink instance.
     *
     * @return Returns an instance of a2dp sink.
     * @since 6.0
     */
    static A2dpSnkService *GetProfile();

private:
    static std::string g_name;     /// service name.
    static std::string g_version;  /// profile version.
    static uint8_t g_role;         // profile role
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_SNK_SERVICE_H