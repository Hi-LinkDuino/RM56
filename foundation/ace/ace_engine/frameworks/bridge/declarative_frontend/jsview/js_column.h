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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_COLUMN_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_COLUMN_H

#include "frameworks/bridge/declarative_frontend/jsview/js_flex.h"

namespace OHOS::Ace::Framework {

class JSColumn : public JSFlex {
public:
    static void Create(const JSCallbackInfo& info);
    static void CreateWithWrap(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);
    static void SetInspectorTag(const std::string& inspectorTag);
    static void ClearInspectorTag();
private:
    static std::string inspectorTag_;
};

class HorizontalAlignDeclaration : public AlignDeclaration {
    DECLARE_ACE_TYPE(HorizontalAlignDeclaration, AlignDeclaration);

public:
    explicit HorizontalAlignDeclaration(HorizontalAlign align) : align_(align) {}
    ~HorizontalAlignDeclaration() override = default;

    static void ConstructorCallback(const JSCallbackInfo& args);
    static void DestructorCallback(HorizontalAlignDeclaration* obj);

    HorizontalAlign GetHorizontalAlign() const override
    {
        return align_;
    }

    DeclarationType GetDeclarationType() override
    {
        return DeclarationType::HORIZONTAL;
    }

private:
    HorizontalAlignDeclaration() = default;
    HorizontalAlign align_ { HorizontalAlign::CENTER };
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_COLUMN_H
