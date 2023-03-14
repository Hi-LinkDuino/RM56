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


/**
 * Store unit testing-related parameters, including test case names, and test runner name.
 *
 * @since 8
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @import import AbilityDelegatorArgs from 'application/abilityDelegatorArgs.d'
 * @permission N/A
 */
export interface AbilityDelegatorArgs {
    /**
     * the bundle name of the application being tested.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     */
    bundleName: string;

    /**
     * the parameters used for unit testing.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     */
    parameters: {[key: string]: string};

    /**
     * the class names of all test cases.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     */
    testCaseNames: string;

    /**
     * the class name of the test runner used to execute test cases.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     */
    testRunnerClassName: string;
}

export default AbilityDelegatorArgs;