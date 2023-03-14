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
    srcStatus: false,
    desStatus: false
  },
  backMain: backPage().backMain,
  changePrev: routePage("pages/file/file01/index").changePage,
  changeNext: routePage("pages/file/file03/index").changePage,
  fileCopy: function() {
    file.copy({
      srcUri: "internal://app/testfile2",
      dstUri: "internal://app/testfile1",
      success: function(uri) {
        console.log("copy call success :" + uri);
      },
      fail: function(data, code) {
        console.log("copy call fail");
      },
      complete: function() {
        console.log("copy call complete");
      }
    })
  },
  checkSrc: function() {
    var obj = this;
    file.access({
      uri: "internal://app/testfile2",
      success: function(data) {
        console.log("copy src file exist.");
        obj.srcStatus = true;
      },
      fail: function(data, code) {
        console.log("copy src file not exist.");
      },
      complete: function() {
        console.log("copy src access call complete.");
      }
    })
  },
  checkDes: function() {
    var obj = this;
    file.access({
      uri: "internal://app/testfile1",
      success: function(data) {
        console.log("copy des file exist.");
        obj.desStatus = true;
      },
      fail: function(data, code) {
        console.log("copy des file not exist.");
      },
      complete: function() {
        console.log("copy des access call complete.");
      }
    })
  }
}