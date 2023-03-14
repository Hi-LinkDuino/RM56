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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_DIALOG_CUSTOM_DIALOG_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_DIALOG_CUSTOM_DIALOG_COMPONENT_H

#include <functional>

#include "base/geometry/dimension.h"
#include "core/components/common/properties/edge.h"
#include "core/event/ace_event_handler.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

using BuildDialogImpl = std::function<void()>;

// CustomDialogController is used in CustomDialogElement to build a dialog
class CustomDialogController : public virtual AceType {
    DECLARE_ACE_TYPE(CustomDialogController, AceType);

public:
    void ShowDialog()
    {
        if (showDialogImpl_) {
            showDialogImpl_();
        }
    }

    void CloseDialog()
    {
        if (closeDialogImpl_) {
            closeDialogImpl_();
        }
    }

    void SetShowDialogImpl(const BuildDialogImpl& showDialogImpl)
    {
        showDialogImpl_ = showDialogImpl;
    }

    void SetCloseDialogImpl(const BuildDialogImpl& closeDialogImpl)
    {
        closeDialogImpl_ = closeDialogImpl;
    }

private:
    BuildDialogImpl showDialogImpl_;
    BuildDialogImpl closeDialogImpl_;
};

class CustomDialogComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(CustomDialogComponent, ComposedComponent);

public:
    CustomDialogComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : ComposedComponent(id, name, child)
    {
        dialogController_ = AceType::MakeRefPtr<CustomDialogController>();
    };

    CustomDialogComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name)
    {
        dialogController_ = AceType::MakeRefPtr<CustomDialogController>();
    };

    ~CustomDialogComponent() override = default;

    RefPtr<Element> CreateElement() override;

    const RefPtr<CustomDialogController>& GetDialogController() const
    {
        return dialogController_;
    }

    const EventMarker& GetOnCancel() const
    {
        return onCancel_;
    }

    void SetOnCancel(const EventMarker& value)
    {
        onCancel_ = value;
    }

    const EventMarker& GetOnShow() const
    {
        return onShow_;
    }

    void SetOnShow(const EventMarker& value)
    {
        onShow_ = value;
    }

    const EventMarker& GetOnClose() const
    {
        return onClose_;
    }

    void SetOnClose(const EventMarker& value)
    {
        onClose_ = value;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    void SetMargin(const Edge& margin)
    {
        margin_ = margin;
        isSetMargin_ = true;
    }

    const Edge& GetMargin() const
    {
        return margin_;
    }

    bool IsSetMargin() const
    {
        return isSetMargin_;
    }

    void SetIsDragable(bool isDragable)
    {
        isDragable_ = isDragable;
    }

    bool IsDragable() const
    {
        return isDragable_;
    }

private:
    RefPtr<CustomDialogController> dialogController_;
    EventMarker onCancel_;
    EventMarker onShow_;
    EventMarker onClose_;
    // initialize height and width to default case
    Dimension height_;
    Dimension width_;
    Edge margin_;
    bool isSetMargin_ = false;
    bool isDragable_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_DIALOG_CUSTOM_DIALOG_COMPONENT_H
