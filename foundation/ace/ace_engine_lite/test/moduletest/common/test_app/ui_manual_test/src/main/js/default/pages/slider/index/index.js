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
import {backPage,routePage} from "../../../common/js/general";

export default {
  data: {
    top: 20,
    left:150,
    width:100,
    height:100,
    padding:2,
    bordradius:2,
    bordWidth:2,
    bkgcor:0xD4F2E7,
    bordColor:0x8A2BE2,
    color:0xff0000,
    colorIndex:0,
    blockColor:0x00ff00,
    blockColorIndex:0,
    selectedColor:0x0000ff,
    selectedColorIndex:0,
    // bordWidth:'BordWidthOne',
    // special 初始值
    min:0,
    max:100,
    value:50,
    bkgcorlorIndex: 0,
    bordColorIndex: 0,
    bordWidthIndex:0,
    colors:[0xFAEBD7,0xFFFF00,0xD4F2E7,0x8A2BE2,0x00FFFF,
    0x0000FF,0x00ff00,0xA52A2A,0xFF0000],

    bordWidths:[3,5,10,1,7,12,15],
  },

  onClickBtnTopAdd(){
      this.top += 5;
      console.log("top:" + this.top);
  },
  onClickBtnTopSub(){
      this.top -= 5;
      console.log("top:" + this.top);
  },
  onClickBtnLeftAdd(){
      this.left += 5;
      console.log("left:" + this.left);
  },
  onClickBtnLeftSub(){
      this.left -= 5;
      console.log("left:" + this.left);
  },
  onClickBtnWidthAdd(){
      this.width +=20;
      console.log("width:" + this.width);
  },
  onClickBtnWidthSub(){
      this.width -=20;
      console.log("width:" + this.width);
  },
  onClickBtnHeightAdd(){
      this.height +=20;
      console.log("height:" + this.height);
  },
  onClickBtnHeightSub(){
      this.height -=20;
      console.log("height:" + this.height);
  },
  onClickBtnPaddingAdd(){
      this.padding +=10;
      console.log('pad:'+ this.padding);
  },
  onClickBtnPaddingSub(){
      this.padding -=10;
      console.log('pad:'+ this.padding);
  },
  onClickBtnBoardRadiusAdd(){
      this.bordradius += 1;
      console.log("bordradius:" + this.bordradius);
  },
  onClickBtnBoardRadiusSub(){
      this.bordradius -= 1;
      console.log("bordradius:" + this.bordradius);
  },
  onClickBtnBoardWidthAdd(){
      if(this.bordWidthIndex >= this.bordWidths.length){
       this.bordWidthIndex = 0;
     }
      this.bordWidth = this.bordWidths[this.bordWidthIndex];
      this.bordWidthIndex += 1;
      console.log("bordWidth:" + this.bordWidth);
  },
  onClickBtnBKGColor(){
     if(this.bkgcorlorIndex >= this.colors.length){
       this.bkgcorlorIndex = 0;
     }
     this.bkgcor = this.colors[this.bkgcorlorIndex];
     console.log('index:'+this.bkgcorlorIndex + '    ' +'bkgcolor:'+ this.bkgcor);
     this.bkgcorlorIndex += 1;
  },
   onClickBtnColor(){
     if(this.colorIndex >= this.colors.length){
       this.colorIndex = 0;
     }
     this.color = this.colors[this.colorIndex];
     console.log('index:'+this.colorIndex + '    ' +'color:'+ this.color);
     this.colorIndex += 1;
  },
  onClickBtnSelectedColor(){
     if(this.selectedColorIndex >= this.colors.length){
       this.selectedColorIndex = 0;
     }
     this.selectedColor = this.colors[this.selectedColorIndex];
     console.log('selectedColorIndex:'+this.selectedColorIndex + '    ' +'selectedColor:'+ this.selectedColor);
     this.selectedColorIndex += 1;
  },
  onClickBtnBlockColor(){
     if(this.blockColorIndex >= this.colors.length){
       this.blockColorIndex = 0;
     }
     this.blockColor = this.colors[this.blockColorIndex];
     console.log('blockColorIndex:'+this.blockColorIndex + '    ' +'blockColor:'+ this.blockColor);
     this.blockColorIndex += 1;
  },
   onClickBtnNextPage:routePage('pages/slider/second/second').changePage

}
/*  */