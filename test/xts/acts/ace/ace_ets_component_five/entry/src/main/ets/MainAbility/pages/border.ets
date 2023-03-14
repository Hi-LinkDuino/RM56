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
struct BorderExample {
  @State borderStyle: BorderStyle = BorderStyle.Auto;
  @State borderRadius: number = 70;

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("Border page state change called:" + JSON.stringify(eventData));
      if(eventData.data.border != null) {
        this.border = eventData.data.border;
      }
      if(eventData.data.borderStyle != null) {
        this.borderStyle = eventData.data.borderStyle;
      }
      if(eventData.data.borderWidth != null) {
        this.borderWidth = eventData.data.borderWidth;
      }
      if(eventData.data.borderColor != null) {
        this.borderColor = eventData.data.borderColor;
      }
      if(eventData.data.borderRadius != null) {
        this.borderRadius = eventData.data.borderRadius;
      }
      console.info("Border borderStyle state change called:" + JSON.stringify(eventData));
      switch(eventData.data.borderStyle) {
        case 'Dotted':
          this.borderStyle = BorderStyle.Dotted;
          break;
        case 'Dashed':
          this.borderStyle = BorderStyle.Dashed;
          break;
        case 'Solid':
          this.borderStyle = BorderStyle.Solid;
          break;
        default:
          this.borderStyle = BorderStyle.Solid;
      }
    }
  }


  onPageShow() {
    console.info('border page show called');
    var stateChangeEvent = {
      eventId: 3,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Flex({ justifyContent: FlexAlign.SpaceAround, alignItems: ItemAlign.Center }) {
      Text('dashed')
        .borderStyle(BorderStyle.Dashed).borderWidth(5)
        .borderColor(0xAFEEEE)
        .borderRadius(this.borderRadius).key("borderRadius")
        .width(120).height(120).textAlign(TextAlign.Center).fontSize(16)
      Text('Solid')
        .borderStyle(BorderStyle.Solid).borderWidth(5).key("borderWidth")
        .borderColor(0xEE0000)
        .borderRadius(10)
        .width(120).height(120).textAlign(TextAlign.Center).fontSize(16)
      Text('dotted')
        .border({ width: 5, color: 0x317AF7, radius: 8, style: BorderStyle.Dotted })
        .key("borderColor")
        .width(120).height(120).textAlign(TextAlign.Center).fontSize(16)
    }.width('100%').height(150)
  }
}