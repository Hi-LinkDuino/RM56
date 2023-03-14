/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
* This module provides the capability to check bad code usage.
*
 *@import import hichecker from '@ohos.hichecker';
* @since 8
* @syscap SystemCapability.HiviewDFX.HiChecker
*/
declare namespace hichecker {


    /**
    * The caution rule print log.
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    const RULE_CAUTION_PRINT_LOG: 9223372036854775808n; // 1 << 63

    /**
    * The caution rule trigger crash.
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    const RULE_CAUTION_TRIGGER_CRASH: 4611686018427387904n; // 1 << 62

    /**
    * The thread rule check slow process.
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    const RULE_THREAD_CHECK_SLOW_PROCESS: 1n;

    /**
    * The process rule check ability connection leak.
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    const RULE_CHECK_ABILITY_CONNECTION_LEAK: 8589934592n; // 1 << 33

    /**
    * add one or more rule.
    * @param rule 
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    function addRule(rule: bigint) : void;

    /**
    * remove one or more rule.
    * @param rule 
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    function removeRule(rule: bigint) : void;

    /**
    * get added rule
    * @return all added thread rule and process rule.
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    function getRule() : bigint;

    /**
    * whether the query rule is added
    * @param rule
    * @return the result of whether the query rule is added.
    * @since 8
    * @syscap SystemCapability.HiviewDFX.HiChecker
    */
    function contains(rule: bigint) : boolean;
}
export default hichecker;