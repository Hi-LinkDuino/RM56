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
// @ts-nocheck
/**
 * Test-ListItem
 */

import { TestData } from '../MainAbility/model/testData'

@Component
export struct TestListItem {
  private testItem: TestData
  build() {
    Navigator({ target: this.testItem.url }) {
      Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
        Row() {
          Image(this.testItem.image)
            .objectFit(ImageFit.Contain)
            .autoResize(false)
            .height(60)
            .width(60)
        }
        .backgroundColor($r("app.color.white"))
//        .margin({ right: 20 })
        .width("30%")
        Text(this.testItem.name)
          .fontSize(20)
          .flexGrow(1)
          .margin({ left: 12 })
          .width("45%")
        Text(this.testItem.detail)
          .fontSize(17)
          .margin({ left: 10 ,right: 5})
          .width("45%")
      }
      .height(85)
    }
    .params({ testId: this.testItem })
    .margin({ right: 10, left: 10, top: 10 })
    .padding({left: 15, right: 15})
    .borderRadius(10)
    .backgroundColor($r("app.color.white"))
  }
}