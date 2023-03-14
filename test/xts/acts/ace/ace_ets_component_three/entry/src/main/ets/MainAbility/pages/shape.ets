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
struct ShapeExample {
  @State strokeDashOffset: number = 0;
  @State strokeLineCap: LineCapStyle = LineCapStyle.Butt;
  @State strokeLineJoin: LineJoinStyle = LineJoinStyle.Miter;
  @State strokeMiterLimit: number = 4;
  @State strokeOpacity: number = 1;
  @State strokeWidth: number = 1;
  @State antiAlias: boolean = true;
  @State strokeDashArray: Array<Length> = [20];
  @State fillOpacity: number = 0;
  @State strokeValue: Color = '#FF000000';
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("text page state change called:" + JSON.stringify(eventData));
      if (eventData.data.strokeDashOffset != null) {
        this.strokeDashOffset = parseInt(eventData.data.strokeDashOffset);
      }
      if (eventData.data.strokeLineCap != null) {
        this.strokeLineCap = eventData.data.strokeLineCap;
      }
      if (eventData.data.strokeLineJoin != null) {
        this.strokeLineJoin = eventData.data.strokeLineJoin;
      }
      if (eventData.data.strokeMiterLimit != null) {
        this.strokeMiterLimit = parseInt(eventData.data.strokeMiterLimit);
      }
      if (eventData.data.strokeOpacity != null) {
        this.strokeOpacity = parseInt(eventData.data.strokeOpacity);
      }
      if (eventData.data.fillOpacity != null) {
        this.fillOpacity = parseInt(eventData.data.fillOpacity);
      }
      if (eventData.data.antiAlias != null) {
        this.antiAlias = eventData.data.antiAlias;
      }
      if (eventData.data.strokeDashArrayOne != null) {
        this.strokeDashArray[0] = parseInt(eventData.data.strokeDashArrayOne);
      }
      if (eventData.data.strokeDashArrayTwo != null) {
        this.strokeDashArray[1] = parseInt(eventData.data.strokeDashArrayTwo);
      }
      if (eventData.data.strokeDashArrayThree != null) {
        this.strokeDashArray[2] = parseInt(eventData.data.strokeDashArrayThree);
      }
      if (eventData.data.stroke != null) {
        this.stroke = eventData.data.stroke;
      }
    }
  }

  onPageShow() {
    console.info('qrCode page show called');
    var stateChangeEvent = {
      eventId: 192,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Column({ space: 2 }) {
      Text('basic').fontSize(30).fontColor(0xCCCCCC).width(320)
      Shape() {
        Rect().width(300).height(50)
        Ellipse().width(300).height(50).offset({ x: 0, y: 60 })
        Path().width(300).height(10).commands('M0 0 L900 0').offset({ x: 0, y: 120 })
      }
      .key('shape')
      .viewPort({ x: -2, y: -2, width: 304, height: 130 })
      .fill(0x317Af7)
      .stroke(this.strokeValue)
      .strokeDashArray(this.strokeDashArray)
      .strokeDashOffset(this.strokeDashOffset)
      .strokeLineCap(this.strokeLineCap)
      .strokeLineJoin(this.strokeLineJoin)
      .strokeMiterLimit(this.strokeMiterLimit)
      .strokeOpacity(this.strokeOpacity)
      .strokeWidth(this.strokeWidth)
      .antiAlias(this.antiAlias)
      .fillOpacity(this.fillOpacity)
      .mesh([], 1, 1)
    }.width('100%').margin({ top: 15 })
  }
}