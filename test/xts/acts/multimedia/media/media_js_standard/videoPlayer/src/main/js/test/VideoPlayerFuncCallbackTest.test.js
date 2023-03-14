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

describe('VideoPlayerFuncCallbackTest', function () {
    const AUDIO_SOURCE = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videoplayer/H264_AAC.mp4';
    const PLAY_TIME = 3000;
    const SEEK_TIME = 5000;
    const WIDTH_VALUE = 720;
    const HEIGHT_VALUE = 480;
    const DURATION_TIME = 10034;
    const CREATE_EVENT = 'create';
    const SETSURFACE_EVENT = 'setDisplaySurface';
    const GETDESCRIPTION = 'getTrackDescription';
    const PREPARE_EVENT = 'prepare';
    const PREPARE2_EVENT = 'prepare2';
    const PLAY_EVENT = 'play';
    const PAUSE_EVENT = 'pause';
    const STOP_EVENT = 'stop';
    const RESET_EVENT = 'reset';
    const RELEASE_EVENT = 'release';
    const SEEK_EVENT = 'seek';
    const SEEK_MODE_EVENT = 'seekMode';
    const SETVOLUME_EVENT = 'volume';
    const SETSPEED_EVENT = 'speed';
    const END_EVENT = 'end';
    const VOLUME_VALUE = 1;
    const SPEED_VALUE = 1;
    const DELTA_TIME = 1000;
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

    function failureCallback(error) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
    }

    function catchCallback(error) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
    }

    function sleep(time) {
        for(let t = Date.now(); Date.now() - t <= time;);
    }

    function msleep(ms) {
        return new Promise((resolve) => setTimeout(resolve, ms));
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

    function setOnCallback(videoPlayer) {
        videoPlayer.on('playbackCompleted', () => {
            console.info('case playbackCompleted success');
        });

        videoPlayer.on('bufferingUpdate', (infoType, value) => {
            console.info('case bufferingUpdate success infoType is ' + infoType);
            console.info('case bufferingUpdate success value is ' + value);
        });

        videoPlayer.on('startRenderFrame', () => {
            console.info('case startRenderFrame success');
        });

        videoPlayer.on('videoSizeChanged', (width, height) => {
            console.info('case videoSizeChanged success');
        });

        videoPlayer.on('error', (error) => {
            console.info(`case error called,errMessage is ${error.message}`);
        });
    }

    eventEmitter.on(CREATE_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        media.createVideoPlayer((err, video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                setOnCallback(videoPlayer);
                expect(videoPlayer.state).assertEqual('idle');
                console.info('case createVideoPlayer success!!');
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
            } else {
                printfError(err, done);
            }
        })
    });

    eventEmitter.on(PREPARE_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        videoPlayer.prepare((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('prepared');
                expect(videoPlayer.duration).assertEqual(DURATION_TIME);
                expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
                expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
                console.info('case prepare success!!');
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(PREPARE2_EVENT, (videoPlayer, steps, done) => {
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
            } else {
                printfError(err, done); 
            }
        });
    });

    eventEmitter.on(PLAY_EVENT, (videoPlayer, steps, done) => {
        steps.shift();
        let startTime = videoPlayer.currentTime;
        videoPlayer.play((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoPlayer.state).assertEqual('playing');
                console.info('case play success!!');
                sleep(PLAY_TIME);
                let endTime = videoPlayer.currentTime;
                expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);
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
                    expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
                } else {
                    expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
                }
                break;
            default:
                break;
        }
    }

    eventEmitter.on(SEEK_EVENT, (videoPlayer, steps, done) => {
        let seekTime = steps[1];
        steps.shift();
        steps.shift();
        videoPlayer.seek(seekTime, (err, seekDoneTime) => {
            if (typeof (err) == 'undefined') {
                if (seekTime > DURATION_TIME) {
                    seekTime = DURATION_TIME;
                }
                checkSeekTime(media.SeekMode.SEEK_PREV_SYNC, seekTime, seekDoneTime);
                console.info('case seek success and seekDoneTime is '+ seekDoneTime);
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(SEEK_MODE_EVENT, (videoPlayer, steps, done) => {
        let seekTime = steps[1];
        let seekMode = steps[2];
        steps.shift();
        steps.shift();
        steps.shift();
        videoPlayer.seek(seekTime, seekMode, (err, seekDoneTime) => {
            if (typeof (err) == 'undefined') {
                if (seekTime > DURATION_TIME) {
                    seekTime = DURATION_TIME;
                }
                checkSeekTime(seekMode, seekTime, seekDoneTime);
                console.info('case seek success and seekDoneTime is '+ seekDoneTime);
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
            } else {
                printfError(err, done);
            }
        });
    });

    function checkSpeedTime(videoPlayer, speedValue, startTime) {
        let endTime = videoPlayer.currentTime;
        if (videoPlayer.state == 'playing') {
            switch (speedValue) {
                case media.PlaybackSpeed.SPEED_FORWARD_0_75_X:
                    expect(endTime - startTime).assertClose(0.75 * 1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_1_00_X:
                    expect(endTime - startTime).assertClose(1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_1_25_X:
                    expect(endTime - startTime).assertClose(1.25 * 1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_1_75_X:
                    expect(endTime - startTime).assertClose(1.75 * 1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_2_00_X:
                    expect(endTime - startTime).assertClose(2 * 1000, DELTA_TIME);
                    break;
            }
        } else {
            console.info('case speed not in play');
        }
    }

    eventEmitter.on(SETSPEED_EVENT, (videoPlayer, steps, done) => {
        let speedValue = steps[1];
        steps.shift();
        steps.shift();
        let startTime = videoPlayer.currentTime;
        videoPlayer.setSpeed(speedValue, (err, speedMode) => {
            if (typeof (err) == 'undefined') {
                sleep(1000);
                expect(speedValue).assertEqual(speedMode);
                console.info('case setSpeed success and speedMode is '+ speedMode);
                checkSpeedTime(videoPlayer, speedValue, startTime);
                toNextStep(videoPlayer, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0100
        * @tc.name      : 001.play (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0100', 0, async function (done) {
        await getFd();
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT,
            PREPARE_EVENT, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0200
        * @tc.name      : 002.pause (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT,
            PLAY_EVENT, PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0300
        * @tc.name      : 003.pause->play (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0400
        * @tc.name      : 004.stop (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT,
            PLAY_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0500
        * @tc.name      : 005.reset (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT,
            PLAY_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0600
        * @tc.name      : 006.seek (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_EVENT, SEEK_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0700
        * @tc.name      : 007.seek mode(callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_MODE_EVENT, SEEK_TIME, media.SeekMode.SEEK_NEXT_SYNC,
            SEEK_MODE_EVENT, SEEK_TIME, media.SeekMode.SEEK_PREV_SYNC,RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0800
        * @tc.name      : 008.setVolume (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SETVOLUME_EVENT, VOLUME_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })
    
    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0900
        * @tc.name      : 009.setSpeed (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SETSPEED_EVENT, SPEED_VALUE, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1000
        * @tc.name      : 010.getTrackDescription (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT,
            GETDESCRIPTION, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1200
        * @tc.name      : 012.play->pause->stop (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1300
        * @tc.name      : 013.play->pause->reset (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1400
        * @tc.name      : 014.play->pause->seek->play (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, SEEK_EVENT, SEEK_TIME, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1500
        * @tc.name      : 015.play->pause->seek(mode)->play (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, SEEK_MODE_EVENT, SEEK_TIME, media.SeekMode.SEEK_NEXT_SYNC,
            SEEK_MODE_EVENT, SEEK_TIME, media.SeekMode.SEEK_PREV_SYNC, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1600
        * @tc.name      : 016.play->pause->setvolume->play (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, SETVOLUME_EVENT, VOLUME_VALUE, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1700
        * @tc.name      : 017.play->pause->setspeed->play (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            PAUSE_EVENT, SETSPEED_EVENT, SPEED_VALUE, PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1800
        * @tc.name      : 018.play->stop->reset (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            STOP_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1900
        * @tc.name      : 019.play->stop->reset->prepare->play (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_1900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            STOP_EVENT, RESET_EVENT, PREPARE2_EVENT,
            PLAY_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2000
        * @tc.name      : 020.play->seek(0)->pause->stop (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_EVENT, 0, PAUSE_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2100
        * @tc.name      : 021.play->seek(0, mode)->pause->stop (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_MODE_EVENT, 0, media.SeekMode.SEEK_NEXT_SYNC, 
            SEEK_MODE_EVENT, 0, media.SeekMode.SEEK_PREV_SYNC, PAUSE_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2200
        * @tc.name      : 022.play->seek(duration)->finish (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_EVENT, DURATION_TIME, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2300
        * @tc.name      : 023.play->seek(duration, mode)->finish (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_MODE_EVENT, DURATION_TIME, media.SeekMode.SEEK_PREV_SYNC,
            SEEK_MODE_EVENT, DURATION_TIME, media.SeekMode.SEEK_NEXT_SYNC, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2400
        * @tc.name      : 024.play->seek(out of duration) (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_EVENT, DURATION_TIME + 1, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2500
        * @tc.name      : 025.play->seek(out of duration, mode) (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SEEK_MODE_EVENT, DURATION_TIME + 1, media.SeekMode.SEEK_PREV_SYNC,
            SEEK_MODE_EVENT, DURATION_TIME + 1, media.SeekMode.SEEK_NEXT_SYNC, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2600
        * @tc.name      : 026. play->setvolume(0~1) (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SETVOLUME_EVENT, 0, SETVOLUME_EVENT, 0.2, SETVOLUME_EVENT, 0.4, SETVOLUME_EVENT, 0.6,
            SETVOLUME_EVENT, 0.8, SETVOLUME_EVENT, 1, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2700
        * @tc.name      : 027.play->setspeed(0~4) (callback)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_2700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let mySteps = new Array(CREATE_EVENT, SETSURFACE_EVENT, PREPARE_EVENT, PLAY_EVENT,
            SETSPEED_EVENT, media.PlaybackSpeed.SPEED_FORWARD_2_00_X, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoPlayer, mySteps, done);            
    })
})
