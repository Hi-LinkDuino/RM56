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
import storage from "@system.storage";

export default {
  data: {
    status: "click delete"
  },
  backMain: backPage().backMain,
  changePrev: routePage("pages/storage/storage01/index").changePage,
  changeNext: routePage("pages/storage/storage03/index").changePage,
  storageDelete: function() {
    this.status = "click get";
    storage.delete({
      key: "Storage1",
      success: function(data) {
        console.log("storageDelete call success, key = Storage1");
      },
      fail: function(data, code) {
        console.log("storageDelete call fail, key = Storage1");
      },
      complete: function() {
        console.log("storageDelete call complete")
      }
    })
  },
  storageGet: function() {
    var obj = this;
    storage.get({
      key: "Storage1",
      success: function(data) {
        console.log("storageGet call success, key = Storage1, expected value = \"\"");
        obj.status = JSON.stringify(data);
      },
      fail: function(data, code) {
        console.log("storageGet call fail, key = Storage1");
      },
      complete: function() {
        console.log("storageGet call complete")
      }
    })
  }
}
