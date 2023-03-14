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

#include "core/components_v2/inspector/actionsheetdialog_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/dialog/dialog_element.h"
#include "core/components/dialog/render_dialog.h"
#include "core/components/text/text_element.h"
#include "core/components_v2/inspector/utils.h"


namespace OHOS::Ace::V2 {
 
const std::unordered_map<std::string, std::function<std::string(const ActionSheetDialogComposedElement &)>> 
    CREATE_JSON_MAP {
        {"show", [](const ActionSheetDialogComposedElement &inspector) { return inspector.Show(); }}
    };

void ActionSheetDialogComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("show: ").append(Show()));
}

std::unique_ptr<JsonValue> ActionSheetDialogComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto &value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

RefPtr<RenderDialog> ActionSheetDialogComposedElement::GetRenderDialog() const
{
    auto node = GetInspectorNode(DialogElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderDialog>(node);
    }
    return nullptr;
}

std::string ActionSheetDialogComposedElement::Show() const
{
    auto jsonArray = JsonUtil::CreateArray(true);
    auto item = JsonUtil::Create(false);
    item->Put("title", GetTitle().c_str());
    item->Put("message", GetMessage().c_str());
    item->Put("autoCancel", GetAutoCancel().c_str());
    item->Put("confirm", GetConfirm());
    item->Put("alignment", GetAlignment().c_str());
    item->Put("offset", GetDialogOffset());
    item->Put("sheet", GetSheets());
    jsonArray->Put(item);
    return jsonArray->ToString();
}

std::string ActionSheetDialogComposedElement::GetTitle() const
{
    auto renderDialog = GetRenderDialog();
    if (!renderDialog) {
        return "";
    }
    return renderDialog->GetDialogProperties().title;
}

std::string ActionSheetDialogComposedElement::GetMessage() const
{
    auto renderDialog = GetRenderDialog();
    if (!renderDialog) {
        return "";
    }
    return renderDialog->GetDialogProperties().content;
}

std::string ActionSheetDialogComposedElement::GetAutoCancel() const
{
    auto renderDialog = GetRenderDialog();
    if (renderDialog) {
        return ConvertBoolToString(renderDialog->GetDialogProperties().autoCancel);
    }
    return "true";
}

std::unique_ptr<JsonValue> ActionSheetDialogComposedElement::GetConfirm() const
{
    auto renderDialog = GetRenderDialog();
    auto confirm = renderDialog->GetDialogProperties().buttons;
    auto jsonArray = JsonUtil::CreateArray(false);
    if (confirm.empty()) {
        for (const auto &dialog : confirm) {
            auto item = JsonUtil::Create(false);
            item->Put("value", dialog.text.c_str());
            item->Put("fontColor", dialog.textColor.c_str());
            item->Put("backgroundColor", ConvertColorToString(dialog.bgColor).c_str());
            jsonArray->Put(item);
        }
        return jsonArray;
    }
    return jsonArray;
}

std::string ActionSheetDialogComposedElement::GetAlignment() const
{
    auto renderDialog = GetRenderDialog();
    auto Alignment = renderDialog ? renderDialog->GetDialogProperties().alignment : DialogAlignment::DEFAULT;
    return ConverDialogAlignmentToString(Alignment);
}

std::unique_ptr<JsonValue> ActionSheetDialogComposedElement::GetDialogOffset() const
{
    auto renderDialog = GetRenderDialog();
    auto dialogoffset = renderDialog->GetDialogProperties().offset;
    auto jsonValue = JsonUtil::Create(false);
    jsonValue->Put("dX", dialogoffset.GetX().Value());
    jsonValue->Put("dY", dialogoffset.GetY().Value());
    return jsonValue;
}

std::string ActionSheetDialogComposedElement::ConverDialogAlignmentToString(DialogAlignment dialogAlignment) const
{
    std::string Alignment = "";
    switch (dialogAlignment) {
        case DialogAlignment::TOP:
            Alignment = "DialogAlignment.TOP";
            break;
        case DialogAlignment::CENTER:
            Alignment = "DialogAlignment.CENTER";
            break;
        case DialogAlignment::BOTTOM:
            Alignment = "DialogAlignment.BOTTOM";
            break;
        case DialogAlignment::TOP_START:
            Alignment = "DialogAlignment.TOP_START";
            break;
        case DialogAlignment::TOP_END:
            Alignment = "DialogAlignment.TOP_END";
            break;
        case DialogAlignment::CENTER_START:
            Alignment = "DialogAlignment.CENTER_START";
            break;
        case DialogAlignment::CENTER_END:
            Alignment = "DialogAlignment.CENTER_END";
            break;
        case DialogAlignment::BOTTOM_START:
            Alignment = "DialogAlignment.BOTTOM_START";
            break;
        case DialogAlignment::BOTTOM_END:
            Alignment = "DialogAlignment.BOTTOM_END";
            break;
        default:
            Alignment = "DialogAlignment.DEFAULT";
    }
    return Alignment;
}

std::unique_ptr<JsonValue> ActionSheetDialogComposedElement::GetSheets() const
{
    auto renderDialog = GetRenderDialog();
    auto actionsheet = renderDialog->GetDialogProperties().sheetsInfo;
    auto jsonArray = JsonUtil::CreateArray(false);
    if (actionsheet.empty()) {
        for (const auto &sheet : actionsheet) {
            auto item = JsonUtil::Create(false);
            item->Put("title", sheet.title.c_str());
            item->Put("icon", sheet.icon.c_str());
            jsonArray->Put(item);
        }
        return jsonArray;
    }
    return jsonArray;
}

} // namespace OHOS::Ace::V2