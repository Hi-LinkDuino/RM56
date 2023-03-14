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
struct fillModeExample {
  @State state: AnimationStatus = AnimationStatus.Initial
  @State preDecode: number = 0
  @State fillMode: FillMode = FillMode.None
  @State reverse: boolean = false
  @State iterations: number = 1
  @State fixedSize: boolean = false

  build() {
    Column({ space:5 }) {
      ImageAnimator()
        .images([
          {
            src: '/comment/bg1.jpg',
            duration: 500,
            width: 325,
            height: 200,
            top: 0,
            left: 0
          },
          {
            src: '/comment/bg2.jpg',
            duration: 500,
            width: 325,
            height: 200,
            top: 0,
            left: 0
          },
          {
            src: '/comment/bg3.jpg',
            duration: 500,
            width: 325,
            height: 200,
            top: 0,
            left: 0
          },
          {
            src: '/comment/bg4.jpg',
            duration: 500,
            width: 325,
            height: 200,
            top: 0,
            left: 0
          }
        ])
        .state(this.state).reverse(this.reverse).fixedSize(this.fixedSize).preDecode(this.preDecode)
        .fillMode(this.fillMode).iterations(this.iterations).width(325).height(210)
        .margin({top:100})
        .onStart(() => {
          console.info('Start')
        })
        .onPause(() => {
          console.info('Pause')
        })
        .onRepeat(() => {
          console.info('Repeat')
        })
        .onCancel(() => {
          console.info('Cancel')
        })
        .onFinish(() => {
          console.info('Finish')
        })
      Row() {
        Button('start').key('button').width(100).padding(5).onClick(() => {
          this.state = AnimationStatus.Running
          this.fillMode = FillMode.Backwards
          try {
            var backData = {
              data: {
                "fillMode": this.fillMode
              }
            }
            var backEvent = {
              eventId: 94,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button emit action state err: " + JSON.stringify(err.message))
          }
        })
        Button('pause').key('button1').width(100).padding(5).onClick(() => {
          this.state = AnimationStatus.Paused
          this.fillMode = FillMode.Both
          try {
            var backData = {
              data: {
                "fillMode": this.fillMode
              }
            }
            var backEvent = {
              eventId: 95,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button1 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button1 emit action state err: " + JSON.stringify(err.message))
          }
        })
        Button('stop').key('button2').width(100).padding(5).onClick(() => {
          this.state = AnimationStatus.Stopped
          this.fillMode = FillMode.None
          try {
            var backData = {
              data: {
                "fillMode": this.fillMode
              }
            }
            var backEvent = {
              eventId: 96,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button2 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button2 emit action state err: " + JSON.stringify(err.message))
          }
        })
      }
      Row() {
        Button('reverse').key('button3').width(100).padding(5).onClick(() => {
          this.reverse = !this.reverse
        })
        Button('once').key('button4').width(100).padding(5).onClick(() => {
          this.iterations = 1
        })
        Button('iteration').key('button5').width(100).padding(5).onClick(() => {
          this.iterations = -1
        })
      }
    }.width('100%').height('100%').backgroundColor(0xF1F3F5)
  }
}