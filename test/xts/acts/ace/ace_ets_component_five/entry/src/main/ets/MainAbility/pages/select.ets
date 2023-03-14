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
struct SliderExample {
  @State value:string=10
  @State font:object={size: 30, weight:400, family: 'serif', style: FontStyle.Normal }
  @State fontColor: Color= 0xCCCCCC

  private stateChangCallBack = (eventData) => {
    console.info("select page state change called:" + JSON.stringify(eventData));
    if (eventData != null) {
      if (eventData.data.value != null) {
        this.value = eventData.data.value;
      }
      if (eventData.data.font != null) {
        this.font = JSON.parse(eventData.data.font);
      }
      if (eventData.data.fontColor != null) {
        this.fontColor = eventData.data.fontColor;
      }
    }
  }

  onPageShow() {
    console.info('select page show called');
    var stateChangeEvent = {
      eventId: 191,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Column() {
      Select([{value:'aaa',icon: "/images/img.png"},
              {value:'bbb',icon: "/images/img.png"},
              {value:'ccc',icon: "/images/img.png"},
              {value:'ddd',icon: "/images/img.png"}])
        .selected(2)
        .value('TTT')
        .value(this.value)
        .font(this.font)
        .fontColor(this.fontColor)
        .selectedOptionFont({size: 40, weight: 500, family: 'serif', style: FontStyle.Normal })
        .optionFont({size: 30, weight: 400, family: 'serif', style: FontStyle.Normal })
        .key('select')
        .onSelected((index:number)=>{
          console.info("Select:" + index)
        })
    }
  }
}