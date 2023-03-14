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
struct PlacementExample {
  @State customPopup: boolean = false
  @State customPopupOne: boolean = false
  @State customPopupTwo: boolean = false
  @State customPopupThree: boolean = false
  @State placement: Placement = Placement.TopLeft
  @State placementOne: Placement = Placement.TopRight
  @State placementTwo: Placement = Placement.BottomLeft
  @State placementThree: Placement = Placement.BottomRight

  @Builder popupBuilder() {
    Row({ space: 2 }) {
      Text('Custom Popup').fontSize(12)
    }.width(100).height(50).backgroundColor(Color.White).key('image')
  }
  @Builder popupBuilderOne() {
    Row({ space: 2 }) {
      Text('Custom PopupOne').fontSize(12)
    }.width(100).height(50).backgroundColor(Color.White).key('image1')
  }
  @Builder popupBuilderTwo() {
    Row({ space: 2 }) {
      Text('Custom PopupTwo').fontSize(12)
    }.width(100).height(50).backgroundColor(Color.White).key('image2')
  }
  @Builder popupBuilderThree() {
    Row({ space: 2 }) {
      Text('Custom PopupThree').fontSize(12)
    }.width(100).height(50).backgroundColor(Color.White).key('image3')
  }

  build() {
    Flex({ direction: FlexDirection.Column }) {
      Button('custom popup')
        .key('button')
        .onClick(() => {
          this.customPopup = !this.customPopup
          try{
            var backData = {
              data:{
                "placement":this.placement
              }
            }
            var backEvent = {
              eventId: 157,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button start to emit action state")
            events_emitter.emit(backEvent, backData)
          }catch(err){
            console.info("button emit action state err: " + JSON.stringify(err.message))
          }
        })
        .bindPopup(this.customPopup, {
          builder: this.popupBuilder,
          placement: this.placement,
          maskColor: 0x33000000,
          popupColor: Color.White,
          enableArrow: true,
          onStateChange: (e) => {
            if (!e.isVisible) {
              this.customPopup = false
            }
          }
        })
        .position({ x: 100, y: 50 })

      Button('custom popupOne')
        .key('button1')
        .onClick(() => {
          this.customPopupOne = !this.customPopupOne
          try{
            var backDataOne = {
              data:{
                "placement":this.placementOne
              }
            }
            var backEventOne = {
              eventId: 158,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button1 start to emit action state")
            events_emitter.emit(backEventOne, backDataOne)
          }catch(err){
            console.info("button1 emit action state err: " + JSON.stringify(err.message))
          }
        })
        .bindPopup(this.customPopupOne, {
          builder: this.popupBuilderOne,
          placement: this.placementOne,
          maskColor: 0x33000000,
          popupColor: Color.White,
          enableArrow: true,
          onStateChange: (e) => {
            if (!e.isVisible) {
              this.customPopupOne = false
            }
          }
        })
        .position({ x: 100, y: 150 })

      Button('custom popupTwo')
        .key('button2')
        .onClick(() => {
          this.customPopupTwo = !this.customPopupTwo
          try{
            var backDataTwo = {
              data:{
                "placement":this.placementTwo
              }
            }
            var backEventTwo = {
              eventId: 159,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button2 start to emit action state")
            events_emitter.emit(backEventTwo, backDataTwo)
          }catch(err){
            console.info("button2 emit action state err: " + JSON.stringify(err.message))
          }
        })
        .bindPopup(this.customPopupTwo, {
          builder: this.popupBuilderTwo,
          placement: this.placementTwo,
          maskColor: 0x33000000,
          popupColor: Color.White,
          enableArrow: true,
          onStateChange: (e) => {
            if (!e.isVisible) {
              this.customPopupTwo = false
            }
          }
        })
        .position({ x: 100, y: 250 })

      Button('custom popupThree')
        .key('button3')
        .onClick(() => {
          this.customPopupThree = !this.customPopupThree
          try{
            var backDataThree = {
              data:{
                "placement":this.placementThree
              }
            }
            var backEventThree = {
              eventId: 160,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button3 start to emit action state")
            events_emitter.emit(backEventThree, backDataThree)
          }catch(err){
            console.info("button3 emit action state err: " + JSON.stringify(err.message))
          }
        })
        .bindPopup(this.customPopupThree, {
          builder: this.popupBuilderThree,
          placement: this.placementThree,
          maskColor: 0x33000000,
          popupColor: Color.White,
          enableArrow: true,
          onStateChange: (e) => {
            if (!e.isVisible) {
              this.customPopupThree = false
            }
          }
        })
        .position({ x: 100, y: 550 })
    }.width('100%').padding({ top: 5 })
  }
}