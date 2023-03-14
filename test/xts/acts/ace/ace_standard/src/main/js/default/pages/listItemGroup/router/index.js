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
export default {
    data: {
        title: "",
        "array": [
            {"value": "列表文本1"},
            {"value": "列表文本2"},
        ],
        "array2": [
            {"value": "item1"},
            {"value": "item2"},
            {"value": "item3"},
            {"value": "item4"},
            {"value": "item5"},
            {"value": "item6"}
        ],
        direction: 'column',
        list: []
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

    collapseOne() {
        this.$element('myList').collapseGroup({
            groupid: 'group1'
        })
    },

    expandOne() {
        this.$element('myList').expandGroup({
            groupid: 'group1'
        })
    },

    collapseAll() {
        this.$element('myList').collapseGroup({

        });
    },

    expandAll() {
        this.$element('myList').expandGroup({

        });
    },

    groupClick() {
        prompt.showToast({
            message: 'groupClick'
        });
    },

    groupCollapse() {
        prompt.showToast({
            message: 'groupCollapse'
        });
    },

    groupExpand() {
        prompt.showToast({
            message: 'groupExpand'
        });
    }
}