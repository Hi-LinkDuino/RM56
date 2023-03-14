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

#include "core/gestures/multi_fingers_recognizer.h"

namespace OHOS::Ace {

void MultiFingersRecognizer::OnAccepted(size_t touchId)
{
    if (!IsInReferee(touchId)) {
        return;
    }

    acceptedCount_++;

    if (refereePointers_.size() == acceptedCount_) {
        OnAccepted();
        refereePointers_.clear();
        acceptedCount_ = 0;
    }
}

void MultiFingersRecognizer::OnRejected(size_t touchId)
{
    if (!IsInReferee(touchId)) {
        return;
    }

    OnRejected();
    refereePointers_.clear();
    acceptedCount_ = 0;
}

void MultiFingersRecognizer::Adjudicate(const RefPtr<GestureRecognizer>& recognizer, GestureDisposal disposal)
{
    if (refereePointers_.empty()) {
        return;
    }

    std::set<size_t> copyIds = refereePointers_;
    GestureRecognizer::BatchAdjudicate(copyIds, recognizer, disposal);
}

void MultiFingersRecognizer::AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer)
{
    auto result = refereePointers_.emplace(touchId);
    if (result.second) {
        GestureRecognizer::AddToReferee(touchId, recognizer);
    }
}

void MultiFingersRecognizer::DelFromReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer)
{
    refereePointers_.erase(touchId);
    GestureRecognizer::DelFromReferee(touchId, recognizer);
}

bool MultiFingersRecognizer::IsInReferee(size_t touchId)
{
    return refereePointers_.find(touchId) != refereePointers_.end();
}

void MultiFingersRecognizer::SetFingerList(const std::map<int32_t, TouchEvent> touchPoints,
    const Offset& coordinateOffset, std::list<FingerInfo>& fingerList)
{
    for (auto& point : touchPoints) {
        Offset localLocation = point.second.GetOffset() - coordinateOffset;
        FingerInfo fingerInfo = {point.first, point.second.GetOffset(), localLocation};
        fingerList.emplace_back(fingerInfo);
    }
}

} // namespace OHOS::Ace
