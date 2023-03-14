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

#include "hands_free_unit_calls.h"
#include "string"
#include <ctime>

namespace OHOS {
namespace bluetooth {
HandsFreeUnitCalls::HandsFreeUnitCalls(const std::string &device, int id, int state, const std::string &number,
    bool multiParty, bool outgoing, bool inBandRing)
    : device_(device), id_(id), state_(state), number_(number), uuid_(Uuid::Random()),
    multiParty_(multiParty), outgoing_(outgoing), inBandRing_(inBandRing)
{
    creationTime_ = clock();
}

HandsFreeUnitCalls::HandsFreeUnitCalls(const std::string &device, int id, int state, const std::string &number,
    Uuid &uuid, bool multiParty, bool outgoing, bool inBandRing, long creationTime)
    : device_(device), id_(id), state_(state), number_(number), uuid_(uuid),
    multiParty_(multiParty), outgoing_(outgoing), inBandRing_(inBandRing),
    creationTime_(creationTime)
{
}

const std::string &HandsFreeUnitCalls::GetRemoteDevice() const
{
    return device_;
}

int HandsFreeUnitCalls::GetId() const
{
    return id_;
}

Uuid HandsFreeUnitCalls::GetUuid() const
{
    return uuid_;
}

int HandsFreeUnitCalls::GetState() const
{
    return state_;
}

const std::string &HandsFreeUnitCalls::GetNumber() const
{
    return number_;
}

long HandsFreeUnitCalls::GetCreationTime() const
{
    return creationTime_;
}

bool HandsFreeUnitCalls::IsMultiParty() const
{
    return multiParty_;
}

bool HandsFreeUnitCalls::IsOutgoing() const
{
    return outgoing_;
}

bool HandsFreeUnitCalls::IsInBandRing() const
{
    return inBandRing_;
}

void HandsFreeUnitCalls::SetState(int state)
{
    state_ = state;
}

void HandsFreeUnitCalls::SetNumber(const std::string &number)
{
    number_ = number;
}

void HandsFreeUnitCalls::SetMultiParty(bool multiParty)
{
    multiParty_ = multiParty;
}
}  // namespace bluetooth
}  // namespace OHOS