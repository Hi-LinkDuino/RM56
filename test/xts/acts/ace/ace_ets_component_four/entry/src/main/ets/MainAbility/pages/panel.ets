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
struct PanelExample {
  @State show: boolean = true
  @State type: PanelType = PanelType.Foldable
  @State mode: PanelMode = PanelMode.Half

  private stateChangCallBack = (eventData) => {
    console.info("rating page stateChangCallBack");
    if (eventData != null) {
      console.info("rating page state change called:" + JSON.stringify(eventData.data.rating));
      if(eventData.data.type != null) {
        this.type = eventData.data.type;
      }
    }
  }

  onPageShow() {
    console.info('rating page show called');
    var stateChangeEvent = {
      eventId: 147,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
    var stateChangeEventTwo = {
      eventId: 148,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBack)
    var stateChangeEventThree = {
      eventId: 149,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventThree, this.stateChangCallBack)
  }

  build() {
    Column() {
      Text('2021-09-30    Today Calendar: 1.afternoon......Click for details')
        .width('90%').height(50).borderRadius(10)
        .backgroundColor(0xFF0000).padding({ left: 20 })
        .onClick(() => {
          this.show = !this.show
        })
      Panel(this.show) {
        Column() {
          Text('HAHA')
          Divider()
          Text('1. afternoon 4:00 The project meeting')
        }
      }
      .type(this.type).mode(this.mode)
      .dragBar(true)
      .halfHeight(500)
      .key('Panel')
      .onChange((value: any) => {
        console.info(`width:${value.width},height:${value.height},mode:${value.mode}`)
      })
    }.width('100%').height('100%').backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}