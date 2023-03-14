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
struct ResponseRegionExample {
  @State touchable: boolean = false;
  @State isTouched: boolean = false;
  @State eventType: string = ''

  onPageShow() {
    let ChangeEvent = {
      eventId: 30,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(ChangeEvent, this.ChangCallBack)
  }

  private ChangCallBack = (eventData) => {
    console.info("[ResponseRegion] ChangCallBack stateChangCallBack");
    if (eventData != null) {
      console.info("[ResponseRegion] ChangCallBack state change called:" + JSON.stringify(eventData));
      if (eventData.data.touchable != null) {
        this.touchable = eventData.data.touchable;
      }
    }
  }

  build() {
    Column() {
      Toggle({ type: ToggleType.Checkbox, isOn: true })
        .selectedColor(0x39a2db)
        .backgroundColor(0xAFEEEE)
        .responseRegion({ x: 1.0, y: 1.0, width: 400, height: 400 })
        .onChange((isOn: boolean) => {
          console.info('[ResponseRegion] Component status:' + isOn)
        })
    }
    .width('100%')
    .height('100%')
    .margin({ top: 5 })
    .key('region')
    .touchable(this.touchable)
    .onTouch((event: TouchEvent) => {
      this.isTouched = true
      if (event.type === TouchType.Down) {
        this.eventType = 'Down'
      }
      if (event.type === TouchType.Up) {
        this.eventType = 'Up'
      }
      if (event.type === TouchType.Move) {
        this.eventType = 'Move'
      }
      console.info('[ResponseRegion] TouchType:' + this.eventType)
      try {
        var backData = {
          data: {
            "ACTION": this.isTouched,
            "EVENT_TYPE": this.eventType
          }
        }
        var backEvent = {
          eventId: 31,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ResponseRegion] start to emit action state")
        events_emitter.emit(backEvent, backData)
      } catch (err) {
        console.info("[ResponseRegion] emit action state err: " + JSON.stringify(err.message))
      }
    })
  }
}

