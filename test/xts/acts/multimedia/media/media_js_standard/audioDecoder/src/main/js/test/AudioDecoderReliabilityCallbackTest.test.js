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

describe('AudioDecoderReliabilityCallback', function () {
    const RESOURCEPATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audiodecoder/'
    const AUDIOPATH =  RESOURCEPATH + 'AAC_48000_32_1.aac';
    const BASIC_PATH = RESOURCEPATH + 'results/decode_reliability_callback';
    const END = 0;
    const CONFIGURE = 1;
    const PREPARE = 2;
    const START = 3;
    const FLUSH = 4;
    const STOP = 5;
    const RESET = 6;
    const HOLDON = 7;
    const WAITFORALLOUTS = 8;
    const CONFIGURE_ERROR = 9;
    const PREPARE_ERROR = 10;
    const START_ERROR = 11;
    const FLUSH_ERROR = 12;
    const STOP_ERROR = 13;
    const JUDGE_EOS = 14;
    const WAITTIME = 3000;
    let audioDecodeProcessor;
    let readStreamSync;
    let EOSFrameNum = 0;
    let samplerate = 44.1;
    let workdoneAtEOS = false;
    let flushAtEOS = false;
    let needGetMediaDes = false;
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
    let ES_LENGTH = 500;
    let mime = 'audio/mp4a-latm';
    let mediaDescription = {
                "channel_count": 2,
                "sample_rate": 44100,
                "audio_sample_format": 1,
    };

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(function() {
        console.info('beforeEach case');
        audioDecodeProcessor = null;
        readStreamSync = undefined;
        EOSFrameNum = 0;
        samplerate = 44.1;
        workdoneAtEOS = false;
        flushAtEOS = false;
        needGetMediaDes = false;
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
        ES_LENGTH = 500;
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

    function resetParam() {
        readStreamSync = undefined;
        EOSFrameNum = 0;
        samplerate = 44.1;
        workdoneAtEOS = false;
        flushAtEOS = false;
        needGetMediaDes = false;
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
    }

    function createAudioDecoder(savepath, mySteps, done) {
        media.createAudioDecoderByMime(mime, (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            setCallback(savepath, done);
            console.info("case start api test");
            nextStep(mySteps, mediaDescription, done);
        })
    }

    function writeFile(path, buf, len) {
        try{
            let writestream = Fileio.createStreamSync(path, "ab+");
            let num = writestream.writeSync(buf, {length:len});
            writestream.flushSync();
            writestream.closeSync();
        }catch(e) {
            console.info(e)
        }
    }

    function readFile(path) {
        console.info('read file start execution');
        try{
            console.info('filepath: ' + path);
            readStreamSync = Fileio.createStreamSync(path, 'rb');
        }catch(e) {
            console.info(e);
        }
    }

    function getContent(buf, len) {
        console.info("start get content");
        let lengthreal = -1;
        lengthreal = readStreamSync.readSync(buf,{length:len});
        console.info('lengthreal: ' + lengthreal);
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

    function nextStep(mySteps, mediaDescription, done) {
        console.info("case myStep[0]: " + mySteps[0]);
        if (mySteps[0] == END) {
            console.info('case to done');
            audioDecodeProcessor.release((err) => {
                expect(err).assertUndefined();
                console.info(`case release 1`);
                audioDecodeProcessor = null;
                done();
            });
        }
        switch (mySteps[0]) {
            case CONFIGURE:
                mySteps.shift();
                console.info(`case to configure`);
                audioDecodeProcessor.configure(mediaDescription, (err) => {
                    expect(err).assertUndefined();
                    console.info(`case configure 1`);
                    readFile(AUDIOPATH);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case PREPARE:
                mySteps.shift();
                console.info(`case to prepare`);
                audioDecodeProcessor.prepare((err) => {
                    expect(err).assertUndefined();
                    console.info(`case prepare 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case START:
                mySteps.shift();
                console.info(`case to start`);
                if (sawOutputEOS) {
                    resetParam();
                    readFile(AUDIOPATH);
                    workdoneAtEOS = true;
                    enqueueAllInputs(inputQueue);
                }
                audioDecodeProcessor.start((err) => {
                    expect(err).assertUndefined();
                    console.info(`case start 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case FLUSH:
                mySteps.shift();
                console.info(`case to flush`);
                inputQueue = [];
                outputQueue = [];
                audioDecodeProcessor.flush((err) => {
                    expect(err).assertUndefined();
                    console.info(`case flush 1`);
                    if (flushAtEOS) {
                        resetParam();
                        readFile(AUDIOPATH);
                        workdoneAtEOS = true;
                        flushAtEOS = false;
                    }
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case STOP:
                mySteps.shift();
                console.info(`case to stop`);
                audioDecodeProcessor.stop((err) => {
                    expect(err).assertUndefined();
                    console.info(`case stop 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case RESET:
                mySteps.shift();
                console.info(`case to reset`);
                resetParam();
                audioDecodeProcessor.reset((err) => {
                    expect(err).assertUndefined();
                    console.info(`case reset 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case HOLDON:
                mySteps.shift();
                setTimeout(() =>{
                    nextStep(mySteps, mediaDescription, done);
                }, WAITTIME);
                break;
            case WAITFORALLOUTS:
                mySteps.shift();
                console.info(`case wait for all outputs`);
                break;
            case CONFIGURE_ERROR:
                mySteps.shift();
                console.info(`case to configure 2`);
                audioDecodeProcessor.configure(mediaDescription, (err) => {
                    expect(err != undefined).assertTrue();
                    console.info(`case configure error 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case PREPARE_ERROR:
                mySteps.shift();
                console.info(`case to prepare`);
                audioDecodeProcessor.prepare((err) => {
                    expect(err != undefined).assertTrue();
                    console.info(`case prepare error 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case START_ERROR:
                mySteps.shift();
                console.info(`case to start`);
                audioDecodeProcessor.start((err) => {
                    expect(err != undefined).assertTrue();
                    console.info(`case start error 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case FLUSH_ERROR:
                mySteps.shift();
                console.info(`case to flush`);
                audioDecodeProcessor.flush((err) => {
                    expect(err != undefined).assertTrue();
                    console.info(`case flush error 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case STOP_ERROR:
                mySteps.shift();
                console.info(`case to stop`);
                audioDecodeProcessor.stop((err) => {
                    expect(err != undefined).assertTrue();
                    console.info(`case stop error 1`);
                    nextStep(mySteps, mediaDescription, done);
                });
                break;
            case JUDGE_EOS:
                mySteps.shift();
                console.info(`case judge EOS state`);
                expect(sawOutputEOS).assertTrue();
                nextStep(mySteps, mediaDescription, done);
                break;
            default:
                break;
        }
    }

    async function enqueueAllInputs(queue) {
        while (queue.length > 0 && !sawInputEOS) {
            let inputobject = queue.shift();
            console.info("frameCnt:" + frameCnt);
            if (frameCnt == EOSFrameNum || frameCnt == ES_LENGTH + 1) {
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
                if (workdoneAtEOS) {
                    await doneWork(done);
                } else {
                    console.info("sawOutputEOS = true");
                }
            }
            else{
                console.info("not last frame, continue");
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
            if (needGetMediaDes) {
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
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0100
        * @tc.name      : 001.create -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0100.pcm';
        let mySteps = new Array(CONFIGURE, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0200
        * @tc.name      : 002.prepare -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0200.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, CONFIGURE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0300
        * @tc.name      : 003.start -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0300.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, CONFIGURE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0400
        * @tc.name      : 004.flush -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0400.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, CONFIGURE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0500
        * @tc.name      : 005.stop -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0500.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP ,CONFIGURE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0600
        * @tc.name      : 006.EOS -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0600.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, CONFIGURE_ERROR, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0700
        * @tc.name      : 007.reset -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0700.pcm';
        let mySteps = new Array(RESET, CONFIGURE, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0800
        * @tc.name      : 008.configure -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0800.pcm';
        let mySteps = new Array(CONFIGURE, CONFIGURE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0900
        * @tc.name      : 009.configure -> reset -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_CONFIGURE_CALLBACK_0900', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0900.pcm';
        let mySteps = new Array(CONFIGURE, RESET, CONFIGURE, END);
        createAudioDecoder(savepath, mySteps, done);
    })

   /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0100
        * @tc.name      : 001.create -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0100.pcm';
        let mySteps = new Array(PREPARE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0200
        * @tc.name      : 002.configure -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0200.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0300
        * @tc.name      : 003.prepare -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0300.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, PREPARE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0400
        * @tc.name      : 004.start -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0400.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, PREPARE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0500
        * @tc.name      : 005.flush -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0500.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, PREPARE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0600
        * @tc.name      : 006.stop -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0600.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, PREPARE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0700
        * @tc.name      : 007.EOS -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0700.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, PREPARE_ERROR, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0800
        * @tc.name      : 008.reset -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_PREPARE_CALLBACK_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0800.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, PREPARE_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

  /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0100
        * @tc.name      : 001.create -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0100.pcm';
        let mySteps = new Array(START_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0200
        * @tc.name      : 002.configure -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0200.pcm';
        let mySteps = new Array(CONFIGURE, START_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0300
        * @tc.name      : 003.prepare -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0300.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0400
        * @tc.name      : 004.start -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0400.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, START_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0500
        * @tc.name      : 005.flush -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0500.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, START_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0600
        * @tc.name      : 006.stop -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0600.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, START, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0700
        * @tc.name      : 007.EOS -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0700.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, START_ERROR, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0800
        * @tc.name      : 008.reset -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_START_CALLBACK_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0800.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, START_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0100
        * @tc.name      : 001.create -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0100.pcm';
        let mySteps = new Array(FLUSH_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0200
        * @tc.name      : 002.configure -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0200.pcm';
        let mySteps = new Array(CONFIGURE, FLUSH_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0300
        * @tc.name      : 003.prepare -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0300.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, FLUSH_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0400
        * @tc.name      : 004.start -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0400.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0500
        * @tc.name      : 005.flush -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0500.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, FLUSH, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0600
        * @tc.name      : 006.stop -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0600.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP ,FLUSH_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0700
        * @tc.name      : 007.EOS -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0700.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, FLUSH, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0800
        * @tc.name      : 008.reset -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_FLUSH_CALLBACK_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0800.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, FLUSH_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0100
        * @tc.name      : 001.create -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0100.pcm';
        let mySteps = new Array(STOP_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0200
        * @tc.name      : 002.configure -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0200.pcm';
        let mySteps = new Array(CONFIGURE, STOP_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0300
        * @tc.name      : 003.prepare -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0300.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, STOP_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0400
        * @tc.name      : 004.start -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0400.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0500
        * @tc.name      : 005.flush -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0500.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, STOP, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0600
        * @tc.name      : 006.stop -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0600.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, STOP_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0700
        * @tc.name      : 007.EOS -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0700.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, STOP, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0800
        * @tc.name      : 008.reset -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_STOP_CALLBACK_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0800.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, STOP_ERROR, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0100
        * @tc.name      : 001.create -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0100.pcm';
        let mySteps = new Array(RESET, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0200
        * @tc.name      : 002.configure -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0200.pcm';
        let mySteps = new Array(CONFIGURE, RESET, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0300
        * @tc.name      : 003.prepare -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0300.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, RESET, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0400
        * @tc.name      : 004.start -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0400.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0500
        * @tc.name      : 005.flush -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0500.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, RESET, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0600
        * @tc.name      : 006.stop -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0600.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, RESET, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0700
        * @tc.name      : 007.EOS -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0700.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, RESET, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0800
        * @tc.name      : 008.reset -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_RESET_CALLBACK_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0800.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, RESET, END);
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0100
        * @tc.name      : 001.EOS -> flush -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0100.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, FLUSH, STOP, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0200
        * @tc.name      : 002.EOS -> flush -> EOS
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0200.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, FLUSH, WAITFORALLOUTS);
        EOSFrameNum = 2;
        flushAtEOS = true;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0300
        * @tc.name      : 003.EOS -> reset -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0300.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, RESET, CONFIGURE, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0400
        * @tc.name      : 004.EOS -> stop -> start -> EOS
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0400.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, STOP, START, WAITFORALLOUTS);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0500
        * @tc.name      : 005.EOS -> stop -> start -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_API_EOS_CALLBACK_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0500.pcm';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, STOP, START, STOP, END);
        EOSFrameNum = 2;
        createAudioDecoder(savepath, mySteps, done);
    })
})