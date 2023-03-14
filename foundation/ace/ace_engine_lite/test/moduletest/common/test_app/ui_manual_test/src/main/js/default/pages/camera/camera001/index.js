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
import {backPage, routePage, lifeCycleFunc} from "../../../common/js/general";
export default {
  backMain:backPage().backMain,
  changeNext:routePage("pages/camera/camera002/index").changePage,
  ...lifeCycleFunc(),
  takePhoto() {
    console.log("JS::takePhoto called!");
    var camera = this.$refs.cam;
    camera.takePhoto({
      quality: "high",
      success: (res) => {
        console.log("JS::camera.takePhoto success, uri=" + res.uri);
      },
      fail: (erromsg, errocode) => {
        console.log("JS::camera.takePhoto fail, errocode=" + errocode + " erromsg=" + erromsg);
      },
      complete: () => {
        console.log("JS::camera.takePhoto complete");
      }
    });
  },
  error: function (e) {
    console.log("JS::camera.onerror detail=" + e.detail + " errorCode=" + e.errorCode);
  },
};
