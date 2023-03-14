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

@Entry
@Component
struct MotionPathExample {
  @State offsetX: number = 0
  @State offsetY: number = 0
  @State toggle: boolean = true
  @State rotatable: boolean = false
  @State path: string= 'Mstart.x start.y L300 200 L300 500 Lend.x end.y'
  @State fromX: number = 0
  @State toY: number = 1

  build() {
    Column() {
      Button('click me')
        .key('button')
        .motionPath({ path: this.path, from: this.fromX, to: this.toY, rotatable: this.rotatable })
        .onClick(() => {
          this.rotatable = true;
          console.info('button rotate current action state is:' + this.rotatable);
          try {
            var backData = {
              data: {
                "ACTION": this.rotatable,
              }
            }
            var backEvent = {
              eventId: 132,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button emit action state err: " + JSON.stringify(err.message))
          }
          animateTo({ duration: 4000, curve: Curve.Linear }, () => {
            this.toggle = !this.toggle;
          })
        })
        .backgroundColor(0x317aff)
      Button('next')
        .key('button1')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "fromX": this.fromX,
                "toY": this.toY
              }
            }
            var backEvent = {
              eventId: 133,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button1 position start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button1 position emit action state err: " + JSON.stringify(err.message))
          }
        })
    }
    .key('motionPath')
    .width('100%').height('100%').alignItems(this.toggle ? HorizontalAlign.Start : HorizontalAlign.Center)
  }

  onPageShow() {
    console.info('motionPath page show called');
    var stateChangeEvent = {
      eventId: 134,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangeCallBack)
  }

  private stateChangeCallBack = (eventData) => {
    if (eventData != null) {
      console.info('motionPath page state change called:' + JSON.stringify(eventData));
      if (eventData.data.fromX != null) {
        this.fromX == parseInt(eventData.data.fromX);
      }
      if (eventData.data.toY != null) {
        this.toY == parseInt(eventData.data.toY);
      }
    }
  }
}

