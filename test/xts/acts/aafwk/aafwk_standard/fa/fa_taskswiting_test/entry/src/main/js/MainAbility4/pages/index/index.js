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
    this.text = "This is entry MainAbility4"
  },
  onShow() {
    console.info("EntryAbility4 onShow");
    commonEvent.publish("EntryAbility4_onShow", () => {
      console.log("EntryAbility4 Publish CallBack EntryAbility4_onShow")
    });
  },

  onHide() {
    console.info("EntryAbility4 onHide");
    commonEvent.publish("EntryAbility4_onHide", () => {
      console.log("EntryAbility4 Publish CallBack EntryAbility4_onHide")
    });
  },

  onActive() {
    console.info("EntryAbility4 onActive");
    commonEvent.publish("EntryAbility4_onActive", () => {
      console.log("EntryAbility4 Publish CallBack EntryAbility4_onActive")
    });
  },

  onInactive() {
    console.info("EntryAbility4 onInactive");
    commonEvent.publish("EntryAbility4_onInactive", () => {
      console.log("EntryAbility4 Publish CallBack EntryAbility4_onInactive")
    });
  },

  onclick: function () {
    router.replace({
      uri: "pages/second/second"
    })
  }
}



