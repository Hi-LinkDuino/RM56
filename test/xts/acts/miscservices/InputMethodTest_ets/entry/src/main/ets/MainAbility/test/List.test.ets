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
import inputmethohJsunit from './InputmethohJsunit.test.ets';
import inputmethodEngineJsunit from './InputmethodEngineJsunit.test.ets';
import inputRequestJsunit from './InputRequestJsunit.test.ets';
import requestJsunit from './RequestJsunit.test.ets';
import requestDownloadJsunit from './RequestDownloadTaskJsunit.test.ets';
import screenLockJsunit from './ScreenLockJsunit.test.ets';

export default function testsuite() {
  inputmethohJsunit();
  inputmethodEngineJsunit();
  inputRequestJsunit();
  requestDownloadJsunit();
  requestJsunit();
  screenLockJsunit();
}