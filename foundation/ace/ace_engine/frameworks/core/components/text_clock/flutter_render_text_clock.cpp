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

#include "core/components/text_clock/flutter_render_text_clock.h"

#include "base/json/json_util.h"
#include "core/components/text/flutter_render_text.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "flutter/common/task_runners.h"

namespace OHOS::Ace {
namespace {
constexpr int32_t INTERVAL_OF_USECOND = 1000000;
constexpr int32_t MICROSECONDS_OF_MILLISECOND = 1000;
} // namespace

void FlutterRenderTextClock::Paint(RenderContext& context, const Offset& offset)
{
    RenderNode::Paint(context, offset);
    if (isStart_) {
        RequestRenderForNextSecond();
    }
}

void FlutterRenderTextClock::RequestRenderForNextSecond()
{
    auto timeOfNow = GetTimeOfNow(hoursWest_);
    auto timeUsec = timeOfNow.timeUsec_;
    // 1 second = 1000 millisecond = 1000000 microsecond.
    // Millisecond is abbreviated as msec. Microsecond is abbreviated as usec.
    // unit of [delayTime] is msec, unit of [tv_usec] is usec
    // when [tv_usec] is 000100, (INTERVAL_OF_USECOND - timeUsec) / MICROSECONDS_OF_MILLISECOND = 999 msec
    // which will cause the delay task still arriving in current second, because 999000 + 000100 = 999100 < 1 second
    // so add an additional millisecond to modify the loss of precision during division
    int32_t delayTime = (INTERVAL_OF_USECOND - timeUsec) / MICROSECONDS_OF_MILLISECOND + 1; // millisecond

    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        return;
    }
    pipelineContext->GetTaskExecutor()->PostDelayedTask(
        [wp = WeakClaim(this)] {
            auto renderTextClock = wp.Upgrade();
            if (!renderTextClock) {
                return;
            }
            if (!renderTextClock->isStart_) {
                return;
            }
            renderTextClock->UpdateTimeTextCallBack();
            bool needRender = !(renderTextClock->GetHidden() || !renderTextClock->GetVisible());
            if (needRender) {
                renderTextClock->MarkNeedRender();
                return;
            }
        },
        TaskExecutor::TaskType::UI, delayTime);
}
} // namespace OHOS::Ace