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

import events_emitter from '@ohos.emitter'
import Matrix4 from '@ohos.matrix4'

@Entry
@Component
struct TransformExample {
  @State rotateValue: object= {
    x: 1,
    y: 1,
    z: 1,
    centerX: '50%',
    centerY: '50%',
    angle: 300
  }
  @State translateValue: object= { x: 100, y: 5 }
  @State scaleValue: object= { x: 2, y: 0.5 }
  @State transformValue: object= Matrix4.identity().translate({ x: 100, y: 100, z: 30 })

  private stateChangCallBack = (eventData) => {
    console.info("transFormExample page state change called:" + JSON.stringify(eventData));
    if (eventData != null) {
      if (eventData.data.rotateValue != null) {
        this.rotateValue = JSON.parse(eventData.data.rotateValue);
      }
      if (eventData.data.scaleValue != null) {
        this.scaleValue = JSON.parse(eventData.data.scaleValue);
      }
    }
  }

  onPageShow() {
    console.info('transFormExample page show called');
    var stateChangeEvent = {
      eventId: 45,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEvent2 = {
      eventId: 46,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent2, this.stateChangCallBack);
  }

  build() {
    Column() {
      Text('rotate').width('90%').fontColor(0xCCCCCC).padding(15).fontSize(30)
      Row()
        .rotate(this.rotateValue)
        .width(100)
        .height(100)
        .backgroundColor(0xAFEEEE)
        .key('rotate')

      Text('translate').width('90%').fontColor(0xCCCCCC).padding(10).fontSize(30)
      Row()
        .translate(this.transformValue)
        .width(100)
        .height(100)
        .backgroundColor(0xAFEEEE)
        .margin({ bottom: 10 })
        .key('rotate2')

      Text('scale').width('90%').fontColor(0xCCCCCC).padding(15).fontSize(30)
      Row()
        .scale(this.scaleValue)
        .width(100)
        .height(100)
        .backgroundColor(0xAFEEEE)
        .key('rotate3')

      Text('Matrix4').width('90%').fontColor(0xCCCCCC).padding(15).fontSize(30)
      Row()
        .width(100)
        .height(100)
        .backgroundColor(0xAFEEEE)
        .transform(Matrix4.identity().translate({ x: 100, y: 100, z: 30 }))
        .key('rotate4')
    }.width('100%').margin({ top: 5 })
  }
}
