/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import prompt from '@system.prompt';

var options = {
  duration: 1500,
  easing: 'friction',
  delay: 100,
  fill: 'forwards',
  iterations: 2,
  direction: 'normal',
};
var frames = [
  {
    transform: {
      translate: '-120px',
      rotate:'10deg',
      scale:0.2,
      skew:'40deg'
    },
    opacity: 0.1,
    offset: 0.0,
    width: '40%',
    height:'20px',
    backgroundColor:'#ff0000',
    backgroundPosition:'10px 20px',
    transformOrigin:'left top'
  },
  {
    transform: {
      translateX: '0px',
      translateY: '5px',
      rotateX:'10deg',
      rotateY:'10deg',
      scaleX:0.5,
      scaleY:0.7,
      skewX:'22deg',
      skewY:'30deg'
    },
    opacity: 0.6,
    offset: 2.0,
    width: '60%',
    height:'30px',
    backgroundColor:'#ff00ff',
    backgroundPosition:'15px 25px',
    transformOrigin:'center top'
  },
  {
    transform: {
      translateX: '100px',
      translateY: '0px',
      translateZ: '20px',
      rotateX:'0deg',
      rotateY:'0deg',
      rotateZ:'30deg',
      scaleX:1,
      scaleY:1,
      scaleZ:2,
      skewX:'0',
      skewY:'0',
      skewZ:'30deg'
    },
    opacity: 1,
    offset: 0.0,
    width: '100%',
    height:'30px',
    backgroundColor:'#ffff00',
    backgroundPosition:'0px',
    transformOrigin:'center center'
  },
];

export default {

  onMenuSelected(e) {
    prompt.showToast({
      message: e.value
    })
  },

  onShow(){
    // 通用属性
    var prop1 =  this.$element('prop1');
    var name1 = prop1.dataSet.name
    var prop2 =  this.$refs.prop2;
    var name2 = prop2.dataSet.name
    prompt.showToast({
      message: 'prop1--' + name1 + '\nprop2--' + name2
    });
  },
}
