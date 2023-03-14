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
struct CheckboxGroupExample {
  @State select: boolean = false

  private stateChangCallBack = (eventData) => {
    console.info("checkboxGroup page stateChangCallBack");
    if (eventData != null) {
      console.info("checkboxGroup page state change called:" + JSON.stringify(eventData));
      if(eventData.data.select != null) {
        this.select = eventData.data.select;
      }
    }
  }

  onPageShow() {
    console.info('checkboxGroup page show called');
    var stateChangeEvent = {
      eventId: 75,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  build() {
    Scroll() {
      Column() {
        CheckboxGroup({group : 'checkboxGroup'})
          .selectedColor(0xed6f21)
          .key('CheckboxGroup')
          .onChange((itemName:CheckboxGroupResult) => {
            console.info("TextPicker::dialogResult is" + JSON.stringify(itemName))
          })
        Checkbox({ name: 'checkbox1', group: 'checkboxGroup' })
          .select(true)
          .selectedColor(0x39a2db)
          .onChange((value: boolean) => {
            console.info('Checkbox1 change is' + value)
          })
        Checkbox({ name: 'checkbox2', group: 'checkboxGroup' })
          .select(false)
          .selectedColor(0x39a2db)
          .onChange((value: boolean) => {
            console.info('Checkbox2 change is' + value)
          })
        Checkbox({ name: 'checkbox3', group: 'checkboxGroup' })
          .select(true)
          .selectedColor(0x39a2db)
          .onChange((value: boolean) => {
            console.info('Checkbox3 change is' + value)
          })
      }
    }
  }
}