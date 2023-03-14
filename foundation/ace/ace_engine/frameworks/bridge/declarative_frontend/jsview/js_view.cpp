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

#include "frameworks/bridge/declarative_frontend/jsview/js_view.h"

#include "base/log/ace_trace.h"
#include "core/pipeline/base/composed_element.h"
#include "frameworks/bridge/declarative_frontend/engine/js_execution_scope_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_register.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

JSView::JSView(const std::string& viewId, JSRef<JSObject> jsObject, JSRef<JSFunc> jsRenderFunction) : viewId_(viewId)
{
    jsViewFunction_ = AceType::MakeRefPtr<ViewFunctions>(jsObject, jsRenderFunction);
    instanceId_ = Container::CurrentId();
    LOGD("JSView constructor");
}

JSView::~JSView()
{
    LOGD("Destroy");
    jsViewFunction_.Reset();
};

RefPtr<OHOS::Ace::Component> JSView::CreateComponent()
{
    ACE_SCOPED_TRACE("JSView::CreateSpecializedComponent");
    // create component, return new something, need to set proper ID

    std::string key = ViewStackProcessor::GetInstance()->ProcessViewId(viewId_);
    auto composedComponent = AceType::MakeRefPtr<ComposedComponent>(key, "view");

    // add callback for element creation to component, and get pointer reference
    // to the element on creation. When state of this view changes, mark the
    // element to dirty.
    auto renderFunction = [weak = AceType::WeakClaim(this)](const RefPtr<Component>& component) -> RefPtr<Component> {
        auto jsView = weak.Upgrade();
        return jsView ? jsView->InternalRender(component) : nullptr;
    };

    auto elementFunction = [weak = AceType::WeakClaim(this), renderFunction](const RefPtr<ComposedElement>& element) {
        auto jsView = weak.Upgrade();
        if (!jsView) {
            LOGE("the js view is nullptr in element function");
            return;
        }
        if (jsView->element_.Invalid()) {
            ACE_SCORING_EVENT("Component[" + jsView->viewId_ + "].Appear");
            if (jsView->jsViewFunction_) {
                jsView->jsViewFunction_->ExecuteAppear();
            }
        }
        jsView->element_ = element;
        // add render function callback to element. when the element rebuilds due
        // to state update it will call this callback to get the new child component.
        if (element) {
            element->SetRenderFunction(std::move(renderFunction));
            if (jsView->jsViewFunction_ && jsView->jsViewFunction_->HasPageTransition()) {
                auto pageTransitionFunction = [weak]() -> RefPtr<Component> {
                    auto jsView = weak.Upgrade();
                    if (!jsView || !jsView->jsViewFunction_) {
                        return nullptr;
                    }
                    {
                        ACE_SCORING_EVENT("Component[" + jsView->viewId_ + "].Transition");
                        jsView->jsViewFunction_->ExecuteTransition();
                    }
                    return jsView->BuildPageTransitionComponent();
                };
                element->SetPageTransitionFunction(std::move(pageTransitionFunction));
            }
        }
    };

    composedComponent->SetElementFunction(std::move(elementFunction));

    if (IsStatic()) {
        LOGD("will mark composedComponent as static");
        composedComponent->SetStatic();
    }
    return composedComponent;
}

RefPtr<OHOS::Ace::PageTransitionComponent> JSView::BuildPageTransitionComponent()
{
    auto pageTransitionComponent = ViewStackProcessor::GetInstance()->GetPageTransitionComponent();
    ViewStackProcessor::GetInstance()->ClearPageTransitionComponent();
    return pageTransitionComponent;
}

RefPtr<OHOS::Ace::Component> JSView::InternalRender(const RefPtr<Component>& parent)
{
    JAVASCRIPT_EXECUTION_SCOPE_STATIC;
    needsUpdate_ = false;
    if (!jsViewFunction_) {
        LOGE("JSView: InternalRender jsViewFunction_ error");
        return nullptr;
    }
    {
        ACE_SCORING_EVENT("Component[" + viewId_ + "].AboutToRender");
        jsViewFunction_->ExecuteAboutToRender();
    }
    {
        ACE_SCORING_EVENT("Component[" + viewId_ + "].Build");
        jsViewFunction_->ExecuteRender();
    }
    {
        ACE_SCORING_EVENT("Component[" + viewId_ + "].OnRenderDone");
        jsViewFunction_->ExecuteOnRenderDone();
    }
    CleanUpAbandonedChild();
    jsViewFunction_->Destroy(this);
    auto buildComponent = ViewStackProcessor::GetInstance()->Finish();
    return buildComponent;
}

/**
 * marks the JSView's composed component as needing update / rerender
 */
void JSView::MarkNeedUpdate()
{
    ACE_DCHECK((!GetElement().Invalid()));
    ACE_SCOPED_TRACE("JSView::MarkNeedUpdate");

    auto element = GetElement().Upgrade();
    if (element) {
        element->MarkDirty();
    }
    needsUpdate_ = true;
}

void JSView::SyncInstanceId()
{
    restoreInstanceId_ = Container::CurrentId();
    ContainerScope::UpdateCurrent(instanceId_);
}

void JSView::RestoreInstanceId()
{
    ContainerScope::UpdateCurrent(restoreInstanceId_);
}

void JSView::Destroy(JSView* parentCustomView)
{
    LOGD("JSView::Destroy start");
    DestroyChild(parentCustomView);
    {
        ACE_SCORING_EVENT("Component[" + viewId_ + "].Disappear");
        jsViewFunction_->ExecuteDisappear();
    }
    {
        ACE_SCORING_EVENT("Component[" + viewId_ + "].AboutToBeDeleted");
        jsViewFunction_->ExecuteAboutToBeDeleted();
    }
    LOGD("JSView::Destroy end");
}

void JSView::Create(const JSCallbackInfo& info)
{
    if (info[0]->IsObject()) {
        JSRef<JSObject> object = JSRef<JSObject>::Cast(info[0]);
        auto* view = object->Unwrap<JSView>();
        if (view == nullptr) {
            LOGE("JSView is null");
            return;
        }
        ViewStackProcessor::GetInstance()->Push(view->CreateComponent(), true);
    } else {
        LOGE("JSView Object is expected.");
    }
}

void JSView::JSBind(BindingTarget object)
{
    JSClass<JSView>::Declare("NativeView");
    JSClass<JSView>::StaticMethod("create", &JSView::Create);
    JSClass<JSView>::Method("markNeedUpdate", &JSView::MarkNeedUpdate);
    JSClass<JSView>::Method("syncInstanceId", &JSView::SyncInstanceId);
    JSClass<JSView>::Method("restoreInstanceId", &JSView::RestoreInstanceId);
    JSClass<JSView>::Method("needsUpdate", &JSView::NeedsUpdate);
    JSClass<JSView>::Method("markStatic", &JSView::MarkStatic);
    JSClass<JSView>::CustomMethod("findChildById", &JSView::FindChildById);
    JSClass<JSView>::Inherit<JSViewAbstract>();
    JSClass<JSView>::Bind(object, ConstructorCallback, DestructorCallback);
}

void JSView::FindChildById(const JSCallbackInfo& info)
{
    LOGD("JSView::FindChildById");
    if (info[0]->IsNumber() || info[0]->IsString()) {
        std::string viewId = info[0]->ToString();
        info.SetReturnValue(GetChildById(viewId));
    } else {
        LOGE("JSView FindChildById with invalid arguments.");
        JSException::Throw("%s", "JSView FindChildById with invalid arguments.");
    }
}

void JSView::ConstructorCallback(const JSCallbackInfo& info)
{
    JSRef<JSObject> thisObj = info.This();
    JSRef<JSVal> renderFunc = thisObj->GetProperty("render");
    if (!renderFunc->IsFunction()) {
        LOGE("View derived classes must provide render(){...} function");
        JSException::Throw("%s", "View derived classes must provide render(){...} function");
        return;
    }

    int argc = info.Length();
    if (argc > 1 && (info[0]->IsNumber() || info[0]->IsString())) {
        std::string viewId = info[0]->ToString();
        auto instance = AceType::MakeRefPtr<JSView>(viewId, info.This(), JSRef<JSFunc>::Cast(renderFunc));
        auto context = info.GetExecutionContext();
        instance->SetContext(context);
        instance->IncRefCount();
        info.SetReturnValue(AceType::RawPtr(instance));
        if (!info[1]->IsUndefined() && info[1]->IsObject()) {
            JSRef<JSObject> parentObj = JSRef<JSObject>::Cast(info[1]);
            auto* parentView = parentObj->Unwrap<JSView>();
            if (parentView != nullptr) {
                auto id = parentView->AddChildById(viewId, info.This());
                instance->id_ = id;
            }
        }
        LOGD("JSView ConstructorCallback: %{public}s", instance->id_.c_str());
    } else {
        LOGE("JSView creation with invalid arguments.");
        JSException::Throw("%s", "JSView creation with invalid arguments.");
    }
}

void JSView::DestructorCallback(JSView* view)
{
    if (view == nullptr) {
        LOGE("DestructorCallback failed: the view is nullptr");
        return;
    }
    LOGD("JSView(DestructorCallback) start: %{public}s", view->id_.c_str());
    view->DecRefCount();
    LOGD("JSView(DestructorCallback) end");
}

void JSView::DestroyChild(JSView* parentCustomView)
{
    LOGD("JSView::DestroyChild start");
    for (auto&& child : customViewChildren_) {
        auto* view = child.second->Unwrap<JSView>();
        if (view != nullptr) {
            view->Destroy(this);
        }
        child.second.Reset();
    }
    customViewChildren_.clear();
    for (auto&& lazyChild : customViewChildrenWithLazy_) {
        auto* view = lazyChild.second->Unwrap<JSView>();
        if (view != nullptr) {
            view->Destroy(this);
        }
        lazyChild.second.Reset();
    }
    customViewChildrenWithLazy_.clear();
    LOGD("JSView::DestroyChild end");
}

void JSView::CleanUpAbandonedChild()
{
    auto startIter = customViewChildren_.begin();
    auto endIter = customViewChildren_.end();
    std::vector<std::string> removedViewIds;
    while (startIter != endIter) {
        auto found = lastAccessedViewIds_.find(startIter->first);
        if (found == lastAccessedViewIds_.end()) {
            LOGD(" found abandoned view with id %{public}s", startIter->first.c_str());
            removedViewIds.emplace_back(startIter->first);
            auto* view = startIter->second->Unwrap<JSView>();
            if (view != nullptr) {
                view->Destroy(this);
            }
            startIter->second.Reset();
        }
        ++startIter;
    }

    for (auto& viewId : removedViewIds) {
        customViewChildren_.erase(viewId);
    }

    lastAccessedViewIds_.clear();
}

JSRef<JSObject> JSView::GetChildById(const std::string& viewId)
{
    auto id = ViewStackProcessor::GetInstance()->ProcessViewId(viewId);
    auto found = customViewChildren_.find(id);
    if (found != customViewChildren_.end()) {
        ChildAccessedById(id);
        return found->second;
    }
    auto lazyItem = customViewChildrenWithLazy_.find(id);
    if (lazyItem != customViewChildrenWithLazy_.end()) {
        return lazyItem->second;
    }
    return {};
}

std::string JSView::AddChildById(const std::string& viewId, const JSRef<JSObject>& obj)
{
    auto id = ViewStackProcessor::GetInstance()->ProcessViewId(viewId);
    JSView* jsview = nullptr;
    if (isLazyForEachProcessed_) {
        auto result = customViewChildrenWithLazy_.try_emplace(id, obj);
        if (!result.second) {
            jsview = result.first->second->Unwrap<JSView>();
            result.first->second = obj;
        }
        lazyItemGroups_[lazyItemGroupId_].emplace_back(id);
    } else {
        auto result = customViewChildren_.try_emplace(id, obj);
        if (!result.second) {
            jsview = result.first->second->Unwrap<JSView>();
            result.first->second = obj;
        }
        ChildAccessedById(id);
    }
    if (jsview != nullptr) {
        jsview->Destroy(this);
    }
    return id;
}

void JSView::RemoveChildGroupById(const std::string& viewId)
{
    JAVASCRIPT_EXECUTION_SCOPE_STATIC;
    LOGD("RemoveChildGroupById in lazy for each case: %{public}s", viewId.c_str());
    auto iter = lazyItemGroups_.find(viewId);
    if (iter == lazyItemGroups_.end()) {
        LOGI("can not find this groud to delete: %{public}s", viewId.c_str());
        return;
    }
    std::vector<std::string> removedViewIds;
    for (auto&& item : iter->second) {
        auto removeView = customViewChildrenWithLazy_.find(item);
        if (removeView != customViewChildrenWithLazy_.end()) {
            auto* view = removeView->second->Unwrap<JSView>();
            if (view != nullptr) {
                view->Destroy(this);
            }
            removeView->second.Reset();
            removedViewIds.emplace_back(item);
        }
    }

    for (auto&& removeId : removedViewIds) {
        customViewChildrenWithLazy_.erase(removeId);
    }
    lazyItemGroups_.erase(iter);
}

void JSView::ChildAccessedById(const std::string& viewId)
{
    lastAccessedViewIds_.emplace(viewId);
}

} // namespace OHOS::Ace::Framework
