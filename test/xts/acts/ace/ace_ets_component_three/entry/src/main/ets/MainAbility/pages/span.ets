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
struct SpanExample {
  @State decorationValue: object= { type: TextDecorationType.None, color: Color.Red }
  @State textCaseValue: TextCase= TextCase.Normal
  @State fontSizeValue: number= 40
  @State fontColorValue: Color= 0xCCCCCC
  private stateChangCallBack = (eventData) => {
    console.info("span page state change called:" + JSON.stringify(eventData));
    if (eventData != null) {
      if (eventData.data.decorationValue != null) {
        this.decorationValue = JSON.parse(eventData.data.decorationValue);
      }
      if (eventData.data.textCaseValue != null) {
        this.textCaseValue = eventData.data.textCaseValue;
      }
      if (eventData.data.fontColorValue != null) {
        this.fontColorValue = eventData.data.fontColorValue;
      }
    }
  }

  onPageShow() {
    console.info('span page show called');
    var stateChangeEvent = {
      eventId: 199,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEvent2 = {
      eventId: 200,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent2, this.stateChangCallBack);

    var stateChangeEvent3 = {
      eventId: 201,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent3, this.stateChangCallBack);

    var stateChangeEvent4 = {
      eventId: 1029,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent4, this.stateChangCallBack);

    var stateChangeEvent5 = {
      eventId: 1030,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent5, this.stateChangCallBack);

    var stateChangeEvent6 = {
      eventId: 1031,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent6, this.stateChangCallBack);

    var stateChangeEvent7 = {
      eventId: 1032,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent7, this.stateChangCallBack);
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceBetween }) {
      Text('Basic Usage').fontSize(9).fontColor(this.fontColorValue).key('fontColor')
      Text() {
        Span('This is the Span component').fontSize(this.fontSizeValue).textCase(this.textCaseValue)
          .decoration(this.decorationValue).key('decoration')
      }
    }.width('100%').height(250).padding({ left: 35, right: 35, top: 35 })
  }
}