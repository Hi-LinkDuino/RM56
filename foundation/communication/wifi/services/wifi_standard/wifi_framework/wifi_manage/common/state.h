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

#ifndef OHOS_STATE_H
#define OHOS_STATE_H

#include <iostream>
#include "message_queue.h"

namespace OHOS {
namespace Wifi {
class State {
public:
    /**
     * @Description Construct a new State:: State object.
     *
     * @param name - State name.[in]
     */
    explicit State(const std::string &name);

    /**
     * @Description Destroy the State:: State object.
     *
     */
    virtual ~State();

public:
    virtual void GoInState() = 0;
    virtual void GoOutState() = 0;
    virtual bool ExecuteStateMsg(InternalMessage *msg) = 0;

    /**
     * @Description : Obtains state name.
     *
     * @return std::string
     */
    std::string GetStateName();

private:
    std::string mStateName;
};
}  // namespace Wifi
}  // namespace OHOS
#endif