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
@Preview
@Component
struct TouchExample {
  @State text: string = ''
  @State eventType: string = ''
  @State touchFlag: boolean = false

  onPageShow() {
    console.info('[touch] page show called');
    var stateChangeEvent = {
      eventId: 42,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[touch] page stateChangCallBack");
    if (eventData != null) {
      console.info("[touch] page state change called:" + JSON.stringify(eventData));
      if (eventData.data.text != null) {
        this.text = eventData.data.text;
      }
      if (eventData.data.eventType != null) {
        this.eventType = eventData.data.eventType;
      }
      if (eventData.data.touchFlag != null) {
        this.touchFlag = eventData.data.touchFlag
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Button('Touch')
        .backgroundColor(0x2788D9)
        .height(40)
        .width(80)
        .key('touch')
        .onTouch((event: Touch) => {
          if (event.type === TouchType.Down) {
            this.eventType = 'Down'
            console.info("[touch] eventType is Down")
            this.touchFlag = true
          }
          if (event.type === TouchType.Up) {
            this.eventType = 'Up'
            console.info("[touch] eventType is Up")
            this.touchFlag = true
          }
          if (event.type === TouchType.Move) {
            this.eventType = 'Move'
            console.info("[touch] eventType is Move")
            this.touchFlag = true
          }
          stopPropagation: () => {
            console.info('stopPropagation')
          }
          console.info('onTouch successful, touchFlag is : ' + this.eventType + ', ' + this.touchFlag)
          console.info(this.text = 'TouchType:' + this.eventType
          + '\nDistance between touch point and touch element:\nx: '
          + event.touches[0].x + '\n' + 'y: ' + event.touches[0].y + '\ncomponent globalPos：（'
          + event.target.area.globalPos.x + ',' + event.target.area.globalPos.y + '）\nwidth:'
          + event.target.area.width + '\nheight：' + event.target.area.height)
          console.info(this.text = 'TouchType:' + this.eventType
          + '\nDistance between touch point and touch element:\nx: '
          + event.changedTouches[0].x + '\n' + 'y: ' + event.changedTouches[0].y + '\ncomponent globalPos：（'
          + event.target.area.globalPos.x + ',' + event.target.area.globalPos.y + '）\nwidth:'
          + event.target.area.width + '\nheight：' + event.target.area.height)
          console.log('fang--->A')
          try {
            console.log('fang--->B')
            var backData = {
              data: {
                "ACTION": this.touchFlag,
              }
            }
            console.log('fang--->C')
            var backEvent = {
              eventId: 43,
              priority: events_emitter.EventPriority.LOW
            }
            console.log('fang--->D')
            console.info("touchFlag start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch {
            console.info("touchFlag emit action state err: " + JSON.stringify(err.message))
          }
        })
      Text(this.text)
    }.height(200).width(350)
  }
}