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
  changeNext: routePage("pages/file/file02/index").changePage,
  fileMove: function() {
    file.move({
      srcUri: "internal://app/testfile1",
      dstUri: "internal://app/testfile2",
      success: function(uri) {
        console.log("move call success :" + uri);
      },
      fail: function(data, code) {
        console.log("move call fail");
      },
      complete: function() {
        console.log("move call complete");
      }
    })
  },
  checkSrc: function() {
    var obj = this;
    file.access({
      uri: "internal://app/testfile1",
      success: function(data) {
        console.log("move src file exist.");
      },
      fail: function(data, code) {
        console.log("move src file not exist.");
        obj.srcStatus = true;
      },
      complete: function() {
        console.log("move src access call complete.");
      }
    })
  },
  checkDes: function() {
    var obj = this;
    file.access({
      uri: "internal://app/testfile2",
      success: function(data) {
        console.log("move des file exist.");
        obj.desStatus = true;
      },
      fail: function(data, code) {
        console.log("move des file not exist.");
      },
      complete: function() {
        console.log("move des access call complete.");
      }
    })
  }
}