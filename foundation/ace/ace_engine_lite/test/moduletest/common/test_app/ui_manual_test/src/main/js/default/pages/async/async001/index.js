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
import router from "@system.router";
export default {
  data: {
    testRes: "failed"
  },
  backMain() {
    router.replace({ uri: "pages/index/index" });
  },
  onInit() {
    console.log("onInit called...");
  },
  onReady() {
    console.log("onReady called...");
  },
  onShow() {
    console.log("onShow called...");
    var thisCtx = this;
    var file = require("@system.file");
    file.access({
      uri: 'internal://app/test',
      success: function() {
        console.log('file.access success');
        thisCtx.testRes = "passed";
      },
      fail: function(data, code) {
        console.log('file.access fail, data: ' + data + ', code: ' + code);
        thisCtx.testRes = "passed";
      }
    });
    console.log('file.access execute finished!');
    thisCtx.testRes = "failed";
  },
  onDestroy() {
    console.log("onDestroy called...");
  }
};
