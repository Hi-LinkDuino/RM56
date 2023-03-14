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
 * @file a2dp_sink.h
 *
 * @brief Declares the class of the a2dp sink.
 *
 * @since 6
 */

#ifndef A2DP_SINK_H
#define A2DP_SINK_H

#include "a2dp_profile.h"

namespace OHOS {
namespace bluetooth {
class A2dpSnkProfile : public A2dpProfile {
public:
    /**
     * @brief A function to get the instance.
     *
     * @since 6.0
     */
    static A2dpSnkProfile *GetInstance();

private:
    /**
     * @brief A constructor used to create an <b>A2dpSnkProfile</b> instance.
     *
     * @since 6.0
     */
    A2dpSnkProfile();

    /**
     * @brief A destructor used to create an <b>A2dpSnkProfile</b> instance.
     *
     * @since 6.0
     */
    ~A2dpSnkProfile();
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_SINK_H