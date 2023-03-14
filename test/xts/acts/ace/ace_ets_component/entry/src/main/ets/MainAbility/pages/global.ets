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
struct GlobalExample {
  onPageShow() {
  }

  build() {
    Flex({
      direction: FlexDirection.Column,
      alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center
    }) {
      Canvas(this.context)
        .width('100%').height('40%').backgroundColor('#00ffff')
        .onReady(() => {
          this.context.imageSmoothingEnabled = false
          this.testImageBitmapSize();
          this.testOffscreenCanvas();
          this.testImageData();
          this.testImage();
        })
    }.width('100%').height('100%')
  }

  testImageBitmapSize() {
    let img = new ImageBitmap("/images/bg.jpg");
    let width = img.width;
    let height = img.height;
    this.context.imageSmoothingEnabled = false;
    this.context.drawImage(img, 30, 950, 160, 100);
    console.log("imagebitmap_width=" + width);
    console.log("imagebitmap_height=" + height);
  }

  testOffscreenCanvas() {
    var offscreen = new OffscreenCanvas(300, 300);
    var offCanvas2 = offscreen.getContext("2d");
    var img = new Image();
    img.src = "/images/bg.jpg";
    offCanvas2.drawImage(img, 0, 0, 100, 100);

    var bitmap = offscreen.transferToImageBitmap();
    this.ctx.transferFromImageBitmap(bitmap);
  }

  testImageData() {
    var offscreen = new OffscreenCanvas(300, 300);
    var offCanvas2 = offscreen.getContext("2d");
    var imageData = offCanvas2.createImageData(100, 100);
    var imgData = imageData.data;
    console.log("imageData_width=" + imageData.width);
    console.log("imageData_height=" + imageData.height);
  }

  testImage() {
    var img = new Image("/images/bg.jpg", 100, 100)
      .onload(() => {
      })
      .onerror(() => {
      });
    console.log("img_width=" + img.width);
    console.log("img_height=" + img.height);
  }
}