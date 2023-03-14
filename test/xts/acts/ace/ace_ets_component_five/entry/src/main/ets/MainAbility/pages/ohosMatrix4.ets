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
import prompt from '@system.prompt'

@Entry
@Component
struct ohosMatrix4 {
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
  @State transformValue: object= Matrix4.identity().translate({ x: 100, y: 100, z: 30 })
  @State matrix1: object= Matrix4.identity().transformPoint([100, 10])
  @State matrix2: object = Matrix4.identity().translate({ x: 300 }).copy()
  @State matrix3: object = Matrix4.identity().scale({ x: 3 }).copy()
  @State matrix4: object = Matrix4.identity().rotate({x:1, y:1, z:2, angle:30})
  @State matrix5: object = Matrix4.identity().scale({x:5, y:1, z:1, centerX:50, centerY:50})
  @State matrix6: object = Matrix4.identity().translate({x:100, y:200, z:30})

  private stateChangCallBack = (eventData) => {
    console.info("transFormExample page state change called:" + JSON.stringify(eventData));
    if (eventData != null) {
      console.info("eventData value is");
      if (eventData.data.matrix2 != null && eventData.data.matrix3 != null) {
        console.info("this.matrix2 value is" + Matrix4.identity()
          .translate(eventData.data.matrix2)
          .copy() + "2" + JSON.stringify(Matrix4.identity().translate(eventData.data.matrix2).copy()));
        this.matrix2 = Matrix4.identity().translate(eventData.data.matrix2).copy();
        console.info("this.matrix3 value is");
        this.matrix3 = Matrix4.identity().scale(eventData.data.matrix3).copy();
      } else {
        console.info("this.matrix value is not");
      }
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
      eventId: 140,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Column() {
      Text('rotate').width('10%').fontColor(0xCCCCCC).padding(5).fontSize(5)
      Row()
        .rotate(this.rotateValue)
        .width(10)
        .height(10)
        .backgroundColor(0xAFEEEE)
        .key('rotate')

      Text('translate').width('10%').fontColor(0xCCCCCC).padding(5).fontSize(5)
      Row()
        .translate(this.transformValue)
        .width(10)
        .height(10)
        .backgroundColor(0xAFEEEE)
        .margin({ bottom: 10 })
        .key('rotate2')

      Text('scale').width('90%').fontColor(0xCCCCCC).padding(5).fontSize(5)
      Row()
        .scale(this.scaleValue)
        .width(10)
        .height(10)
        .backgroundColor(0xAFEEEE)
        .key('rotate3')

      Text('Matrix4').width('10%').fontColor(0xCCCCCC).padding(5).fontSize(5)
      Row()
        .width(10)
        .height(10)
        .backgroundColor(0xAFEEEE)
        .transform(Matrix4.identity().translate({ x: 100, y: 100, z: 30 }))
        .key('rotate4')
      Button("get Point").padding(40)
        .onClick(() => {
          prompt.showToast({ message: JSON.stringify(this.matrix1), duration: 200 })
          this.onClickValue = true;
          try {
            var backData = {
              data: {
                "matrix1": "[100, 10]",
                "ACTION": this.onClickValue
              }
            }
            var backEvent = {
              eventId: 141,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("click to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("click action state err: " + JSON.stringify(err.message))
          }
        }).backgroundColor(0x2788D9).key('rotate5')
      Image('/images/bg.jpg')
        .padding(5)
        .transform(this.matrix2.combine(this.matrix3))
        .width("40%")
        .height(100)
        .margin({ top: 5 })
        .key('rotate6')
      Image('/images/bg.jpg').transform(this.matrix4)
        .padding(5)
        .width("40%")
        .height(100)
        .key('rotate7')
      Image('/images/bg.jpg').transform(this.matrix5)
        .padding(5)
        .width("40%")
        .height(100)
        .key('rotate8')
      Image('/images/bg.jpg').transform(this.matrix6)
        .width("40%")
        .height(100)
        .key('rotate9')
    }.width('100%').margin({ top: 5 })
  }
}
