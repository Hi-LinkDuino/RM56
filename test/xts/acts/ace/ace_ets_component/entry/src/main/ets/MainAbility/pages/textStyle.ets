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

import events_emitter from '@ohos.emitter';

@Entry
@Component
struct TextStyle {
  @State fontColor: Color = Color.Red;
  @State fontSize: string = 20;
  @State fontStyle: FontStyle = FontStyle.Italic;
  @State fontWeight: string = 700;
  @State fontFamily: string = 'sans-serif';

  build() {
    Column({ space: 20 }) {
      Text('default text').fontSize(20)

      Text(`text font color ${this.fontColor}`)
        .fontColor(this.fontColor)
        .fontSize(20)
        .key('fontColor')

      Text(`text font size ${this.fontSize}`)
        .fontSize(this.fontSize)
        .key('fontSize')

      Text(`text font style ${this.fontStyle}`)
        .fontStyle(this.fontStyle)
        .fontSize(20)
        .key('fontStyle')

      Text(`text fontWeight ${this.fontWeight}`)
        .fontWeight(this.fontWeight)
        .fontSize(20)
        .key('fontWeight')

      Text(`text fontFamily ${this.fontFamily}`)
        .fontFamily(this.fontFamily)
        .fontSize(20)
        .key('fontFamily')

      Text('red 20 Italic bold cursive text')
        .fontColor(Color.Red)
        .fontSize(20)
        .fontStyle(FontStyle.Italic)
        .fontWeight(700)
        .fontFamily('cursive')
        .textAlign(TextAlign.Center)
        .width('90%')

      Text('Orange 18 Normal source-sans-pro text')
        .fontColor(Color.Orange)
        .fontSize(20)
        .fontStyle(FontStyle.Normal)
        .fontWeight(400)
        .fontFamily('source-sans-pro,cursive,sans-serif')
    }.width('100%').margin({ top: 20 })
  }

  onPageShow() {
    let fontColorChangeEvent = {
      eventId: 37,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(fontColorChangeEvent, this.textStyleChangCallBack);
    let fontSizeChangeEvent = {
      eventId: 38,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(fontSizeChangeEvent, this.textStyleChangCallBack);
    let fontStyleChangeEvent = {
      eventId: 39,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(fontStyleChangeEvent, this.textStyleChangCallBack);
    let fontWeightChangeEvent = {
      eventId: 40,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(fontWeightChangeEvent, this.textStyleChangCallBack);
    let fontFamilyChangeEvent = {
      eventId: 41,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(fontFamilyChangeEvent, this.textStyleChangCallBack);
  }

  textStyleChangCallBack = (eventData) => {
    console.info("textStyle page stateChangCallBack" + JSON.stringify(eventData));
    if (eventData && eventData.data) {
      if (eventData.data.fontColor) {
        this.fontColor = eventData.data.fontColor;
      } else if (eventData.data.fontSize) {
        this.fontSize = eventData.data.fontSize;
      } else if (eventData.data.fontWeight) {
        this.fontWeight = eventData.data.fontWeight;
      } else if (eventData.data.fontFamily) {
        this.fontFamily = eventData.data.fontFamily;
      } else {
        this.fontStyle = eventData.data.fontStyle;
      }
    }
  }
}