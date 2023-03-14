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
    console.info("Application3 onCreate");

    setTimeout(()=>{
      featureAbility.terminateSelf().then((data) => {
        console.log("EntryAbility3 terminateSelf windowStage data: " + JSON.stringify(data))
      }).catch((error) => {
        console.log("EntryAbility3 terminateSelf windowStage error: " + JSON.stringify(error))
      })
    }, 4500)
  },
  onDestroy() {
    console.info("Application3 onDestroy");
    commonEvent.publish("EntryAbility3_onDestroy", () => {
      console.log("EntryAbility3 Publish CallBack EntryAbility3_onDestroy")
    });
  }
};
