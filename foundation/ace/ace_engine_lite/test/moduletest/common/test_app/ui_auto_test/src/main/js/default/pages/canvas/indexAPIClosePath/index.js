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
    closePath001Func: function () {
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        ctx.closePath();
        ctx.rect(10,10,50,50);
        ctx.rect(70,70,30,30);
        ctx.stroke();
    },
    closePath002Func: function () {
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        ctx.closePath();
        ctx.rect(10,10,50,50);
        ctx.moveTo(70,10);
        ctx.lineTo(80,60);
        ctx.stroke();
    },
    closePath003Func: function () {
        var canvasRef = this.$refs.canvas3;
        var ctx = canvasRef.getContext("2d");
        ctx.closePath();
        ctx.rect(10,10,50,50);
        ctx.lineTo(80,60);
        ctx.lineTo(100,60);
        ctx.stroke();
    },
    closePath004Func: function () {
        var canvasRef = this.$refs.canvas4;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.closePath();
        ctx.lineTo(80,60);
        ctx.lineTo(100,60);
        ctx.stroke();
    },
    closePath005Func: function () {
        var canvasRef = this.$refs.canvas5;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(80,60);
        ctx.lineTo(80,100);
        ctx.closePath();
        ctx.lineTo(100,60);
        ctx.stroke();
    },
    closePath006Func: function () {
        var canvasRef = this.$refs.canvas6;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.closePath();
        ctx.moveTo(70,10);
        ctx.lineTo(80,60);
        ctx.stroke();
    },
    closePath007Func: function () {
        var canvasRef = this.$refs.canvas7;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.closePath();
        ctx.lineTo(80,60);
        ctx.stroke();
    },
    closePath008Func: function () {
        var canvasRef = this.$refs.canvas8;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.closePath();
        ctx.arc(100,100,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
        ctx.stroke();
    },
    closePath009Func: function () {
        var canvasRef = this.$refs.canvas9;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(10,50);
        ctx.closePath();
        ctx.arc(100,100,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
        ctx.stroke();
    },
    closePath010Func: function () {
        var canvasRef = this.$refs.canvas10;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.closePath();
        ctx.arc(150,70,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
        ctx.stroke();
    },
    closePath011Func: function () {
        var canvasRef = this.$refs.canvas11;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.closePath();
        ctx.lineTo(50,10);
        ctx.stroke();
    },
    closePath012Func: function () {
        var canvasRef = this.$refs.canvas12;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(80,60);
        ctx.closePath();
        ctx.stroke();
    },
    closePath013Func: function () {
        var canvasRef = this.$refs.canvas13;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(80,60);
        ctx.lineTo(100,60);
        ctx.closePath();
        ctx.stroke();
    },
    closePath014Func: function () {
        var canvasRef = this.$refs.canvas14;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.moveTo(70,10);
        ctx.lineTo(80,60);
        ctx.closePath();
        ctx.stroke();
    },
    closePath015Func: function () {
        var canvasRef = this.$refs.canvas15;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.lineTo(80,60);
        ctx.closePath();
        ctx.stroke();
    },
    closePath016Func: function () {
        var canvasRef = this.$refs.canvas16;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.arc(120,70,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
        ctx.closePath();
        ctx.stroke();
    },
    closePath017Func: function () {
        var canvasRef = this.$refs.canvas17;
        var ctx = canvasRef.getContext("2d");
        ctx.moveTo(10,10);
        ctx.lineTo(10,50);
        ctx.arc(120,70,50,Math.PI*2*(30/360),Math.PI*2*(150/360),false);
        ctx.closePath();
        ctx.stroke();
    },
    closePath018Func: function () {
        var canvasRef = this.$refs.canvas18;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.arc(150,70,50,Math.PI*2*(150/360),Math.PI*2*(30/360),false);
        ctx.closePath();
        ctx.stroke();
    },
    closePath019Func: function () {
        var canvasRef = this.$refs.canvas19;
        var ctx = canvasRef.getContext("2d");
        ctx.arc(50,50,30,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
        ctx.lineTo(50,10);
        ctx.closePath();
        ctx.stroke();
    },
    closePath020Func: function () {
        var canvasRef = this.$refs.canvas20;
        var ctx = canvasRef.getContext("2d");
        ctx.rect(10,10,50,50);
        ctx.arc(150,70,50,Math.PI*2*(30/360),Math.PI*2*(150/360),false);
        ctx.closePath();
        ctx.stroke();
    }
}
