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
    rect001Func: function () {
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.stroke();
    },
    rect002Func: function () {
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(32768,10,50,50);
        ctx.stroke();
    },
    rect003Func: function () {
        var canvasRef = this.$refs.canvas3;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,-32769,50,50);
        ctx.lineTo(80,60);
        ctx.stroke();
    },
    rect004Func: function () {
        var canvasRef = this.$refs.canvas4;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,32768,50);
        ctx.stroke();
    },
    rect005Func: function () {
        var canvasRef = this.$refs.canvas5;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,-32769);
        ctx.stroke();
    }
}