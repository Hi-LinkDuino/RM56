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
struct PolygonExample {
  onPageShow() {
    console.info('polygon page show called');
  }

  onBuildDone() {
    console.info('polygon page build done called');
  }
  build() {
    Column({ space: 5 }) {
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Polygon({ width: 100, height: 100 }).points([[0, 0], [50, 100], [100, 0]]).key('polygon')
        Polygon().width(100).height(100).points([[0, 0], [0, 100], [100, 100], [100, 0]]).key('polygon1')
        Polygon().width(100).height(100).points([[50, 0], [0, 50], [20, 100], [80, 100], [100, 50]]).key('polygon2')
      }.width('100%')
    }.margin({ top: 5 })
  }
}
