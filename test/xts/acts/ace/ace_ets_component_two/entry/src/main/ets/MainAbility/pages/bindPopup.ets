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
struct PopupExample {
  @State noHandlePopup: boolean = false
  @State handlePopup: boolean = false
  @State customPopup: boolean = false
  @State maskColor: string = 0x33000000
  @State enableArrow: boolean = true
  @State placementOnTop: boolean = false

  @Builder popupBuilder() {
    Row({ space: 2 }) {
      Image($rawfile('ic_health_heart.png')).width(24).height(24).margin({ left: -5 }).key('image')
      Text('Custom Popup').fontSize(12).key('text')
    }.width(100).height(50).backgroundColor(Color.White)
  }

  build() {
    Flex({ direction: FlexDirection.Column }) {
      Button('no handle popup')
        .key('button2')
        .onClick(() => {
          this.noHandlePopup = !this.noHandlePopup
          try {
            var backData = {
              data: {
                "placementOnTop": this.placementOnTop,
              }
            }
            var backEvent = {
              eventId: 65,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button2 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button2 emit action state err: " + JSON.stringify(err.message))
          }
        })
        .bindPopup(this.noHandlePopup, {
          message: 'content1 content1',
          placementOnTop: this.placementOnTop,
          onStateChange: (e) => {
            console.info(e.isVisible.toString())
            if (!e.isVisible) {
              this.noHandlePopup = false
            }
          }
        })
        .position({ x: 100, y: 50 })

      Button('custom popup 1')
        .key('button1')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "maskColor": this.maskColor,
                "enableArrow": this.enableArrow
              }
            }
            var backEvent = {
              eventId: 66,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button1 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button1 emit action state err: " + JSON.stringify(err.message))
          }
        })
        .position({ x: 100, y: 200 })

      Button('custom popup')
        .key('button')
        .onClick(() => {
          this.customPopup = !this.customPopup
          try {
            var backData = {
              data: {
                "maskColor": this.maskColor,
                "enableArrow": this.enableArrow
              }
            }
            var backEvent = {
              eventId: 67,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button emit action state err: " + JSON.stringify(err.message))
          }
        })
        .bindPopup(this.customPopup, {
          builder: this.popupBuilder,
          placement: Placement.Bottom,
          maskColor: this.maskColor,
          popupColor: Color.White,
          enableArrow: this.enableArrow,
          onStateChange: (e) => {
            if (!e.isVisible) {
              this.customPopup = false
            }
          }
        })
        .position({ x: 100, y: 350 })
    }.width('100%').padding({ top: 5 })
  }

  onPageShow() {
    console.info('customPopupOption page show called');
    var stateChangeEvent = {
      eventId: 68,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("customPopupOption page state change called:" + JSON.stringify(eventData));
      if (eventData.data.enableArrow != null) {
        this.enableArrow = eventData.data.enableArrow;
      }
      if (eventData.data.maskColor != null) {
        this.maskColor = eventData.data.maskColor;
      }
    }
  }
}