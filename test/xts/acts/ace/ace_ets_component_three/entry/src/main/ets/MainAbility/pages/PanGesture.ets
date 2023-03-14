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
struct PanGestureExample {
  @State offsetX: number = 0
  @State offsetY: number = 0
  @State touchable: boolean = true;
  @State isTouched: boolean = false;

  onPageShow() {
    let ChangeEvent = {
      eventId: 23,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(ChangeEvent, this.ChangCallBack)
  }

  private ChangCallBack = (eventData) => {
    console.info("[PanGesture] ChangCallBack stateChangCallBack");
    if (eventData != null) {
      console.info("[PanGesture] ChangCallBack state change called:" + JSON.stringify(eventData));
      if (eventData.data.touchable != null) {
        this.touchable = eventData.data.touchable;
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Text('PanGesture offset:\nX: ' + this.offsetX + '\n' + 'Y: ' + this.offsetY)
    }
    .height(300)
    .width(300)
    .padding(20)
    .border({ width: 1 })
    .margin(80)
    .key('PanGesture')
    .translate({ x: this.offsetX, y: this.offsetY, z: 5 })
    .touchable(this.touchable)
    .gesture(
    PanGesture({})
      .setDirection(PanDirection.All)
      .setDistance(1)
      .setFingers(1)
      .onActionStart((event: PanGestureEvent) => {
        console.info('Pan start')
        console.log('PanGesture globalX' + event.globalX);
        console.log('PanGesture globalY' + event.globalY);
        console.log('PanGesture localX' + event.localX);
        console.log('PanGesture localY' + event.localY);
        console.log('PanGesture fingerList' + event.fingerList);
        console.log('PanGesture FingerInfo' + event.fingerList.FingerInfo);
        console.log('PanGesture speed' + event.speed);
        console.log('PanGesture pinchCenterX' + event.pinchCenterX);
        console.log('PanGesture pinchCenterY' + event.pinchCenterY);
      })
      .onActionUpdate((event: PanGestureEvent) => {
        this.offsetX = event.offsetX
        this.offsetY = event.offsetY
      })
      .onActionEnd(() => {
        console.info('Pan end')
      })
      .onTouch((event: TouchEvent) => {
        console.log('[PanGesture] TouchType start');
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
        console.info('[PanGesture] TouchType:' + this.eventType)
        try {
          var backData = {
            data: {
              "ACTION": this.isTouched,
              "EVENT_TYPE": this.eventType
            }
          }
          var backEvent = {
            eventId: 24,
            priority: events_emitter.EventPriority.LOW
          }
          console.info("[PanGesture] start to emit action state")
          events_emitter.emit(backEvent, backData)
        } catch (err) {
          console.info("[PanGesture] emit action state err: " + JSON.stringify(err.message))
        }
      })
    )
  }
}

