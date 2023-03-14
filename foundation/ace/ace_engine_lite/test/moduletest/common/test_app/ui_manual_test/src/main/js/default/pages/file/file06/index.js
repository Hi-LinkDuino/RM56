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
    content: "content"
  },
  backMain: backPage().backMain,
  changePrev: routePage("pages/file/file05/index").changePage,
  changeNext: routePage("pages/file/file07/index").changePage,
  fileList: function() {
    var obj = this;
    file.list({
      uri: "internal://app",
      success: function(data) {
        console.log("list call success " + JSON.stringify(data.fileList));
        obj.content = JSON.stringify(data.fileList);
      },
      fail: function(data, code) {
        console.log("list call fail");
      },
      complete: function() {
        console.log("list call complete");
      }
    })
  }
}