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
/**
 * Test-ImageDisplay
 */
@Component
export struct TestImageDisplay {
  private testItem: TestData
  build() {
    Stack({ alignContent: Alignment.BottomStart }) {
      Flex({ alignItems: ItemAlign.Start }) {
        Stack() {
          Image(this.testItem.image)
            .height(80)
            .width(80)
            .margin({left: 10, bottom: 10})
        }
        .width("20%")
        Column() {
          Text(this.testItem.name)
            .fontSize(26)
            .fontWeight(500)
              //            .textAlign(Start)
            .margin({ left: 26, top: 10 })
            .width("90%")
          Text(this.testItem.detail)
            .fontSize(17)
            .fontWeight(500)
            .margin({ top: 8 })
            .width("80%")
        }
        .width("80%")
        .padding({left: 20})
      }
    }
    .backgroundColor('#FFedf2f5')
    .height(160)
  }
}