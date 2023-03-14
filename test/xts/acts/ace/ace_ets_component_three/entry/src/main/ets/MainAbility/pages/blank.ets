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
struct BlankExample {
  @State colorValue: Color = Color.Blue;
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("Blank page state change called:" + JSON.stringify(eventData));
      if (eventData.data.colorValue != null) {
        this.colorValue = eventData.data.colorValue;
      }
    }
  }

  onPageShow() {
    console.info('Blank page1 show called');
    var stateChangeEvent = {
      eventId: 69,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Column() {
      Row() {
        Text('Bluetooth').fontSize(18)
        Blank().color(this.colorValue).key('blank')
        Toggle({ type: ToggleType.Switch })
      }.width('100%')
      .backgroundColor(0xFFFFFF)
      .borderRadius(15)
      .padding({ left: 12 })
    }.backgroundColor(0xEFEFEF).padding(20)
  }
}