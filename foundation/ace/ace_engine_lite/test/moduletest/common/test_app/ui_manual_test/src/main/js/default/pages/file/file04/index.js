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
    status: "write"
  },
  backMain: backPage().backMain,
  changePrev: routePage("pages/file/file03/index").changePage,
  changeNext: routePage("pages/file/file05/index").changePage,
  fileWrite: function() {
    file.writeText({
      uri: "internal://app/testfile1",
      text: "read",
      success: function(data) {
        console.log("write call success");
      },
      fail: function(data, code) {
        console.log("write call fail");
      },
      complete: function() {
        console.log("write call complete");
      }
    })
  },
  fileRead: function() {
    var obj = this;
    file.readText({
      uri: "internal://app/testfile1",
      success: function(data) {
        console.log("read call success");
        obj.status = data.text;
      },
      fail: function(data, code) {
        console.log("read call fail");
      },
      complete: function() {
        console.log("read call complete");
      }
    })
  }
}