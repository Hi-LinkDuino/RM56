// @ts-nocheck
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

import featureAbility from '@ohos.ability.featureability'
import commonEvent from '@ohos.commonevent'

export default {
  onCreate() {
    console.info("Application2 onCreate");
    commonEvent.publish("EntryAbility2_onCreate", () => {
      console.log("EntryAbility2 Publish CallBack EntryAbility2_onCreate")
    });
    setTimeout(()=>{
      featureAbility.terminateSelf().then((data) => {
        console.log("EntryAbility2 terminateSelf windowStage data: " + JSON.stringify(data))
      }).catch((error) => {
        console.log("EntryAbility2 terminateSelf windowStage error: " + JSON.stringify(error))
      })
    }, 4500)
  },

  onDestroy() {
    console.info("Application2 onDestroy");
    commonEvent.publish("EntryAbility2_onDestroy", () => {
      console.log("EntryAbility2 Publish CallBack EntryAbility2_onDestroy")
    });
  }
};
