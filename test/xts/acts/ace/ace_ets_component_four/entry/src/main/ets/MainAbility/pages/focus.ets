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
struct FocusableExample {
  @State textOne: string = ''
  @State textTwo: string = ''
  @State textThree: string = 'The third button cannot be focused'
  @State oneButtonColor: string = '#FF0000'
  @State twoButtonColor: string = '#FFC0CB'
  @State threeButtonColor: string = '#87CEFA'
  @State focusable: boolean = false

  build() {
    Column({ space:20 }){
      Button(this.textOne)
        .backgroundColor(this.oneButtonColor)
        .width(300).height(70).fontColor(Color.Black)
        .focusable(this.focusable)
        .onFocus(() => {
          this.textOne = 'First Button onFocus'
          this.oneButtonColor = '#AFEEEE'
        })
        .onBlur(() => {
          this.textOne = 'First Button onBlur'
          this.oneButtonColor = '#FFC0CB'
        })
      Button(this.textTwo)
        .backgroundColor(this.twoButtonColor)
        .width(300).height(70).fontColor(Color.Black)
        .focusable(this.focusable)
        .onFocus(() => {
          this.textTwo = 'Second Button onFocus'
          this.twoButtonColor = '#AFEEEE'
        })
        .onBlur(() => {
          this.textTwo = 'Second Button onBlur'
          this.twoButtonColor = '#FFC0CB'
        })
      Button(this.textThree)
        .backgroundColor(this.threeButtonColor)
        .width(300).height(70).fontColor(Color.Black)
        .focusable(this.focusable)
        .onFocus(() => {
          this.textThree = 'Third Button onFocus'
          this.threeButtonColor = '#AFEEEE'
        })
        .onFocusMove(()=>{
          console.info('on FocusMove Start!')
        })
        .onBlur(() => {
          this.textThree = 'Third Button onBlur'
          this.threeButtonColor = '#FFC0CB'
        })
    }.width('100%').margin({ top:20 })
  }

  onPageShow(){
    console.info('focus page show called');
    var stateChangeEvent = {
      eventId: 101,
      priority:events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("focus page state change called:" + JSON.stringify(eventData));
      if (eventData.data.focusable != null) {
        this.focusable = eventData.data.focusable;
      }
    }
  }
}