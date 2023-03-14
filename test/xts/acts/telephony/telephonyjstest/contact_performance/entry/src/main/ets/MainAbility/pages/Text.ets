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
 
// @ts-nocheck
import router from '@system.router'

var paramInfo = router.getParams()
console.info("cd paramInfo" + JSON.stringify(paramInfo))

@Entry
@Component
struct text_textcase {
  @State textAlign: TextAlign= TextAlign.End
  @State lineHeight: string= '15fp'
  @State overflow: TextOverflow = TextOverflow.Ellipsis
  @State textOverflow: textOverflow<{ "overflow": TextOverflow }> = { "overflow": this.overflow }
  @State maxLines: number = 2
  @State baselineOffset: number = 0
  @State textCase: TextCase= TextCase.Normal
  @State decoration: decoration<{
    type: TextDecorationType,
    color: Color
  }> = { type: TextDecorationType.Underline, color: Color.Red }
  @State color: Color = Color.Red
  @State width: string= "200"
  @State height: string= "100"
  @State size: size = { width: 200, height: 100 }
  @State marginBottom: string = "10px"
  @State marginTop: string = "10px"
  @State marginLeft: string = "10px"
  @State marginRingt: string= "10px"
  @State paddingBottom: string= "5"
  @State paddingTop: string= "5"
  @State paddingLeft: string= "5"
  @State paddingRingt: string= "5"
  @State constraintSize: constraintSize = { minWidth: 20, maxWidth: 200, minHeight: 0, maxHeight: 50 }
  @State layoutPriority: string= "9px"
  @State layoutWeight: string = "10px"
  @State align: Alignment = Alignment.Center
  @State direction: Direction = Direction.Auto
  @State position: position<{
    x: string,
    y: string
  }> = { x: "50", y: "50" }
  @State markAnchor: markAnchor<{
    x: string,
    y: string
  }> = { x: "50", y: "50" }
  @State offset: offset<{
    x: string,
    y: string
  }> = { x: "50", y: "50" }
  @State flexGrow: number = 5
  @State flexShrink: number = 2
  @State flexBasis: number = 0
  @State alignSelf: ItemAlign = ItemAlign.Auto
  @State borderStyle: BorderStyle = BorderStyle.Dashed
  @State borderRadius: number = 10
  @State borderWidth: string= "4px"
  @State borderColor: Color= Color.Blue
  @State backgroundColor: Color = Color.Pink
  @State backgroundImageSize: ImageSize = ImageSize.Cover
  @State backgroundImagePosition: Alignment= Alignment.Center
  @State opacity: number= 1
  @State visibility: Visibility= Visibility.Visible
  @State enabled: boolean = false
  @State zIndex: number = 1
  @State fontStyle: FontStyle = FontStyle.Italic
  @State fontSize: number = 15
  @State minFontSize: number = 15
  @State maxFontSize: number = 15
  @State letterSpacing: number= 10
  @State fontWeight: FontWeight= FontWeight.Bold
  @State fontColor: Color= Color.Black
  @State active: boolean = false
  private content: string = "Text Page";

  onPageShow() {
    console.info('Text page show called');
  }

  onBuildDone() {
    console.info('Text page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Text('Text Anto test,Text Anto test,Text Anto test,Text Anto test,test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test')
        .width(this.width)
        .height(this.height)
        .padding({ bottom: this.paddingBottom, top: this.paddingTop, right: this.paddingRingt, left: this.paddingLeft })
        .margin({ bottom: this.marginBottom, top: this.marginTop, right: this.marginRingt, left: this.marginLeft })
        .constraintSize(this.constraintSize)
        .layoutWeight(this.layoutWeight)
        .align(this.align)
        .direction(this.direction)
        .position(this.position)
        .markAnchor(this.markAnchor)
        .offset(this.offset)
        .flexBasis(this.flexBasis)
        .flexGrow(this.flexGrow)
        .flexShrink(this.flexShrink)
        .alignSelf(this.alignSelf)
        .borderStyle(this.borderStyle)
        .borderRadius(this.borderRadius)
        .borderWidth(this.borderWidth)
        .borderColor(this.borderColor)
        .backgroundColor(this.backgroundColor)
        .opacity(this.opacity)
        .visibility(this.visibility)
        .enabled(this.enabled)
        .zIndex(this.zIndex)
        .fontColor(this.fontColor)
        .fontSize(this.fontSize)
        .minFontSize(this.minFontSize)
        .letterSpacing(this.letterSpacing)
        .maxFontSize(this.maxFontSize)
        .fontWeight(this.fontWeight)
        .fontStyle(this.fontStyle)
        .fontFamily("sans-serif")
        .textAlign(this.textAlign)
        .textOverflow(this.textOverflow)
        .lineHeight(this.lineHeight)
        .baselineOffset(this.baselineOffset)
        .maxLines(2)
        .textCase(this.textCase)
        .decoration(this.decoration)
        .aspectRatio(1)
        .fontColor(Color.Black)
        .key('Text')
      Text('Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test')
        .size({ width: 100, height: 50 })
        .padding({ bottom: 5, top: 5, right: 5, left: 10 })
        .margin({ bottom: 15, top: 10, right: 10, left: 10 })
        .fontSize('13')
        .textAlign(TextAlign.Start)
        .align(Alignment.Start)
        .direction(Direction.Ltr)
        .alignSelf(ItemAlign.Start)
        .borderStyle(BorderStyle.Dotted)
        .borderWidth(2)
        .visibility(Visibility.Hidden)
        .enabled(true)
        .fontWeight(FontWeight.Normal)
        .textAlign(TextAlign.Start)
        .textOverflow(TextOverflow.Clip)
        .maxLines(2)
        .textCase(TextCase.LowerCase)
        .decoration({ "type": TextDecorationType.LineThrough, "color": Color.Red })
        .fontColor(Color.White)
        .key('Text1')
      Text('Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test,Text Anto test')
        .fontSize(13)
        .width('100%')
        .textAlign(TextAlign.Center)
        .align(Alignment.TopEnd)
        .direction(Direction.Rtl)
        .alignSelf(ItemAlign.Center)
        .borderStyle(BorderStyle.Solid)
        .borderWidth(2)
        .visibility(Visibility.None)
        .fontWeight(FontWeight.Bolder)
        .textAlign(TextAlign.Center)
        .textOverflow(TextOverflow.None)
        .textCase(TextCase.UpperCase)
        .decoration({ "type": TextDecorationType.None, "color": Color.Red })
        .fontColor(Color.Blue)
        .key('Text2')
      Text('Text Anto test,Text Anto test,Text Anto test,Text Anto test')
        .fontSize(13)
        .align(Alignment.Top)
        .alignSelf(ItemAlign.End)
        .borderWidth(2)
        .width('100%')
        .fontWeight(FontWeight.Lighter)
        .decoration({ type: TextDecorationType.Overline, color: Color.Red })
        .fontColor(Color.Gray)
        .key('Text3')
      Text('Text Anto test,Text Anto test')
        .fontSize(13)
        .align(Alignment.TopStart)
        .alignSelf(ItemAlign.Baseline)
        .borderWidth(2)
        .width('100%')
        .fontWeight(FontWeight.Medium)
        .fontColor(Color.Brown)
        .key('Text4')
      Text('Text Anto test,Text Anto test,Text Anto test')
        .fontSize(13)
        .align(Alignment.End)
        .alignSelf(ItemAlign.Stretch)
        .borderWidth(2)
        .width('100%')
        .fontWeight(FontWeight.Regular)
        .fontColor(Color.Grey)
        .key('Text5')
      Text('Text Anto test,Text Anto test,Text Anto test')
        .fontSize(13)
        .align(Alignment.BottomEnd)
        .width('100%')
        .fontColor(Color.Orange)
        .key('Text6')
      Text('Text Anto test,Text Anto test,Text Anto test')
        .fontSize(13)
        .align(Alignment.Bottom)
        .width('100%')
        .fontColor(Color.Yellow)
        .key('Text7')
      Text('Text Anto test,Text Anto test,Text Anto test')
        .fontSize(13)
        .align(Alignment.BottomStart)
        .width('100%')
        .key('Text8')

    }
    .key('flex')
    .width('100%')
    .height('100%')
  }
}


