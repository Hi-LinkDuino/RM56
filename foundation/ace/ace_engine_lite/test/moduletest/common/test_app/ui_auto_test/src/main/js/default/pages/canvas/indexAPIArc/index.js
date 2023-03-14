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
    arc001Func: function () {
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.stroke();
    },
    arc002Func: function () {
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(32768,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.stroke();
    },
    arc003Func: function () {
        var canvasRef = this.$refs.canvas3;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,-32769,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.stroke();
    },
    arc004Func: function () {
        var canvasRef = this.$refs.canvas4;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,32768,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.stroke();
    },
    arc005Func: function () {
        var canvasRef = this.$refs.canvas5;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(32768/360),Math.PI*2*(90/360),false);
        ctx.stroke();
    },
    arc006Func: function () {
        var canvasRef = this.$refs.canvas6;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(-32769/360),false);
        ctx.stroke();
    },
    arc007Func: function () {
        var canvasRef = this.$refs.canvas7;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360));
        ctx.stroke();
    },
    arc008Func: function () {
        var canvasRef = this.$refs.canvas8;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.stroke();
    },
    arc009Func: function () {
        var canvasRef = this.$refs.canvas9;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),true);
        ctx.stroke();
    },
    arc010Func: function () {
        var canvasRef = this.$refs.canvas10;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(100,70,50,Math.PI*2*(10/360),Math.PI*2*(1000/360),false);
        ctx.lineTo(100,100);
        ctx.stroke();
    },
    arc011Func: function () {
        var canvasRef = this.$refs.canvas11;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(100,70,50,Math.PI*2*(32412/360),Math.PI*2*(32052/360),false);
        ctx.lineTo(100,100);
        ctx.stroke();
    },
    arc012Func: function () {
        var canvasRef = this.$refs.canvas12;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(100,70,50,Math.PI*2*(1000/360),Math.PI*2*(10/360),true);
        ctx.lineTo(100,100);
        ctx.stroke();
    },
    arc013Func: function () {
        var canvasRef = this.$refs.canvas13;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(100,70,50,Math.PI*2*(-32412/360),Math.PI*2*(-32052/360),true);
        ctx.lineTo(100,100);
        ctx.stroke();
    }
}
