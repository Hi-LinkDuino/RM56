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
import { backPage, routePage } from "../../../common/js/general";
import file from "@system.file";

export default {
  data: {
    status: "mkdir"
  },
  backMain: backPage().backMain,
  changePrev: routePage("pages/file/file04/index").changePage,
  changeNext: routePage("pages/file/file06/index").changePage,
  fileMkdir: function() {
    file.mkdir({
      uri: "internal://app/testDirectory",
      success: function(data) {
        console.log("mkdir call success");
      },
      fail: function(data, code) {
        console.log("mkdir call fail");
      },
      complete: function() {
        console.log("mkdir call complete");
      }
    })
  },
  fileRmdir: function() {
    var obj = this;
    file.rmdir({
      uri: "internal://app/testDirectory",
      recursive: true,
      success: function(data) {
        console.log("rmdir call success");
        obj.status = "rmdir";
      },
      fail: function(data, code) {
        console.log("rmdir call fail");
      },
      complete: function() {
        console.log("rmdir call complete");
      }
    })
  }
}