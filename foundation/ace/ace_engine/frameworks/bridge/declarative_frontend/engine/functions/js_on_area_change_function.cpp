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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_on_area_change_function.h"

#include "base/log/log.h"
#include "base/utils/system_properties.h"

namespace OHOS::Ace::Framework {
namespace {

JSRef<JSObject> CreateAreaObject(const Rect& rect, const Offset& origin)
{
    JSRef<JSObjTemplate> objectTemplate = JSRef<JSObjTemplate>::New();
    JSRef<JSObject> area = objectTemplate->NewInstance();
    JSRef<JSObject> offset = objectTemplate->NewInstance();
    JSRef<JSObject> globalOffset = objectTemplate->NewInstance();
    auto localOffset = rect.GetOffset();
    offset->SetProperty<double>("x", SystemProperties::Px2Vp(localOffset.GetX()));
    offset->SetProperty<double>("y", SystemProperties::Px2Vp(localOffset.GetY()));
    globalOffset->SetProperty<double>("x", SystemProperties::Px2Vp(localOffset.GetX() + origin.GetX()));
    globalOffset->SetProperty<double>("y", SystemProperties::Px2Vp(localOffset.GetY() + origin.GetY()));
    // keep compatible, need remove after
    area->SetPropertyObject("pos", offset);
    area->SetPropertyObject("position", offset);
    // keep compatible, need remove after
    area->SetPropertyObject("globalPos", globalOffset);
    area->SetPropertyObject("globalPosition", globalOffset);
    area->SetProperty<double>("width", SystemProperties::Px2Vp(rect.Width()));
    area->SetProperty<double>("height", SystemProperties::Px2Vp(rect.Height()));
    return area;
}
} // namespace

void JsOnAreaChangeFunction::Execute(
    const Rect& oldRect, const Offset& oldOrigin, const Rect& rect, const Offset& origin)
{
    auto oldArea = CreateAreaObject(oldRect, oldOrigin);
    auto area = CreateAreaObject(rect, origin);
    JSRef<JSVal> params[2];
    params[0] = oldArea;
    params[1] = area;
    JsFunction::ExecuteJS(2, params);
}

} // namespace OHOS::Ace::Framework
