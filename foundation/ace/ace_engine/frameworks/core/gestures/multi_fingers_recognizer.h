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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_MULTI_FINGERS_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_MULTI_FINGERS_RECOGNIZER_H

#include <functional>
#include <map>

#include "core/gestures/gesture_recognizer.h"

namespace OHOS::Ace {

// ClickRecognizer identifies only single click events.
// For long press and double click, see: LongPressRecognizer and DoubleClickRecognizer.
class MultiFingersRecognizer : public GestureRecognizer {
    DECLARE_ACE_TYPE(MultiFingersRecognizer, GestureRecognizer);

public:
    MultiFingersRecognizer() = default;
    ~MultiFingersRecognizer() override = default;

    void OnAccepted(size_t touchId) override;
    void OnRejected(size_t touchId) override;
    void AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer) override;
    void DelFromReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer) override;
    bool IsInReferee(size_t touchId);

    void Adjudicate(const RefPtr<GestureRecognizer>& recognizer, GestureDisposal disposal);

    virtual void OnAccepted() {}
    virtual void OnRejected() {}

    std::set<size_t> refereePointers_;

    void SetFingerList(const std::map<int32_t, TouchEvent> touchPoints, const Offset& coordinateOffset,
        std::list<FingerInfo>& fingerList);

private:
    size_t acceptedCount_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_MULTI_FINGERS_RECOGNIZER_H
