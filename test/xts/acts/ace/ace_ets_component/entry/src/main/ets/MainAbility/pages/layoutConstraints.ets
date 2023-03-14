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

class ContainerInfo {
  label: string = ''
  size: string = ''
};

class ChildInfo {
  text: string = ''
  priority: number = 0
};

@Entry
@Component
struct LayoutConstraints {
  private children: string[] = ['1', '2', '3', '4', '5', '6'];
  @State aspectRatio: number = 1.5;
  private container: ContainerInfo[] = [
    { label: 'Big container', size: '97%' },
    { label: 'Small container', size: '30%' }];
  @State priorityChildren: any[] = [
    { text: '1\n(priority:1)', priority: 1 },
    { text: '2\n(priority:2)', priority: 2 },
    { text: '3\n(priority:3)', priority: 3 },
    { text: '4\n(priority:4)', priority: 4 }];
  @State currentIndex: number = 0;

  build() {
    Column({ space: 20 }) {
      Text('using container: row').fontSize(20).fontColor(Color.Black).width('100%')
      Row({ space: 10 }) {
        ForEach(this.children, (item) => {
          Text(item)
            .backgroundColor(0xbbb2cb)
            .fontSize(20)
            .aspectRatio(this.aspectRatio)
            .height(60)
          Text(item)
            .backgroundColor(0xbbb2cb)
            .fontSize(20)
            .aspectRatio(this.aspectRatio)
            .width(60)
            .key('aspectRatio')
        }, item => item)
      }
      .size({ width: "100%", height: 100 })
      .backgroundColor(0xd2cab3)
      .clip(true)

      Text('using container: grid').fontSize(20).fontColor(Color.Black).width('100%')
      Grid() {
        ForEach(this.children, (item) => {
          GridItem() {
            Text(item)
              .backgroundColor(0xbbb2cb)
              .fontSize(40)
              .aspectRatio(this.aspectRatio)
          }
        }, item => item)
      }
      .columnsTemplate('1fr 1fr 1fr')
      .columnsGap(10)
      .rowsGap(10)
      .size({ width: "100%", height: 165 })
      .backgroundColor(0xd2cab3)

      Text('displayPriority 4').fontSize(20).fontColor(Color.Black).width('100%')
      Flex({ justifyContent: FlexAlign.SpaceBetween }) {
        ForEach(this.priorityChildren, (item) => {
          if (item.priority === 1 || item.priority === 5) {
            Text(`1\n(priority:${item.priority})`)
              .width(135)
              .height(60)
              .fontSize(18)
              .textAlign(TextAlign.Center)
              .backgroundColor(0xbbb2cb)
              .displayPriority(item.priority)
              .key('displayPriority')
          } else {
            Text(item.text)
              .width(135)
              .height(60)
              .fontSize(18)
              .textAlign(TextAlign.Center)
              .backgroundColor(0xbbb2cb)
              .displayPriority(item.priority)
          }
        }, item => item.text)
      }
      .width(this.container[this.currentIndex].size)
      .backgroundColor(0xd2cab3)
    }.padding(10)
  }

  onPageShow() {
    let aspectRatioChangeEvent = {
      eventId: 20,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(aspectRatioChangeEvent, this.stateChangCallBack);

    let priorityChangeEvent = {
      eventId: 21,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(priorityChangeEvent, this.stateChangCallBack);

    let priorityChangeEvent2 = {
      eventId: 1112,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(priorityChangeEvent2, this.stateChangCallBack);
  }

  stateChangCallBack = (eventData) => {
    console.info("LayoutConstraints page stateChangCallBack" + JSON.stringify(eventData));
    if (eventData) {
      if (eventData.data.aspectRatio) {
        this.aspectRatio = eventData.data.aspectRatio;
      } else if (eventData.data.displayPriority) {
        let priority = JSON.parse(JSON.stringify(this.priorityChildren));
        priority[0].priority = eventData.data.displayPriority;
        this.priorityChildren = [];
        this.priorityChildren = priority;
      }
    }
  }
}