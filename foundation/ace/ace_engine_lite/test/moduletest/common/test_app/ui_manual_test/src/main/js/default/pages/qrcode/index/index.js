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
        top: 50,
        left: 10,
        width: 100,
        height: 200,
        borderWidth: 5,
        qrValue: 'helloworld',
        bgcolor: '#00ff00',
        color: '#0000ff'
    },
    clickAction:function(){
        console.log('trigger click');
    },
    swipeAction:function(){
        console.log('trigger swipe');
    },
    changeStyle:function(){
        this.top = this.top + 10;
        this.left = this.left + 10;
        this.borderWidth = this.borderWidth + 5;
        this.bgcolor = '#0000ff';
        this.color = '#00ff00';
        this.width = 200;
        console.log('qrcode top:' + this.top);
        console.log('qrcode left:' + this.left);
        console.log('qrcode borderWidth:' + this.borderWidth);
    },
    changeValue:function(){
        this.qrValue = this.qrValue + 'qrcode';
        console.log('qrcode top:' + this.value);
    },
    invalidColor:function(){
        console.log('invalidColor');
        this.bgcolor = 'invalid bgcolor';
        this.color = 'invalid color';
    },
    invalidSize:function(){
        console.log('invalidSize');
        this.width = 14;
        this.height = 14;
    },
    backMain:backPage().backMain
}
