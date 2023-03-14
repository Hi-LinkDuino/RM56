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
import { alterPage } from "../../../common/js/general";
import { listData } from "../../../common/js/list";
export default {
  data: {
    listData: listData
  },
  ...alterPage("pages/list/list1/index", ""),
  listClick() {
    console.log("list onclick is triggered");
  },
  scrollStart(startState, startComponentIndex) {
    console.log(
      "list onscrollstart is triggered, start component index=" +
        startComponentIndex
    );
  },
  scrollEnd(endState, endComponentIndex) {
    console.log(
      "list onscrollend is triggered, end component index=" + endComponentIndex
    );
  },
  itemSelected(passState, passComponentIndex) {
    console.log(
      "list onitemselected is triggered, pass component index=" +
        passComponentIndex
    );
  },
  clickSplice() {
    console.log("delete first item and add new item at current position");
    this.listData.splice(0, 1, "common/leftHead.png");
  },
  clickPush() {
    console.log("add new item after last item");
    this.listData.push("common/leftHead.png");
  },
  clickPop() {
    console.log("delete the last item");
    this.listData.pop();
  },
  clickShift() {
    console.log("delete the first item");
    this.listData.shift();
  },
  clickUnShift() {
    console.log("add new item before the first item");
    this.listData.unshift("common/leftHead.png");
  },
  onInit() {
    console.log("on init called...");
  }
};