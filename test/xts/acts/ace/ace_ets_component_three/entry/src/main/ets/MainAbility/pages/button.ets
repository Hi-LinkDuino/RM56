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
struct ButtonExample {
  @State buttonTypeValue: ButtonType = ButtonType.Normal;
  @State fontSizeValue: number = 20;
  @State fontColorValue: Color = Color.Black;
  @State text: string = '';
  @State onClickValue: boolean = false;
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("Button page state change called:" + JSON.stringify(eventData));
      if (eventData.data.fontSizeValue != null) {
        this.fontSizeValue = eventData.data.fontSizeValue;
      }
      if (eventData.data.fontColorValue != null) {
        this.fontColorValue = eventData.data.fontColorValue;
      }
      if (eventData.data.buttonTypeValue != null) {
        this.buttonTypeValue = eventData.data.buttonTypeValue;
      }
    }
  }

  onPageShow() {
    console.info('Button page show called');
    var stateChangeEvent = {
      eventId: 70,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
    var stateChangeEventOne = {
      eventId: 74,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventOne, this.stateChangCallBack);
    var stateChangeEventTwo = {
      eventId: 75,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBack);
    var stateChangeEventThree = {
      eventId: 76,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventThree, this.stateChangCallBack);
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceBetween }) {
      Button('Ok', { type: this.buttonTypeValue, stateEffect: true })
        .borderRadius(8).backgroundColor(0x317aff).width(90).key('button')
      Button('ok').type(`${this.buttonTypeValue}`).key('button1')
        .backgroundColor(0x317aff).width(90)
      Button('ok')
        .type(this.buttonTypeValue)
        .key('button2')
        .fontSize(this.fontSizeValue)
        .backgroundColor(0x317aff)
        .width(90)
        .fontColor(this.fontColorValue)
        .onClick(() => {
          try {
            var backData = {
              data: {
                "fontColorValue": this.fontColorValue,
              }
            }
            var backEvent = {
              eventId: 71,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button2 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button2 emit action state err: " + JSON.stringify(err.message))
          }
        })
      Button('ok!')
        .type(this.buttonTypeValue)
        .key('button3')
        .fontColor(this.fontColorValue)
        .backgroundColor(0x317aff)
        .width(90)
        .onClick(() => {
          try {
            var backData = {
              data: {
                "fontColorValue": this.fontColorValue,
              }
            }
            var backEvent = {
              eventId: 72,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button3 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button3 emit action state err: " + JSON.stringify(err.message))
          }
        })
      Button('Click')
        .backgroundColor(0x2788D9)
        .width(200)
        .height(100)
        .fontSize(20)
        .onClick((event: ClickEvent) => {
          console.info(this.text = 'Click Point:' + '\n  screenX:' + event.screenX + '\n  screenY:' + event.screenY
          + '\n  x :' + event.x + '\n  y:' + event.y + '\ntarget:' + '\n  component globalPos：（'
          + event.target.area.globalPos.x + ',' + event.target.area.globalPos.y + '）\n  width:'
          + event.target.area.width + '\n  height：' + event.target.area.height);
          this.onClickValue = true;
          try {
            var backData = {
              data: {
                "ACTION": this.onClickValue,
                "event": JSON.stringify(event)
              }
            }
            var backEvent = {
              eventId: 4,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("click to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("click action state err: " + JSON.stringify(err.message))
          }
        })
        .key('buttonClick')
    }.height(400).padding({ left: 35, right: 35, top: 35 })
  }
}