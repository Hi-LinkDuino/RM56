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
import prompt from '@system.prompt'

@Entry
@Preview
@Component
struct AppearExample {
  @State isShow: boolean = true
  @State appearFlag: boolean = false
  @State disAppearFlag: boolean = false
  private changeAppear: string = 'Hide Text'
  private myText: string = 'Text for onAppear'

  onPageShow() {
    console.info('[appear] page show called');
    var stateChangeEvent = {
      eventId: 58,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[appear] page stateChangCallBack");
    if (eventData != null) {
      console.info("[appear] page state change called:" + JSON.stringify(eventData));
      if (eventData.data.isShow != null) {
        this.isShow = eventData.data.isShow;
      }
    }
  }

  build() {
    Column() {
      Button(this.changeAppear)
        .key('appear')
        .onClick(() => {
          this.isShow = !this.isShow
        }).margin(3).backgroundColor(0x2788D9)
      if (this.isShow) {
        Text(this.myText)
          .onAppear(() => {
            this.appearFlag = true
            this.changeAppear = 'Show Text'
            console.info('appearFlag current action state is: ' + this.appearFlag);
            prompt.showToast({ message: 'The text is shown', duration: 2000 })
            try {
              var backData = {
                data: {
                  "ACTION": this.appearFlag,
                }
              }
              var backEvent = {
                eventId: 59,
                priority: events_emitter.EventPriority.LOW
              }
              console.info("appearFlag start to emit action state")
              events_emitter.emit(backEvent, backData)
            } catch {
              console.info("appearFlag emit action state err: " + JSON.stringify(err.message))
            }
          })
          .onDisAppear(() => {
            this.disAppearFlag = true
            this.changeAppear = 'Hide Text'
            console.info('disAppearFlag current action state is: ' + this.disAppearFlag);
            prompt.showToast({ message: 'The text is hidden', duration: 2000 })
            try {
              var backData = {
                data: {
                  "ACTION": this.disAppearFlag,
                }
              }
              var backEvent = {
                eventId: 60,
                priority: events_emitter.EventPriority.LOW
              }
              console.info("disAppearFlag start to emit action state")
              events_emitter.emit(backEvent, backData)
            } catch {
              console.info("disAppearFlag emit action state err: " + JSON.stringify(err.message))
            }
          })
      }
    }.padding(30).width('100%')
  }
}