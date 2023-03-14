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

        var styleValues = this.getStyleValues();
        var propsValues = this.getPropValues();

        globalThis.value = {
            styleValues:styleValues,
            propsValues:propsValues
        }
    },

    getStyleValues(){
        var styleValue1 = this.$element("style1").getInspector()
        var styleValue2 = this.$element("style2").getInspector()
        var styleValue3 = this.$element("style3").getInspector()
        var styleValue4 = this.$element("style4").getInspector()
        var styleValue5 = this.$element("style5").getInspector()
        var styleValue6 = this.$element("style6").getInspector()

        return {
            style1:styleValue1,
            style2:styleValue2,
            style3:styleValue3,
            style4:styleValue4,
            style5:styleValue5,
            style6:styleValue6,
        }
    },

    getPropValues(){
        var propValue1 = this.$element("prop1").getInspector()
        var propValue2 = this.$element("prop2").getInspector()
        var propValue3 = this.$element("prop3").getInspector()

        return {
            prop1:propValue1,
            prop2:propValue2,
            prop3:propValue3
        }
    },
}
