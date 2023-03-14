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

#include "core/components/dialog/action_sheet/action_sheet_component.h"

#include "base/utils/system_properties.h"
#include "core/components/flex/flex_component.h"
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/components/image/image_component.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components_v2/list/list_component.h"
#include "core/components_v2/list/list_item_component.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension SHEET_IMAGE_SIZE = 40.0_vp;
constexpr Dimension SHEET_IMAGE_PADDING = 16.0_vp;
constexpr Dimension SHEET_DIVIDER_WIDTH = 1.0_px;
constexpr Dimension SHEET_LIST_PADDING = 24.0_vp;

} // namespace

void ActionSheetComponent::BuildChild(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return;
    }
    dialogTheme_ = AceType::DynamicCast<DialogTheme>(themeManager->GetTheme(DialogTheme::TypeId()));
    if (!dialogTheme_) {
        return;
    }
    if (!isDeviceTypeSet_) {
        deviceType_ = SystemProperties::GetDeviceType();
    }
    bool isLimit = true;
    auto box = BuildBox(isLimit);
    auto transition = BuildAnimation(box);
    BuildDialogTween(transition, isLimit, margin_);

    auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
    std::list<RefPtr<Component>> columnChildren;
    auto column = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, columnChildren);
    column->SetMainAxisSize(MainAxisSize::MIN);
    BuildTitle(column);
    BuildContent(column);
    BuildMenu(column);
    BuildActions(themeManager, column);
    BuildFocusChild(column, focusCollaboration);
    if (deviceType_ == DeviceType::WATCH) {
        auto scroll = AceType::MakeRefPtr<ScrollComponent>(focusCollaboration);
        box->SetChild(scroll);
    } else {
        box->SetChild(focusCollaboration);
    }
    box->SetTextDirection(GetTextDirection());
}

void ActionSheetComponent::BuildMenu(const RefPtr<ColumnComponent>& column)
{
    auto sheetsInfo = properties_.sheetsInfo;
    if (sheetsInfo.empty()) {
        LOGD("sheet info is empty");
        return;
    }

    auto list = AceType::MakeRefPtr<V2::ListComponent>();
    for (const auto& sheetInfo : sheetsInfo) {
        if (!sheetInfo.IsValid()) {
            continue;
        }
        auto listItem = AceType::MakeRefPtr<V2::ListItemComponent>();
        std::list<RefPtr<Component>> rowChildren;
        auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, rowChildren);
        if (!sheetInfo.icon.empty()) {
            auto image = AceType::MakeRefPtr<ImageComponent>(sheetInfo.icon);
            image->SetWidth(SHEET_IMAGE_SIZE);
            image->SetHeight(SHEET_IMAGE_SIZE);
            auto padding = AceType::MakeRefPtr<PaddingComponent>();
            padding->SetPadding(Edge(SHEET_IMAGE_PADDING));
            padding->SetChild(image);
            row->AppendChild(padding);
        }
        if (!sheetInfo.title.empty()) {
            auto text = AceType::MakeRefPtr<TextComponent>(sheetInfo.title);
            auto style = dialogTheme_->GetContentTextStyle();
            style.SetMaxLines(dialogTheme_->GetTitleMaxLines());
            style.SetTextOverflow(TextOverflow::ELLIPSIS);
            style.SetAdaptTextSize(style.GetFontSize(), dialogTheme_->GetTitleMinFontSize());
            text->SetTextStyle(style);
            text->SetFocusColor(style.GetTextColor());
            row->AppendChild(text);
        }
        auto box = AceType::MakeRefPtr<BoxComponent>();
        box->SetOnClick(sheetInfo.gesture);
        box->SetChild(row);
        listItem->SetChild(box);
        list->AppendChild(listItem);

        auto divider = std::make_unique<V2::ItemDivider>();
        divider->strokeWidth = SHEET_DIVIDER_WIDTH;
        divider->color = Color::GRAY;
        list->SetItemDivider(std::move(divider));
    }
    auto padding = AceType::MakeRefPtr<PaddingComponent>();
    padding->SetPadding(Edge(SHEET_LIST_PADDING));
    padding->SetChild(list);
    auto flexItem = AceType::MakeRefPtr<FlexItemComponent>(0, 1, 0.0, padding);
    column->AppendChild(flexItem);
}

void ActionSheetComponent::BuildContent(const RefPtr<ColumnComponent>& column)
{
    if (!content_) {
        return;
    }
    auto contentPadding = AceType::MakeRefPtr<PaddingComponent>();
    if (contentPadding_ == Edge::NONE) {
        if (!title_) {
            contentPadding_ = actions_.empty() ? dialogTheme_->GetDefaultPadding() : dialogTheme_->GetAdjustPadding();
        } else {
            contentPadding_ =
                actions_.empty() ? dialogTheme_->GetContentDefaultPadding() : dialogTheme_->GetContentAdjustPadding();
        }
    }
    contentPadding->SetPadding(std::move(contentPadding_));
    RefPtr<FlexItemComponent> contentFlex;
    if (deviceType_ == DeviceType::WATCH) {
        contentPadding->SetChild(content_);
        contentFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0.0, contentPadding);
    } else {
        auto scroll = AceType::MakeRefPtr<ScrollComponent>(content_);
        contentPadding->SetChild(scroll);
        if (properties_.sheetsInfo.empty()) {
            contentFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 1, 0.0, contentPadding);
        } else {
            contentFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0.0, contentPadding);
        }
    }
    column->AppendChild(GenerateComposed("dialogContent", contentFlex, true));
}

void ActionSheetComponent::BuildActions(const RefPtr<ThemeManager>& themeManager, const RefPtr<ColumnComponent>& column)
{
    DialogComponent::BuildActions(themeManager, column);
    // Bind success event between button and dialog.
    BackEndEventManager<void(int32_t)>::GetInstance().BindBackendEvent(
        onSuccessId_, [properties = properties_, context = context_](int32_t successType) {
            if (successType == 0) {
                auto func = AceAsyncEvent<void()>::Create(
                    properties.primaryId.IsEmpty() ? properties.secondaryId : properties.primaryId, context);
                if (func) {
                    func();
                }
            } else if (successType == 1) {
                auto func = AceAsyncEvent<void()>::Create(properties.secondaryId, context);
                if (func) {
                    func();
                }
            }
        });
}

} // namespace OHOS::Ace