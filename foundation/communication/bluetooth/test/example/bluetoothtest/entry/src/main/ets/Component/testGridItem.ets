/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import { TestData } from '../MainAbility/model/testData'
import router from '@system.router'
/**
 * Test-GridItem
 */
@Component
export struct TestGridItem {
  private testItem: TestData
  build() {
    Column() {
      Row() {
        Image(this.testItem.image)
          .objectFit(ImageFit.Contain)
          .height(152)
          .width('100%')
      }.backgroundColor('#FFf1f3f5')
      Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
        Text(this.testItem.name)
          .fontSize(14)
          .flexGrow(1)
          .padding({ left: 8 })
        Text(this.testItem.detail)
          .fontSize(14)
          .margin({ right: 6 })
      }
      .height(32)
      .width('100%')
      .backgroundColor('#FFe5e5e5')
    }
    .height(184)
    .width('100%')
    .onClick(() => {
      router.push({ uri: 'pages/TestDetail', params: { testId: this.testItem } })
    })
  }
}