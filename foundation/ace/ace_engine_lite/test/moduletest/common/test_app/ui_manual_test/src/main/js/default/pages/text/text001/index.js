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
    title: "Text",
    top: 20,
    left: 100,
    width: 200,
    height: 60,
    padding: 2,
    bordradius: 2,
    bordwidth: 2,
    bkgcor: 0xd4f2e7,

    margin: 2,
    marginLeft: 2,
    marginRight: 2,
    marginTop: 2,
    marginBottom: 2,

    bordColor: 0x8a2be2,
    display: "flex",
    bordWidth: "BordWidthOne",
    // special init value
    color: 0x8a2be2,
    fontsize: 30,
    letterspacing: 2,
    textalign: "left",
    textoverflow: "clip",
    fontfamily: "HYQiHei-65S",

    bkgcorlorIndex: 0,
    bordColorIndex: 0,
    displayIndex: 0,
    bordWidthIndex: 0,
    // special Index
    colorIndex: 0,
    fontsizeIndex: 0,
    textalignIndex: 0,
    textoverflowIndex: 0,
    fontfamilyIndex: 0,

    colors: [
      0xfaebd7,
      0xffff00,
      0xd4f2e7,
      0x8a2be2,
      0x00ffff,
      0x0000ff,
      0x00ff00,
      0xa52a2a,
      0xff0000
    ],

    displays: ["flex", "none"],

    bordWidths: ["BordWidthOne", "BordWidthTwo", "BordWidthThree"],

    // special Array

    fontsizes: [38, 30],
    textaligns: ["left", "center", "right"],
    textoverflows: ["clip", "ellipsis"],
    fontfamilys: ["HYQiHei-65S"]
  },

  /****************************  special **************************************/
  onClickBtncolor() {
    if (this.colorIndex >= this.colors.length) {
      this.colorIndex = 0;
    }
    this.color = this.colors[this.colorIndex];
    console.log("color:" + this.color);

    this.colorIndex += 1;
  },
  onClickBtnFontsize() {
    if (this.fontsizeIndex >= this.fontsizes.length) {
      this.fontsizeIndex = 0;
    }
    this.fontsize = this.fontsizes[this.fontsizeIndex];
    console.log("fontsize:" + this.fontsize);
    this.fontfamily = "HYQiHei-65S";
    this.fontsizeIndex += 1;
  },
  onClickBtnLetterspacingAdd() {
    this.letterspacing += 1;
  },
  onClickBtnLetterspacingSub() {
    this.letterspacing -= 1;
  },
  onClickBtnTextalign() {
    if (this.textalignIndex >= this.textaligns.length) {
      this.textalignIndex = 0;
    }
    this.textalign = this.textaligns[this.textalignIndex];
    console.log("textalign:" + this.textalign);
    this.textalignIndex += 1;
  },
  onClickBtnTextoverflow() {
    if (this.textoverflowIndex >= this.textoverflows.length) {
      this.textoverflowIndex = 0;
    }
    this.textoverflow = this.textoverflows[this.textoverflowIndex];
    console.log("textoverflow:" + this.textoverflow);
    this.textoverflowIndex += 1;
  },
  onClickBtnFontfamily() {
    if (this.fontfamilyIndex >= this.fontfamilys.length) {
      this.fontfamilyIndex = 0;
    }
    this.fontfamily = this.fontfamilys[this.fontfamilyIndex];
    console.log("fontfamily:" + this.fontfamily);
    this.fontfamilyIndex += 1;
  },


  //  *************************  pages  *********************************
  backMain:backPage().backMain,
  changeNext:routePage("pages/text/text002/index").changePage
};
