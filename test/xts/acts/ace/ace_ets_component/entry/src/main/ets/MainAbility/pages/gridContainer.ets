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
struct GridContainerExample {
  @State sizeType: SizeType = SizeType.XS;
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("gridContainer page state change called:" + JSON.stringify(eventData));
      switch (eventData.data.sizeType) {
        case 'Auto':
          this.sizeType = SizeType.Auto;
          break;
        case 'XS':
          this.sizeType = SizeType.XS;
          break;
        case 'SM':
          this.sizeType = SizeType.SM;
          break;
        case 'MD':
          this.sizeType = SizeType.MD;
          break;
        case 'LG':
          this.sizeType = SizeType.LG;
          break;
        default:
          this.sizeType = SizeType.Auto;
      }
    }
  }

  onPageShow() {
    console.info('gridContainer page show called');
    var stateChangeEvent = {
      eventId: 109,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Column({ space: 5 }) {
      GridContainer({ columns: 12, sizeType: this.sizeType, gutter: 10, margin: 20 }) {
        Row() {
          Text('1')
            .fontSize(20)
            .useSizeType({
              xs: { span: 6, offset: 0 },
              sm: { span: 2, offset: 0 },
              md: { span: 2, offset: 0 },
              lg: { span: 2, offset: 0 }
            })
            .height(50)
            .backgroundColor(0x4582B4)
            .textAlign(TextAlign.Center)
          Text('2')
            .fontSize(20)
            .useSizeType({
              xs: { span: 2, offset: 6 },
              sm: { span: 6, offset: 2 },
              md: { span: 2, offset: 2 },
              lg: { span: 2, offset: 2 }
            })
            .height(50)
            .backgroundColor(0x00BFFF)
            .textAlign(TextAlign.Center)
          Text('3')
            .fontSize(20)
            .useSizeType({
              xs: { span: 2, offset: 8 },
              sm: { span: 2, offset: 8 },
              md: { span: 6, offset: 4 },
              lg: { span: 2, offset: 4 }
            })
            .height(50)
            .backgroundColor(0x4682B4)
            .textAlign(TextAlign.Center)
          Text('4')
            .fontSize(20)
            .useSizeType({
              xs: { span: 2, offset: 10 },
              sm: { span: 2, offset: 10 },
              md: { span: 2, offset: 10 },
              lg: { span: 6, offset: 6 }
            })

            .height(50)
            .backgroundColor(0x00BFFF)
            .textAlign(TextAlign.Center)
        }
      }.width('90%').key('gridContainer').margin({ top: 300 })
    }
  }
}