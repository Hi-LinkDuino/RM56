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
  @State outSetValue: number = 40
  @State inSetValue: number = 40
  @State outVerticalSetValue: number = 40
  @State inVerticalSetValue: number = 40
  @State stepValue: number = 1
  @State decorationValue: Axis = Axis.Vertical
  @State value: number = 40

  private stateChangCallBack = (eventData) => {
    console.info("slider page state change called:" + JSON.stringify(eventData));
    if (eventData != null) {
      if (eventData.data.decorationValue != null) {
        this.decorationValue = JSON.parse(eventData.data.decorationValue);
      }
      if (eventData.data.value != null) {
        this.value = eventData.data.value;
      }
      if (eventData.data.stepValue != null) {
        this.stepValue = eventData.data.stepValue;
      }
    }
  }

  onPageShow() {
    console.info('slider page show called');
    var stateChangeEvent = {
      eventId: 196,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Column({ space: 5 }) {
      Text('slider out set').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Row() {
        Slider({
          value: this.outSetValue,
          min: 0,
          max: 100,
          step: 1,
          style: SliderStyle.OutSet
        })
          .blockColor(Color.Blue)
          .trackColor(Color.Gray)
          .selectedColor(Color.Blue)
          .showSteps(true)
          .showTips(true)
          .onChange((value: number, mode: SliderChangeMode) => {
            this.outSetValue = value
            console.info('value:' + value + 'mode:' + mode.toString())
          })
        Text(this.outSetValue.toFixed(0)).fontSize(16)
      }
      .padding({ top: 50 })
      .width('80%')

      Text('slider in set').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Row() {
        Slider({
          value: this.inSetValue,
          min: 0,
          max: 100,
          step: 1,
          style: SliderStyle.InSet,
        })
          .blockColor(0xCCCCCC)
          .trackColor(Color.Black)
          .selectedColor(0xCCCCCC)
          .showSteps(false)
          .showTips(false)
          .onChange((value: number, mode: SliderChangeMode) => {
            this.inSetValue = value
            console.info('value:' + value + 'mode:' + mode.toString())
          })
        Text(this.inSetValue.toFixed(0)).fontSize(16)
      }
      .width('80%')

      Row() {
        Column() {
          Text('slider out direction set').fontSize(9).fontColor(0xCCCCCC).width('50%')
          Slider({
            value: this.outVerticalSetValue,
            min: 0,
            max: 100,
            step: 1,
            style: SliderStyle.OutSet,
            direction: Axis.Vertical
          })
            .blockColor(Color.Blue)
            .trackColor(Color.Gray)
            .selectedColor(Color.Blue)
            .showSteps(true)
            .showTips(true)
            .onChange((value: number, mode: SliderChangeMode) => {
              this.outVerticalSetValue = value
              console.info('value:' + value + 'mode:' + mode.toString())
            })
          Text(this.outVerticalSetValue.toFixed(0)).fontSize(16)
        }.width('50%').height(300)

        Column() {
          Text('slider in direction set').fontSize(9).fontColor(0xCCCCCC).width('50%')
          Slider({
            value: this.inVerticalSetValue,
            min: 0,
            max: 100,
            step: this.stepValue,
            style: SliderStyle.InSet,
            direction: this.decorationValue
          })
            .key('slider')
            .blockColor(0xCCCCCC)
            .trackColor(Color.Black)
            .selectedColor(0xCCCCCC)
            .showSteps(false)
            .showTips(false)
            .trackThickness(1)
            .onChange((value: number, mode: SliderChangeMode) => {
              this.inVerticalSetValue = value
              console.info('value:' + value + 'mode:' + mode.toString())
            })
          Text(this.inVerticalSetValue.toFixed(0)).fontSize(16)
        }.width('50%').height(300)
      }

    }.width('100%').margin({ top: 5 })
  }
}