/*
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
 
import router from '@system.router'

@Entry
@Component
struct rowSplit_testcase {
  @State resizeable: boolean = false
  private content: string = "rowSplit Page";

  onPageShow() {
    console.info('RowSplit page show called');
  }

  onBuildDone() {
    console.info('RowSplit page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      RowSplit() {
        Column() {
          Text('Demo1.RowSplit')
            .fontSize(15)
            .fontWeight(FontWeight.Bold)
        }

        Column() {
          Text('Demo2.RowSplit')
            .fontSize(15)
            .fontWeight(FontWeight.Bold)
        }
      }
      .height(100)
      .key('rowSplit')
      .resizeable(this.resizeable)
      .onClick(() => {
        router.back()
      })

      RowSplit() {
        Column() {
          Text('Demo3.RowSplit')
            .fontSize(15)
            .fontWeight(FontWeight.Bold)
        }

        Column() {
          Text('Demo4.RowSplit')
            .fontSize(15)
            .fontWeight(FontWeight.Bold)
        }
      }.height(100)
      .key('rowSplit1')
      .resizeable(true)
    }
    .key('flex')
    .width('100%')
    .height('100%')
  }
}

