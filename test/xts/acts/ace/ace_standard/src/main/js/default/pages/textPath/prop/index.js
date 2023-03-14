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
        pathProp : null,
        pathPropNone: null,
        startOffsetPropLength : null,
        startOffsetPropPercentage : null,
        startOffsetPropNone : null,
        byProp : null,
        byPropNone : null,
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
            pathProp : this.pathProp,
            pathPropNone : this.pathPropNone,
            fontSizeProp : this.fontSizeProp,
            fontSizePropNone : this.fontSizePropNone,
            startOffsetPropLength : this.startOffsetPropLength,
            startOffsetPropPercentage : this.startOffsetPropPercentage,
            startOffsetPropNone : this.startOffsetPropNone,
            byProp : this.byProp,
            byPropNone : this.byPropNone,
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
        this.pathProp = this.$element("pathProp").getInspector()
        this.pathPropNone = this.$element("pathPropNone").getInspector()
        this.fontSizeProp = this.$element("fontSizeProp").getInspector()
        this.fontSizePropNone = this.$element("fontSizePropNone").getInspector()
        this.startOffsetPropLength = this.$element("startOffsetPropLength").getInspector()
        this.startOffsetPropPercentage = this.$element("startOffsetPropPercentage").getInspector()
        this.startOffsetPropNone = this.$element("startOffsetPropNone").getInspector()
        this.byProp = this.$element("byProp").getInspector()
        this.byPropNone = this.$element("byPropNone").getInspector()
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
