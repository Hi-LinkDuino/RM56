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

#include "frameworks/bridge/declarative_frontend/jsview/js_lazy_foreach.h"

#include <functional>
#include <set>
#include <string>

#include "base/utils/utils.h"
#include "bridge/common/utils/utils.h"
#include "bridge/declarative_frontend/engine/js_object_template.h"
#include "bridge/declarative_frontend/jsview/js_view.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/common/container_scope.h"
#include "core/components_v2/foreach/lazy_foreach_component.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/multi_composed_component.h"

namespace OHOS::Ace::Framework {

// Avoid the problem that clang expands template static variable assignment with
// thread_local keyword in anonymous namespace and does not take effect
class JSDataChangeListener : public Referenced {
public:
    static void JSBind(BindingTarget globalObj)
    {
        JSClass<JSDataChangeListener>::Declare("__ohos_ace_inner_JSDataChangeListener__");
        // API7 onEditChanged deprecated
        JSClass<JSDataChangeListener>::CustomMethod("onDataReloaded", &JSDataChangeListener::OnDataReloaded);
        JSClass<JSDataChangeListener>::CustomMethod("onDataReload", &JSDataChangeListener::OnDataReloaded);
        // API7 onDataAdded deprecated
        JSClass<JSDataChangeListener>::CustomMethod("onDataAdded", &JSDataChangeListener::OnDataAdded);
        JSClass<JSDataChangeListener>::CustomMethod("onDataAdd", &JSDataChangeListener::OnDataAdded);
        // API7 onDataDeleted deprecated
        JSClass<JSDataChangeListener>::CustomMethod("onDataDeleted", &JSDataChangeListener::OnDataDeleted);
        JSClass<JSDataChangeListener>::CustomMethod("onDataDelete", &JSDataChangeListener::OnDataDeleted);
        // API7 onDataChanged deprecated
        JSClass<JSDataChangeListener>::CustomMethod("onDataChanged", &JSDataChangeListener::OnDataChanged);
        JSClass<JSDataChangeListener>::CustomMethod("onDataChange", &JSDataChangeListener::OnDataChanged);
        // API7 onDataMoved deprecated
        JSClass<JSDataChangeListener>::CustomMethod("onDataMoved", &JSDataChangeListener::OnDataMoved);
        JSClass<JSDataChangeListener>::CustomMethod("onDataMove", &JSDataChangeListener::OnDataMoved);
        JSClass<JSDataChangeListener>::Bind(
            globalObj, &JSDataChangeListener::Constructor, &JSDataChangeListener::Destructor);
    }

    void AddListener(const RefPtr<V2::DataChangeListener>& listener)
    {
        listeners_.emplace(listener);
    }

    void RemoveListener(const RefPtr<V2::DataChangeListener>& listener)
    {
        WeakPtr<V2::DataChangeListener> weak = listener;
        listeners_.erase(weak);
    }

private:
    static void Constructor(const JSCallbackInfo& args)
    {
        auto listener = Referenced::MakeRefPtr<JSDataChangeListener>();
        listener->instanceId_ = ContainerScope::CurrentId();
        listener->IncRefCount();
        args.SetReturnValue(Referenced::RawPtr(listener));
    }

    static void Destructor(JSDataChangeListener* listener)
    {
        if (listener != nullptr) {
            listener->DecRefCount();
        }
    }

    void OnDataReloaded(const JSCallbackInfo& args)
    {
        NotifyAll(&V2::DataChangeListener::OnDataReloaded);
    }

    void OnDataAdded(const JSCallbackInfo& args)
    {
        NotifyAll(&V2::DataChangeListener::OnDataAdded, args);
    }

    void OnDataDeleted(const JSCallbackInfo& args)
    {
        NotifyAll(&V2::DataChangeListener::OnDataDeleted, args);
    }

    void OnDataChanged(const JSCallbackInfo& args)
    {
        NotifyAll(&V2::DataChangeListener::OnDataChanged, args);
    }

    void OnDataMoved(const JSCallbackInfo& args)
    {
        ContainerScope scope(instanceId_);
        size_t from = 0;
        size_t to = 0;
        if (args.Length() < 2 || !ConvertFromJSValue(args[0], from) || !ConvertFromJSValue(args[1], to)) {
            return;
        }
        NotifyAll(&V2::DataChangeListener::OnDataMoved, from, to);
    }

    template<class... Args>
    void NotifyAll(void (V2::DataChangeListener::*method)(Args...), const JSCallbackInfo& args)
    {
        ContainerScope scope(instanceId_);
        size_t index = 0;
        if (args.Length() > 0 && ConvertFromJSValue(args[0], index)) {
            NotifyAll(method, index);
        }
    }

    template<class... Args>
    void NotifyAll(void (V2::DataChangeListener::*method)(Args...), Args... args)
    {
        ContainerScope scope(instanceId_);
        for (auto it = listeners_.begin(); it != listeners_.end();) {
            auto listener = it->Upgrade();
            if (!listener) {
                it = listeners_.erase(it);
                continue;
            }
            ++it;
            ((*listener).*method)(args...);
        }
    }

    std::set<WeakPtr<V2::DataChangeListener>> listeners_;
    int32_t instanceId_ = -1;
};

namespace {

enum {
    PARAM_VIEW_ID = 0,
    PARAM_PARENT_VIEW,
    PARAM_DATA_SOURCE,
    PARAM_ITEM_GENERATOR,
    PARAM_KEY_GENERATOR,

    MIN_PARAM_SIZE = PARAM_KEY_GENERATOR,
    MAX_PARAM_SIZE,
};

bool ParseAndVerifyParams(const JSCallbackInfo& info, JSRef<JSVal> (&params)[MAX_PARAM_SIZE])
{
    if (info.Length() < MIN_PARAM_SIZE) {
        return false;
    }

    if (!info[PARAM_VIEW_ID]->IsNumber() && !info[PARAM_VIEW_ID]->IsString()) {
        return false;
    }
    if (!info[PARAM_PARENT_VIEW]->IsObject()) {
        return false;
    }
    if (!info[PARAM_DATA_SOURCE]->IsObject()) {
        return false;
    }
    if (!info[PARAM_ITEM_GENERATOR]->IsFunction()) {
        return false;
    }
    if (info.Length() > MIN_PARAM_SIZE && !info[PARAM_KEY_GENERATOR]->IsFunction()) {
        return false;
    }

    for (int32_t idx = PARAM_VIEW_ID; idx < std::min(info.Length(), static_cast<int32_t>(MAX_PARAM_SIZE)); ++idx) {
        params[idx] = info[idx];
    }
    return true;
}

inline JSRef<JSFunc> GetFunctionFromObject(const JSRef<JSObject>& obj, const char* funcName)
{
    JSRef<JSVal> jsVal = obj->GetProperty(funcName);
    if (jsVal->IsFunction()) {
        return JSRef<JSFunc>::Cast(jsVal);
    }
    return JSRef<JSFunc>();
}

template<class... T>
JSRef<JSVal> CallJSFunction(const JSRef<JSFunc>& func, const JSRef<JSObject>& obj, T&&... args)
{
    JSRef<JSVal> params[] = { ConvertToJSValue(std::forward<T>(args))... };
    return func->Call(obj, ArraySize(params), params);
}
class DefaultDataChangeListener : public V2::DataChangeListener {
public:
    explicit DefaultDataChangeListener(JSView* parentView) : parentView_(parentView) {}
    ~DefaultDataChangeListener() override = default;

    void OnDataReloaded() override
    {
        if (parentView_ != nullptr) {
            parentView_->MarkNeedUpdate();
        }
    }
    void OnDataAdded(size_t index) override
    {
        if (parentView_ != nullptr) {
            parentView_->MarkNeedUpdate();
        }
    }
    void OnDataDeleted(size_t index) override
    {
        if (parentView_ != nullptr) {
            parentView_->MarkNeedUpdate();
        }
    }
    void OnDataChanged(size_t index) override
    {
        if (parentView_ != nullptr) {
            parentView_->MarkNeedUpdate();
        }
    }
    void OnDataMoved(size_t from, size_t to) override
    {
        if (parentView_ != nullptr) {
            parentView_->MarkNeedUpdate();
        }
    }

private:
    JSView* parentView_ = nullptr;

    ACE_DISALLOW_COPY_AND_MOVE(DefaultDataChangeListener);
};

using ItemKeyGenerator = std::function<std::string(const JSRef<JSVal>&, size_t)>;

class JSLazyForEachComponent : public V2::LazyForEachComponent {
    DECLARE_ACE_TYPE(JSLazyForEachComponent, V2::LazyForEachComponent);

public:
    explicit JSLazyForEachComponent(const std::string& id) : V2::LazyForEachComponent(id) {}
    ~JSLazyForEachComponent() override
    {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_);
        JSRef<JSObject> listenerObj = listenerProxyObj_.Lock();
        if (listenerObj.IsEmpty() || unregisterListenerFunc_.IsEmpty()) {
            return;
        }

        JSRef<JSVal> args[] = { listenerObj };
        unregisterListenerFunc_->Call(dataSourceObj_, ArraySize(args), args);
    }

    size_t OnGetTotalCount() override
    {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_, 0);
        if (totalCountFunc_.IsEmpty()) {
            return 0;
        }

        int64_t value = 0;
        if (!ConvertFromJSValue(totalCountFunc_->Call(dataSourceObj_), value)) {
            return 0;
        }
        if (value < 0 || static_cast<uint64_t>(value) >= std::numeric_limits<size_t>::max() - 1) {
            return 0;
        }
        return static_cast<size_t>(value);
    }

    RefPtr<Component> OnGetChildByIndex(size_t index) override
    {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_, nullptr);
        if (getDataFunc_.IsEmpty()) {
            return nullptr;
        }

        JSRef<JSVal> result = CallJSFunction(getDataFunc_, dataSourceObj_, index);
        std::string key = keyGenFunc_(result, index);

        ScopedViewStackProcessor scopedViewStackProcessor;
        auto viewStack = ViewStackProcessor::GetInstance();
        auto multiComposed = AceType::MakeRefPtr<MultiComposedComponent>(key, "LazyForEach");
        viewStack->Push(multiComposed);
        if (parentView_) {
            parentView_->MarkLazyForEachProcess(key);
        }
        viewStack->PushKey(key);
        itemGenFunc_->Call(JSRef<JSObject>(), 1, &result);
        viewStack->PopKey();
        if (parentView_) {
            parentView_->ResetLazyForEachProcess();
        }
        auto component = viewStack->Finish();
        ACE_DCHECK(multiComposed == component);

        while (multiComposed) {
            const auto& children = multiComposed->GetChildren();
            if (children.empty()) {
                return AceType::MakeRefPtr<ComposedComponent>(key, "LazyForEachItem");
            }

            component = children.front();
            multiComposed = AceType::DynamicCast<MultiComposedComponent>(component);
        }

        return AceType::MakeRefPtr<ComposedComponent>(key, "LazyForEachItem", component);
    }

    void RegisterDataChangeListener(const RefPtr<V2::DataChangeListener>& listener) override
    {
        if (!listener) {
            return;
        }

        auto listenerProxy = listenerProxy_.Upgrade();
        if (listenerProxy) {
            listenerProxy->AddListener(listener);
            return;
        }

        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_);
        if (registerListenerFunc_.IsEmpty()) {
            return;
        }

        JSRef<JSObject> listenerObj = JSClass<JSDataChangeListener>::NewInstance();
        auto unwrapObj = listenerObj->Unwrap<JSDataChangeListener>();
        if (unwrapObj == nullptr) {
            return;
        }
        listenerProxy = Referenced::Claim(unwrapObj);
        listenerProxy->AddListener(listener);
        listenerProxyObj_ = listenerObj;
        listenerProxy_ = listenerProxy;

        JSRef<JSVal> args[] = { listenerObj };
        registerListenerFunc_->Call(dataSourceObj_, ArraySize(args), args);
    }

    void UnregisterDataChangeListener(const RefPtr<V2::DataChangeListener>& listener) override
    {
        if (!listener) {
            return;
        }

        auto listenerProxy = listenerProxy_.Upgrade();
        if (listenerProxy) {
            listenerProxy->RemoveListener(listener);
        }
    }

    void SetJSExecutionContext(const JSExecutionContext& context)
    {
        context_ = context;
    }

    void SetParentViewObj(const JSRef<JSObject>& parentViewObj)
    {
        parentView_ = parentViewObj->Unwrap<JSView>();
    }

    void SetDataSourceObj(const JSRef<JSObject>& dataSourceObj)
    {
        dataSourceObj_ = dataSourceObj;
        totalCountFunc_ = GetFunctionFromObject(dataSourceObj, "totalCount");
        getDataFunc_ = GetFunctionFromObject(dataSourceObj, "getData");
        registerListenerFunc_ = GetFunctionFromObject(dataSourceObj, "registerDataChangeListener");
        unregisterListenerFunc_ = GetFunctionFromObject(dataSourceObj, "unregisterDataChangeListener");
    }

    void SetItemGenerator(const JSRef<JSFunc>& itemGenFunc, ItemKeyGenerator&& keyGenFunc)
    {
        itemGenFunc_ = itemGenFunc;
        keyGenFunc_ = std::move(keyGenFunc);
    }

    void ReleaseChildGroupByComposedId(const std::string& composedId) override
    {
        if (parentView_ != nullptr) {
            parentView_->RemoveChildGroupById(composedId);
        }
    }

private:
    std::list<RefPtr<Component>>& ExpandChildren() override
    {
        // Register data change listener while expanding the lazy foreach component
        if (!Expanded()) {
            defaultListener_ = Referenced::MakeRefPtr<DefaultDataChangeListener>(parentView_);
            RegisterDataChangeListener(defaultListener_);
        }
        return LazyForEachComponent::ExpandChildren();
    }

    JSExecutionContext context_;

    JSView* parentView_ = nullptr;

    JSRef<JSObject> dataSourceObj_;
    JSRef<JSFunc> totalCountFunc_;
    JSRef<JSFunc> getDataFunc_;
    JSRef<JSFunc> registerListenerFunc_;
    JSRef<JSFunc> unregisterListenerFunc_;

    JSRef<JSFunc> itemGenFunc_;
    ItemKeyGenerator keyGenFunc_;

    JSWeak<JSObject> listenerProxyObj_;
    WeakPtr<JSDataChangeListener> listenerProxy_;
    RefPtr<DefaultDataChangeListener> defaultListener_;

    ACE_DISALLOW_COPY_AND_MOVE(JSLazyForEachComponent);
};

} // namespace

void JSLazyForEach::JSBind(BindingTarget globalObj)
{
    JSClass<JSLazyForEach>::Declare("LazyForEach");
    JSClass<JSLazyForEach>::StaticMethod("create", &JSLazyForEach::Create);
    JSClass<JSLazyForEach>::StaticMethod("pop", &JSLazyForEach::Pop);
    JSClass<JSLazyForEach>::Bind(globalObj);

    JSDataChangeListener::JSBind(globalObj);
}

void JSLazyForEach::Create(const JSCallbackInfo& info)
{
    JSRef<JSVal> params[MAX_PARAM_SIZE];
    if (!ParseAndVerifyParams(info, params)) {
        LOGE("Invalid arguments for LazyForEach");
        return;
    }

    std::string viewId = ViewStackProcessor::GetInstance()->ProcessViewId(params[PARAM_VIEW_ID]->ToString());
    JSRef<JSObject> parentViewObj = JSRef<JSObject>::Cast(params[PARAM_PARENT_VIEW]);
    JSRef<JSObject> dataSourceObj = JSRef<JSObject>::Cast(params[PARAM_DATA_SOURCE]);
    JSRef<JSFunc> itemGenerator = JSRef<JSFunc>::Cast(params[PARAM_ITEM_GENERATOR]);
    ItemKeyGenerator keyGenFunc;
    if (params[PARAM_KEY_GENERATOR]->IsUndefined()) {
        keyGenFunc = [viewId](const JSRef<JSVal>&, size_t index) { return viewId + "-" + std::to_string(index); };
    } else {
        keyGenFunc = [viewId, keyGenerator = JSRef<JSFunc>::Cast(params[PARAM_KEY_GENERATOR])](
                         const JSRef<JSVal>& jsVal, size_t index) {
            JSRef<JSVal> params[] = { jsVal };
            auto key = keyGenerator->Call(JSRef<JSObject>(), ArraySize(params), params);
            return viewId + "-" + (key->IsString() || key->IsNumber() ? key->ToString() : std::to_string(index));
        };
    }

    auto component = AceType::MakeRefPtr<JSLazyForEachComponent>(viewId);
    component->SetJSExecutionContext(info.GetExecutionContext());
    component->SetParentViewObj(parentViewObj);
    component->SetDataSourceObj(dataSourceObj);
    component->SetItemGenerator(itemGenerator, std::move(keyGenFunc));

    ViewStackProcessor::GetInstance()->Push(component);
}

void JSLazyForEach::Pop()
{
    ViewStackProcessor::GetInstance()->PopContainer();
}

} // namespace OHOS::Ace::Framework
