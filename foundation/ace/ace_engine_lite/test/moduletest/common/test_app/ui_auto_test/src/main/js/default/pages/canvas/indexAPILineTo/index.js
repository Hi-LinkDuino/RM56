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

import {routePage} from "../../../common/js/general";
export default {
    ...routePage("pages/canvas/indexAPIDraw2/index"),
    lineTo001Func: function () {
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(10,50);
        ctx.stroke();
    },
    lineTo002Func: function () {
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(32768,50);
        ctx.stroke();
    },
    lineTo003Func: function () {
        var canvasRef = this.$refs.canvas3;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(10,-32769);
        ctx.stroke();
    }
}