/**
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

import addContextAndAbilityJsunit from './addContextAndAbilityJsunit.test.ets';
import getHapModuleInfoJsunit from './getHapModuleInfoJsunit.test.ets';
import getAbilityInfoJsunit from './getAbilityInfoJsunit.test.ets';
import getApplicationContext from './getApplicationContextJsunit.test.ets';
import getAppVersionInfoJsunit from './getAppVersionInfoJsunit.test.ets';
import ContextJsunit from './ContextJsunit.test.ets';

export default function testsuite() {
    ContextJsunit();
    addContextAndAbilityJsunit();
    getHapModuleInfoJsunit();
    getAbilityInfoJsunit();
    getApplicationContext();
    getAppVersionInfoJsunit();
}