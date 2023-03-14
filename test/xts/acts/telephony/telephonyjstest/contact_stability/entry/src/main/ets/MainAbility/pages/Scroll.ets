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
struct scroll_testcase {
  @State scrollBarWidth: number = 20
  @State scrollable: ScrollDirection = ScrollDirection.None
  @State scrollBarColor: Color = Color.Pink
  @State scrollBar: BarState= BarState.On
  private value1: Array<number> = [1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30]
  @State edge: Edge= Edge.Top
  private content: string = "Scroll Page";
  scroller: Scroller = new Scroller();

  onPageShow() {
    console.info('Button page show called');
  }

  onBuildDone() {
    console.info('Button page build done called');
  }

  build() {

    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Scroll(this.scroller) {
        Column() {
          ForEach(
            this.value1,
            item => {
              Text(`${item}`)
                .fontSize(15)
                .width(400)
                .margin(10)
                .height(70)
                .backgroundColor(0xfff5deb3)
            },
            item => item
          )
        }
      }
      .scrollable(ScrollDirection.Vertical)
      .scrollBar(this.scrollBar)
      .scrollBarColor(this.scrollBarColor)
      .scrollBarWidth(this.scrollBarWidth)
      .key('scroll')
      .width(400)
      .height(200)
      .onScroll((xOffset: number, yOffset: number) => {
        console.info(` current offset is ${xOffset} ${yOffset}`)
      })
      .onScrollEnd(() => {
        console.info(` now on  scroll end`)
      })
      .onScrollEdge((side: Edge) => {
        console.info(`To the edge`)
      })
      Scroll() {
        Row() {
          Text('scroll uppppp')
            .width(60)
            .height(100)
            .backgroundColor(Color.Yellow)
            .margin({
              right: 5,
              left: 5
            })
            .key('text')

          Text('scroll to')
            .width(60)
            .height(100)
            .backgroundColor(Color.Green)
            .margin({
              right: 5,
              left: 5
            })
            .onClick(() => {
              this.scroller.scrollTo({
                xOffset: 100,
                yOffset: 100,
              })
            })
          Text('scroll page')
            .width(60)
            .height(100)
            .backgroundColor(Color.Blue)
            .margin({
              right: 5,
              left: 5
            })
            .onClick(() => {
              this.scroller.scrollPage({
                next: true
              })
            })
          Text('scroll Edge')
            .width(60)
            .height(100)
            .backgroundColor(Color.Blue)
            .margin({
              right: 5,
              left: 5
            })
            .onClick(() => {
              this.scroller.scrollEdge(Edge.Top)
            })
          Text('scroll Edge')
            .width(60)
            .height(100)
            .backgroundColor(Color.Blue)
            .margin({
              right: 5,
              left: 5
            })
            .onClick(() => {
              this.scroller.currentOffset()
            })
          Text('scroll to index')
            .width(60)
            .height(100)
            .backgroundColor(Color.Blue)
            .margin({
              right: 5,
              left: 5
            })
            .onClick(() => {
              this.scroller.scrollToIndex(0)
              console.info(` now on  scroll end`)
            })
        }
      }
      .key('scroll1')
      .height(100)
      .scrollable(ScrollDirection.Horizontal)

      Scroll() {
        Column() {
          Text("start").fontSize(30)
          Text("scrollable1")
          Text("scrollable2")
          Text("scrollable3")
          Text("scrollable4")
          Text("scrollable1")
          Text("scrollable2")
          Text("scrollable3")
          Text("scrollable4")
        }
      }
      .padding(10)
      .scrollable(ScrollDirection.None)
      .scrollBar(BarState.Off)
      .scrollBarColor(Color.Red)
      .scrollBarWidth(5)
      .width(300)
      .height(200)
      .key('scroll2')

      Scroll() {
        Column() {
          Text("start").fontSize(30)
          Text("scrollable1")
          Text("scrollable2")
          Text("scrollable3")
          Text("scrollable4")
          Text("scrollable1")
          Text("scrollable2")
          Text("scrollable3")
          Text("scrollable4")
        }
      }
      .padding(10)
      .scrollable(ScrollDirection.Free)
      .scrollBar(BarState.Off)
      .scrollBarColor(Color.Red)
      .scrollBarWidth(5)
      .width(300)
      .height(200)
      .key('scroll3')
    }
    .key('flex')

  }
}
