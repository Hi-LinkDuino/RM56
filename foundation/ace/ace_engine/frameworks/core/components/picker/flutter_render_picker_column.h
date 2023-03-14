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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_FLUTTER_RENDER_PICKER_COLUMN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_FLUTTER_RENDER_PICKER_COLUMN_H

#include "core/components/picker/render_picker_column.h"
#include "core/pipeline/layers/opacity_layer.h"

namespace OHOS::Ace {

class FlutterRenderPickerColumn final : public RenderPickerColumn {
    DECLARE_ACE_TYPE(FlutterRenderPickerColumn, RenderPickerColumn);

public:
    FlutterRenderPickerColumn() = default;
    ~FlutterRenderPickerColumn() override = default;

    RenderLayer GetRenderLayer() override;

private:
    RefPtr<Flutter::OpacityLayer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_FLUTTER_RENDER_PICKER_COLUMN_H
