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
import {goPage} from "../../../common/js/general";
export default {
  data: {
    btnValue1: "属性变化",
    color: 0x3366ff,
    fontSize: 30,
    width: 300,
    height: 40,
    padding: 10,
    borderRadius: 50,
    borderWidth: 1,
    margin: 3,
    marginleft: 5,
    marginright: 5,
    margintop: 5,
    marginbottom: 5,
    borderwidth: 1,
    bordercolor: 0x0000ff,
    backgroundcolor: 0x004E8C
  },
  onInit: function() {
    console.info("----- bindTest3 page onInit-------");
  },
  Test1: function() {
    this.color = 0xffcc33;
    this.fontSize = 38;
    this.width = 150;
    this.height = 100;
    this.padding = 100;
    this.borderRadius = 100;
    this.borderWidth = 5;
    this.padding = 50;
    this.margin = 13;
    this.marginleft = 15;
    this.marginright = 15;
    this.margintop = 15;
    this.marginbottom = 15;
    this.borderwidth = 10;
    this.bordercolor = 0X00ACAC;
    this.backgroundcolor = 0xFEF102;
  },
  Test2: function() {
    this.color = 0x3366ff;
    this.fontSize = 30;
    this.width = 300;
    this.height = 40;
    this.padding = 10;
    this.borderRadius = 50;
    this.borderWidth = 1;
    this.padding = 10;
    this.margin = 3;
    this.marginleft = 5;
    this.marginright = 5;
    this.margintop = 5;
    this.marginbottom = 5;
    this.borderwidth = 1;
    this.bordercolor = 0x0000ff;
    this.backgroundcolor = 0x004E8C;
  },
  ...goPage("pages/input/CheckboxA029/index", "pages/input/RadioEventTest031/index")
};