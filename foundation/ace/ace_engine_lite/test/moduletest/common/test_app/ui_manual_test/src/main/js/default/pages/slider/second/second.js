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
    margin:0,
    marginLeft:0,
    marginRight:0,
    marginTop:0,
    marginBottom:0
  },

  onClickBtnmargin(operator){
      if(operator == "+"){
       this.margin += 5;
     }else if(operator == "-"){
       this.margin -= 5;
     }
      console.log("marginLeft:" + this.marginLeft);
      console.log("margin:" + this.margin);
  },
  onClickBtnmarginLeft(operator){
     if(operator == "+"){
       this.marginLeft += 5;
     }else if(operator == "-"){
       this.marginLeft -= 5;
     }
      console.log("marginLeft:" + this.marginLeft);
  },
  onClickBtnmarginRight(operator){
      if(operator == "+"){
        this.marginRight += 5;
      }else if(operator == "-"){
        this.marginRight -= 5;
      }
      console.log("marginRight:" + this.marginRight);
  },
  onClickBtnmarginTop(operator){
      if(operator == "+"){
        this.marginTop += 5;
      }else if(operator == "-"){
        this.marginTop -= 5;
      }
      console.log("marginTop:" + this.marginTop);
  },
  onClickBtnmarginBottom(operator){
    if(operator == "+"){
        this.marginBottom += 5;
      }else if(operator == "-"){
        this.marginBottom -= 5;
      }
      console.log("marginBottom:" + this.marginBottom);
  },
  onClickBtnNextPage:routePage('pages/slider/third/index').changePage

}
/*  */