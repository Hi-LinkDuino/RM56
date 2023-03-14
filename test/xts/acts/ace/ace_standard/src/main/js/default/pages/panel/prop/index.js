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
    data:{
        listOne:[{}],
        listThree:[{},{},{}],
        idProp : null,
        classProp : null,
        classPropNone : null,
        styleProp : null,
        refProp : null,
        refPropNone : null,
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

        typePropFoldAble : null,
        typePropMinibar : null,
        typePropTemporary : null,
        modePropMini : null,
        modePropHalf: null,
        modePropFull: null,
        dragBarPropTrue: null,
        dragBarPropFalse: null,
        fullHeightPro: null,
        halfHeightPro: null,
        miniHeightPro: null
    },

    onShow(){
        this.getCommonPropValues();
        globalThis.value = {
            idProp : this.idProp,
            classProp : this.classProp,
            classPropNone : this.classPropNone,
            styleProp : this.styleProp,
            refProp : this.refProp,
            refPropNone : this.refPropNone,
            dataProp : this.dataProp,
            dataPropNone : this.dataPropNone,
            clickEffectPropSmall : this.clickEffectPropSmall,
            clickEffectPropMedium : this.clickEffectPropMedium,
            clickEffectPropLarge : this.clickEffectPropLarge,
            clickEffectPropNone : this.clickEffectPropNone,
            dirPropRtl : this.dirPropRtl,
            dirPropAuto : this.dirPropAuto,
            dirPropLtr : this.dirPropLtr,
            dirPropNone : this.dirPropNone,

            typePropFoldAble : this.typePropFoldAble,
            typePropMinibar : this.typePropMinibar,
            typePropTemporary : this.typePropTemporary,
            modePropMini : this.modePropMini,
            modePropHalf : this.modePropHalf,
            modePropFull : this.modePropFull,
            dragBarPropTrue : this.dragBarPropTrue,
            dragBarPropFalse : this.dragBarPropFalse,

            fullHeightPro : this.fullHeightPro,
            halfHeightPro : this.halfHeightPro,
            miniHeightPro : this.miniHeightPro,

        }
    },

    getCommonPropValues(){
        this.idProp = this.$element("idProp").getInspector()
        this.classProp = this.$element("classProp").getInspector()
        this.classPropNone = this.$element("classPropNone").getInspector()
        this.styleProp = this.$element("styleProp").getInspector()
        this.refProp = this.$element("refProp").getInspector()
        this.refPropNone = this.$element("refPropNone").getInspector()
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

        this.typePropFoldAble = this.$element("typePropFoldAble").getInspector()
        this.typePropMinibar = this.$element("typePropMinibar").getInspector()
        this.typePropTemporary = this.$element("typePropTemporary").getInspector()
        this.modePropMini = this.$element("modePropMini").getInspector()
        this.modePropHalf = this.$element("modePropHalf").getInspector()
        this.modePropFull = this.$element("modePropFull").getInspector()
        this.dragBarPropTrue = this.$element("dragBarPropTrue").getInspector()
        this.dragBarPropFalse = this.$element("dragBarPropFalse").getInspector()
        this.fullHeightPro = this.$element("fullHeightPro").getInspector()
        this.halfHeightPro = this.$element("halfHeightPro").getInspector()
        this.miniHeightPro = this.$element("miniHeightPro").getInspector()
    },
}
