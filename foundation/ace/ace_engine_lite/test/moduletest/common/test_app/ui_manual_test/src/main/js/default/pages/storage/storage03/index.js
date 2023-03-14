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
    status: "click set info1"
  },
  backMain: backPage().backMain,
  changePrev: routePage("pages/storage/storage02/index").changePage,
  storageSet1: function() {
    this.status = "click set info2";
    storage.set({
      key: "Storage2",
      value: "Info2",
      success: function() {
        console.log("storageSet call success, key = Storage2, value = Info2");
      },
      fail: function(data, code) {
        console.log("storageSet call fail, key = Storage2, value = Info2");
      },
      complete: function() {
        console.log("storageSet call complete")
      }
    })
  },
  storageSet2: function() {
    this.status = "click clear";
    storage.set({
      key: "Storage3",
      value: "Info3",
      success: function() {
        console.log("storageSet call success, key = Storage3, value = Info3");
      },
      fail: function(data, code) {
        console.log("storageSet call fail, key = Storage3, value = Info3");
      },
      complete: function() {
        console.log("storageSet call complete")
      }
    })
  },
  storageClear: function() {
    this.status = "click getInfo1 and getInfo2";
    storage.clear({
      success: function(data) {
        console.log("storageClear call success");
      },
      fail: function(data, code) {
        console.log("storageClear call fail");
      },
      complete: function() {
        console.log("storageClear call complete")
      }
    })
  },
  storageGet1: function() {
    var object = this;
    storage.get({
      key: "Storage2",
      success: function(data) {
        console.log("storageGet call success, key = Storage2");
        object.status = JSON.stringify(data);
      },
      fail: function(data, code) {
        console.log("storageGet call fail, key = Storage2");
      },
      complete: function() {
        console.log("storageGet call complete")
      }
    })
  },
  storageGet2: function() {
    var object = this;
    storage.get({
      key: "Storage3",
      success: function(data) {
        console.log("storageGet call success, key = Storage3");
        object.status = JSON.stringify(data);
      },
      fail: function(data, code) {
        console.log("storageGet call fail, key = Storage3");
      },
      complete: function() {
        console.log("storageGet call complete")
      }
    })
  }
}
