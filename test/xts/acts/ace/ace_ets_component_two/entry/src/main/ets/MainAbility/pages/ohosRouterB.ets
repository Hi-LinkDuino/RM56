// @ts-nocheck
/**
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
import router from '@system.router';
import events_emitter from '@ohos.emitter';

@Entry
@Component
struct OhosRouterB {
  build() {
    Flex({direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center}) {
      Text('B Page')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
    }.width('100%').height('100%')
  }
  onPageShow() {
    let params = router.getParams();
    if (params && params.data) {
      console.info("[OhosRouterB] getParams result: " + params.data);
      this.getRouterParams(params.data);
    }
  }
  getRouterParams(data) {
    try {
      let paramsData = {
        data: {
          "params": data,
        }
      }
      let paramsEvent = {
        eventId: 142,
        priority: events_emitter.EventPriority.LOW
      }
      console.info("[OhosRouterB] start to emit params");
      events_emitter.emit(paramsEvent, paramsData);
    } catch (err) {
      console.info("[OhosRouterB] emit params err: " + JSON.stringify(err.message));
    }
  }
}