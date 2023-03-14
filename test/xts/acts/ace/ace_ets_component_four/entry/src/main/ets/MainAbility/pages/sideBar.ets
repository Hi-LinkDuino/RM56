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
struct SideBarExample {
  normalIcon : Resource = ($rawfile("ic_health_heart.png"))
  selectedIcon: Resource = ($rawfile("test.png"))
  @State arr: number[] = [1, 2, 3]
  @State current: number = 1
  @State widthValue: number = 10
  @State heightValue: number = 10
  @State controlButton: ButtonStyle = ButtonStyle.left
  @State iconsValue: object= { shown: string=10, hidden: string=10, switching:string=10 };

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("sideBar page state change called:" + JSON.stringify(eventData));
      if (eventData.data.widthValue != null) {
        this.widthValue = eventData.data.widthValue;
      }
      if (eventData.data.heightValue != null) {
        this.heightValue = JSON.parse(eventData.data.heightValue);
      }
      if (eventData.data.iconsValue != null) {
        this.iconsValue = eventData.data.iconsValue;
      }
      if (eventData.data.controlButton != null) {
        this.controlButton = eventData.data.controlButton;
      }
    }
  }
  onPageShow() {
    console.info('sideBar page show called');
    var stateChangeEvent = {
      eventId: 195,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }
  build() {
    SideBarContainer(SideBarContainerType.Embed)
    {
      Column() {
        ForEach(this.arr, (item, index) => {
          Column({ space: 5 }) {
            Image(this.current === item ? this.selectedIcon : this.normalIcon).width(64).height(64)
            Text("Index0" + item)
              .fontSize(25)
              .fontColor(this.current === item ? '#0A59F7' : '#999')
              .fontFamily('source-sans-pro,cursive,sans-serif')
          }
          .onClick(() => {
            this.current = item
          })
        }, item => item)
      }.width('100%')
      .backgroundColor('#19000000')
      RowSplit() {
        Column(){
          Text('Split page one').fontSize(30)
        }
        Column(){
          Text('Split page two').fontSize(30)
        }
      }.width('100%')
    }
    .key('sidebar')
    .width(this.widthValue)
    .height(this.heightValue)
    .controlButton(this.controlButton)
    .sideBarWidth(240)
    .minSideBarWidth(210)
    .maxSideBarWidth(260)
    .onChange((value: boolean) => {
      console.info('status:' + value)
    })
  }
}

