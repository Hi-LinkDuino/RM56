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
struct ScrollBarExample {
  private scroller1: Scroller = new Scroller();
  private scroller2: Scroller = new Scroller();
  private scroller3: Scroller = new Scroller();
  private arr1: number[] = [1, 2, 3, 4, 5, 6, 7]
  private arr2: string[] = ['a', 'b', 'c', 'd', 'e', 'f', 'g']
  private arr3: string[] = ['A', 'B', 'C', 'D', 'E', 'F', 'G']
  private content: string = "Scroll Page";

  onPageShow() {
    console.info('stepper page show called');
  }

  onBuildDone() {
    console.info('stepper page build done called');
  }

  build() {
    Column({ space: 10 }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Flex({ direction: FlexDirection.Row }) {
        Scroll(this.scroller1) {
          Flex({ direction: FlexDirection.Column }) {
            ForEach(this.arr1, (item) => {
              Row() {
                Text(item.toString())
                  .width('90%')
                  .height(50)
                  .backgroundColor('#3366CC')
                  .fontSize(16)
                  .textAlign(TextAlign.Center)
                  .margin(5)
              }
            }, item => item)
          }.margin({ left: 50 })
        }
        .scrollable(ScrollDirection.Vertical)

        ScrollBar({ scroller: this.scroller1, direction: ScrollBarDirection.Vertical, state: BarState.Auto }) {
          Text()
            .width(30)
            .height(100)
            .borderRadius(10)
            .backgroundColor('#C0C0C0')
        }.width(30).backgroundColor('#ededed').key('scrollbar1')

        ScrollBar({ scroller: this.scroller1, direction: ScrollBarDirection.Vertical, state: BarState.On }) {
          Text()
            .width(30)
            .height(100)
            .borderRadius(10)
            .backgroundColor('#C0C0C0')
        }.width(30).backgroundColor('#ededed').key('scrollbar2')

        ScrollBar({ scroller: this.scroller1, direction: ScrollBarDirection.Vertical, state: BarState.Off }) {
          Text()
            .width(30)
            .height(100)
            .borderRadius(10)
            .backgroundColor('#C0C0C0')
        }.width(30).backgroundColor('#ededed').key('scrollbar3')
      }.height('45%')

      Flex({ direction: FlexDirection.Column }) {
        Scroll(this.scroller2) {
          Flex({ direction: FlexDirection.Row }) {
            ForEach(this.arr2, (item) => {
              Row() {
                Text(item.toString())
                  .width('10%')
                  .height(150)
                  .backgroundColor(0xfff5deb3)
                  .fontSize(16)
                  .textAlign(TextAlign.Center)
                  .margin(5)
              }
            }, item => item)
          }.margin({ left: 15 })
        }
        .scrollable(ScrollDirection.Horizontal)

        ScrollBar({ scroller: this.scroller2, direction: ScrollBarDirection.Horizontal, state: BarState.Auto }) {
          Text()
            .width('90%')
            .height(20)
            .borderRadius(10)
            .backgroundColor('#C0C0C0')
        }.height(20).backgroundColor('#ededed').key('scrollbar4').margin({top:30})

        ScrollBar({ scroller: this.scroller2, direction: ScrollBarDirection.Horizontal, state: BarState.On }) {
          Text()
            .width('90%')
            .height(20)
            .borderRadius(10)
            .backgroundColor('#C0C0C0')
        }.width(30).backgroundColor('#ededed').key('scrollbar5').margin({top:30})

        ScrollBar({ scroller: this.scroller2, direction: ScrollBarDirection.Horizontal, state: BarState.Off }) {
          Text()
            .width('90%')
            .height(20)
            .borderRadius(10)
            .backgroundColor('#C0C0C0')
        }.width(30).backgroundColor('#ededed').key('scrollbar6').margin({top:30})
      }.height('45%').width('90%').margin({top:30})
    }
  }
}
