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
    stroke001Func: function () {
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        ctx.stroke();
        ctx.rect(10,10,50,50);
        ctx.arc(100,70,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
    },
    stroke002Func: function () {
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.stroke();
        ctx.lineTo(80,60);
    },
    stroke003Func: function () {
        var canvasRef = this.$refs.canvas3;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.stroke();
        ctx.arc(100,70,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
    },
    stroke004Func: function () {
        var canvasRef = this.$refs.canvas4;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(10,50);
        ctx.stroke();
        ctx.arc(100,70,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
    },
    stroke005Func: function () {
        var canvasRef = this.$refs.canvas5;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.stroke();
        ctx.arc(100,70,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
    },
    stroke006Func: function () {
        var canvasRef = this.$refs.canvas6;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.stroke();
        ctx.lineTo(50,10);
    }
}
