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
    data:{
        array: [
            {id: 1, name: 'jack', age: 18},
            {id: 2, name: 'tony', age: 18},
        ],
        listOne:[{}],
        listThree:[{},{},{}],
        idProp : null,
        classProp : null,
        classPropNone : null,
        styleProp : null,
        refProp : null,
        refPropNone : null,
        disabledPropTrue : null,
        disabledPropFalse : null,
        disabledPropNone : null,
        focusablePropTrue : null,
        focusablePropFalse : null,
        focusablePropNone : null,
        dataProp : null,
        dataPropNone : null,
        clickEffectPropSmall : null,
        clickEffectPropMedium : null,
        clickEffectPropLarge : null,
        clickEffectPropNone : null,
        dirPropRtl : null,
        dirPropAuto : null,
        dirPropLtr : null,
        dirPropNone : null,
        forPropNull : null,
        forPropOne : null,
        forPropThree : null,
        ifPropTrue : null,
        showPropTrue : null,
        showPropFalse : null,
        showPropNone : null,

        min : null,
        max : null,
        maxNone : null,
        step : null,
        stepNone : null,
        value : null,
        typeContinuous : null,
        typeIntermittent : null,
        typeNone : null,
        minicon : null,
        miniconNone : null,
        maxicon : null,
        maxiconNone : null,
        modeOutset : null,
        modeInset : null,
        modeNone : null,
        showstepsTrue : null,
        showstepsFalse : null,
        showstepsNone : null,
        showtipsTrue : null,
        showtipsFalse : null,
        showtipsNone : null
    },

    onShow(){
        this.getCommonPropValues();
        this.getSpecificPropValues();
        globalThis.value = {
            idProp : this.idProp, classProp : this.classProp, classPropNone : this.classPropNone,
            styleProp : this.styleProp, refProp : this.refProp, refPropNone : this.refPropNone,
            disabledPropTrue : this.disabledPropTrue, disabledPropFalse : this.disabledPropFalse,
            disabledPropNone : this.disabledPropNone, focusablePropTrue : this.focusablePropTrue,
            focusablePropFalse : this.focusablePropFalse, focusablePropNone : this.focusablePropNone,
            dataProp : this.dataProp, dataPropNone : this.dataPropNone,
            clickEffectPropSmall : this.clickEffectPropSmall, clickEffectPropMedium : this.clickEffectPropMedium,
            clickEffectPropLarge : this.clickEffectPropLarge, clickEffectPropNone : this.clickEffectPropNone,
            dirPropRtl : this.dirPropRtl, dirPropAuto : this.dirPropAuto, dirPropLtr : this.dirPropLtr,
            dirPropNone : this.dirPropNone, forPropNull : this.forPropNull, forPropOne : this.forPropOne,
            forPropThree : this.forPropThree, ifPropTrue : this.ifPropTrue, showPropTrue : this.showPropTrue,
            showPropFalse : this.showPropFalse, showPropNone : this.showPropNone,

            min : this.min,
            max : this.max,
            maxNone : this.maxNone,
            step : this.step,
            stepNone : this.stepNone,
            value : this.value,
            typeContinuous : this.typeContinuous,
            typeIntermittent : this.typeIntermittent,
            typeNone : this.typeNone,
            minicon : this.minicon,
            miniconNone : this.miniconNone,
            maxicon : this.maxicon,
            maxiconNone : this.maxiconNone,
            modeOutset : this.modeOutset,
            modeInset : this.modeInset,
            modeNone : this.modeNone,
            showstepsTrue : this.showstepsTrue,
            showstepsFalse : this.showstepsFalse,
            showstepsNone : this.showstepsNone,
            showtipsTrue : this.showtipsTrue,
            showtipsFalse : this.showtipsFalse,
            showtipsNone : this.showtipsNone
        }
    },

    getCommonPropValues(){
        this.idProp = this.$element("idProp").getInspector()
        this.classProp = this.$element("classProp").getInspector()
        this.classPropNone = this.$element("classPropNone").getInspector()
        this.styleProp = this.$element("styleProp").getInspector()
        this.refProp = this.$element("refProp").getInspector()
        this.refPropNone = this.$element("refPropNone").getInspector()
        this.disabledPropTrue = this.$element("disabledPropTrue").getInspector()
        this.disabledPropFalse = this.$element("disabledPropFalse").getInspector()
        this.disabledPropNone = this.$element("disabledPropNone").getInspector()
        this.focusablePropTrue = this.$element("focusablePropTrue").getInspector()
        this.focusablePropFalse = this.$element("focusablePropFalse").getInspector()
        this.focusablePropNone = this.$element("focusablePropNone").getInspector()
        this.dataProp = this.$element("dataProp").getInspector()
        this.dataPropNone = this.$element("dataPropNone").getInspector()
        this.clickEffectPropSmall = this.$element("clickEffectPropSmall").getInspector()
        this.clickEffectPropMedium = this.$element("clickEffectPropMedium").getInspector()
        this.clickEffectPropLarge = this.$element("clickEffectPropLarge").getInspector()
        this.clickEffectPropNone = this.$element("clickEffectPropNone").getInspector()
        this.dirPropRtl = this.$element("dirPropRtl").getInspector()
        this.dirPropAuto = this.$element("dirPropAuto").getInspector()
        this.dirPropLtr = this.$element("dirPropLtr").getInspector()
        this.dirPropNone = this.$element("dirPropNone").getInspector()
        this.forPropNull = this.$element("forPropNull").getInspector()
        this.forPropOne = this.$element("forPropOne").getInspector()
        this.forPropThree = this.$element("forPropThree").getInspector()
        this.ifPropTrue = this.$element("ifPropTrue").getInspector()
        this.showPropTrue = this.$element("showPropTrue").getInspector()
        this.showPropFalse = this.$element("showPropFalse").getInspector()
        this.showPropNone = this.$element("showPropNone").getInspector()
    },

    getSpecificPropValues(){
        this.min = this.$element("min").getInspector()
        this.max = this.$element("max").getInspector()
        this.maxNone = this.$element("maxNone").getInspector()
        this.step = this.$element("step").getInspector()
        this.stepNone = this.$element("stepNone").getInspector()

        this.value = this.$element("value").getInspector()
        this.typeContinuous = this.$element("typeContinuous").getInspector()
        this.typeIntermittent = this.$element("typeIntermittent").getInspector()
        this.typeNone = this.$element("typeNone").getInspector()
        this.minicon = this.$element("minicon").getInspector()

        this.miniconNone = this.$element("miniconNone").getInspector()
        this.maxicon = this.$element("maxicon").getInspector()
        this.maxiconNone = this.$element("maxiconNone").getInspector()
        this.modeOutset = this.$element("modeOutset").getInspector()
        this.modeInset = this.$element("modeInset").getInspector()

        this.modeNone = this.$element("modeNone").getInspector()
        this.showstepsTrue = this.$element("showstepsTrue").getInspector()
        this.showstepsFalse = this.$element("showstepsFalse").getInspector()
        this.showstepsNone = this.$element("showstepsNone").getInspector()
        this.showtipsTrue = this.$element("showtipsTrue").getInspector()
        this.showtipsFalse = this.$element("showtipsFalse").getInspector()
        this.showtipsNone = this.$element("showtipsNone").getInspector()
    }
}
