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
struct dataPanel_testcase {
  @State width: string= "200"
  @State height: string= "100"
  @State active: boolean = false
  private content: string = "DataPanel Page"

  onPageShow() {
    console.info('Blank page show called');
  }

  onBuildDone() {
    console.info('Blank page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
      DataPanel({ values: [20, 40, 20], max: 100, type: DataPanelType.Line })
        .width(this.width)
        .height(this.height)
        .key('DataPanel')
        .onClick(() => {
          router.back()
        })
        .closeEffect(false)

      DataPanel({ values: [20, 40, 20], max: 100, type: DataPanelType.Circle})
        .width(this.width)
        .height(this.height)
        .key('DataPanel2')
        .onClick(() => {
          router.back()
        })
        .closeEffect(true)
      DataPanel({ values: [20, 40, 20], max: 100})
        .width(this.width)
        .height(this.height)
        .key('DataPanel3')
        .onClick(() => {
          router.back()
        })
        .closeEffect(false)
    }
    .key('flex')
    .width('100%')
    .height('100%')

  }
}
