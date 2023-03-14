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
struct Second {
  @State accumulateTime: number = 0;
  @State fontSize: number = 20;
  @State fontColor: Color= Color.Red;
  @State format: string = "hhmmss";
  @State onActionCalled: boolean = false;
  controller: TextClockController = new TextClockController();

  onPageShow() {
    console.info('swiper page show called');
    var stateChangeEvent = {
      eventId: 219,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)

    var stateChangeEventOne = {
      eventId: 220,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventOne, this.stateChangCallBack)

    var stateChangeEventTwo = {
      eventId: 221,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBack)

    var stateChangeEventThree = {
      eventId: 222,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventThree, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("swiper page state change called:" + JSON.stringify(eventData));
      if (eventData.data.fontSize != null) {
        this.fontSize = eventData.data.fontSize;
      }
      if (eventData.data.fontColor != null) {
        this.fontColor = eventData.data.fontColor;
      }
      if (eventData.data.format != null) {
        this.format = eventData.data.format;
      }
      if (eventData.data.onActionCalled != null) {
        this.onActionCalled = eventData.data.onActionCalled;
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center}) {
      Text('current milliseconds is' + this.accumulateTime)
        .fontSize(this.fontSize)
        .fontColor(this.fontColor)
      TextClock({timeZoneOffset: -8, controller: this.controller})
        .key('textClock')
        .format(this.format)
        .fontSize(this.fontSize)
        .fontColor(this.fontColor)
        .onDateChange((value: number) => {
          this.accumulateTime = value
          this.onActionCalled= true;
          console.info('onChange current action state is: ' + this.onActionCalled);
          try {
            var backData = {
              data: {
                "ACTION": this.onActionCalled,
              }
            }
            var backEvent = {
              eventId: 223,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("onChange start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("onChange emit action state err: " + JSON.stringify(err.message))
          }
        })
        .margin(20)
        .fontSize(30)
      Button("start TextClock")
        .margin({ bottom: 10 })
        .onClick(()=>{
          this.controller.start()
        })
      Button("stop TextClock")
        .key('button')
        .onClick(()=>{
          this.controller.stop()
          this.onActionCalledOne = true;
          console.info('button2 current action state is: ' + this.onActionCalledOne);
          try {
            var backData = {
              data: {
                "ACTION": this.onActionCalledOne,
              }
            }
            var backEvent = {
              eventId: 224,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button2 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button2 emit action state err: " + JSON.stringify(err.message))
          }
        })
    }
    .width('100%')
    .height('100%')
  }
}