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
let ItemDragInfo: {
  x: 5,
  y: 5
}

@Entry
@Component
struct ListExample {
  private arr1: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
  private arr2: number[] = ['a', 'b', 'c', 'd', 'e', 'f']
  @State editFlag: boolean = false
  @State bool1: boolean = false
  @State bool2: boolean = false

  build() {
    Stack({ alignContent: Alignment.TopStart }) {
      Column() {
        List({ space: 20, initialIndex: 0 }) {
          ForEach(this.arr1, (item) => {
            ListItem() {
              Text('' + item)
                .width('100%')
                .height(100)
                .fontSize(16)
                .textAlign(TextAlign.Center)
                .borderRadius(10)
                .backgroundColor(0xFFFFFF)
            }.editable(true)
          }, item => item)
        }
        .key('list')
        .listDirection(Axis.Vertical)
        .divider({ strokeWidth: 2, color: 0xFFFFFF, startMargin: 20, endMargin: 20 })
        .edgeEffect(EdgeEffect.None)
        .chainAnimation(false)
        .onScrollIndex((firstIndex: number, lastIndex: number) => {
          console.info('first' + firstIndex)
          console.info('last' + lastIndex)
        })
        .onScroll((scrollOffset: number, scrollState: ScrollState) => {
          console.info('scrollOffset:' + scrollOffset)
          console.info('ScrollState:' + scrollState)
        })
        .editMode(this.editFlag)
        .onItemDelete((index: number) => {
          console.info(this.arr1[index] + 'Delete')
          this.arr1.splice(index, 1)
          console.info(JSON.stringify(this.arr1))
          this.editFlag = false
          return true
        })
        .onReachStart(() => {
          console.info("On Reach Start")
        })
        .onReachEnd(() => {
          console.info("On Reach End")
        })
        .onScrollStop(() => {
          console.info("On Scroll Stop")
        })
        .onItemMove((from: number, to: number) => {
          console.info(from + 'from')
          console.info(to + 'to')
          return true
        })
        .onItemDragStart((event: ItemDragInfo, itemIndex: number) => {
          this.bool1 = true
          this.text = this.arr1[itemIndex]
          console.log('onItemDragStart, itemIndex:' + itemIndex);
          return this.pixelMapBuilder;
        })
        .onItemDragEnter((event: ItemDragInfo) => {
          console.log('onItemDragEnter');
        })
        .onItemDragMove((event: ItemDragInfo, itemIndex: number, insertIndex: number) => {
          console.log('onItemDragMove,itemIndex:' + itemIndex + 'insertIndex:' + insertIndex)
        })
        .onItemDragLeave((event: ItemDragInfo, itemIndex: number) => {
          console.log('onItemDragLeave, itemIndex:' + itemIndex);
        })
        .onItemDrop((event: ItemDragInfo, itemIndex: number, insertIndex: number, isSuccess: boolean) => {
          console.log('ItemDragInfo, itemIndex:' + itemIndex + 'insertIndex:' + insertIndex + 'isSuccess:' + isSuccess);
          if (isSuccess) {
            if (this.bool2) {
              this.arr2.splice(itemIndex, 1)
              this.arr1.splice(insertIndex, 0, this.text)
              this.bool1 = false
              this.bool2 = false
            } else if (this.bool1) {
              this.arr2.splice(itemIndex, 1)
              this.arr1.splice(insertIndex, 0, this.text)
              this.bool1 = false
              this.bool2 = false
            }
          }
        })
        .width('90%')
      }.width('100%')

      Button('edit list')
        .onClick(() => {
          this.editFlag = !this.editFlag
        }).margin({ top: 5, left: 20 })
    }.width('100%').height('50%').backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}