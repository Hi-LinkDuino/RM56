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
    left:100,
    width:200,
    height:60,
    padding:2,
    bordradius:2,
    bordwidth:2,
    bkgcor: 0xD4F2E7,
    margin:2,
    marginLeft:2,
    marginRight:2,
    marginTop:2,
    marginBottom:2,
    bordColor:0x8A2BE2,
    display:'flex',
    color: 0x8a2be2,

    bkgcorlorIndex: 0,
    bordColorIndex: 0,
    displayIndex: 0,

    colors:[0xFAEBD7,0xFFFF00,0xD4F2E7,0x8A2BE2,0x00FFFF,
    0x0000FF,0x00ff00,0xA52A2A,0xFF0000],
    displays:['flex','none'],
  },

  onClickBtnWidthAdd(){
      this.width +=20;
  },
  onClickBtnWidthSub(){
      this.width -=20;
  },
  onClickBtnHeightAdd(){
      this.height +=20;
  },
  onClickBtnHeightSub(){
      this.height -=20;
  },
  onClickBtnPaddingAdd(){
      this.padding +=10;
      console.log('pad:'+ this.padding);
  },
  onClickBtnBoardRadiusAdd(){
      this.bordradius += 1;
  },
  onClickBtnPaddingSub(){
      this.padding -=10;
      console.log('pad:'+ this.padding);
  },
  onClickBtnBoardRadiusSub(){
      this.bordradius -= 1;
  },
  onClickBtnBoardWidthAdd(e){
      this.bordwidth += 3;
      console.log("border-width:"+ this.bordwidth);
  },
  onClickBtnBoardWidthSub(e){
      this.bordwidth -= 3;
      console.log("border-width:"+ this.bordwidth);
  },
  onClickBtnBKGColor(){
     if(this.bkgcorlorIndex >= this.colors.length){
       this.bkgcorlorIndex = 0;
     }
     this.bkgcor = this.colors[this.bkgcorlorIndex];
     console.log('index:'+this.bkgcorlorIndex + '    ' +'bkgcolor:'+ this.bkgcor);
     this.bkgcorlorIndex += 1;
  },

  onClickBtnMarginAdd(){
      console.log('margin:'+ this.margin);
      this.margin += 2;
  },
  onClickBtnMarginSub(){
      console.log('margin:'+ this.margin);
      this.margin -=2;
  },
   onClickBtnBoardColor(){
     if(this.bordColorIndex >= this.colors.length){
     this.bordColorIndex = 0;
     }
     this.bordColor = this.bkgcors[this.bordColorIndex];
     this.bordColorIndex += 1;
  },

  //  *************************  pages  *********************************
  backMain:backPage().backMain,
  changePrev:routePage("pages/text/text001/index").changePage
}
/*  */
