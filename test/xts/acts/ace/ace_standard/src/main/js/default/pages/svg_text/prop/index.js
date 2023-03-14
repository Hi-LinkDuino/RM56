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
        xPropLength : null,
        xPropPercentage: null,
        xPropNone : null,
        yPropLength : null,
        yPropPercentage : null,
        yPropNone : null,
        dxPropLength : null,
        dxPropPercentage: null,
        dxPropNone : null,
        dyPropLength : null,
        dyPropPercentage : null,
        dyPropNone : null,
        rotateProp : null,
        rotatePropNone: null,
        fontSizeProp : null,
        fontSizePropNone: null,
        fillProp : null,
        fillPropNone : null,
        fillOpacityProp : null,
        fillOpacityPropNone : null,
        opacityProp : null,
        opacityPropNone : null,
        strokeProp : null,
        strokePropNone : null,
        strokeWidthProp : null,
        strokeWidthPropNone : null,
        strokeOpacityProp : null,
        strokeOpacityPropNone : null,
    },

    onShow(){
        this.getCommonPropValues();
        globalThis.value = {
            idProp : this.idProp,
            xPropLength : this.xPropLength,
            xPropPercentage: this.xPropPercentage,
            xPropNone : this.xPropNone,
            yPropLength : this.yPropLength,
            yPropPercentage : this.yPropPercentage,
            yPropNone : this.yPropNone,
            dxPropLength : this.dxPropLength,
            dxPropPercentage: this.dxPropPercentage,
            dxPropNone : this.dxPropNone,
            dyPropLength : this.dyPropLength,
            dyPropPercentage : this.dyPropPercentage,
            dyPropNone : this.dyPropNone,
            rotateProp : this.rotateProp,
            rotatePropNone : this.rotatePropNone,
            fontSizeProp : this.fontSizeProp,
            fontSizePropNone : this.fontSizePropNone,
            fillProp : this.fillProp,
            fillPropNone : this.fillPropNone,
            fillOpacityProp : this.fillOpacityProp,
            fillOpacityPropNone : this.fillOpacityPropNone,
            opacityProp : this.opacityProp,
            opacityPropNone : this.opacityPropNone,
            strokeProp : this.strokeProp,
            strokePropNone : this.strokePropNone,
            strokeWidthProp : this.strokeWidthProp,
            strokeWidthPropNone : this.strokeWidthPropNone,
            strokeOpacityProp : this.strokeOpacityProp,
            strokeOpacityPropNone : this.strokeOpacityPropNone,
        }
    },

    getCommonPropValues(){
        this.idProp = this.$element("idProp").getInspector()
        this.xPropLength = this.$element("xPropLength").getInspector()
        this.xPropPercentage = this.$element("xPropPercentage").getInspector()
        this.xPropNone = this.$element("xPropNone").getInspector()
        this.yPropLength = this.$element("yPropLength").getInspector()
        this.yPropPercentage = this.$element("yPropPercentage").getInspector()
        this.yPropNone = this.$element("yPropNone").getInspector()
        this.dxPropLength = this.$element("dxPropLength").getInspector()
        this.dxPropPercentage = this.$element("dxPropPercentage").getInspector()
        this.dxPropNone = this.$element("dxPropNone").getInspector()
        this.dyPropLength = this.$element("dyPropLength").getInspector()
        this.dyPropPercentage = this.$element("dyPropPercentage").getInspector()
        this.dyPropNone = this.$element("dyPropNone").getInspector()
        this.rotateProp = this.$element("rotateProp").getInspector()
        this.rotatePropNone = this.$element("rotatePropNone").getInspector()
        this.fontSizeProp = this.$element("fontSizeProp").getInspector()
        this.fontSizePropNone = this.$element("fontSizePropNone").getInspector()
        this.fillProp = this.$element("fillProp").getInspector()
        this.fillPropNone = this.$element("fillPropNone").getInspector()
        this.fillOpacityProp = this.$element("fillOpacityProp").getInspector()
        this.fillOpacityPropNone = this.$element("fillOpacityPropNone").getInspector()
        this.opacityProp = this.$element("opacityProp").getInspector()
        this.opacityPropNone = this.$element("opacityPropNone").getInspector()
        this.strokeProp = this.$element("strokeProp").getInspector()
        this.strokePropNone = this.$element("strokePropNone").getInspector()
        this.strokeWidthProp = this.$element("strokeWidthProp").getInspector()
        this.strokeWidthPropNone = this.$element("strokeWidthPropNone").getInspector()
        this.strokeOpacityProp = this.$element("strokeOpacityProp").getInspector()
        this.strokeOpacityPropNone = this.$element("strokeOpacityPropNone").getInspector()
    },
}
