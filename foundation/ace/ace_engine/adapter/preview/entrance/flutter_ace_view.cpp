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

#include "adapter/preview/entrance/flutter_ace_view.h"

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "core/common/ace_engine.h"
#include "core/components/theme/theme_manager.h"
#include "core/event/mouse_event.h"
#include "core/event/touch_event.h"
#include "core/image/image_cache.h"
#include "core/pipeline/layers/flutter_scene_builder.h"

namespace OHOS::Ace::Platform {
namespace {
constexpr int32_t DEFAULT_ACTION_ID = 0;

TouchPoint ConvertTouchPoint(flutter::PointerData* pointerItem)
{
    TouchPoint touchPoint;
    // just get the max of width and height
    touchPoint.size = pointerItem->size; 
    touchPoint.id = pointerItem->device;
    touchPoint.force = pointerItem->pressure;
    touchPoint.x = pointerItem->physical_x;
    touchPoint.y = pointerItem->physical_y;
    touchPoint.screenX = pointerItem->physical_x;
    touchPoint.screenY = pointerItem->physical_y;
    return touchPoint;
}

void ConvertTouchEvent(const std::vector<uint8_t>& data, std::vector<TouchEvent>& events)
{
    const auto* origin = reinterpret_cast<const flutter::PointerData*>(data.data());
    size_t size = data.size() / sizeof(flutter::PointerData);
    auto current = const_cast<flutter::PointerData*>(origin);
    auto end = current + size;

    while (current < end) {
        std::chrono::microseconds micros(current->time_stamp);
        TimeStamp time(micros);
        TouchEvent point {
            static_cast<int32_t>(DEFAULT_ACTION_ID), static_cast<float>(current->physical_x),
            static_cast<float>(current->physical_y), static_cast<float>(current->physical_x),
            static_cast<float>(current->physical_y), TouchType::UNKNOWN, time, current->size,
            static_cast<float>(current->pressure), static_cast<int64_t>(current->device)
        };
        point.pointers.emplace_back(ConvertTouchPoint(current));
        switch (current->change) {
            case flutter::PointerData::Change::kCancel:
                point.type = TouchType::CANCEL;
                events.push_back(point);
                break;
            case flutter::PointerData::Change::kAdd:
            case flutter::PointerData::Change::kRemove:
            case flutter::PointerData::Change::kHover:
                break;
            case flutter::PointerData::Change::kDown:
                point.type = TouchType::DOWN;
                events.push_back(point);
                break;
            case flutter::PointerData::Change::kMove:
                point.type = TouchType::MOVE;
                events.push_back(point);
                break;
            case flutter::PointerData::Change::kUp:
                point.type = TouchType::UP;
                events.push_back(point);
                break;
        }
        current++;
    }
}

} // namespace

void FlutterAceView::RegisterTouchEventCallback(TouchEventCallback&& callback)
{
    ACE_DCHECK(callback);
    touchEventCallback_ = std::move(callback);
}

void FlutterAceView::RegisterKeyEventCallback(KeyEventCallback&& callback)
{
    ACE_DCHECK(callback);
    keyEventCallback_ = std::move(callback);
}

void FlutterAceView::RegisterMouseEventCallback(MouseEventCallback&& callback)
{
    ACE_DCHECK(callback);
    mouseEventCallback_ = std::move(callback);
}

void FlutterAceView::RegisterAxisEventCallback(AxisEventCallback&& callback)
{
    ACE_DCHECK(callback);
    axisEventCallback_ = std::move(callback);
}

void FlutterAceView::RegisterRotationEventCallback(RotationEventCallBack&& callback)
{
    ACE_DCHECK(callback);
    rotationEventCallBack_ = std::move(callback);
}

void FlutterAceView::Launch()
{
}

bool FlutterAceView::Dump(const std::vector<std::string>& params)
{
    return false;
}

void FlutterAceView::ProcessIdleEvent(int64_t deadline)
{
    if (idleCallback_) {
        idleCallback_(deadline);
    }
}

bool FlutterAceView::HandleTouchEvent(const std::vector<uint8_t>& data)
{
    std::vector<TouchEvent> touchEvents;
    ConvertTouchEvent(data, touchEvents);
    for (const auto& point : touchEvents) {
        LOGD("HandleTouchEvent point.x: %lf, point.y: %lf, point.size: %lf", point.x, point.y, point.size);
        if (point.type == TouchType::UNKNOWN) {
            LOGW("Unknown event.");
            continue;
        }
        if (touchEventCallback_) {
            touchEventCallback_(point);
        }
    }

    return true;
}

bool FlutterAceView::HandleTouchEvent(const TouchEvent& touchEvent)
{
    if (touchEvent.type == TouchType::UNKNOWN) {
        LOGW("Unknown event.");
        return false;
    }

    LOGD("HandleTouchEvent touchEvent.x: %lf, touchEvent.y: %lf, touchEvent.size: %lf",
        touchEvent.x, touchEvent.y, touchEvent.size);
    auto event = touchEvent.UpdatePointers();
    if (touchEventCallback_) {
        touchEventCallback_(event);
    }

    return true;
}

std::unique_ptr<DrawDelegate> FlutterAceView::GetDrawDelegate()
{
    auto drawDelegate = std::make_unique<DrawDelegate>();

    drawDelegate->SetDrawFrameCallback([this](RefPtr<Flutter::Layer>& layer, const Rect& dirty) {
        if (!layer) {
            return;
        }
        RefPtr<Flutter::FlutterSceneBuilder> flutterSceneBuilder = AceType::MakeRefPtr<Flutter::FlutterSceneBuilder>();
        layer->AddToScene(*flutterSceneBuilder, 0.0, 0.0);
        auto scene_ = flutterSceneBuilder->Build();
        if (!flutter::UIDartState::Current()) {
            LOGE("uiDartState is nullptr");
            return;
        }
        auto window = flutter::UIDartState::Current()->window();
        if (window != nullptr && window->client() != nullptr) {
            window->client()->Render(scene_.get());
        }
    });

    return drawDelegate;
}

std::unique_ptr<PlatformWindow> FlutterAceView::GetPlatformWindow()
{
    return nullptr;
}

const void* FlutterAceView::GetNativeWindowById(uint64_t textureId)
{
    return nullptr;
}
} // namespace OHOS::Ace::Platform
