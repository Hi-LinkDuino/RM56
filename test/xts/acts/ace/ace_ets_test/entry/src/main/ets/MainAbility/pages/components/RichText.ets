// @ts-nocheck
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

@Entry
@Component
struct richTextTest {
  @State data: string= "<div class=\"item-title\"><h1>文本测试(h1测试)</h1></div>"
  onPageShow() {
    console.info('richText page show called');
  }

  onBuildDone() {
    console.info('richText page build done called');
  }
  build() {
    Column() {
      RichText(this.data)
        .key('richText')
        .onStart(() => {
          console.info("RichText onStart")
        })
        .onComplete(() => {
          console.info("RichText onComplete")
        })
    }.width('100%').height('100%')
  }
}