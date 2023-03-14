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

//other import
import convertOptionsJsunit from './ConvertOptionsJsunit.test.ets'
import radioJsunit from './RadioJsunit.test.ets'
import radioSecondJsunit from './RadioSecondJsunit.test.ets'
import observerJsunit from './ObserverJsunit.test.ets'
import smsUiJsunit from './SmsMmsJsunit.test.ets';
import callJsunit from './CallJsunit.test.ets';
import dataJsunit from './DataJsunit.test.ets';
import simJsunit from './SimJsunit.test.ets';
import statisticsUiJsunit from './StatisticsJsunit.test.ets';

export default function testsuite() {
  //other
  dataJsunit();
  callJsunit();
  smsUiJsunit();
  convertOptionsJsunit();
  observerJsunit();
  radioJsunit();
  radioSecondJsunit();
  simJsunit();
  statisticsUiJsunit();
}