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

#include "require_module_tdd_test.h"
#include "internal/jsi_internal.h"
#include "jerryscript.h"
#include "jsi.h"
#include "module_manager.h"

namespace OHOS {
namespace ACELite {
RequireModuleTddTest::RequireModuleTddTest() : moduleManager_(nullptr) {}

void RequireModuleTddTest::SetUp()
{
    jerry_init(JERRY_INIT_EMPTY);
    moduleManager_ = ModuleManager::GetInstance();
}

void RequireModuleTddTest::TearDown()
{
    moduleManager_->CleanUpModule();
    moduleManager_->OnTerminate();
    jerry_cleanup();
}

void RequireModuleTddTest::RequireModuleTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require a module with module name registered
     */
    const char *moduleName = "system.app";
    JSIValue moduleRequired = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. moduleRequired is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(moduleRequired));
    JSI::ReleaseValue(moduleRequired);
    TDD_CASE_END();
}

void RequireModuleTddTest::RequireModuleTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require a module with module name unregistered
     */
    const char *moduleName = "system.unregistered";
    JSIValue moduleRequired = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. moduleRequired is undefined
     */
    EXPECT_TRUE(JSI::ValueIsUndefined(moduleRequired));
    JSI::ReleaseValue(moduleRequired);
    TDD_CASE_END();
}

void RequireModuleTddTest::RequireModuleTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require a module with incorrect category
     */
    const char *moduleName = "service.app";
    JSIValue moduleRequired = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. moduleRequired is undefined
     */
    EXPECT_TRUE(JSI::ValueIsUndefined(moduleRequired));
    JSI::ReleaseValue(moduleRequired);
    TDD_CASE_END();
}

void RequireModuleTddTest::RequireModuleTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require a module with illegal module name
     */
    const char *moduleName = "illegal";
    JSIValue moduleRequired = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. moduleRequired is undefined
     */
    EXPECT_TRUE(JSI::ValueIsUndefined(moduleRequired));
    JSI::ReleaseValue(moduleRequired);
    TDD_CASE_END();
}

void RequireModuleTddTest::RequireModuleTest005()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require app module
     */
    const char *moduleName = "system.app";
    JSIValue moduleRequired = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. moduleRequired is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(moduleRequired));
    /**
     * @tc.steps: step2. get "getInfo" property of moduleRequired
     */
    const char *funcName = "getInfo";
    JSIValue jFunc = JSI::GetNamedProperty(moduleRequired, funcName);
    /**
     * @tc.expected: step2. jFunc is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(jFunc));
    /**
     * @tc.steps: step3. call jFunc and the value returned is undefined
     */
    jerry_value_t jResult = jerry_call_function(AS_JERRY_VALUE(jFunc), AS_JERRY_VALUE(moduleRequired), nullptr, 0);
    JSIValue result = AS_JSI_VALUE(jResult);
    /**
     * @tc.expected: step3. AppModule native function will be called an result is undefined
     */
    EXPECT_TRUE(JSI::ValueIsUndefined(result));
    JSI::ReleaseValueList(moduleRequired, jFunc, result);
    TDD_CASE_END();
}

void RequireModuleTddTest::RequireModuleTest006()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require app module
     */
    const char *moduleName = "system.app";
    JSIValue moduleRequired = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. moduleRequired is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(moduleRequired));
    /**
     * @tc.steps: step2. get "terminate" property of moduleRequired
     */
    const char *funcName = "terminate";
    JSIValue jFunc = JSI::GetNamedProperty(moduleRequired, funcName);
    /**
     * @tc.expected: step2. jFunc is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(jFunc));
    /**
     * @tc.steps: step3. call jFunc
     */
    jerry_value_t jResult = jerry_call_function(AS_JERRY_VALUE(jFunc), AS_JERRY_VALUE(moduleRequired), nullptr, 0);
    JSIValue result = AS_JSI_VALUE(jResult);
    /**
     * @tc.expected: step3. AppModule native function will be called an result is undefined
     */
    EXPECT_TRUE(JSI::ValueIsUndefined(result));
    JSI::ReleaseValueList(moduleRequired, jFunc, result);
    TDD_CASE_END();
}

void RequireModuleTddTest::RequireModuleTest007()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require router module
     */
    const char *moduleName = "system.router";
    JSIValue moduleRequired = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. moduleRequired is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(moduleRequired));
    /**
     * @tc.steps: step2. get "replace" property of moduleRequired
     */
    const char *funcName = "replace";
    JSIValue jFunc = JSI::GetNamedProperty(moduleRequired, funcName);
    /**
     * @tc.expected: step2. jFunc is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(jFunc));
    /**
     * @tc.steps: step3. call jFunc
     */
    jerry_value_t jResult = jerry_call_function(AS_JERRY_VALUE(jFunc), AS_JERRY_VALUE(moduleRequired), nullptr, 0);
    JSIValue result = AS_JSI_VALUE(jResult);
    /**
     * @tc.expected: step3. RouterModule native function will be called an result is undefined
     */
    EXPECT_TRUE(JSI::ValueIsError(result));
    JSI::ReleaseValueList(moduleRequired, jFunc, result);
    TDD_CASE_END();
}

void RequireModuleTddTest::RequireModuleTest008()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. require a module with module name registered which is ohos
     */
    const char *moduleName = "ohos.app";
    JSIValue module = moduleManager_->RequireModule(moduleName);
    /**
     * @tc.expected: step1. module is not undefined
     */
    EXPECT_FALSE(JSI::ValueIsUndefined(module));
    JSI::ReleaseValue(module);
    TDD_CASE_END();
}

void RequireModuleTddTest::RunTests()
{
    RequireModuleTest001();
    RequireModuleTest002();
    RequireModuleTest003();
    RequireModuleTest004();
    RequireModuleTest005();
    RequireModuleTest006();
    RequireModuleTest007();
    RequireModuleTest008();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: RequireModuleTest001
 * @tc.desc: Verify require a registered module.
 */
HWTEST_F(RequireModuleTddTest, test001, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest001();
}

/**
 * @tc.name: RequireModuleTest002
 * @tc.desc: Verify require an unregistered module.
 */
HWTEST_F(RequireModuleTddTest, test002, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest002();
}

/**
 * @tc.name: RequireModuleTest003
 * @tc.desc: Verify require a module with incorrect category.
 */
HWTEST_F(RequireModuleTddTest, test003, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest003();
}

/**
 * @tc.name: RequireModuleTest004
 * @tc.desc: Verify require a module with illegal module name.
 */
HWTEST_F(RequireModuleTddTest, test004, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest004();
}

/**
 * @tc.name: RequireModuleTest005
 * @tc.desc: Verify require app module, and call it's getInfo().
 */
HWTEST_F(RequireModuleTddTest, test005, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest005();
}

/**
 * @tc.name: RequireModuleTest006
 * @tc.desc: Verify require app module, and call it's terminate().
 */
HWTEST_F(RequireModuleTddTest, test006, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest006();
}

/**
 * @tc.name: RequireModuleTest007
 * @tc.desc: Verify require router module, and call it's replace().
 */
HWTEST_F(RequireModuleTddTest, test007, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest007();
}

/**
 * @tc.name: RequireModuleTest008
 * @tc.desc: Verify the ohos module importing process.
 */
HWTEST_F(RequireModuleTddTest, test008, TestSize.Level1)
{
    RequireModuleTddTest::RequireModuleTest008();
}
#endif
} // namespace ACELite
} // namespace OHOS
