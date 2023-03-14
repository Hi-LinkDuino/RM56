// @ts-nocheck
/**
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
struct Canvas2Example {
  private scroller: Scroller = new Scroller();
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private offContext: OffscreenCanvasRenderingContext2D = new OffscreenCanvasRenderingContext2D(600, 600, this.settings);

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Start }) {
      Scroll(this.scroller) {
        Column() {
          Canvas(this.context)
            .width('100%')
            .height('100%')
            .backgroundColor('#ffff00')
            .onReady(() => {
              this.transferFromImageBitmap();
              this.offScreenToDataURL();
            })
            .key('canvas2')
        }.width('100%').height('100%')
      }.scrollable(ScrollDirection.Vertical).scrollBar(BarState.On)
      .scrollBarColor(Color.Gray).scrollBarWidth(10)
    }
    .width('100%')
    .height('100%')
  }

  onPageShow() {
  }

  transferFromImageBitmap() {
    this.offContext.fillStyle = '#0000ff';
    this.offContext.fillRect(20, 160, 150, 100);
    let image = this.offContext.transferToImageBitmap();
    this.context.transferFromImageBitmap(image);
  }

  offScreenToDataURL() {
    let dataUrl = this.offContext.toDataURL();
    console.info("[canvas] offContext canvas rendering context2D toDataURL-----" + JSON.stringify(dataUrl));
  }
}