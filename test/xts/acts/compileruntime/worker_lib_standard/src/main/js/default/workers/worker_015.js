/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import worker from "@ohos.worker"

const parentPort = worker.parentPort;

var ss = undefined;
var backValue = undefined;
var flag = false;

parentPort.onmessage = function(e) {
  var data = e.data;
  switch(data.type) {
    case "new":
      ss = new worker.Worker("workers/worker_0151.js");
      ss.onmessage = function(ee) {
        backValue = ee.data; // 16
        flag = true;
      }
      ss.postMessage({type: "new", value: data.value + 1}); // 11
      break;
    case "wait":
      ss.postMessage({type: "wait"});
      if (flag) {
        parentPort.postMessage(backValue); // 16
      }
      break;
    default:
      break;
  }
}