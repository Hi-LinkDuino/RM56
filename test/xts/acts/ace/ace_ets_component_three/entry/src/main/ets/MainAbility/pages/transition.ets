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
struct TransitionExample {
  @State btn1: boolean = false
  @State show: string = "show"
  @State opacity: number = 1
  @State onActionCalled: boolean = false
  @State transitionTypeOne: TransitionType = TransitionType.Insert
  @State transitionTypeTwo: TransitionType = TransitionType.Delete
  @State transitionTypeThree: TransitionType = TransitionType.All

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, }) {
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 50 })
        .key('button')
        .opacity(this.opacity)
        .onClick(() => {
          this.onActionCalled = true;
          try {
            var backData = {
              data: {
                "btn1": this.btn1,
              }
            }
            var backEvent = {
              eventId: 233,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("transitionTest_0200 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("transitionTest_0200 emit action state err: " + JSON.stringify(err.message))
          }
          animateTo({ duration: 3000 }, () => {
            this.btn1 = !this.btn1
            if (this.btn1) {
              this.show = "hide"
            } else {
              this.show = "show"
            }
          })
        })
      if (this.btn1) {
        Button() {
          Image($rawfile('test.png')).width("80%").height(300).key('image')
        }.transition({ type: this.transitionTypeOne, scale: { x: 0, y: 1.0, z: 1.0 }, opacity: this.opacity })
        .key('button1')
        .transition({ type: this.transitionTypeTwo, scale: { x: 1.0, y: 0.0 }, opacity: this.opacity })
      }
    }.height(400).width("100%").padding({ top: 100 })
  }

  onPageShow() {
    console.info('transition page show called');
    var stateChangeEvent = {
      eventId: 234,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("transition state change called:" + JSON.stringify(eventData));
      if (eventData.data.opacity != null) {
        this.opacity = parseInt(eventData.data.opacity);
      }
    }
  }
}