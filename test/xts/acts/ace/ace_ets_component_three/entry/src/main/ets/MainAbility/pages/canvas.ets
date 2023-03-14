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
struct CanvasExample {
  private scroller: Scroller = new Scroller();
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Start }) {
      Scroll(this.scroller) {
        Column() {
          Canvas(this.context)
            .width('100%')
            .height('100%')
            .backgroundColor('#ffff00')
            .onReady(() => {
              this.fillStyle();
              this.lineWidth();
              this.strokeStyle();
              this.lineCap();
              this.lineJoin();
              this.miterLimit();
              this.font();
              this.textAlign();
              this.textBaseline();
              this.globalAlpha();
              this.lineDashOffset();
              this.globalCompositeOperation();
              this.shadowBlur();
              this.shadowColor();
              this.shadowOffsetX();
              this.shadowOffsetY();
              this.imageSmoothingEnabled();
              this.fillRect();
              this.strokeRect();
              this.clearRect();
              this.fillText();
              this.strokeText();
              this.measureText();
              this.beginPath();
              this.closePath();
              this.createPattern();
              this.bezierCurveTo();
              this.quadraticCurveTo();
              this.arcTo();
              this.rect();
              this.transform();
              this.setTransform();
              this.translate();
              this.drawImage();
              this.createImageData();
              this.createImageDataByImageData();
              this.getImageData();
              this.getImageWidth();
              this.getImageHeight();
              this.putImageData();
              this.createLinearGradient();
              this.createRadialGradient();
              this.restore();
              this.save();
              this.addPath();
              this.lineDash();
              this.toDataURL();
              this.getWidth();
              this.getHeight();
              this.fill();
              this.textMetrics();
              this.getBitImageSize();
              this.rotate();
              this.getPixelMap();
            })
            .key('canvas1')
        }.width('100%').height('350%')
      }.scrollable(ScrollDirection.Vertical).scrollBar(BarState.On)
      .scrollBarColor(Color.Gray).scrollBarWidth(10)
    }
    .width('100%')
    .height('100%')
  }

  onPageShow() {
  }

  fillStyle() {
    this.context.fillStyle = '#0000ff';
    this.context.fillRect(20, 120, 75, 55);
  }

  lineWidth() {
    this.context.lineWidth = 5;
    this.context.strokeRect(25, 25, 55, 75);
  }

  strokeStyle() {
    this.context.lineWidth = 10;
    this.context.strokeStyle = '#0000ff';
    this.context.strokeRect(115, 25, 75, 55);
  }

  lineCap() {
    this.context.lineWidth = 8;
    this.context.beginPath();
    this.context.lineCap = 'round';
    this.context.moveTo(220, 50);
    this.context.lineTo(320, 50);
    this.context.stroke();
  }

  lineJoin() {
    this.context.beginPath();
    this.context.lineWidth = 8;
    this.context.lineJoin = 'miter';
    this.context.moveTo(370, 25);
    this.context.lineTo(430, 50);
    this.context.lineTo(370, 75);
    this.context.stroke();
  }

  miterLimit() {
    this.context.lineWidth = 8;
    this.context.lineJoin = 'miter';
    this.context.miterLimit = 3;
    this.context.moveTo(130, 155);
    this.context.lineTo(190, 160);
    this.context.lineTo(130, 162);
    this.context.stroke();
  }

  font() {
    this.context.font = '20px sans-serif';
    this.context.fillText("Hello World", 250, 170);
  }

  textAlign() {
    let height = 230;
    this.context.strokeStyle = '#0000ff';
    this.context.moveTo(220, 10 + height);
    this.context.lineTo(220, 210 + height);
    this.context.stroke();
    this.context.font = '20px sans-serif';
    this.context.textAlign = 'start';
    this.context.fillText('textAlign=start', 240, 50 + height);
    this.context.textAlign = 'end';
    this.context.fillText('textAlign=end', 210, 80 + height);
    this.context.textAlign = 'left';
    this.context.fillText('textAlign=left', 240, 110 + height);
    this.context.textAlign = 'center';
    this.context.fillText('textAlign=center', 230, 150 + height);
    this.context.textAlign = 'right';
    this.context.fillText('textAlign=right', 210, 200 + height);
  }

  textBaseline() {
    let height = 550;
    this.context.strokeStyle = '#0000ff';
    this.context.moveTo(50, height);
    this.context.lineTo(450, height);
    this.context.stroke();
    this.context.font = '20px sans-serif';
    this.context.textBaseline = 'top';
    this.context.fillText('Top', 60, height + 10);
    this.context.textBaseline = 'bottom';
    this.context.fillText('Bottom', 150, height - 10);
    this.context.textBaseline = 'middle';
    this.context.fillText('Middle', 225, height);
    this.context.textBaseline = 'alphabetic';
    this.context.fillText('Alphabetic', 345, height - 10);
    this.context.textBaseline = 'hanging';
    this.context.fillText('Hanging', 450, height + 10);
  }

  globalAlpha() {
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(0, 620, 50, 50);
    this.context.globalAlpha = 0.4;
    this.context.fillStyle = 'rgb(0,0,255)';
    this.context.fillRect(50, 670, 50, 50);
  }

  lineDashOffset() {
    this.context.globalAlpha = 1;
    this.context.beginPath();
    this.context.arc(180, 650, 50, 0, 6.28);
    this.context.setLineDash([10, 20]);
    this.context.stroke();
    this.context.setLineDash([0, 0]);
  }

  globalCompositeOperation() {
    let height = 600;
    let width = 260;
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(20 + width, 20 + height, 50, 50);
    this.context.globalCompositeOperation = 'source-over';
    this.context.fillStyle = 'rgb(0,0,255)';
    this.context.fillRect(50 + width, 50 + height, 50, 50);
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(120 + width, 20 + height, 50, 50);
    this.context.globalCompositeOperation = 'destination-over';
    this.context.fillStyle = 'rgb(0,0,255)';
    this.context.fillRect(150 + width, 50 + height, 50, 50);
    this.context.globalCompositeOperation = 'source-over';
  }

  shadowBlur() {
    this.context.shadowBlur = 30;
    this.context.shadowColor = 'rgb(0,0,0)';
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(10, 800, 100, 80);
  }

  shadowColor() {
    this.context.shadowBlur = 30;
    this.context.shadowColor = 'rgb(0,0,255)';
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(150, 800, 80, 100);
  }

  shadowOffsetX() {
    this.context.shadowBlur = 10;
    this.context.shadowOffsetX = 20;
    this.context.shadowColor = 'rgb(0,0,0)';
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(250, 800, 80, 80);
    this.resetShadow(true);
  }

  shadowOffsetY() {
    this.context.shadowBlur = 10;
    this.context.shadowOffsetY = 20;
    this.context.shadowColor = 'rgb(0,0,0)';
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(360, 800, 80, 80);
    this.resetShadow(false);
  }

  resetShadow(isX) {
    this.context.shadowBlur = 0;
    if (isX) {
      this.context.shadowOffsetX = 0;
    } else {
      this.context.shadowOffsetY = 0;
    }
  }

  imageSmoothingEnabled() {
    let img = new ImageBitmap('/images/img.jpeg');
    this.context.imageSmoothingEnabled = false;
    this.context.drawImage(img, 30, 950, 160, 100);
  }

  imageSmoothingQuality() {
    let img = new ImageBitmap('/images/img.jpeg');
    this.context.imageSmoothingQuality('high');
    this.context.drawImage(img, 30, 950, 160, 100);
  }

  fillRect() {
    this.context.fillRect(10, 1080, 80, 80);
  }

  strokeRect() {
    this.context.strokeRect(110, 1080, 100, 60);
  }

  clearRect() {
    this.context.fillStyle = 'rgb(0,0,255)';
    this.context.fillRect(240, 950, 200, 200);
    this.context.clearRect(260, 970, 100, 60);
  }

  fillText() {
    this.context.font = '20px sans-serif';
    this.context.fillText("Hello World!", 120, 1200);
  }

  strokeText() {
    this.context.font = '20px sans-serif';
    this.context.strokeText("Hello World!", 260, 1195);
  }

  measureText() {
    this.context.font = '20px sans-serif';
    this.context.fillText("Hello World!", 120, 1250);
    this.context.fillText("width:" + this.context.measureText("Hello World!").width, 310, 1250);
  }

  beginPath() {
    this.context.beginPath();
    this.context.lineWidth = 6;
    this.context.strokeStyle = '#0000ff';
    this.context.moveTo(310, 1200);
    this.context.lineTo(450, 1260);
    this.context.stroke();
  }

  closePath() {
    this.context.beginPath();
    this.context.moveTo(30, 1300);
    this.context.lineTo(110, 1300);
    this.context.lineTo(70, 1360);
    this.context.closePath();
    this.context.stroke();
  }

  createPattern() {
    let img = new ImageBitmap('/images/ic_health_heart.png');
    let pattern = this.context.createPattern(img, 'repeat');
    this.context.fillStyle = pattern;
    this.context.fillRect(140, 1300, 250, 100);
  }

  bezierCurveTo() {
    let height = 1400;
    this.context.beginPath();
    this.context.moveTo(10, 10 + height);
    this.context.bezierCurveTo(20, 100 + height, 200, 100 + height, 200, 20 + height);
    this.context.stroke();
  }

  quadraticCurveTo() {
    let height = 1400;
    let width = 210;
    this.context.beginPath();
    this.context.moveTo(20 + width, 20 + height);
    this.context.quadraticCurveTo(100 + width, 100 + height, 200 + width, 20 + height);
    this.context.stroke();
  }

  arcTo() {
    this.context.moveTo(30, 20 + 1500);
    this.context.arcTo(80, 20 + 1500, 80, 70 + 1500, 50);
    this.context.stroke();
  }

  rect() {
    this.context.rect(130, 20 + 1500, 80, 80);
    this.context.stroke();
  }

  rotate() {
    this.context.rotate(45 * Math.PI / 180);
    this.context.fillRect(180, 20, 50, 50);
    this.context.rotate(-45 * Math.PI / 180);
  }

  transform() {
    this.context.fillStyle = 'rgb(0,0,0)';
    this.context.fillRect(250, 1500, 100, 100);
    this.context.transform(1, 0.1, 0.01, 1, 10, 10);
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(250, 1500, 100, 100);
    this.context.transform(1, 0.1, 0.01, 1, 10, 10);
    this.context.fillStyle = 'rgb(0,0,255)';
    this.context.fillRect(250, 1500, 100, 100);
  }

  setTransform() {
    this.context.fillStyle = 'rgb(255,0,0)';
    this.context.fillRect(0, 1650, 100, 100);
    this.context.setTransform(1, 0.1, 0.01, 1, 10, 10);
    this.context.fillStyle = 'rgb(0,0,255)';
    this.context.fillRect(0, 1650, 100, 100);
    this.context.setTransform(1, 0, 0, 1, 0, 0);
  }

  getTransform() {
    let data = this.context.getTransform();
    console.info("[canvas] get transform----" + data);
  }

  resetTransform() {
    this.context.resetTransform();
  }

  translate() {
    this.context.fillRect(180, 1650, 50, 50);
    this.context.translate(50, 50);
    this.context.fillRect(180, 1650, 50, 50);
  }

  drawImage() {
    let img = new ImageBitmap('/images/img.jpeg');
    this.context.drawImage(img, 0, 0, 500, 500, 0, 1780, 200, 100);
  }

  createImageData() {
    let imageData = this.context.createImageData(100, 100);
    console.info("[canvas] create image data-----" + JSON.stringify(imageData));
  }

  createImageDataByImageData() {
    let imageData = this.context.createImageData(50, 50);
    let imgData = this.context.createImageData(imageData);
    console.info("[canvas] create image data by image data----" + JSON.stringify(imgData));
  }

  getImageData() {
    let imageData = this.context.getImageData(10, 10, 80, 80);
    let data = imageData.data;
    console.info("[canvas] get image data-----" + JSON.stringify(data));
  }

  getImageWidth() {
    let imageData = this.context.getImageData(10, 10, 80, 80);
    let width = imageData.width;
    console.info("[canvas] get image width-----" + width);
  }

  getImageHeight() {
    let imageData = this.context.getImageData(10, 10, 80, 80);
    let height = imageData.height;
    console.info("[canvas] get image height-----" + height);
  }

  putImageData() {
    let imageData = this.context.createImageData(80, 80);
    for (var i = 0; i < imageData.data.length; i += 4) {
      imageData.data[i + 0] = 255;
      imageData.data[i + 1] = 0;
      imageData.data[i + 2] = 255;
      imageData.data[i + 3] = 255;
    }
    this.context.putImageData(imageData, 250, 1660);
  }

  createLinearGradient() {
    let grad = this.context.createLinearGradient(50, 1900, 150, 2000);
    grad.addColorStop(0.0, 'red');
    grad.addColorStop(0.5, 'white');
    grad.addColorStop(1.0, 'green');
    this.context.fillStyle = grad;
    this.context.fillRect(10, 1900, 150, 150);
  }

  createRadialGradient() {
    let grad = this.context.createRadialGradient(280, 1970, 40, 280, 1970, 80);
    grad.addColorStop(0.0, 'red');
    grad.addColorStop(0.5, 'white');
    grad.addColorStop(1.0, 'green');
    this.context.fillStyle = grad;
    this.context.fillRect(200, 1900, 150, 150);
    this.context.closePath();
  }

  restore() {
    this.context.restore();
  }

  save() {
    this.context.save();
  }

  addPath() {
    let path2Da = new Path2D("M250 150 L150 350 L350 350 Z");
    let path2Db = new Path2D();
    path2Db.addPath(path2Da);
    this.context.stroke(path2Db);
  }

  lineDash() {
    this.context.beginPath();
    this.context.arc(20, 2130, 50, 0, 6.28);
    this.context.setLineDash([10, 20]);
    this.context.stroke();
  }

  toDataURL() {
    let dataUrl = this.context.toDataURL();
    console.info("[canvas] canvas rendering context2D toDataURL-----" + JSON.stringify(dataUrl));
  }

  getWidth() {
    let width = this.context.width;
    console.info("[canvas] canvas width---" + width);
  }

  getHeight() {
    let height = this.context.height;
    console.info("[canvas] canvas height---" + height);
  }

  fill() {
    this.context.beginPath();
    this.context.rect(130, 2080, 100, 100);
    this.context.fill('nonzero');
  }

  direction() {
    this.context.direction('inherit');
  }

  textMetrics() {
    this.context.font = '20px sans-serif';
    this.context.fillText("Hello World!", 120, 2150);
    let obj = this.context.measureText("Hello World!");
    console.info("[canvas] textMetrics info---" + JSON.stringify(obj));
  }

  getBitImageSize() {
    let img = new ImageBitmap('/images/img.jpeg');
    let width = img.width;
    let height = img.height;
  }

  filter() {
    let img = new ImageBitmap('/images/img.jpeg');
    this.context.filter('grayscale(50%)');
    this.context.drawImage(img, 0, 0, 500, 500, 0, 2200, 200, 100);
  }

  getLineDash() {
    this.context.beginPath();
    this.context.setLineDash([10, 20]);
    this.context.moveTo(250, 2220);
    this.context.lineTo(400, 2220);
    this.context.stroke();
    let obj = this.context.getLineDash();
  }

  setPixelMap() {
    this.context.setPixelMap();
  }

  getPixelMap() {
    let pixelMap = this.context.getPixelMap(10, 10, 80, 80);
    console.info("[canvas] getPixelMap ----" + JSON.stringify(pixelMap));
  }
}