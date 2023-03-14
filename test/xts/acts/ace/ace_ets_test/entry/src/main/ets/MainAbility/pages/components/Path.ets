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
struct PathExample {
  onPageShow() {
    console.info('path page show called');
  }

  onBuildDone() {
    console.info('path page build done called');
  }
  build() {
    Column({ space: 5 }) {
      Text('Straight line').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Path().width(300).height(10).commands('M0 0 L900 0').stroke(Color.Black).strokeWidth(3).key('path')

      Text('Straight line graph').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Path().width(100).height(100).commands('M150 0 L300 300 L0 300 Z').key('path1')
        Path().width(100).height(100).commands('M0 0 H300 V300 H0 Z').key('path2')
        Path().width(100).height(100).commands('M150 0 L0 150 L60 300 L240 300 L300 150 Z').key('path3')
      }.width('100%')

      Text('Curve graphics').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Path().width(100).height(100).commands("M0 300 S150 0 300 300 Z").key('path4')
        Path().width(100).height(100).commands('M0 150 C0 150 150 0 300 150 L150 300 Z').key('path5')
      }
    }.width('100%').margin({ top: 5 })
  }
}
