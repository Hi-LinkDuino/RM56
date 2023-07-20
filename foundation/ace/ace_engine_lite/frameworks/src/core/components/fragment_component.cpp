/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "fragment_component.h"
#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"
#include "js_fwk_common.h"
#include "fragment/js_fragment_state_machine.h"
#include "jsi.h"
#include "js_ability_impl.h"
#include "js_app_context.h"

namespace OHOS {
namespace ACELite {
FragmentComponent::FragmentComponent(jerry_value_t options,
                           jerry_value_t children,
                           AppStyleManager* styleManager)
    : Component(options, children, styleManager)
{
    tempPath_ = nullptr;
    fragmentSM_ = nullptr;
    fragmentPath_ = nullptr;
    rootComponent_ = nullptr;
    SetComponentName(K_BES_FRAGEMNT);
    nativeView_.SetStyle(STYLE_BACKGROUND_OPA, 0);
}


FragmentComponent::~FragmentComponent()
{
    DetachFragment();
}

inline UIView *FragmentComponent::GetComponentRootView() const
{
    return const_cast<FlexLayout *>(&nativeView_);
}


bool FragmentComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    bool result = true;
    switch (attrKeyId) {
        case K_BES_PATH: {
            if(!jerry_value_is_string(attrValue)){
                break;
            }

            tempPath_ = MallocStringOf(attrValue);
            if(tempPath_ == nullptr || strlen(tempPath_) == 0){
                ace_free(tempPath_);
                tempPath_ = nullptr;
            }

            break;
        }
        default:
            result = false;
            break;
    }

    return result;
}


bool FragmentComponent::ProcessChildren()
{
    // add all children to this container
    AppendChildren(this);
    return true;
}

void FragmentComponent::PostUpdate(uint16_t attrKeyId)
{
    if(tempPath_ != nullptr){
        LoadFragment(tempPath_);
        tempPath_ = nullptr;
    }
    nativeView_.LayoutChildren();
}

void FragmentComponent::AttachView(const Component *child)
{
    if (child == nullptr) {
        return;
    }

    nativeView_.Add(child->GetComponentRootView());
}

void FragmentComponent::LayoutChildren()
{
    nativeView_.LayoutChildren();
}


void FragmentComponent::LoadFragment(char *path)
{
    if(path == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "LoadFragment error path is empty");
        return;
    }

    if((fragmentPath_ != nullptr) && (!strcmp(fragmentPath_, path))){
        HILOG_DEBUG(HILOG_MODULE_ACE, "LoadFragment is same path %s", path);
        ace_free(path);
        path = nullptr;
        return;
    }

    FragmentStateMachine* newSM = new FragmentStateMachine();
    bool ret = newSM->Init(path);

    if(!ret){
        ace_free(path);
        path = nullptr;
        delete newSM;
        newSM = nullptr;
        return;
    }

    DetachPageMachine();
    DetachFragment();
 
    fragmentPath_ = path;
    fragmentSM_ = newSM;

    AttachPageMachine();

    JsAppContext::GetInstance()->SetTempJsPath(fragmentPath_);
    fragmentSM_->ChangeState(FRAGMENT_INIT_STATE);
    JsAppContext::GetInstance()->SetTempJsPath(nullptr);

    rootComponent_ = fragmentSM_->GetRootComponent();
    if(rootComponent_ == nullptr){
        return;
    }

    ConstrainedParameter rootViewParam(GetWidth(), GetHeight());
    Component::BuildViewTree(rootComponent_, nullptr, rootViewParam);

    rootComponent_->OnViewAttached();
    UIView *view = rootComponent_->GetComponentRootView();

    if (view == nullptr) { 
        return;
    }

    nativeView_.Add(view);
    fragmentSM_->ChangeState(FRAGMENT_SHOW_STATE);
    nativeView_.Invalidate();
}

void FragmentComponent::DetachFragment()
{
    if(fragmentSM_ != nullptr){
        fragmentSM_->ChangeState(FRAGMENT_HIDE_STATE);
        fragmentSM_->ChangeState(FRAGMENT_DESTROY_STATE);
        delete fragmentSM_;
        fragmentSM_ = nullptr;
    }

    if(fragmentPath_ != nullptr){
        JsAppContext::GetInstance()->ReleaseStyles(fragmentPath_);
        ace_free(fragmentPath_);
        fragmentPath_ = nullptr;
    }

    if(rootComponent_ != nullptr){
         UIView *view = rootComponent_->GetComponentRootView();
         if(view != nullptr){
            nativeView_.Remove(view);
         }
         ComponentUtils::ReleaseComponents(rootComponent_);
         rootComponent_ = nullptr;
    }
  
}

void FragmentComponent::AttachPageMachine()
{
    if(fragmentSM_ == nullptr){
        return;
    }
    JsAppContext* appContext = JsAppContext::GetInstance();
    const JSAbilityImpl* topJsAbilityImpl = appContext->GetTopJSAbilityImpl();
    if(topJsAbilityImpl == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "topJsAbilityImpl is null.");
        return ;
    }

    Router* router = const_cast<Router *>(topJsAbilityImpl->GetRouter());
    if(router == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "router is null");
        return ;
    }

    StateMachine* sm = router->GetCurrentSM();

    if(sm == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "current state machine is null");
        return;
    }

    sm->AddFragment(fragmentSM_);
}

void FragmentComponent::DetachPageMachine()
{
    if(fragmentSM_ == nullptr){
        return;
    }
    JsAppContext* appContext = JsAppContext::GetInstance();
    const JSAbilityImpl* topJsAbilityImpl = appContext->GetTopJSAbilityImpl();
    if(topJsAbilityImpl == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "topJsAbilityImpl is null.");
        return ;
    }

    Router* router = const_cast<Router *>(topJsAbilityImpl->GetRouter());
    if(router == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "router is null");
        return ;
    }

    StateMachine* sm = router->GetCurrentSM();

    if(sm == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "current state machine is null");
        return;
    }

    sm->RemoveFragment(fragmentSM_);
}

} // namespace ACELite
} // namespace OHOS
