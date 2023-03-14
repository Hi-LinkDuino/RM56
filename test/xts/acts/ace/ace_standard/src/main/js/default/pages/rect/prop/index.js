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

export default {
    data: {
        fillProp : null,
        fillOpacityPropTwo : null,
        fillOpacityPropOne : null,
        fillOpacityPropThree : null,
        fillRulePropTwo : null,
        fillRulePropOne : null,
        opacityPropOne : null,
        opacityPropTwo : null,
        opacityPropThree : null,
        strokeProp : null,
        strokeDashArrayProp : null,
        strokeDashOffsetProp : null,
        strokeLineJoinPropTwo : null,
        strokeLineJoinPropOne : null,
        strokeLineJoinPropThree : null,
        strokeLineCapPropOne : null,
        strokeLineCapPropTwo : null,
        strokeLineCapPropThree : null,
        strokeMiterLimitProp : null,
        strokeOpacityPropOne : null,
        strokeOpacityPropTwo : null,
        strokeOpacityPropThree : null,
        strokeWidthProp : null,
        transformProp : null,
        idProp : null,
        widthPropOne : null,
        heightPropOne : null,
        heightPropTwo : null,
        widthPropTwo : null,
        xProp : null,
        yProp : null,
        rxProp : null,
        ryProp : null,
        ifPropTrue : null,
        showPropTrue : null,
        showPropFalse : null,
        showPropNone : null
    },

    onShow(){
        this.getCommonPropValues();
        globalThis.value = {
            fillProp : this.fillProp,
            fillOpacityPropTwo : this.fillOpacityPropTwo,
            fillOpacityPropOne : this.fillOpacityPropOne,
            fillOpacityPropThree : this.fillOpacityPropThree,
            fillRulePropTwo : this.fillRulePropTwo,
            fillRulePropOne : this.fillRulePropOne,
            opacityPropOne : this.opacityPropOne,
            opacityPropTwo : this.opacityPropTwo,
            opacityPropThree : this.opacityPropThree,
            strokeProp : this.strokeProp,
            strokeDashArrayProp : this.strokeDashArrayProp,
            strokeDashOffsetProp : this.strokeDashOffsetProp,
            strokeLineJoinPropTwo : this.strokeLineJoinPropTwo,
            strokeLineJoinPropOne : this.strokeLineJoinPropOne,
            strokeLineJoinPropThree : this.strokeLineJoinPropThree,
            strokeLineCapPropOne : this.strokeLineCapPropOne,
            strokeLineCapPropTwo : this.strokeLineCapPropTwo,
            strokeLineCapPropThree : this.strokeLineCapPropThree,
            strokeMiterLimitProp : this.strokeMiterLimitProp,
            strokeOpacityPropOne : this.strokeOpacityPropOne,
            strokeOpacityPropTwo : this.strokeOpacityPropTwo,
            strokeOpacityPropThree : this.strokeOpacityPropThree,
            strokeWidthProp : this.strokeWidthProp,
            transformProp : this.transformProp,
            idProp : this.idProp,
            widthPropOne : this.widthPropOne,
            heightPropOne : this.heightPropOne,
            heightPropTwo : this.heightPropTwo,
            widthPropTwo : this.widthPropTwo,
            xProp : this.xProp,
            yProp : this.yProp,
            rxProp : this.rxProp,
            ryProp : this.ryProp,
            ifPropTrue: this.ifPropTrue,
            showPropTrue: this.showPropTrue,
            showPropFalse: this.showPropFalse,
            showPropNone: this.showPropNone
        }
    },

    getCommonPropValues(){
        this.fillProp = this.$element("fillProp").getInspector()
        this.fillOpacityPropTwo = this.$element("fillOpacityPropTwo").getInspector()
        this.fillOpacityPropOne = this.$element("fillOpacityPropOne").getInspector()
        this.fillOpacityPropThree = this.$element("fillOpacityPropThree").getInspector()
        this.fillRulePropTwo = this.$element("fillRulePropTwo").getInspector()
        this.fillRulePropOne = this.$element("fillRulePropOne").getInspector()
        this.opacityPropOne = this.$element("opacityPropOne").getInspector()
        this.opacityPropTwo = this.$element("opacityPropTwo").getInspector()
        this.opacityPropThree = this.$element("opacityPropThree").getInspector()
        this.strokeProp = this.$element("strokeProp").getInspector()
        this.strokeDashArrayProp = this.$element("strokeDashArrayProp").getInspector()
        this.strokeDashOffsetProp = this.$element("strokeDashOffsetProp").getInspector()
        this.strokeLineJoinPropTwo = this.$element("strokeLineJoinPropTwo").getInspector()
        this.strokeLineJoinPropOne = this.$element("strokeLineJoinPropOne").getInspector()
        this.strokeLineJoinPropThree = this.$element("strokeLineJoinPropThree").getInspector()
        this.strokeLineCapPropOne = this.$element("strokeLineCapPropOne").getInspector()
        this.strokeLineCapPropTwo = this.$element("strokeLineCapPropTwo").getInspector()
        this.strokeLineCapPropThree = this.$element("strokeLineCapPropThree").getInspector()
        this.strokeMiterLimitProp = this.$element("strokeMiterLimitProp").getInspector()
        this.strokeOpacityPropOne = this.$element("strokeOpacityPropOne").getInspector()
        this.strokeOpacityPropTwo = this.$element("strokeOpacityPropTwo").getInspector()
        this.strokeOpacityPropThree = this.$element("strokeOpacityPropThree").getInspector()
        this.strokeWidthProp = this.$element("strokeWidthProp").getInspector()
        this.transformProp = this.$element("transformProp").getInspector()
        this.idProp = this.$element("idProp").getInspector()
        this.widthPropOne = this.$element("widthPropOne").getInspector()
        this.heightPropOne = this.$element("heightPropOne").getInspector()
        this.heightPropTwo = this.$element("heightPropTwo").getInspector()
        this.widthPropTwo = this.$element("widthPropTwo").getInspector()
        this.xProp = this.$element("xProp").getInspector()
        this.yProp = this.$element("yProp").getInspector()
        this.rxProp = this.$element("rxProp").getInspector()
        this.ryProp = this.$element("ryProp").getInspector()
        this.ifPropTrue = this.$element("ifPropTrue").getInspector()
        this.showPropTrue = this.$element("showPropTrue").getInspector()
        this.showPropFalse = this.$element("showPropFalse").getInspector()
        this.showPropNone = this.$element("showPropNone").getInspector()
    },
}