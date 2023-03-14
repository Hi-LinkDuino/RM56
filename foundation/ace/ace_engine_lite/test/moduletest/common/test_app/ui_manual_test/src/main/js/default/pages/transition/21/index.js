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
    title: 'World',
    animationName:'rotate1',
    index:1,
    ifFlag:true
  },
  clickAction(){
     if(this.index > 4){
       this.index = 1;
     }
     console.log(this.index);
     this.animationName = "rotate"+this.index;
     this.index++;
  },
  changeIf() {
    this.ifFlag = this.ifFlag == true ? false : true;
  },
  changePrev:routePage('pages/transition/20/index').changePage,
  backMain:backPage().backMain
}
