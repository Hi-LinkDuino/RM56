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

  touchStart(event){
    var globalX = event.touches[0].globalX
    var globalY = event.touches[0].globalY
    var localX = event.touches[0].localX
    var localY = event.touches[0].localY
    var size = event.touches[0].size
    var force = event.touches[0].force
    var changeGlobalX = event.changedTouches[0].globalX
    var changeGlobalY = event.changedTouches[0].globalY
    var changeLocalX = event.changedTouches[0].localX
    var changeLocalY = event.changedTouches[0].localY
    var changeSize = event.changedTouches[0].size
    var changeForce = event.changedTouches[0].force
    var message = 'globalX--' + globalX + ',globalY--' + globalY +
    ',localX--' + localX + ',localY--' + localY  + ',size--' + size + ',force--' + force +
    ',changeGlobalX--' + changeGlobalX + ',changeGlobalY--' + changeGlobalY +
    ',changeLocalX--' + changeLocalX + ',changeLocalY--' + changeLocalY  +
    ',changeSize--' + changeSize + ',changeForce--' + changeForce;
    prompt.showToast({
      message: 'touchstart:\n' + message
    });
  },

  touchMove(event){
    var globalX = event.touches[0].globalX
    var globalY = event.touches[0].globalY
    var localX = event.touches[0].localX
    var localY = event.touches[0].localY
    var size = event.touches[0].size
    var force = event.touches[0].force
    var changeGlobalX = event.changedTouches[0].globalX
    var changeGlobalY = event.changedTouches[0].globalY
    var changeLocalX = event.changedTouches[0].localX
    var changeLocalY = event.changedTouches[0].localY
    var changeSize = event.changedTouches[0].size
    var changeForce = event.changedTouches[0].force
    var message = 'globalX--' + globalX + ',globalY--' + globalY +
    ',localX--' + localX + ',localY--' + localY  + ',size--' + size + ',force--' + force +
    ',changeGlobalX--' + changeGlobalX + ',changeGlobalY--' + changeGlobalY +
    ',changeLocalX--' + changeLocalX + ',changeLocalY--' + changeLocalY  +
    ',changeSize--' + changeSize + ',changeForce--' + changeForce;
    prompt.showToast({
      message: 'touchMove:\n' +message
    });
  },

  touchEnd(event){
    var globalX = event.touches[0].globalX
    var globalY = event.touches[0].globalY
    var localX = event.touches[0].localX
    var localY = event.touches[0].localY
    var size = event.touches[0].size
    var force = event.touches[0].force
    var changeGlobalX = event.changedTouches[0].globalX
    var changeGlobalY = event.changedTouches[0].globalY
    var changeLocalX = event.changedTouches[0].localX
    var changeLocalY = event.changedTouches[0].localY
    var changeSize = event.changedTouches[0].size
    var changeForce = event.changedTouches[0].force
    var message = 'globalX--' + globalX + ',globalY--' + globalY +
    ',localX--' + localX + ',localY--' + localY  + ',size--' + size + ',force--' + force +
    ',changeGlobalX--' + changeGlobalX + ',changeGlobalY--' + changeGlobalY +
    ',changeLocalX--' + changeLocalX + ',changeLocalY--' + changeLocalY  +
    ',changeSize--' + changeSize + ',changeForce--' + changeForce;
    prompt.showToast({
      message: 'touchEnd:\n' +message
    });
  },

  touchCancel(event){
    var globalX = event.touches[0].globalX
    var globalY = event.touches[0].globalY
    var localX = event.touches[0].localX
    var localY = event.touches[0].localY
    var size = event.touches[0].size
    var force = event.touches[0].force
    var changeGlobalX = event.changedTouches[0].globalX
    var changeGlobalY = event.changedTouches[0].globalY
    var changeLocalX = event.changedTouches[0].localX
    var changeLocalY = event.changedTouches[0].localY
    var changeSize = event.changedTouches[0].size
    var changeForce = event.changedTouches[0].force
    var message = 'globalX--' + globalX + ',globalY--' + globalY +
    ',localX--' + localX + ',localY--' + localY  + ',size--' + size + ',force--' + force +
    ',changeGlobalX--' + changeGlobalX + ',changeGlobalY--' + changeGlobalY +
    ',changeLocalX--' + changeLocalX + ',changeLocalY--' + changeLocalY  +
    ',changeSize--' + changeSize + ',changeForce--' + changeForce;
    prompt.showToast({
      message: 'touchCancel:\n' +message
    });
  },

  click(){
    prompt.showToast({
      message: 'click'
    });
  },

  doubleClick(){
    prompt.showToast({
      message: 'doubleClick'
    });
  },

  longPress(){
    prompt.showToast({
      message: 'longPress'
    });
  },

  focus(){
    prompt.showToast({
      message: 'focus'
    });
  },

  blur(){
    prompt.showToast({
      message: 'blur'
    });
  },

  key(event){
    var code = event.code;
    var action = event.action;
    var repeatCount = event.repeatCount;
    var timestampStart = event.timestampStart;
    var message = 'code--' + code + ',action--' + action +
    ',repeatCount--' + repeatCount + ',timestampStart--' + timestampStart;
    prompt.showToast({
      message: 'key:\n' + message
    });
  },

  swipe(event){
    var direction = event.direction;
    var distance = event.distance;
    var message = 'direction--' + direction + ',distance--' + distance;
    prompt.showToast({
      message: 'swipe:\n' + message
    });
  },

  attached(){
    prompt.showToast({
      message: 'attached'
    });
  },

  detached(){
    prompt.showToast({
      message: 'detached'
    });
  },

  pinchStart(event){
    var scale = event.scale
    var pinchCenterX = event.pinchCenterX
    var pinchCenterY = event.pinchCenterY
    var message = 'scale--' + scale + ',pinchCenterX--' + pinchCenterX +
    ',pinchCenterY--' + pinchCenterY;
    prompt.showToast({
      message: 'pinchStart:\n' + message
    });
  },

  pinchUpdate(event){
    var scale = event.scale
    var pinchCenterX = event.pinchCenterX
    var pinchCenterY = event.pinchCenterY
    var message = 'scale--' + scale + ',pinchCenterX--' + pinchCenterX +
    ',pinchCenterY--' + pinchCenterY;
    prompt.showToast({
      message: 'pinchUpdate:\n' + message
    });
  },

  pinchEnd(event){
    var scale = event.scale
    var pinchCenterX = event.pinchCenterX
    var pinchCenterY = event.pinchCenterY
    var message = 'scale--' + scale + ',pinchCenterX--' + pinchCenterX +
    ',pinchCenterY--' + pinchCenterY;
    prompt.showToast({
      message: 'pinchUpdate:\n' + message
    });
  },

  pinchCancel(event){
    var scale = event.scale
    var pinchCenterX = event.pinchCenterX
    var pinchCenterY = event.pinchCenterY
    var message = 'scale--' + scale + ',pinchCenterX--' + pinchCenterX +
    ',pinchCenterY--' + pinchCenterY;
    prompt.showToast({
      message: 'pinchCancel:\n' + message
    });
  },

  dragStart(event){
    var type = event.type
    var globalX = event.globalX
    var globalY = event.globalY
    var timestamp = event.timestamp
    var message = 'type--' + type + ',globalX--' + globalX +
    ',globalY--' + globalY + ',timestamp--' + timestamp;
    prompt.showToast({
      message: 'dragStart:\n' + message
    });
  },

  drag(event){
    var type = event.type
    var globalX = event.globalX
    var globalY = event.globalY
    var timestamp = event.timestamp
    var message = 'type--' + type + ',globalX--' + globalX +
    ',globalY--' + globalY + ',timestamp--' + timestamp;
    prompt.showToast({
      message: 'drag:\n' + message
    });
  },

  dragEnd(event){
    var type = event.type
    var globalX = event.globalX
    var globalY = event.globalY
    var timestamp = event.timestamp
    var message = 'type--' + type + ',globalX--' + globalX +
    ',globalY--' + globalY + ',timestamp--' + timestamp;
    prompt.showToast({
      message: 'dragEnd:\n' + message
    });
  },

  dragEnter(event){
    var type = event.type
    var globalX = event.globalX
    var globalY = event.globalY
    var timestamp = event.timestamp
    var message = 'type--' + type + ',globalX--' + globalX +
    ',globalY--' + globalY + ',timestamp--' + timestamp;
    prompt.showToast({
      message: 'dragEnter:\n' + message
    });
  },

  dragOver(event){
    var type = event.type
    var globalX = event.globalX
    var globalY = event.globalY
    var timestamp = event.timestamp
    var message = 'type--' + type + ',globalX--' + globalX +
    ',globalY--' + globalY + ',timestamp--' + timestamp;
    prompt.showToast({
      message: 'dragOver:\n' + message
    });
  },

  dragLeave(event){
    var type = event.type
    var globalX = event.globalX
    var globalY = event.globalY
    var timestamp = event.timestamp
    var message = 'type--' + type + ',globalX--' + globalX +
    ',globalY--' + globalY + ',timestamp--' + timestamp;
    prompt.showToast({
      message: 'dragLeave:\n' + message
    });
  },

  drop(event){
    var type = event.type
    var globalX = event.globalX
    var globalY = event.globalY
    var timestamp = event.timestamp
    var message = 'type--' + type + ',globalX--' + globalX +
    ',globalY--' + globalY + ',timestamp--' + timestamp;
    prompt.showToast({
      message: 'drop:\n' + message
    });
  },

  functionTest1(event){
    var function1 =  this.$element('function1');
    function1.focus(true)
    var rect = function1.getBoundingClientRect();
    var width = rect.width;
    var height = rect.height;
    var left = rect.left;
    var top = rect.top;
    var message = 'width--' + width + ',height--' + height +
    ',left--' + left + ',top--' + top;
    prompt.showToast({
      message: 'function1 rect:\n' + message
    });
  },

  functionTest2(event){
    var function2 =  this.$element('function2');
    let observer = function2.createIntersectionObserver({
      ratios: [0.2, 0], // number
    });
    observer.observe((isVisible, ratio)=> {
      console.info('this element is ' + isVisible + 'ratio is ' + ratio)
      prompt.showToast({
        message: 'function2 observer:\n' + 'isVisible--' + isVisible + ',ratio--' + ratio
      });
    })

    observer.unobserve()
  },

  functionTest3(event){
    var function3 =  this.$element('function3');

    var animation = function3.animate(frames, options);
    animation.play()
    animation.onfinish = function(){
      prompt.showToast({
        message: 'The animation is finished.'
      });
    };

    animation.oncancel = function(){
      prompt.showToast({
        message: 'The animation is canceled.'
      });
    };

    animation.onrepeat = function(){
      prompt.showToast({
        message: 'The animation is repeated.'
      });
    };

    setTimeout(() => {
      animation.reverse()
    }, 500)

    setTimeout(() => {
      animation.pause()
    }, 1000)

    setTimeout(() => {
      animation.cancel()
    }, 1500)
  },

  functionTest4(event){
    var function4 =  this.$element('function4');
    var scrollOffset = function4.getScrollOffset();
    var x = scrollOffset.x;
    var y = scrollOffset.y;
    var message = 'x--' + x + ',y--' + y;
    prompt.showToast({
      message: 'functionTest4 scrollOffset:\n' + message
    });

    var scrollParam = {
      dx:60,
      dy:0,
      smooth:true
    }
    function4.scrollBy(scrollParam)
  },

  reachStart(){
    prompt.showToast({
      message: 'reachStart'
    });
  },

  reachEnd(){
    prompt.showToast({
      message: 'reachEnd'
    });
  },

  reachTop(){
    prompt.showToast({
      message: 'reachTop'
    });
  },

  reachBottom(){
    prompt.showToast({
      message: 'reachBottom'
    });
  },

  onAccessibility(event) {
    if (event.eventType == 1) {
      console.log("onAccessibility" + event.eventType);
    }
    console.log("onAccessibility" + JSON.stringify(event));
  }
}
