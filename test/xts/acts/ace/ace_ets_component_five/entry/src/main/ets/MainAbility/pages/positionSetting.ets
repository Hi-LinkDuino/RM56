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
struct PositionSetting {
  @State align: Alignment = Alignment.TopStart;
  @State direction: Direction = Direction.Rtl;
  @State position1: {x: string, y: string} = {x: 25, y: 15};
  @State position2: {x: string, y: string} = {x: '50%', y: '70%'};
  @State anchor: {x: string, y: string} = {x: 25, y: 25};
  @State offset: {x: string, y: string} = {x: 10, y: 15};
  @State opacityValue: string = 1;

  build() {
    Column() {
      Column({space: 10}) {
        Text('opacity(1)').fontSize(20).fontColor(Color.Black).width('90%').opacity(`${this.opacityValue}`).key("opacity")
        Text('top start')
          .align(this.align)
          .height(50)
          .width('90%')
          .fontSize(20)
          .backgroundColor(0xFFE4C4)
          .key('textAlign')

        Text('direction').fontSize(20).fontColor(Color.Black).width('90%')
        Row() {
          Text('1').height(50).width('25%').fontSize(20).backgroundColor(0xF5DEB3)
          Text('2').height(50).width('25%').fontSize(20).backgroundColor(0xD2B48C)
          Text('3').height(50).width('25%').fontSize(20).backgroundColor(0xF5DEB3)
          Text('4').height(50).width('25%').fontSize(20).backgroundColor(0xD2B48C)
        }
        .width('90%')
        .direction(this.direction)
        .key('rowDirection')

        Text('position').fontSize(20).fontColor(Color.Black).width('90%')
        Row({ space: 20 }) {
          Text('1')
            .size({ width: '45%', height: '50' })
            .backgroundColor(0xdeb887)
            .border({ width: 1 })
            .fontSize(20)
          Text(`2 position(${this.position1.x}, ${this.position1.y})`)
            .size({ width: '60%', height: '30' })
            .backgroundColor(0xbbb2cb).border({ width: 1 })
            .fontSize(20).align(Alignment.Start)
            .position({ x: this.position1.x, y: this.position1.y })
            .key('position1')
          Text('3')
            .size({ width: '45%', height: '50' })
            .backgroundColor(0xdeb887)
            .border({ width: 1 }).fontSize(20)
          Text(`4 position(${this.position2.x}, ${this.position2.y})`)
            .size({ width: '50%', height: '50' })
            .backgroundColor(0xbbb2cb)
            .border({ width: 1 }).fontSize(20)
            .position({ x: this.position2.x, y: this.position2.y })
            .key('position2')
        }.width('90%').height(100).border({ width: 1, style: BorderStyle.Dashed })

        Text('markAnchor').fontSize(20).fontColor(Color.Black).width('90%')
        Stack({ alignContent: Alignment.TopStart }) {
          Row()
            .size({ width: '100', height: '100' })
            .backgroundColor(0xdeb887)
          Image('/images/ic_health_heart.png')
            .size({ width: 25, height: 25 })
            .markAnchor({ x: this.anchor.x, y: this.anchor.y })
            .key('markAnchor')
          Image('/images/ic_health_heart.png')
            .size({ width: 25, height: 25 })
            .markAnchor({ x: 25, y: 25 })
            .position({ x: '100%', y: '100%' })
        }.margin({ top: 25 }).border({ width: 1, style: BorderStyle.Dashed })

        Text('offset').fontSize(20).fontColor(Color.Black).width('90%')
        Row() {
          Text('1').size({ width: '15%', height: '100' }).backgroundColor(0xdeb887).border({ width: 1 }).fontSize(20)
          Text(`2\noffset(${this.offset.x}, ${this.offset.y})`)
            .size({ width: 170, height: '50' }).backgroundColor(0xbbb2cb).border({ width: 1 })
            .fontSize(20).align(Alignment.Start)
            .offset({ x: this.offset.x, y: this.offset.y })
            .key('offset')
          Text('3').size({ width: '15%', height: '50' }).backgroundColor(0xdeb887).border({ width: 1 }).fontSize(20)
          Text('4\noffset(-10%, 20%)')
            .size({ width: 170, height: '50' }) .backgroundColor(0xbbb2cb).border({ width: 1 }).fontSize(20)
            .offset({ x: '-10%', y: '50%' })
        }.width('90%').height(100).border({ width: 1, style: BorderStyle.Dashed })
      }
    }
    .width('100%').margin({ top: 5 })
  }
  onPageShow() {
    let stateChangeEvent = {
      eventId: 25,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    let directionChangeEvent = {
      eventId: 26,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(directionChangeEvent, this.stateChangCallBack);

    let positionChangeEvent = {
      eventId: 27,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(positionChangeEvent, this.stateChangCallBack);

    let markAnchorChangeEvent = {
      eventId: 28,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(markAnchorChangeEvent, this.stateChangCallBack);

    let offsetChangeEvent = {
      eventId: 29,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(offsetChangeEvent, this.stateChangCallBack);
  }
  stateChangCallBack = (eventData) => {
    console.info("position page stateChangCallBack" + JSON.stringify(eventData));
    if (eventData) {
      if (eventData.data.textAlign) {
        this.align = Alignment.BottomEnd;
      } else if (eventData.data.direction) {
        this.direction = eventData.data.direction;
      } else if (eventData.data.position1) {
        this.position1 = JSON.parse(eventData.data.position1);
        this.position2 = JSON.parse(eventData.data.position2);
      } else if (eventData.data.markAnchor) {
        this.anchor = JSON.parse(eventData.data.markAnchor);
      } else if (eventData.data.offset) {
        this.offset = JSON.parse(eventData.data.offset);
      } else if (eventData.data.opacityValue) {
        this.opacityValue = eventData.data.opacityValue;
      }
    }
  }
}