/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import media from '@ohos.multimedia.media'
import Fileio from '@ohos.fileio'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('AudioDecoderFuncCallback', function () {
    const RESOURCEPATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audiodecoder/'
    const AUDIOPATH =  RESOURCEPATH + 'AAC_48000_32_1.aac';
    const AUDIOPATH2 = RESOURCEPATH + 'AAC_16000_1.aac';
    const AUDIOPATH3 = RESOURCEPATH + 'FLAC_48000_32_1.flac';
    const BASIC_PATH = RESOURCEPATH + 'results/decode_func_callback_';
    let audioDecodeProcessor;
    let readStreamSync;
    let eosframenum = 0;
    let samplerate = 44.1;
    let workdoneAtEOS = false;
    let stopAtEOS = false;
    let resetAtEOS = false;
    let flushAtEOS = false;
    let needGetMediaDes = false;
    let needrelease = false;
    let frameCnt = 1;
    let timestamp = 0;
    let sawInputEOS = false;
    let sawOutputEOS = false;
    let inputQueue = [];
    let outputQueue = [];
    let ES = [0, 283, 336, 291, 405, 438, 411, 215, 215, 313, 270, 342, 641, 554, 545, 545, 546,
            541, 540, 542, 552, 537, 533, 498, 472, 445, 430, 445, 427, 414, 386, 413, 370, 380,
            401, 393, 369, 391, 367, 395, 396, 396, 385, 391, 384, 395, 392, 386, 388, 384, 379,
            376, 381, 375, 373, 349, 391, 357, 384, 395, 384, 380, 386, 372, 386, 383, 378, 385,
            385, 384, 342, 390, 379, 387, 386, 393, 397, 362, 393, 394, 391, 383, 385, 377, 379,
            381, 369, 375, 379, 346, 382, 356, 361, 366, 394, 393, 385, 362, 406, 399, 384, 377,
            385, 389, 375, 346, 396, 388, 381, 383, 352, 357, 397, 382, 395, 376, 388, 373, 374,
            353, 383, 384, 393, 379, 348, 364, 389, 380, 381, 388, 423, 392, 381, 368, 351, 391,
            355, 358, 395, 390, 385, 382, 383, 388, 388, 389, 376, 379, 376, 384, 369, 354, 390,
            389, 396, 393, 382, 385, 353, 383, 381, 377, 411, 387, 390, 377, 349, 381, 390, 378,
            373, 375, 381, 351, 392, 381, 380, 381, 378, 387, 379, 383, 348, 386, 364, 386, 371,
            399, 399, 385, 380, 355, 397, 395, 382, 380, 386, 352, 387, 390, 373, 372, 388, 378,
            385, 368, 385, 370, 378, 373, 383, 368, 373, 388, 351, 384, 391, 387, 389, 383, 355,
            361, 392, 386, 354, 394, 392, 397, 392, 352, 381, 395, 349, 383, 390, 392, 350, 393,
            393, 385, 389, 393, 382, 378, 384, 378, 375, 373, 375, 389, 377, 383, 387, 373, 344,
            388, 379, 391, 373, 384, 358, 361, 391, 394, 363, 350, 361, 395, 399, 389, 398, 375,
            398, 400, 381, 354, 363, 366, 400, 400, 356, 370, 400, 394, 398, 385, 378, 372, 354,
            359, 393, 381, 363, 396, 396, 355, 390, 356, 355, 371, 399, 367, 406, 375, 377, 405,
            401, 390, 393, 392, 384, 386, 374, 358, 397, 389, 393, 385, 345, 379, 357, 388, 356,
            381, 389, 367, 358, 391, 360, 394, 396, 357, 395, 388, 394, 383, 357, 383, 392, 394,
            376, 379, 356, 386, 395, 387, 377, 377, 389, 377, 385, 351, 387, 350, 388, 384, 345,
            358, 368, 399, 394, 385, 384, 395, 378, 387, 386, 386, 376, 375, 382, 351, 359, 356,
            401, 388, 363, 406, 363, 374, 435, 366, 400, 393, 392, 371, 391, 359, 359, 397, 388,
            390, 420, 411, 369, 384, 382, 383, 383, 375, 381, 361, 380, 348, 379, 386, 379, 379,
            386, 371, 352, 378, 378, 388, 384, 385, 352, 355, 387, 383, 379, 362, 386, 399, 376,
            390, 350, 387, 357, 403, 398, 397, 360, 351, 394, 400, 399, 393, 388, 395, 370, 377,
            395, 360, 346, 381, 370, 390, 380, 391, 387, 382, 384, 383, 354, 349, 394, 358, 387,
            400, 386, 402, 354, 396, 387, 391, 365, 377, 359, 361, 365, 395, 388, 388, 384, 388,
            378, 374, 382, 376, 377, 389, 378, 341, 390, 376, 381, 375, 414, 368, 369, 387, 411,
            396, 391, 378, 389, 349, 383, 344, 381, 387, 380, 353, 361, 391, 365, 390, 396, 382,
            386, 385, 385, 409, 387, 386, 378, 372, 372, 374, 349, 388, 389, 348, 395, 380, 382,
            388, 375, 347, 383, 359, 389, 368, 361, 405, 398, 393, 395, 359, 360, 395, 395, 362,
            354, 388, 348, 388, 386, 390, 350, 388, 356, 369, 364, 404, 404, 391, 394, 385, 439,
            432, 375, 366, 441, 362, 367, 382, 374, 346, 391, 371, 354, 376, 390, 373, 382, 385,
            389, 378, 377, 347, 414, 338, 348, 385, 352, 385, 386, 381, 388, 387, 364, 465, 405,
            443, 387, 339, 376, 337, 379, 387, 370, 374, 358, 354, 357, 393, 356, 381, 357, 407,
            361, 397, 362, 394, 394, 392, 394, 391, 381, 386, 379, 354, 351, 392, 408, 393, 389,
            388, 385, 375, 388, 375, 388, 375, 354, 384, 379, 386, 394, 383, 359, 405, 395, 352,
            345, 403, 427, 373, 380, 350, 415, 378, 434, 385, 388, 387, 400, 405, 329, 391, 356,
            419, 358, 359, 375, 367, 391, 359, 369, 361, 376, 378, 379, 348, 390, 345, 388, 390,
            406, 349, 368, 364, 391, 384, 401, 384, 391, 361, 399, 359, 386, 392, 382, 386, 380,
            383, 345, 376, 393, 400, 395, 343, 352, 354, 381, 388, 357, 393, 389, 384, 389, 388,
            384, 404, 372, 358, 381, 352, 355, 485, 393, 371, 376, 389, 377, 391, 387, 376, 342,
            390, 375, 379, 396, 376, 402, 353, 392, 382, 383, 387, 386, 372, 377, 382, 388, 381,
            387, 357, 393, 385, 346, 389, 388, 357, 362, 404, 398, 397, 402, 371, 351, 370, 362,
            350, 388, 399, 402, 406, 377, 396, 359, 372, 390, 392, 368, 383, 346, 384, 381, 379,
            367, 384, 389, 381, 371, 358, 422, 372, 382, 374, 444, 412, 369, 362, 373, 389, 401,
            383, 380, 366, 365, 361, 379, 372, 345, 382, 375, 376, 375, 382, 356, 395, 383, 384,
            391, 361, 396, 407, 365, 351, 385, 378, 403, 344, 352, 387, 397, 399, 377, 371, 381,
            415, 382, 388, 368, 383, 405, 390, 386, 384, 374, 375, 381, 371, 372, 374, 377, 346,
            358, 381, 377, 359, 385, 396, 385, 390, 389, 391, 375, 357, 389, 390, 377, 370, 379,
            351, 381, 381, 380, 371, 386, 389, 389, 383, 362, 393, 388, 355, 396, 383, 352, 384,
            352, 383, 362, 396, 385, 396, 357, 388, 382, 377, 373, 379, 383, 386, 350, 393, 355,
            380, 401, 392, 391, 402, 391, 427, 407, 394, 332, 398, 367, 373, 343, 381, 383, 386,
            382, 349, 353, 393, 378, 386, 375, 390, 356, 392, 384, 387, 380, 381, 385, 386, 383,
            378, 379, 359, 381, 382, 388, 357, 357, 397, 358, 424, 382, 352, 409, 374, 368, 365,
            399, 352, 393, 389, 385, 352, 380, 398, 389, 385, 387, 387, 353, 402, 396, 386, 357,
            395, 368, 369, 407, 394, 383, 362, 380, 385, 368, 375, 365, 379, 377, 388, 380, 346,
            383, 381, 399, 359, 386, 455, 368, 406, 377, 339, 381, 377, 373, 371, 338, 371, 387,
            376, 386, 388, 382, 381, 377, 389, 352, 391, 396, 398, 374, 352, 358, 387, 368, 401,
            395, 387, 387, 354, 393, 401, 382, 390, 371, 374, 387, 377, 379, 377, 383, 374, 367,
            372, 393, 365, 392, 350, 378, 385, 377, 382, 388, 353, 389, 359, 366, 404, 390, 381,
            396, 388, 394, 380, 379, 372, 355, 397, 382, 351, 397, 378, 375, 378, 369, 387, 380,
            383, 346, 357, 362, 371, 413, 393, 365, 395, 391, 384, 386, 385, 390, 355, 385, 394,
            384, 383, 431, 369, 373, 392, 397, 396, 353, 391, 352, 375, 382, 397, 393, 371, 403,
            375, 338, 418, 368, 372, 370, 344, 392, 360, 402, 402, 425, 374, 378, 376, 380, 378,
            347, 379, 414, 377, 390, 383, 357, 390, 385, 377, 378, 368, 399, 389, 383, 368, 372,
            358, 386, 355, 380, 386, 349, 381, 379, 380, 351, 389, 380, 353, 359, 365, 370, 407,
            388, 381, 358, 364, 396, 369, 407, 391, 358, 440, 419, 412, 395, 344, 382, 391, 353,
            403, 391, 371, 354, 362, 407, 411, 397, 385, 388, 357, 348, 368, 401, 404, 380, 370,
            369, 365, 349, 373, 346, 351, 394, 399, 387, 393, 363, 402, 394, 375, 389, 386, 395,
            386, 386, 353, 403, 363, 376, 398, 383, 372, 350, 381, 353, 445, 386, 369, 383, 351,
            392, 397, 384, 389, 354, 400, 392, 379, 386, 355, 391, 388, 361, 390, 408, 357, 390,
            387, 352, 393, 357, 358, 401, 383, 391, 384, 388, 377, 380, 354, 393, 387, 357, 396,
            360, 362, 388, 390, 359, 367, 366, 396, 388, 394, 363, 391, 394, 351, 391, 391, 350,
            360, 365, 397, 366, 375, 400, 412, 372, 380, 383, 384, 383, 390, 380, 361, 363, 387,
            358, 385, 359, 385, 399, 351, 392, 388, 385, 384, 350, 379, 385, 383, 375, 389, 381,
            382, 376, 389, 390, 375, 381, 382, 390, 385, 380, 359, 351, 383, 391, 388, 379, 354,
            387, 382, 358, 381, 381, 386, 397, 373, 381, 387, 387, 381, 378, 383, 370, 360, 386,
            362, 402, 357, 398, 398, 397, 378, 394, 381, 375, 380, 390, 373, 369, 388, 347, 356,
            384, 391, 386, 394, 388, 357, 403, 381, 376, 414, 383, 387, 349, 379, 383, 384, 377,
            394, 426, 373, 392, 367, 381, 378, 378, 374, 373, 369, 368, 332, 335, 381, 392, 388,
            381, 424, 367, 398, 399, 392, 379, 391, 379, 370, 343, 405, 378, 367, 345, 379, 347,
            388, 353, 392, 382, 384, 387, 350, 363, 398, 398, 359, 402, 454, 440, 341, 383, 381,
            385, 355, 349, 369, 360, 388, 383, 354, 435, 405, 404, 399, 385, 386, 364, 388, 373,
            376, 384, 395, 400, 408, 390, 375, 341, 372, 371, 407, 345, 378, 345, 407, 365, 398,
            396, 339, 379, 375, 410, 386, 407, 387, 365, 375, 384, 347, 393, 377, 350, 376, 375,
            381, 410, 394, 386, 345, 345, 354, 397, 386, 375, 390, 347, 411, 381, 383, 374, 379,
            380, 378, 391, 380, 339, 390, 383, 375];
    let ES_LENGTH = 1500;

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(function() {
        console.info('beforeEach case');
        audioDecodeProcessor = null;
        readStreamSync = undefined;
        eosframenum = 0;
        samplerate = 44.1;
        workdoneAtEOS = false;
        stopAtEOS = false;
        resetAtEOS = false;
        flushAtEOS = false;
        needGetMediaDes = false;
        needrelease = false;
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
        ES = [0, 283, 336, 291, 405, 438, 411, 215, 215, 313, 270, 342, 641, 554, 545, 545, 546,
            541, 540, 542, 552, 537, 533, 498, 472, 445, 430, 445, 427, 414, 386, 413, 370, 380,
            401, 393, 369, 391, 367, 395, 396, 396, 385, 391, 384, 395, 392, 386, 388, 384, 379,
            376, 381, 375, 373, 349, 391, 357, 384, 395, 384, 380, 386, 372, 386, 383, 378, 385,
            385, 384, 342, 390, 379, 387, 386, 393, 397, 362, 393, 394, 391, 383, 385, 377, 379,
            381, 369, 375, 379, 346, 382, 356, 361, 366, 394, 393, 385, 362, 406, 399, 384, 377,
            385, 389, 375, 346, 396, 388, 381, 383, 352, 357, 397, 382, 395, 376, 388, 373, 374,
            353, 383, 384, 393, 379, 348, 364, 389, 380, 381, 388, 423, 392, 381, 368, 351, 391,
            355, 358, 395, 390, 385, 382, 383, 388, 388, 389, 376, 379, 376, 384, 369, 354, 390,
            389, 396, 393, 382, 385, 353, 383, 381, 377, 411, 387, 390, 377, 349, 381, 390, 378,
            373, 375, 381, 351, 392, 381, 380, 381, 378, 387, 379, 383, 348, 386, 364, 386, 371,
            399, 399, 385, 380, 355, 397, 395, 382, 380, 386, 352, 387, 390, 373, 372, 388, 378,
            385, 368, 385, 370, 378, 373, 383, 368, 373, 388, 351, 384, 391, 387, 389, 383, 355,
            361, 392, 386, 354, 394, 392, 397, 392, 352, 381, 395, 349, 383, 390, 392, 350, 393,
            393, 385, 389, 393, 382, 378, 384, 378, 375, 373, 375, 389, 377, 383, 387, 373, 344,
            388, 379, 391, 373, 384, 358, 361, 391, 394, 363, 350, 361, 395, 399, 389, 398, 375,
            398, 400, 381, 354, 363, 366, 400, 400, 356, 370, 400, 394, 398, 385, 378, 372, 354,
            359, 393, 381, 363, 396, 396, 355, 390, 356, 355, 371, 399, 367, 406, 375, 377, 405,
            401, 390, 393, 392, 384, 386, 374, 358, 397, 389, 393, 385, 345, 379, 357, 388, 356,
            381, 389, 367, 358, 391, 360, 394, 396, 357, 395, 388, 394, 383, 357, 383, 392, 394,
            376, 379, 356, 386, 395, 387, 377, 377, 389, 377, 385, 351, 387, 350, 388, 384, 345,
            358, 368, 399, 394, 385, 384, 395, 378, 387, 386, 386, 376, 375, 382, 351, 359, 356,
            401, 388, 363, 406, 363, 374, 435, 366, 400, 393, 392, 371, 391, 359, 359, 397, 388,
            390, 420, 411, 369, 384, 382, 383, 383, 375, 381, 361, 380, 348, 379, 386, 379, 379,
            386, 371, 352, 378, 378, 388, 384, 385, 352, 355, 387, 383, 379, 362, 386, 399, 376,
            390, 350, 387, 357, 403, 398, 397, 360, 351, 394, 400, 399, 393, 388, 395, 370, 377,
            395, 360, 346, 381, 370, 390, 380, 391, 387, 382, 384, 383, 354, 349, 394, 358, 387,
            400, 386, 402, 354, 396, 387, 391, 365, 377, 359, 361, 365, 395, 388, 388, 384, 388,
            378, 374, 382, 376, 377, 389, 378, 341, 390, 376, 381, 375, 414, 368, 369, 387, 411,
            396, 391, 378, 389, 349, 383, 344, 381, 387, 380, 353, 361, 391, 365, 390, 396, 382,
            386, 385, 385, 409, 387, 386, 378, 372, 372, 374, 349, 388, 389, 348, 395, 380, 382,
            388, 375, 347, 383, 359, 389, 368, 361, 405, 398, 393, 395, 359, 360, 395, 395, 362,
            354, 388, 348, 388, 386, 390, 350, 388, 356, 369, 364, 404, 404, 391, 394, 385, 439,
            432, 375, 366, 441, 362, 367, 382, 374, 346, 391, 371, 354, 376, 390, 373, 382, 385,
            389, 378, 377, 347, 414, 338, 348, 385, 352, 385, 386, 381, 388, 387, 364, 465, 405,
            443, 387, 339, 376, 337, 379, 387, 370, 374, 358, 354, 357, 393, 356, 381, 357, 407,
            361, 397, 362, 394, 394, 392, 394, 391, 381, 386, 379, 354, 351, 392, 408, 393, 389,
            388, 385, 375, 388, 375, 388, 375, 354, 384, 379, 386, 394, 383, 359, 405, 395, 352,
            345, 403, 427, 373, 380, 350, 415, 378, 434, 385, 388, 387, 400, 405, 329, 391, 356,
            419, 358, 359, 375, 367, 391, 359, 369, 361, 376, 378, 379, 348, 390, 345, 388, 390,
            406, 349, 368, 364, 391, 384, 401, 384, 391, 361, 399, 359, 386, 392, 382, 386, 380,
            383, 345, 376, 393, 400, 395, 343, 352, 354, 381, 388, 357, 393, 389, 384, 389, 388,
            384, 404, 372, 358, 381, 352, 355, 485, 393, 371, 376, 389, 377, 391, 387, 376, 342,
            390, 375, 379, 396, 376, 402, 353, 392, 382, 383, 387, 386, 372, 377, 382, 388, 381,
            387, 357, 393, 385, 346, 389, 388, 357, 362, 404, 398, 397, 402, 371, 351, 370, 362,
            350, 388, 399, 402, 406, 377, 396, 359, 372, 390, 392, 368, 383, 346, 384, 381, 379,
            367, 384, 389, 381, 371, 358, 422, 372, 382, 374, 444, 412, 369, 362, 373, 389, 401,
            383, 380, 366, 365, 361, 379, 372, 345, 382, 375, 376, 375, 382, 356, 395, 383, 384,
            391, 361, 396, 407, 365, 351, 385, 378, 403, 344, 352, 387, 397, 399, 377, 371, 381,
            415, 382, 388, 368, 383, 405, 390, 386, 384, 374, 375, 381, 371, 372, 374, 377, 346,
            358, 381, 377, 359, 385, 396, 385, 390, 389, 391, 375, 357, 389, 390, 377, 370, 379,
            351, 381, 381, 380, 371, 386, 389, 389, 383, 362, 393, 388, 355, 396, 383, 352, 384,
            352, 383, 362, 396, 385, 396, 357, 388, 382, 377, 373, 379, 383, 386, 350, 393, 355,
            380, 401, 392, 391, 402, 391, 427, 407, 394, 332, 398, 367, 373, 343, 381, 383, 386,
            382, 349, 353, 393, 378, 386, 375, 390, 356, 392, 384, 387, 380, 381, 385, 386, 383,
            378, 379, 359, 381, 382, 388, 357, 357, 397, 358, 424, 382, 352, 409, 374, 368, 365,
            399, 352, 393, 389, 385, 352, 380, 398, 389, 385, 387, 387, 353, 402, 396, 386, 357,
            395, 368, 369, 407, 394, 383, 362, 380, 385, 368, 375, 365, 379, 377, 388, 380, 346,
            383, 381, 399, 359, 386, 455, 368, 406, 377, 339, 381, 377, 373, 371, 338, 371, 387,
            376, 386, 388, 382, 381, 377, 389, 352, 391, 396, 398, 374, 352, 358, 387, 368, 401,
            395, 387, 387, 354, 393, 401, 382, 390, 371, 374, 387, 377, 379, 377, 383, 374, 367,
            372, 393, 365, 392, 350, 378, 385, 377, 382, 388, 353, 389, 359, 366, 404, 390, 381,
            396, 388, 394, 380, 379, 372, 355, 397, 382, 351, 397, 378, 375, 378, 369, 387, 380,
            383, 346, 357, 362, 371, 413, 393, 365, 395, 391, 384, 386, 385, 390, 355, 385, 394,
            384, 383, 431, 369, 373, 392, 397, 396, 353, 391, 352, 375, 382, 397, 393, 371, 403,
            375, 338, 418, 368, 372, 370, 344, 392, 360, 402, 402, 425, 374, 378, 376, 380, 378,
            347, 379, 414, 377, 390, 383, 357, 390, 385, 377, 378, 368, 399, 389, 383, 368, 372,
            358, 386, 355, 380, 386, 349, 381, 379, 380, 351, 389, 380, 353, 359, 365, 370, 407,
            388, 381, 358, 364, 396, 369, 407, 391, 358, 440, 419, 412, 395, 344, 382, 391, 353,
            403, 391, 371, 354, 362, 407, 411, 397, 385, 388, 357, 348, 368, 401, 404, 380, 370,
            369, 365, 349, 373, 346, 351, 394, 399, 387, 393, 363, 402, 394, 375, 389, 386, 395,
            386, 386, 353, 403, 363, 376, 398, 383, 372, 350, 381, 353, 445, 386, 369, 383, 351,
            392, 397, 384, 389, 354, 400, 392, 379, 386, 355, 391, 388, 361, 390, 408, 357, 390,
            387, 352, 393, 357, 358, 401, 383, 391, 384, 388, 377, 380, 354, 393, 387, 357, 396,
            360, 362, 388, 390, 359, 367, 366, 396, 388, 394, 363, 391, 394, 351, 391, 391, 350,
            360, 365, 397, 366, 375, 400, 412, 372, 380, 383, 384, 383, 390, 380, 361, 363, 387,
            358, 385, 359, 385, 399, 351, 392, 388, 385, 384, 350, 379, 385, 383, 375, 389, 381,
            382, 376, 389, 390, 375, 381, 382, 390, 385, 380, 359, 351, 383, 391, 388, 379, 354,
            387, 382, 358, 381, 381, 386, 397, 373, 381, 387, 387, 381, 378, 383, 370, 360, 386,
            362, 402, 357, 398, 398, 397, 378, 394, 381, 375, 380, 390, 373, 369, 388, 347, 356,
            384, 391, 386, 394, 388, 357, 403, 381, 376, 414, 383, 387, 349, 379, 383, 384, 377,
            394, 426, 373, 392, 367, 381, 378, 378, 374, 373, 369, 368, 332, 335, 381, 392, 388,
            381, 424, 367, 398, 399, 392, 379, 391, 379, 370, 343, 405, 378, 367, 345, 379, 347,
            388, 353, 392, 382, 384, 387, 350, 363, 398, 398, 359, 402, 454, 440, 341, 383, 381,
            385, 355, 349, 369, 360, 388, 383, 354, 435, 405, 404, 399, 385, 386, 364, 388, 373,
            376, 384, 395, 400, 408, 390, 375, 341, 372, 371, 407, 345, 378, 345, 407, 365, 398,
            396, 339, 379, 375, 410, 386, 407, 387, 365, 375, 384, 347, 393, 377, 350, 376, 375,
            381, 410, 394, 386, 345, 345, 354, 397, 386, 375, 390, 347, 411, 381, 383, 374, 379,
            380, 378, 391, 380, 339, 390, 383, 375];
        ES_LENGTH = 1500;
    })

    afterEach(async function() {
        console.info('afterEach case');
        if (audioDecodeProcessor != null) {
            await audioDecodeProcessor.release().then(() => {
                console.info('audioDecodeProcessor release success');
                audioDecodeProcessor = null;
            }, failCallback).catch(failCatch);
        }
    })

    afterAll(function() {
        console.info('afterAll case');
    })

    function resetParam(){
        readStreamSync = undefined;
        eosframenum = 0;
        samplerate = 44.1;
        workdoneAtEOS = false;
        stopAtEOS = false;
        resetAtEOS = false;
        flushAtEOS = false;
        needGetMediaDes = false;
        needrelease = false;
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
    }

    function writeFile(path, buf, len){
        try{
            let writestream = Fileio.createStreamSync(path, "ab+");
            let num = writestream.writeSync(buf, {length:len});
            writestream.flushSync();
            writestream.closeSync();
        }catch(e){
            console.info(e)
        }
    }

    function readFile(path){
        console.info('read file start execution');
        try{
            console.info('filepath: ' + path);
            readStreamSync = Fileio.createStreamSync(path, 'rb');
        }catch(e){
            console.info(e);
        }
    }

    function getContent(buf, len){
        console.info("start get content");
        let lengthreal = -1;
        lengthreal = readStreamSync.readSync(buf,{length:len});
        console.info('lengthreal: ' + lengthreal);
    }

    async function stopWork() {
        audioDecodeProcessor.stop((err) => {
            expect(err).assertUndefined();
            console.info("case stop success")
        })
    }

    async function resetWork() {
        resetParam();
        audioDecodeProcessor.reset((err) => {
            expect(err).assertUndefined();
            console.info("case reset success");
            if (needrelease) {
                audioDecodeProcessor.release((err) => {
                    expect(err).assertUndefined();
                    console.log("case release success");
                    audioDecodeProcessor = null;
                })
            }
        })
    }

    async function flushWork() {
        inputQueue = [];
        outputQueue = [];
        audioDecodeProcessor.flush((err) => {
            expect(err).assertUndefined();
            console.info("case flush at inputeos success");
            resetParam();
            readFile(AUDIOPATH);
            workdoneAtEOS =true;
        })
    }

    async function doneWork(done) {
        audioDecodeProcessor.stop((err) => {
            expect(err).assertUndefined();
            console.info("case stop success");
            resetParam();
            audioDecodeProcessor.reset((err) => {
                expect(err).assertUndefined();
                console.log("case reset success");
                audioDecodeProcessor.release((err) => {
                    expect(err).assertUndefined();
                    console.log("case release success");
                    audioDecodeProcessor = null;
                    done();
                })
            })
        })
    }

    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time));
    }

    function wait(time) {
        for(let t = Date.now(); Date.now() - t <= time;);
    }

    async function enqueueAllInputs(queue){
        while (queue.length > 0 && !sawInputEOS){
            let inputobject = queue.shift();
            console.info("frameCnt:" + frameCnt);
            if (frameCnt == eosframenum || frameCnt == ES_LENGTH + 1){
                inputobject.flags = 1;
                inputobject.timeMs = 0;
                inputobject.length = 0;
                sawInputEOS = true;
            }
            else{
                console.info("read frame from file");
                inputobject.timeMs = timestamp;
                inputobject.offset = 0;
                inputobject.length = ES[frameCnt];
                getContent(inputobject.data, ES[frameCnt]);
                inputobject.flags = 0;
            }
            timestamp += ES[frameCnt]/samplerate;
            frameCnt += 1;
            audioDecodeProcessor.pushInputData(inputobject, () => {
                console.info('queueInput success');
            })
        }
    }

    async function dequeueAllOutputs(queue, savepath, done) {
        while (queue.length > 0 && !sawOutputEOS) {
            let outputobject = queue.shift();
            if (outputobject.flags == 1) {
                sawOutputEOS = true;
                if (stopAtEOS) {
                    await stopWork();
                } else if (resetAtEOS) {
                    await resetWork();
                } else if (flushAtEOS) {
                    await flushWork();
                } else if (workdoneAtEOS) {
                    await doneWork(done);
                } else {
                    console.info("saw output EOS");
                }
            }
            else{
                writeFile(savepath, outputobject.data, outputobject.length);
                console.info("write to file success");
            }
            audioDecodeProcessor.freeOutputBuffer(outputobject, () => {
                console.info('release output success');
            })
        }
    }

    function setCallback(savepath, done) {
        console.info('case callback');
        audioDecodeProcessor.on('needInputData', async(inBuffer) => {
            console.info('inputBufferAvailable');
            inputQueue.push(inBuffer);
            await enqueueAllInputs(inputQueue);
        });
        audioDecodeProcessor.on('newOutputData', async(outBuffer) => {
            console.info('outputBufferAvailable');
            if (needGetMediaDes){
                audioDecodeProcessor.getOutputMediaDescription((err, MediaDescription) => {
                    expect(err).assertUndefined();
                    console.info("get OutputMediaDescription success");
                    console.info('get outputMediaDescription : ' + MediaDescription);
                    needGetMediaDes=false;
                });
            }
            outputQueue.push(outBuffer);
            await dequeueAllOutputs(outputQueue, savepath, done);
        });
        audioDecodeProcessor.on('error',(err) => {
            console.info('case error called,errName is' + err);
        });
        audioDecodeProcessor.on('streamChanged',(format) => {
            console.info('Output format changed: ' + format);
        });
    }

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_00_0100
        * @tc.name      : 000.test set EOS after last frame and reset
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_00_0100', 0, async function (done) {
        console.info("case test set EOS after last frame and reset");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 2,
                    "sample_rate": 44100,
                    "audio_sample_format": 1,
        }
        workdoneAtEOS = true;
        needGetMediaDes = true;
        let savepath = BASIC_PATH + '0000.pcm';
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        eventEmitter.on('createAudioDecoder', () => {
            media.createAudioDecoderByName('avdec_aac', (err, processor) => {
                expect(err).assertUndefined();
                console.info(`case createAudioDecoder 1`);
                audioDecodeProcessor = processor;
                eventEmitter.emit('getAudioDecoderCaps');
            })
        });
        media.getMediaCapability((err, mediaCaps) => {
            expect(err).assertUndefined();
            console.info('getMediaCapability success');
            if (typeof (mediaCaps) != 'undefined') {
                mediaCaps.getAudioDecoderCaps((err, audioCapsArray) => {
                    expect(err).assertUndefined();
                    console.info('getAudioDecoderCaps success');
                    if (typeof (audioCapsArray) != 'undefined') {
                        console.info('get first audioCaps'+ audioCapsArray[0]);
                        console.info('codecInfo: '+ audioCapsArray[0].codecInfo);
                        console.info('supportedBitrate: '+ audioCapsArray[0].supportedBitrate);
                        console.info('supportedBitrate.min: '+ audioCapsArray[0].supportedBitrate.min);
                    } else {
                        console.info('audioCaps is not defined');
                    }
                })
            } else {
                console.info('mediaCaps is not defined');
            }
            eventEmitter.emit('createAudioDecoder');
        })        
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0100
        * @tc.name      : 001.test set EOS manually before last frame and reset
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0100', 0, async function (done) {
        console.info("case test set EOS manually before last frame and reset");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        eosframenum = 500;
        workdoneAtEOS = true;
        let savepath = BASIC_PATH + '0100.pcm';
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.getMediaCapability((err, mediaCaps) => {
            expect(err).assertUndefined();
            console.info(`case getMediaCapability 1`);
            mediaCaps.getAudioDecoderCaps((err, audioCaps) => {
                expect(err).assertUndefined();
                console.info('getAudioDecoderCaps success');
                if (typeof (audioCaps) != 'undefined') {
                    console.info("case audioCaps " + audioCaps);
                } else {
                    console.info("case audioCaps is not defined");
                }
            })
        })
        media.createAudioDecoderByName('avdec_aac', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0200
        * @tc.name      : 002.test flush at running state
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0200', 0, async function (done) {
        console.info("case test flush at running state");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        workdoneAtEOS = true;
        let savepath = BASIC_PATH + '0200.pcm';
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
                setTimeout(() => {eventEmitter.emit('flush')},2000)
            })
        });
        eventEmitter.on('flush', () => {
            inputQueue = [];
            outputQueue = [];
            audioDecodeProcessor.flush((err) => {
                expect(err).assertUndefined();
                console.info(`case flush after 2s`);
            })
        });
        media.createAudioDecoderByMime('audio/mp4a-latm', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0300
        * @tc.name      : 003. test flush at EOS state
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0300', 0, async function (done) {
        console.info("case test flush at EOS state");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 2,
                    "sample_rate": 44100,
                    "audio_sample_format": 1,
        }
        eosframenum = 200;
        flushAtEOS = true;
        let savepath = BASIC_PATH + '0300.pcm';

         eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByMime('audio/mp4a-latm', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0400
        * @tc.name      : 004. test stop at running state and reset
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0400', 0, async function (done) {
        console.info("case test stop at running state and reset");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 2,
                    "sample_rate": 44100,
                    "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + '0400.pcm';
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
                eventEmitter.emit('stop');
            })
        });
        eventEmitter.on('stop', () => {
            sleep(5000).then(() => {
                audioDecodeProcessor.stop((err) => {
                    expect(err).assertUndefined();
                    console.info(`case stop 1`);
                    eventEmitter.emit('reset');
                })
            })
        });
        eventEmitter.on('reset', () => {
            resetParam();
            audioDecodeProcessor.reset((err) => {
                expect(err).assertUndefined();
                console.info(`case reset 1`);
                eventEmitter.emit('release');
            })
        });
        eventEmitter.on('release', () => {
            audioDecodeProcessor.release((err) => {
                expect(err).assertUndefined();
                console.info(`case release 1`);
                audioDecodeProcessor = null;
                done();
            })
        });
        media.createAudioDecoderByMime('audio/mp4a-latm', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0500
        * @tc.name      : 005. test stop and restart
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0500', 0, async function (done) {
        console.info("case test start - stop - restart");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        eosframenum = 200;
        let savepath = BASIC_PATH + '0500.pcm';
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
                eventEmitter.emit('stop');
            })
        });
        eventEmitter.on('stop', () => {
            sleep(5000).then(() => {
                audioDecodeProcessor.stop((err) => {
                    expect(err).assertUndefined();
                    console.info(`stop after 5s`);
                    resetParam();
                    readFile(AUDIOPATH);
                    eventEmitter.emit('restart');
                })
            })
        });
        eventEmitter.on('restart', () => {
            sleep(2000).then(() => {
                audioDecodeProcessor.start((err) => {
                    expect(err).assertUndefined();
                    console.info(`restart after 2s`);
                    workdoneAtEOS=true;
                    enqueueAllInputs(inputQueue);
                })
            })
        });
        media.createAudioDecoderByMime('audio/mp4a-latm', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0600
        * @tc.name      : 006. test reconfigure for new file with the same format
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0600', 0, async function (done) {
        console.info("case test reconfigure codec for new file with the same format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 2,
                    "sample_rate": 44100,
                    "audio_sample_format": 1,
        }
        eosframenum = 200;
        resetAtEOS = true;
        let savepath = BASIC_PATH + '0600.pcm';
        let mediaDescription2 = {
            "channel_count": 1,
            "sample_rate": 16000,
            "audio_sample_format": 1,
        }
        let hasreconfigured = false;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
                if (!hasreconfigured){
                    eventEmitter.emit('reconfigure', mediaDescription2);
                }
            })
        });
        eventEmitter.on('reconfigure', (mediaDescription2) => {
            sleep(10000).then(() => {
                audioDecodeProcessor.configure(mediaDescription2, (err) => {
                    expect(err).assertUndefined();
                    console.info(`case configure 2`);
                    resetParam();
                    console.info('resetParam success, resetAtEOS IS :' + resetAtEOS)
                    readFile(AUDIOPATH2)
                    savepath = BASIC_PATH + '0601.pcm';
                    workdoneAtEOS = true;
                    ES = [0, 239, 302, 309, 330, 474, 684, 699, 683, 674, 647, 649, 638, 644, 640,
                        639, 650, 702, 713, 718, 707, 707, 683, 670, 674, 699, 654, 650, 715, 770,
                        764, 736, 697, 664, 643, 649, 704, 730, 720, 710, 689, 670, 673, 653, 635,
                        643, 638, 653, 658, 683, 683, 705, 690, 681, 648, 659, 743, 681, 706, 707,
                        718, 714, 746, 725, 741, 709, 675, 675, 751, 698, 736, 738, 711, 702, 691,
                        668, 668, 650, 661, 678, 694, 719, 685, 700, 657, 671, 668, 701, 689, 685,
                        682, 668, 696, 690, 683, 739, 704, 667, 694, 716, 757, 714, 701, 664, 619,
                        609, 591, 644, 573, 572, 605, 595, 597, 529, 581, 578, 586, 537, 577, 535,
                        572, 570, 534, 547, 581, 530, 591, 521, 556, 570, 557, 541, 506, 572, 579,
                        502, 516, 582, 540, 584, 548, 555, 539, 578, 588, 541, 539, 526, 575, 535,
                        571, 525, 569, 560, 584, 551, 574, 572, 537, 550, 582, 512, 559, 593, 601,
                        594, 546, 560, 567, 547, 529, 576, 577, 556, 551, 530, 542, 575, 510, 559,
                        613, 628, 511, 566, 515, 539, 594, 536, 565, 551, 533, 613, 557, 523, 574,
                        582, 616, 610, 573, 509, 535];
                    ES_LENGTH = 200;
                    samplerate = 16;
                    hasreconfigured = true;
                    eventEmitter.emit('prepare');
                })
            })
        });
        media.createAudioDecoderByMime('audio/mp4a-latm', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0700
        * @tc.name      : 007. test reconfigure for new file with different formats
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_DECODER_FUNCTION_CALLBACK_01_0700', 0, async function (done) {
        console.info("case test reconfigure codec for new file with different format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let mediaDescription2 = {
            "channel_count": 1,
            "sample_rate": 48000,
            "audio_sample_format": 1,
        }
        let hasrecreate = false;
        eosframenum = 200;
        resetAtEOS = true;
        needrelease = true;
        let savepath = BASIC_PATH + '0700.pcm';
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.info("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
                if (!hasrecreate){
                    eventEmitter.emit('recreate');
                }
            })
        });
        eventEmitter.on('recreate', () => {
            sleep(10000).then(() => {
                media.createAudioDecoderByMime('audio/flac', (err, processor) => {
                    expect(err).assertUndefined();
                    console.info(`case createAudioDecoder flac`);
                    audioDecodeProcessor = processor;
                    hasrecreate = true;
                    eventEmitter.emit('reconfigure', mediaDescription2);
                })
            })
        });
        eventEmitter.on('reconfigure', (mediaDescription2) => {
            audioDecodeProcessor.configure(mediaDescription2, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 2`);
                resetParam();
                readFile(AUDIOPATH3)
                savepath = BASIC_PATH + '0701.pcm';
                workdoneAtEOS = true;
                ES = [0, 2116, 2093, 2886, 2859, 2798, 2778, 2752, 2752, 2754, 2720, 2898, 2829,
                    2806, 2796, 2786, 2774, 2758, 2741, 3489, 3342, 3272, 3167, 3048, 3060, 2919,
                    2839, 2794, 2770, 2763, 2756, 2722, 2950, 2876, 2842, 2827, 2792, 3121, 3075,
                    2969, 3240, 3199, 3082, 2975, 2895, 3135, 3024, 2970, 2902, 2860, 2802, 2778,
                    2779, 2963, 2893, 2870, 2831, 2800, 2762, 2752, 2760, 2759, 2726, 2889, 2960,
                    2876, 3223, 3040, 3006, 3042, 2980, 3097, 3069, 3215, 3365, 3296, 3653, 3480,
                    3300, 3140, 3014, 2975, 2952, 2896, 2897, 2842, 2929, 3567, 3929, 3485, 3082,
                    3625, 4953, 5239, 5094, 4993, 4821, 4431, 5195, 5542, 5557, 4894, 4414];
                ES_LENGTH = 100;
                samplerate = 48;
                eventEmitter.emit('prepare');
            })
        });
        media.createAudioDecoderByName('avdec_aac', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })
})

