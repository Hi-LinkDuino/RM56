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

#include "router_module_tdd_test.h"

#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_debugger_config.h"
#include "js_fwk_common.h"
#include "js_page_state_machine.h"
#include "js_router.h"
#include "router_module.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
static const char PAGE_VIEW_MODEL_BUNDLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'] }, [\n"
    "        _c('div', { staticClass: ['pane'] }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'CLICK ME' }\n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    }\n"
    "  });\n"
    "})();";

void RouterModuleTddTest::SetUp()
{
    // set sample ability info
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("root", "bundleName", 0);
    JsAppEnvironment::GetInstance()->InitJsFramework();
}

void RouterModuleTddTest::TearDown()
{
    // clear sample ability info
    JsAppContext::GetInstance()->SetCurrentAbilityInfo(nullptr, nullptr, 0);
    JsAppEnvironment::GetInstance()->Cleanup();
}

void RouterModuleTddTest::ReleaseJSValue(JSValue &value)
{
    jerry_release_value(value);
    value = UNDEFINED;
}

/**
 * @tc.name: RouterModuleTestTest001
 * @tc.desc: router replace with invalid router param
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with undefined
     */
    JSValue routerParam = UNDEFINED;
    /**
     * @tc.steps: step2. trigger router replace
     */
    Router router;
    JSValue replaceResult = router.Replace(routerParam, false);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(jerry_value_is_error(replaceResult));
    ReleaseJSValue(replaceResult);
}

/**
 * @tc.name: RouterModuleTestTest002
 * @tc.desc: Verify the router replace process with valid uri but invalid path
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with only uri
     */
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "invalid path");
    /**
     * @tc.steps: step2. trigger router replace
     */
    Router router;
    JSValue replaceResult = router.Replace(routerParam, false);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(jerry_value_is_error(replaceResult));
    ReleaseJSValue(replaceResult);
}

/**
 * @tc.name: RouterModuleTestTest003
 * @tc.desc: Verify the params handling
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "path");
    JSValue extraParam = jerry_create_object();
    JSObject::SetNumber(extraParam, "count", 0);
    JSObject::Set(routerParam, "params", extraParam);
    /**
     * @tc.steps: step2. trigger router replace
     */
    Router router;
    JSValue replaceResult = router.Replace(routerParam, false);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(jerry_value_is_error(replaceResult));
    ReleaseJSValue(extraParam);
    ReleaseJSValue(routerParam);
    ReleaseJSValue(replaceResult);
}

void RouterModuleTddTest::PrepareRouterStateMachine(StateMachine &sm, JSValue &routerParam)
{
    routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "sample path");
    JSValue extraParam = jerry_create_object();
    JSObject::SetNumber(extraParam, "size", 0);
    JSObject::Set(routerParam, "params", extraParam);
    ReleaseJSValue(extraParam);

    JSValue jsResult = UNDEFINED;
    bool initResult = sm.Init(routerParam, jsResult);
    EXPECT_FALSE(initResult);
    EXPECT_TRUE(jerry_value_is_error(jsResult));
    ReleaseJSValue(jsResult);
}

void RouterModuleTddTest::HookViewModel(StateMachine &sm, const char *jsBundle, size_t bundleLength)
{
    if (jsBundle == nullptr || strlen(jsBundle) == 0 || bundleLength == 0) {
        return;
    }

    // clear ability info to avoid page evaling
    JsAppContext::GetInstance()->SetCurrentAbilityInfo(nullptr, nullptr, 0);
    // eval our own view model, and hook it into state machine
    JSValue viewModel = jerry_eval(reinterpret_cast<const jerry_char_t *>(PAGE_VIEW_MODEL_BUNDLE),
                                   strlen(PAGE_VIEW_MODEL_BUNDLE), JERRY_PARSE_NO_OPTS);
    sm.SetViewModel(viewModel);
    // initialize state machine to set correct app root path
    JSValue result = UNDEFINED;
    sm.Init(UNDEFINED, result);
}

/**
 * @tc.name: RouterModuleTestTest004
 * @tc.desc: Verify the page statemachie processing
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue routerParam = UNDEFINED;
    StateMachine pageSM;
    PrepareRouterStateMachine(pageSM, routerParam);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(pageSM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state
     */
    pageSM.ChangeState(INIT_STATE);

    /**
     * @tc.steps: step4. check the result
     */
    int8_t currentState = pageSM.GetCurrentState();
    EXPECT_EQ(currentState, READY_STATE);
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest005
 * @tc.desc: Verify the cache distribution process and overflow checking
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue routerParam = UNDEFINED;
    StateMachine pageStateMachine;
    PrepareRouterStateMachine(pageStateMachine, routerParam);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(pageStateMachine, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to ready directly
     */
    pageStateMachine.ChangeState(READY_STATE);

    /**
     * @tc.steps: step4. check the result, the state changing will be refused
     */
    int8_t currentState = pageStateMachine.GetCurrentState();
    EXPECT_EQ(currentState, UNDEFINED_STATE);
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest006
 * @tc.desc: Verify the cache distribution process, considering the magic number length
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue prameter = UNDEFINED;
    StateMachine stateMachine;
    PrepareRouterStateMachine(stateMachine, prameter);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(stateMachine, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateMachine.ChangeState(SHOW_STATE);

    /**
     * @tc.steps: step4. check the result, the state changing will be refused
     */
    int8_t currentState = stateMachine.GetCurrentState();
    EXPECT_EQ(currentState, UNDEFINED_STATE);
    ReleaseJSValue(prameter);
}

/**
 * @tc.name: RouterModuleTestTest007
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue prameterObject = UNDEFINED;
    StateMachine pageStateM;
    PrepareRouterStateMachine(pageStateM, prameterObject);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(pageStateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    pageStateM.ChangeState(BACKGROUND_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(pageStateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(prameterObject);
}

/**
 * @tc.name: RouterModuleTestTest008
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue object = UNDEFINED;
    StateMachine stateM;
    PrepareRouterStateMachine(stateM, object);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(stateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateM.ChangeState(DESTROY_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(stateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(object);
}

/**
 * @tc.name: RouterModuleTestTest009
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue object = UNDEFINED;
    StateMachine stateM;
    PrepareRouterStateMachine(stateM, object);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(stateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateM.ChangeState(BACKGROUND_STATE);
    stateM.ChangeState(DESTROY_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(stateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(object);
}

/**
 * @tc.name: RouterModuleTestTest010
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters
     */
    JSValue object = UNDEFINED;
    StateMachine stateM;
    PrepareRouterStateMachine(stateM, object);
    /**
     * @tc.steps: step2. hook view model to one
     */
    HookViewModel(stateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateM.ChangeState(DESTROY_STATE);
    stateM.ChangeState(SHOW_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(stateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(object);
}

/**
 * @tc.name: RouterModuleTestTest011
 * @tc.desc: pass wrong parameters into router API
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router parameters
     */
    JSIValue thisVal = JSI::CreateUndefined();
    const uint8_t argsNum = 2;
    JSIValue args[argsNum] = {JSI::CreateUndefined(), JSI::CreateUndefined()};
    /**
     * @tc.steps: step2. call router module function to simulator the API using
     */
    JSIValue callResult = RouterModule::Replace(thisVal, args, argsNum);
    /**
     * @tc.steps: step4. check the result
     */
    EXPECT_TRUE(JSI::ValueIsError(callResult));
    JSI::ReleaseValue(callResult);
}

/**
 * @tc.name: RouterModuleTestTest012
 * @tc.desc: pass wrong parameters into router API
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router parameters
     */
    JSIValue context = JSI::CreateUndefined();
    const uint8_t argsNumber = 1;
    /**
     * @tc.steps: step2. pass null args to API
     */
    JSIValue result = RouterModule::Replace(context, nullptr, argsNumber);
    /**
     * @tc.steps: step4. check the result
     */
    EXPECT_TRUE(JSI::ValueIsError(result));
    JSI::ReleaseValue(result);
}

/**
 * @tc.name: RouterModuleTestTest013
 * @tc.desc: the router replace will fail before the js ability is launched
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router parameters
     */
    JSIValue context = JSI::CreateObject();
    const uint8_t argsNumber = 1;
    JSIValue args[argsNumber] = {JSI::CreateObject()};
    /**
     * @tc.steps: step2. pass null args to API
     */
    JSIValue result = RouterModule::Replace(context, nullptr, argsNumber);
    /**
     * @tc.steps: step4. check the result
     */
    EXPECT_TRUE(JSI::ValueIsError(result));
    JSI::ReleaseValue(result);
    JSI::ReleaseValue(args[0]);
    JSI::ReleaseValue(context);
}
} // namespace ACELite
} // namespace OHOS
