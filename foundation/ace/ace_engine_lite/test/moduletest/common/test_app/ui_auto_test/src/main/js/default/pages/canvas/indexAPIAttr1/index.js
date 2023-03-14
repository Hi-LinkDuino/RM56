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
    ...routePage("pages/canvas/indexAPI/index"),
    fillStyle1Func: function () {
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        ctx.fillStyle = "#000000";
        ctx.fillRect(20,20,100,150);
    },
    fillStyle2Func: function () {
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        ctx.fillStyle = "#FFFFFF";
        ctx.fillRect(20,20,100,150);
    },
    fillStyle3Func: function () {
        var canvasRef = this.$refs.canvas3;
        var ctx = canvasRef.getContext("2d");
        ctx.fillStyle = "abcdef";
        ctx.fillRect(20,20,100,150);
    },
    strokeStyle1Func: function () {
        var canvasRef = this.$refs.canvas4;
        var ctx = canvasRef.getContext("2d");
        ctx.fillStyle = "#000000";
        ctx.fillRect(20,20,100,150);
    },
    strokeStyle2Func: function () {
        var canvasRef = this.$refs.canvas5;
        var ctx = canvasRef.getContext("2d");
        ctx.fillStyle = "#FFFFFF";
        ctx.fillRect(20,20,100,150);
    },
    strokeStyle3Func: function () {
        var canvasRef = this.$refs.canvas6;
        var ctx = canvasRef.getContext("2d");
        ctx.fillStyle = "#ghijkl";
        ctx.fillRect(20,20,100,150);
    },
    lineWidth1Func: function () {
        var canvasRef = this.$refs.canvas7;
        var ctx = canvasRef.getContext("2d");
        ctx.strokeStyle = "#ff0000";
        ctx.lineWidth = "-32768";
        ctx.strokeRect(20,20,100,150);
    },
    lineWidth2Func: function () {
        var canvasRef = this.$refs.canvas8;
        var ctx = canvasRef.getContext("2d");
        ctx.strokeStyle = "#ff0000";
        ctx.lineWidth = "32767";
        ctx.strokeRect(0,0,100,100);
    },
    lineWidth3Func: function () {
        var canvasRef = this.$refs.canvas9;
        var ctx = canvasRef.getContext("2d");
        ctx.strokeStyle = "#ff0000";
        ctx.lineWidth = "abcdef";
        ctx.strokeRect(0,0,100,100);
    }
}