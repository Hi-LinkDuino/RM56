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
        attributeNameProp : null,
        attributeNamePropNone: null,
        beginProp : null,
        beginPropNone : null,
        durProp : null,
        durPropNone : null,
        endProp : null,
        endPropNone: null,
        repeatCountProp : null,
        repeatCountPropNone : null,
        fillPropFreeze : null,
        fillPropRemove : null,
        calcModePropDiscrete : null,
        calcModePropLinear: null,
        calcModePropPaced : null,
        calcModePropSpline: null,
        keyTimesProp : null,
        keyTimesPropNone : null,
        keySplinesProp : null,
        keySplinesPropNone : null,
        byProp : null,
        byPropNone : null,
        fromProp : null,
        fromPropNone : null,
        toProp : null,
        toPropNone : null,
        valuesProp : null,
        valuesPropNone : null,
        typePropTranslate : null,
        typePropScale : null,
        typePropRotate : null,
        typePropSkewX : null,
        typePropSkewY : null,
    },

    onShow(){
        this.getCommonPropValues();
        globalThis.value = {
            idProp : this.idProp,
            attributeNameProp : this.attributeNameProp,
            attributeNamePropNone: this.attributeNamePropNone,
            beginProp : this.beginProp,
            beginPropNone : this.beginPropNone,
            durProp : this.durProp,
            durPropNone : this.durPropNone,
            endProp : this.endProp,
            endPropNone: this.endPropNone,
            repeatCountProp : this.repeatCountProp,
            repeatCountPropNone : this.repeatCountPropNone,
            fillPropFreeze : this.fillPropFreeze,
            fillPropRemove : this.fillPropRemove,
            calcModePropDiscrete : this.calcModePropDiscrete,
            calcModePropLinear : this.calcModePropLinear,
            calcModePropPaced : this.calcModePropPaced,
            calcModePropSpline : this.calcModePropSpline,
            keyTimesProp : this.keyTimesProp,
            keyTimesPropNone : this.keyTimesPropNone,
            keySplinesProp : this.keySplinesProp,
            keySplinesPropNone : this.keySplinesPropNone,
            byProp : this.byProp,
            byPropNone : this.byPropNone,
            fromProp : this.fromProp,
            fromPropNone : this.fromPropNone,
            toProp : this.toProp,
            toPropNone : this.toPropNone,
            valuesProp : this.valuesProp,
            valuesPropNone : this.valuesPropNone,
            typePropTranslate : this.typePropTranslate,
            typePropScale : this.typePropScale,
            typePropRotate : this.typePropRotate,
            typePropSkewX : this.typePropSkewX,
            typePropSkewY : this.typePropSkewY,
        }
    },

    getCommonPropValues(){
        this.idProp = this.$element("idProp").getInspector()
        this.attributeNameProp = this.$element("attributeNameProp").getInspector()
        this.attributeNamePropNone = this.$element("attributeNamePropNone").getInspector()
        this.beginProp = this.$element("beginProp").getInspector()
        this.beginPropNone = this.$element("beginPropNone").getInspector()
        this.durProp = this.$element("durProp").getInspector()
        this.durPropNone = this.$element("durPropNone").getInspector()
        this.endProp = this.$element("endProp").getInspector()
        this.endPropNone = this.$element("endPropNone").getInspector()
        this.repeatCountProp = this.$element("repeatCountProp").getInspector()
        this.repeatCountPropNone = this.$element("repeatCountPropNone").getInspector()
        this.fillPropFreeze = this.$element("fillPropFreeze").getInspector()
        this.fillPropRemove = this.$element("fillPropRemove").getInspector()
        this.calcModePropDiscrete = this.$element("calcModePropDiscrete").getInspector()
        this.calcModePropLinear = this.$element("calcModePropLinear").getInspector()
        this.calcModePropPaced = this.$element("calcModePropPaced").getInspector()
        this.calcModePropSpline = this.$element("calcModePropSpline").getInspector()
        this.keyTimesProp = this.$element("keyTimesProp").getInspector()
        this.keyTimesPropNone = this.$element("keyTimesPropNone").getInspector()
        this.keySplinesProp = this.$element("keySplinesProp").getInspector()
        this.keySplinesPropNone = this.$element("keySplinesPropNone").getInspector()
        this.byProp = this.$element("byProp").getInspector()
        this.byPropNone = this.$element("byPropNone").getInspector()
        this.fromProp = this.$element("fromProp").getInspector()
        this.fromPropNone = this.$element("fromPropNone").getInspector()
        this.toProp = this.$element("toProp").getInspector()
        this.toPropNone = this.$element("toPropNone").getInspector()
        this.valuesProp = this.$element("valuesProp").getInspector()
        this.valuesPropNone = this.$element("valuesPropNone").getInspector()
        this.typePropTranslate = this.$element("typePropTranslate").getInspector()
        this.typePropScale = this.$element("typePropScale").getInspector()
        this.typePropRotate = this.$element("typePropRotate").getInspector()
        this.typePropSkewX = this.$element("typePropSkewX").getInspector()
        this.typePropSkewY = this.$element("typePropSkewY").getInspector()
    },
}
