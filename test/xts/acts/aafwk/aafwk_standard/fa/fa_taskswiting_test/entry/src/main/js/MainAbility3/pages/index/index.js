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

import router from '@system.router'
import commonEvent from '@ohos.commonEvent';

export default {
  data: {
    title: "",
    text: ""
  },
  onInit() {
    this.title = this.$t('strings.world');
    this.text = "This is entry MainAbility3"
  },
  onShow() {
    console.info("EntryAbility3 onShow");
    commonEvent.publish("EntryAbility3_onShow", () => {
      console.log("EntryAbility3 Publish CallBack EntryAbility3_onShow")
    });
  },

  onHide() {
    console.info("EntryAbility3 onHide");
    commonEvent.publish("EntryAbility3_onHide", () => {
      console.log("EntryAbility3 Publish CallBack EntryAbility3_onHide")
    });
  },

  onActive() {
    console.info("EntryAbility3 onActive");
    commonEvent.publish("EntryAbility3_onActive", () => {
      console.log("EntryAbility3 Publish CallBack EntryAbility3_onActive")
    });
  },

  onInactive() {
    console.info("EntryAbility3 onInactive");
    commonEvent.publish("EntryAbility3_onInactive", () => {
      console.log("EntryAbility3 Publish CallBack EntryAbility3_onInactive")
    });
  },
  onclick: function () {
    router.replace({
      uri: "pages/second/second"
    })
  }
}



