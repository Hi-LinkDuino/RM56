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
import events_emitter from '@ohos.emitter';

@Entry
@Component
struct LoadingProgressExample {
  @State color: color = Color.Blue

  private stateChangCallBack = (eventData) => {
    console.info("LoadingProgress page stateChangCallBack");
    if (eventData != null) {
      console.info("LoadingProgress page state change called:" + JSON.stringify(eventData));
      if(eventData.data.color != null) {
        this.color = eventData.data.color;
      }
      }
    }

  onPageShow() {
    console.info('LoadingProgress page show called');
    var stateChangeEvent = {
      eventId: 124,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  build() {
    Column({ space: 5 }) {
      Text('Orbital LoadingProgress ')
        .fontSize(19)
        .fontColor(0xCCCCCC)
        .width('90%')
        .key('LP')
      LoadingProgress()
        .color(Color.Black)
    }.width('100%').margin({ top: 6 })
  }
}