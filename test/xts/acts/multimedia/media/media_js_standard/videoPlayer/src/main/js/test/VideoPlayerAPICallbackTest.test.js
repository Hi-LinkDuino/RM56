/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import media from '@ohos.multimedia.media'
import router from '@system.router'
import fileIO from '@ohos.fileio'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('VideoPlayerAPICallbackTest', function () {
    const AUDIO_SOURCE = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videoplayer/H264_AAC.mp4';
    const PLAY_TIME = 1000;
    const SEEK_TIME = 5000;
    const SEEK_CLOSEST = 3;
    const WIDTH_VALUE = 720;
    const HEIGHT_VALUE = 480;
    const DURATION_TIME = 10034;
    const CREATE_EVENT = 'create';
    const SETSURFACE_EVENT = 'setDisplaySurface';
    const GETDESCRIPTION = 'getTrackDescription';
    const PREPARE_EVENT = 'prepare';
    const PLAY_EVENT = 'play';
    const PAUSE_EVENT = 'pause';
    const STOP_EVENT = 'stop';
    const RESET_EVENT = 'reset';
    const RELEASE_EVENT = 'release';
    const SEEK_EVENT = 'seek';
    const SEEK_MODE_EVENT = 'seekMode';
    const SETVOLUME_EVENT = 'volume';
    const SETSPEED_EVENT = 'speed';
    const ERROR_EVENT = 'error';
    const END_EVENT = 'end';
    const VOLUME_VALUE = 1;
    const SPEED_VALUE = 1;
    const NEXT_FRAME_TIME = 8333;
    const PREV_FRAME_TIME = 4166;
    let surfaceID = '';
    let fdPath;
    let fdValue;
    let temp = 0;
    let events = require('events');
    let eventEmitter = new events.EventEmitter();

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(async function() {
        console.info('case flush surfaceID start');
        await toNewPage();
        console.info('case flush surfaceID end');
        await msleep(1000).then(() => {
        }, failureCallback).catch(catchCallback);
        console.info('beforeEach case');
    })

    afterEach(async function() {
        await router.clear();
        console.info('afterEach case');
    })

    afterAll(async function() {
        await fileIO.close(fdValue);
        console.info('afterAll case');
    })

    function msleep(ms) {
        return new Promise((resolve) => setTimeout(resolve, ms));
    }

    function failureCallback(error) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
    }

    function catchCallback(error) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
    }

    async function getFd() {
        fdPath = 'fd://';
        await fileIO.open(AUDIO_SOURCE).then((fdNumber) => {
            fdPath = fdPath + '' + fdNumber;
            fdValue = fdNumber;
            console.info('[fileIO]case open fd success,fdPath is ' + fdPath);
        }, (err) => {
            console.info('[fileIO]case open fd failed');
        }).catch((err) => {
            console.info('[fileIO]case catch open fd failed');
        });
    }

    async function toNewPage() {
        let path = '';
        if (temp == 0) {
            path = 'pages/surfaceTest/surfaceTest';
            temp = 1;
        } else {
            path = 'pages/surfaceTest/surfaceTest2';
            temp = 0;
        }
        let options = {
            uri: path,
        }
        try {
            let result = await router.push(options);
        } catch {
            console.info('case route failed');
        }
    }

    function sleep(time) {
        for(let t = Date.now(); Date.now() - t <= time;);
    }

    function printfError(error, done) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
        done();
    }

    function printfDescription(obj) { 
        let description = ""; 
        for(let i in obj) { 
            let property = obj[i];
            console.info('case key is  '+ i);
            console.info('case value is  '+ property);
            description += i + " = " + property + "\n"; 
        } 
    }

    function toNextStep(videoPlayer, steps, done) {
        if (steps[0] == END_EVENT) {
            console.info('case success!!');
            done();
        } else {
            eventEmitter.emit(steps[0], videoPlayer, steps, done);
        }
    }

    eventEmitter.on(CREATE_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        media.createVideoPlayer((err, video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
                console.info('case createVideoPlayer success!!');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(SETSURFACE_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.url = fdPath;
        videoPlayer.setDisplaySurface(surfaceID, (err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('idle');
                console.info('case setDisplaySurface success!!');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        })
    });

    eventEmitter.on(PREPARE_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.url = fdPath;
        videoPlayer.prepare((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('prepared');
                expect(videoPlayer.duration).assertEqual(DURATION_TIME);
                expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
                expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
                console.info('case prepare success!!');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(GETDESCRIPTION, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.getTrackDescription((err, arrlist) => {
            if (typeof (err) == 'undefined') {
                for (let i = 0; i < arrlist.length; i++) {
                    printfDescription(arrlist[i]);
                }
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(PLAY_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.play((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('playing');
                console.info('case play success!!');
                sleep(PLAY_TIME);
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(PAUSE_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.pause((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('paused');
                console.info('case pause success!!');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(STOP_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.stop((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('stopped');
                console.info('case stop success!!');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(RESET_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.reset((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('idle');
                console.info('case reset success!!');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(RELEASE_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.release((err) => {
            if (typeof (err) == 'undefined') {
                console.info('case release success!!');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(SEEK_EVENT, (videoPlayer, steps, done) => {
        let seekTime = steps[1];
        steps.shift();
        steps.shift();
        videoPlayer.seek(seekTime, (err, seekDoneTime) => {
            if (typeof (err) == 'undefined') {
                console.info('case seek success and seekDoneTime is '+ seekDoneTime);
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    function checkSeekTime(seekMode, seekTime, seekDoneTime) {
        switch (seekMode) {
            case media.SeekMode.SEEK_NEXT_SYNC:
                if (seekTime == 0) {
                    expect(seekDoneTime).assertEqual(0);
                } else if (seekTime == DURATION_TIME) {
                    expect(seekDoneTime).assertEqual(DURATION_TIME);
                } else {
                    expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
                }
                break;
            case media.SeekMode.SEEK_PREV_SYNC:
                if (seekTime == 0) {
                    expect(seekDoneTime).assertEqual(0);
                } else if (seekTime == DURATION_TIME) {
                    expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
                } else {
                    expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
                }
                break;
            default:
                break;
        }
    }

    eventEmitter.on(SEEK_MODE_EVENT, (videoPlayer, steps, done) => {
        let seekTime = steps[1];
        steps.shift();
        steps.shift();
        videoPlayer.seek(seekTime, media.SeekMode.SEEK_NEXT_SYNC, (err, seekDoneTime) => {
            if (typeof (err) == 'undefined') {
                checkSeekTime(media.SeekMode.SEEK_NEXT_SYNC, seekTime, seekDoneTime);
                console.info('case seek success and seekDoneTime is '+ seekDoneTime);
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(SETVOLUME_EVENT, (videoPlayer, steps, done) => {
        let volumeValue = steps[1];
        steps.shift();
        steps.shift();
        videoPlayer.setVolume(volumeValue, (err) => {
            if (typeof (err) == 'undefined') {
                console.info('case setVolume success');
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(SETSPEED_EVENT, (videoPlayer, steps, done) => {
        let speedValue = steps[1];
        steps.shift();
        steps.shift();
        videoPlayer.setSpeed(speedValue, (err, speedMode) => {
            if (typeof (err) == 'undefined') {
                expect(speedValue).assertEqual(speedMode);
                console.info('case setSpeed success and speedMode is '+ speedMode);
                toNextStep(videoPlayer, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0100
        * @tc.name      : 01.create->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0100', 0, async function (done) {
        await getFd();
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0200
        * @tc.name      : 02.play->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0300
        * @tc.name      : 03.pause->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0400
        * @tc.name      : 04.stop->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            STOP_EVENT, PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0500
        * @tc.name      : 05.reset->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            RESET_EVENT, PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0600
        * @tc.name      : 06.seek->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_EVENT, SEEK_TIME, PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0700
        * @tc.name      : 07.seek(mode)->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_MODE_EVENT, SEEK_TIME, PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0800
        * @tc.name      : 08.setvolume->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SETVOLUME_EVENT, VOLUME_VALUE, PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0900
        * @tc.name      : 09.setspeed->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SETSPEED_EVENT, SPEED_VALUE, PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1000
        * @tc.name      : 10.setDisplaySurface->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1100
        * @tc.name      : 11.getTrackDescription->prepare
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, GETDESCRIPTION,
            PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1200
        * @tc.name      : 12.prepare 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PREPARE_EVENT,
            PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0100
        * @tc.name      : 01.create->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, PLAY_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0200
        * @tc.name      : 02.prepare->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0300
        * @tc.name      : 03.pause->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0400
        * @tc.name      : 04.stop->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, PLAY_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0500
        * @tc.name      : 05.reset->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RESET_EVENT, PLAY_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0600
        * @tc.name      : 06.seek->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0700
        * @tc.name      : 07.seek(mode)->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_MODE_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0800
        * @tc.name      : 08.setvolume->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0900
        * @tc.name      : 09.setspeed->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1000
        * @tc.name      : 10.setDisplaySurface->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PLAY_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1100
        * @tc.name      : 11.getTrackDescription->play
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, GETDESCRIPTION, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1200
        * @tc.name      : 12.play 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PLAY_EVENT, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0100
        * @tc.name      : 01.create->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0200
        * @tc.name      : 02.prepare->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0300
        * @tc.name      : 03.play->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0400
        * @tc.name      : 004.stop->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0500
        * @tc.name      : 05.reset->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RESET_EVENT, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0600
        * @tc.name      : 06.seek->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0700
        * @tc.name      : 07.seek(mode)->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_MODE_EVENT, SEEK_TIME, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0800
        * @tc.name      : 08.setvolume->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0900
        * @tc.name      : 09.setspeed->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1000
        * @tc.name      : 10.setDisplaySurface->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1100
        * @tc.name      : 11.getTrackDescription->pause
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, GETDESCRIPTION, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1200
        * @tc.name      : 12.pause 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, PAUSE_EVENT, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0100
        * @tc.name      : 001.create->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0200
        * @tc.name      : 02.prepare->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0300
        * @tc.name      : 03.play->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0400
        * @tc.name      : 04.pause->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0500
        * @tc.name      : 05.reset->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RESET_EVENT, STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0600
        * @tc.name      : 06.seek->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0700
        * @tc.name      : 07.seek(mode)->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_MODE_EVENT, SEEK_TIME, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0800
        * @tc.name      : 08.setvolume->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0900
        * @tc.name      : 09.setspeed->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1000
        * @tc.name      : 10.setDisplaySurface->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1100
        * @tc.name      : 11.getTrackDescription->stop
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, GETDESCRIPTION, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1200
        * @tc.name      : 12.stop 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, STOP_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0100
        * @tc.name      : 01.create->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0200
        * @tc.name      : 02.prepare->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0300
        * @tc.name      : 03.play->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0400
        * @tc.name      : 04.pause->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0500
        * @tc.name      : 05.stop->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0600
        * @tc.name      : 06.seek->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0700
        * @tc.name      : 07.seek(mode)->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_MODE_EVENT, SEEK_TIME, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0800
        * @tc.name      : 08.setvolume->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0900
        * @tc.name      : 09.setspeed->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1000
        * @tc.name      : 10.setDisplaySurface->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1100
        * @tc.name      : 11.getTrackDescription->reset
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, GETDESCRIPTION, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1200
        * @tc.name      : 12.reset 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, RESET_EVENT, RESET_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0100
        * @tc.name      : 01.create->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0200
        * @tc.name      : 02.prepare->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0300
        * @tc.name      : 03.play->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0400
        * @tc.name      : 04.pause->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0500
        * @tc.name      : 05.stop->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0600
        * @tc.name      : 06.seek->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0700
        * @tc.name      : 07.seek(mode)->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_MODE_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0800
        * @tc.name      : 08.setvolume->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0900
        * @tc.name      : 09.setspeed->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1000
        * @tc.name      : 10.setDisplaySurface->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1100
        * @tc.name      : 11.getTrackDescription->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, GETDESCRIPTION, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1200
        * @tc.name      : 12.reset->release
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0100
        * @tc.name      : 01.create->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SEEK_EVENT, SEEK_TIME, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0200
        * @tc.name      : 02.prepare->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0300
        * @tc.name      : 03.play->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0400
        * @tc.name      : 04.pause->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0500
        * @tc.name      : 05.stop->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, SEEK_EVENT, SEEK_TIME, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0600
        * @tc.name      : 06.reset->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RESET_EVENT, SEEK_EVENT, SEEK_TIME, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0700
        * @tc.name      : 07.setvolume->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, SEEK_EVENT,
            SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0800
        * @tc.name      : 08.setspeed->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE,
            SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0900
        * @tc.name      : 09.setDisplaySurface->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            SEEK_EVENT, SEEK_TIME, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1000
        * @tc.name      : 10.getTrackDescription->seek
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, GETDESCRIPTION, SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1100
        * @tc.name      : 11.seek 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, SEEK_EVENT, SEEK_TIME,
            SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1300
        * @tc.name      : 13.seek(-1)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, -1, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1400
        * @tc.name      : 14.seek(out of duration)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, DURATION_TIME + 1000, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0100
        * @tc.name      : 01.create->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0200
        * @tc.name      : 02.prepare->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0300
        * @tc.name      : 03.play->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0400
        * @tc.name      : 04.pause->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0500
        * @tc.name      : 05.stop->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0600
        * @tc.name      : 06.reset->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RESET_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0700
        * @tc.name      : 07.seek->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME, SETVOLUME_EVENT,
            VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0800
        * @tc.name      : 08.setspeed->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, SETVOLUME_EVENT,
            VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0900
        * @tc.name      : 09.setDisplaySurface->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            SETVOLUME_EVENT, VOLUME_VALUE, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1000
        * @tc.name      : 10.getTrackDescription->setvolume
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, GETDESCRIPTION, SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1100
        * @tc.name      : 11.setvolume 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, SETVOLUME_EVENT, VOLUME_VALUE,
            SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1300
        * @tc.name      : 13.setvolume(<0)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, -1, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1400
        * @tc.name      : 14.setvolume(> 1)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, 2, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0100
        * @tc.name      : 01.create->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSPEED_EVENT, SPEED_VALUE,ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0200
        * @tc.name      : 02.prepare->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0300
        * @tc.name      : 03.play->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0400
        * @tc.name      : 04.pause->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, PAUSE_EVENT, SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0500
        * @tc.name      : 05.stop->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, STOP_EVENT, SETSPEED_EVENT, SPEED_VALUE,
            ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0600
        * @tc.name      : 06.reset->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RESET_EVENT, SETSPEED_EVENT, SPEED_VALUE,
            ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0700
        * @tc.name      : 07.seek->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SEEK_EVENT, SEEK_TIME,
            SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0800
        * @tc.name      : 08.setvolume->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, SETSPEED_EVENT,
            SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0900
        * @tc.name      : 09.setDisplaySurface->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            SETSPEED_EVENT, SPEED_VALUE, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1000
        * @tc.name      : 10.getTrackDescription->setspeed
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, GETDESCRIPTION, SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1100
        * @tc.name      : 11.setspeed 3 times
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, SPEED_VALUE, SETSPEED_EVENT, SPEED_VALUE,
            SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1300
        * @tc.name      : 13.setspeed(< 0)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, -1, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1400
        * @tc.name      : 14.setspeed(> 4)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_SETSPEED_CALLBACK_1400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, SETSPEED_EVENT, 5, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);
    })
})
