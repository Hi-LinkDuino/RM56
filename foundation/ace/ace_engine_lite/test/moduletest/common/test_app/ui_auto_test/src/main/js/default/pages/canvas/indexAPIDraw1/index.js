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
    fillRect1Func: function () {
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        ctx.fillRect(-32769,32768,-32769,32768);
    },
    fillRect2Func: function () {
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        ctx.fillRect(10,10);
    },
    strokeRect1Func: function () {
        var canvasRef = this.$refs.canvas3;
        var ctx = canvasRef.getContext("2d");
        ctx.strokeRect(-32769,32768,-32769,32768);
    },
    strokeRect2Func: function () {
        var canvasRef = this.$refs.canvas4;
        var ctx = canvasRef.getContext("2d");
        ctx.strokeRect(10,10);
    },
    fillText1Func: function () {
        var canvasRef = this.$refs.canvas5;
        var ctx = canvasRef.getContext("2d");
        ctx.font = "30px SourceHanSansSC-Regular";
        ctx.fillText("Hello JS", -32769, 32768);
    },
    fillText2Func: function () {
        var canvasRef = this.$refs.canvas6;
        var ctx = canvasRef.getContext("2d");
        ctx.font = "30px SourceHanSansSC-Regular";
        ctx.fillText("Hello JS");
    }
}
