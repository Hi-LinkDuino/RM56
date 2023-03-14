/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

// @ts-ignore
import router from '@system.router'
import audio from '@system.audio'
import file from '@system.file'
import app from '@system.app'
import devicemanager from '@system.devicemanager'

var modeImages = ['common/ic_music_listorder.png', 'common/ic_music_listcycle.png', 'common/ic_music_shuffle.png', 'common/ic_music_singlecycle.png'];

export default {
    data: {
        showMusicList: false,
        playMode: 1,  //0:sequential,1:list loop,2:random,3:single
        modeImage: 'common/ic_music_listcycle.png', //列表循环播放list loop
        playImage: 'common/ic_music_play.png',
        disImage:'common/dis.png',
        playProgress: 0,  //progress
        curMusicIndex: 0,  //music index
        playStatus: 2,  //0:play,1:pause,2:stop
        curMusic: {  //current music info
            curMusicName: "",
            singer: "",
            playTime: "00:00",
            allTime: "00:00"
        },
        musicList: [ ],

        dmpage:false,  //DM variable definition
        status: null,
        statusType:'',
        subscribeId : 0,
        IntervalID:0,
        StartDiscoveryTimer:0,
        timeout:0,
        timeRemaining:0,
        pinNumb: 0,
        pin: ['','','','','',''],
        statusInfo: {
            deviceId: '',
            deviceName: '',
            deviceTypeId: 0
        },
        GetPonken:{
            deviceId:'',
            pinTone:''
        },
        StatInfo: {
            deviceName: "",
            appName: '',
            appIcon: null,
            pinCode: '',
            pinToken: '0'
        }
    },
    onInit() {
        //get music duration
        this.DMinit()
/*        this.StartDiscoveryTimer = setInterval(this.OndeviceFound, 2000);  //DM data initialization
        this.startDevice()*/

   /*     var array = devicemanager.getTrustedDeviceListSync();
        console.info(JSON.stringify(array))
        if(array == null)
        {
            this.IntervalID = setInterval(this.initStatue, 3000);
        }*/

  
        audio.onplay = () => {
            console.log("onplay start");
            this.curMusic.allTime = this.timeChange(audio.duration);
            this.playStatus = 0;
        };
        audio.onCallback((data) => {
            console.log(JSON.stringify(data));
            this.playStatus = 2;
            this.playMusic();
            this.playStatus = 0;
            this.playImage = "common/ic_music_pause.png";
            this.changeProgress(data);
//            this.playImage = "common/ic_music_pause.png";
//            this.playStatus = 0;
//            this.playProgress = data.progress;
//            this.curMusic.curMusicName = "BBK";
//            this.curMusic.singer = "unknown singer";
//            audio.src = "/data/bbg.mp3";
//            this.changeProgress(data);
//            audio.loop = true;
//            audio.play();
        });

        //update play progress
        audio.ontimeupdate = () => {
            if (audio.duration == null || audio.duration <= 0) {
                return;
            }
            console.log("ontime update:");
            this.curMusic.playTime = this.timeChange(audio.currentTime);
            console.log("ontime update:" + this.curMusic.playTime);
            this.playProgress = Math.floor(audio.currentTime * 100 / audio.duration);
            console.log("ontime update progress:" + this.playProgress + ",duration:" + audio.duration);
        };
        //play next song when meet an error
        audio.onerror = () => {
            console.log("onerror");
            this.changeSong();
        };
        //perform operations based on the playback mode.
        audio.onended = () => {
            console.log("onend");
            this.playStatus = 2;
            //after played the last song in sequential mode ,stop playback
            if (this.playMode == 0 && this.curMusicIndex == (this.musicList.length - 1)) {
                this.playStatus = 2;
                this.playImage = "common/ic_music_play.png";
                console.log("played the last song in sequential mode");
                return;
            }
            let mode = "";
            if (this.playMode == 2) {
                console.log("random");
                mode = "random";
            }
            this.changeSong(mode);
        };
        this.getMusicList();
    },
    onDestroy() {
        this.stopPlay();
    },
    clickListItem(index) {
        console.log("listItemClick:start:" + index);
        if (index == undefined || index == null) {
            index = 0;
        }
        this.curMusicIndex = index;
        this.playStatus = 2;
        this.playMusic();
        this.showMusicList = false;
        console.log("listItemClick:end");
    },
    changeProgress(value) {
        console.log("changeProgress start:" + value.progress);
        if (value == null || value.progress == null) {
            return;
        }
        if (this.playStatus == 2) {
            return;
        }
        let val = Math.floor((value.progress / 100) * audio.duration);
        console.log("changeProgress change:" + val);
        audio.currentTime = val;
        console.log("changeProgress end:");
    },
     playMusic() {
        console.log("playMusic:start");
        //play music when is paused
        this.playImage = "common/ic_music_pause.png";
        if (this.playStatus == 1) {
            audio.play();
            this.playStatus = 0;
            console.log("playMusic:play");

            return;
        } else if (this.playStatus == 0) {
            this.playImage = "common/ic_music_play.png";
            audio.pause();
            this.playStatus = 1;
            console.log("playMusic:pause");
            return;
        }

        if (this.curMusicIndex == null) {
            this.curMusicIndex = 0;
        }
        let music = this.musicList[this.curMusicIndex];
        if (music != null) {
            this.playProgress = 0;
            this.curMusic.curMusicName = music.name;
            this.curMusic.singer = music.singer;
            audio.src = music.src;
            console.log("playMusic:src:" + music.src);
            audio.play();
            if (this.playMode == 3) {
                audio.loop = true;
            } else {
                audio.loop = false;
            }
            console.log("playMusic:end");
        }
    },
    stopPlay() {
        console.log("stopPlay");
        audio.src = null;
        audio.onstop = null;
        audio.onplay = null;
        audio.ontimeupdate = null;
        audio.onerror = null;
    },
    changeMode() {
        console.log("changeMode start");
        if (this.playMode == 3) {
            this.playMode = 0;
        } else {
            this.playMode++;
        }
        if (this.playMode == 3) {
            audio.loop = true;
        } else {
            audio.loop = false;
        }
        console.log("changeMode:" + this.playMode);
        this.modeImage = modeImages[this.playMode];
    },
    changeSong(mode) {
        console.log("ChangeSong:start");
        if (this.playStatus == 0 || this.playStatus == 1) {
            this.playStatus = 2;
        }
        if (this.playMode == 2) {
            mode = "random";
        }
        //previous

        if (mode == "previous") {
                this.curMusicIndex == 0 ? (this.curMusicIndex = (this.musicList.length - 1)) : this.curMusicIndex--;
                console.log("ChangeSong:previous:" + this.curMusicIndex);
        } else if (mode == "random") {  //random
            this.curMusicIndex = parseInt(Math.random() * (this.musicList.length), 10);
            console.log("ChangeSong:random:" + this.curMusicIndex);
        } else {  //next
                (this.curMusicIndex == this.musicList.length - 1) ? this.curMusicIndex = 0 : this.curMusicIndex++;
                console.log("ChangeSong:next" + this.curMusicIndex);
        }
        this.curMusic.curMusicName = this.musicList[this.curMusicIndex].name;
        this.curMusic.singer = this.musicList[this.curMusicIndex].singer;
        this.playMusic();
        if(this.showMusicList){
            this.$refs.musicList.scrollTo({ index: (this.curMusicIndex == 0?0:this.curMusicIndex-1) });
        }
        console.log("ChangeSong:end");
    },
    timeChange(seconds) {
        console.log("timeChange:start:" + seconds);
        if (seconds == null || seconds < 0) {
            return "00:00";
        }
        let minute = 0;
        let second = 0; 
        minute = Math.floor(seconds / 60);
        second = Math.floor(seconds % 60);
        let result = (minute > 9 ? minute : "0" + minute) + ":" + (second > 9 ? second : "0" + second);
        console.log("timeChange:end:" + result);
        return result;
    },
    playRemote(progress) {
        if (this.playStatus == 0) { // play
            this.playImage = "common/ic_music_play.png";
            audio.pause();
            this.playStatus = 1;
            console.log("playMusic:pause");
        } else if (this.playStatus == 1) { // pause
        } else { // init
            this.playProgress = 0
        }
        console.log("play remote progress: " + this.playProgress)

        audio.playRemote(this.playProgress)
    },
    changePage(operation) {
        console.log("changePage:start");
        if (operation != null && operation == "close") {
            console.log("operation != null && operation == close");
            this.showMusicList = false;
            return;
        }''
        this.showMusicList = !this.showMusicList;
        console.log("changePage: showMusicList " + this.showMusicList);
        if(this.showMusicList){
            console.log("changePage: showMusicList index " + this.curMusicIndex);
            this.$refs.musicList.scrollTo({ index: (this.curMusicIndex == 0?0:this.curMusicIndex-1) });
        }
    },
    exitApp(){
        console.log("start exit");
        app.terminate();
        console.log("end exit");
    },
    swipeEvent(e){
        if(e.direction == "right"){
            console.log("start exit");
            this.exitApp();
        }
    },
    getMusicList(){
        let that = this;
        //get all music file
        file.readText({
            uri:"internal://app/musicList.txt",
            success(data){
                console.log(data.text);
                let music = JSON.parse(data.text);
                console.log("---------" + music);
                if(music != null){
                    that.musicList = music;
                }
            },
            fail(data,code){
                console.log("get music list failed :"+code);
            }
        });
    },
    openDailog(){
        var arrayFa = devicemanager.getTrustedDeviceListSync();
        console.info('host:getTrustedDeviceListSync'+JSON.stringify(arrayFa))
/*        if(arrayFa[0].deviceId != this.statusInfo.deviceId)*/
        if(arrayFa == null)
        {
           this.dmpage  = true
/*           this.statusType = 'DeviceType'*/
           this.status = 'start'
        }
        else{
            this.dmpage = true
            this.status = 'device-online'
            this.arrayFa = null
        }
    },

    DMinit(){
        devicemanager.createDeviceManager('com.ohos.devicemanagerui', {
            success(data0){
                console.log("in createDeviceManager success:")
            },
            fail(err0) {
                console.log("in createDeviceManager fail:")
            }
        });
        devicemanager.on('deviceFound', (data0,data1) => {
            if (data1 == null) {
                console.info("deviceFound error data=null")
                return;
            }
            console.info("deviceFound:" + JSON.stringify(data0));
            console.info("deviceFound:" + JSON.stringify(data1));
            this.statusInfo = {
                deviceId: data1.deviceId,
                deviceName: data1.deviceName,
                deviceTypeId: data1.deviceTypeId
            }
        });
        this.OndeviceStateChange()
       /* this.ondiscoverFail()*/
        this.onserviceDie()
        this.OndmFaCallback()
    },
    startDevice(){
        //开始设备发现
        console.info("Start Device discovery");
        this.subscribeId = Math.floor(Math.random() * 10000 + 1000)
        console.info('Start subscribeId'+ JSON.stringify(this.subscribeId))
        var info = {
            "subscribeId": this.subscribeId,
            "mode": 0xAA,
            "medium": 0,
            "freq": 2,
            "isSameAccount": false,
            "isWakeRemote": false,
            "capability": 0
        };
        devicemanager.startDeviceDiscovery(info);
    },
    stopDevice(){
        console.log(this.subscribeId);
        devicemanager.stopDeviceDiscovery(this.subscribeId);

    },
    AuthenticateDevice(){
        let extraInfo = {
            targetPkgName: 'test',
            appName: "Newname",
            appDescription: "testAPP",
            business: '0',
            displayOwner: 0
        };
        let AuthParam = {
            authType: 1,
            appIcon:null,
            appThumbnail:null,
            extraInfo: extraInfo
        };
        console.log("ready authenticateDevice")
   /*     let _this = this;*/
        devicemanager.authenticateDevice(this.statusInfo, AuthParam, {
            success(data0,data1){
                console.log("in authenticateDevice success:")
                console.log(JSON.stringify(data0))
                console.log(JSON.stringify(data1))
/*                _this.GetPonken ={
                    deviceId:data0.deviceId,
                    pinTone:data1.pinTone
                }
                console.log('this.GetPonken.deviceId'+ _this.GetPonken.deviceId);
                console.log('this.GetPonken.pinTone'+ _this.GetPonken.pinTone);
                _this.mainPin()*/
            },
            fail(err0,err1) {
                console.log("authenticateDevice fail*")
                console.log(JSON.stringify(err0))
                console.log(JSON.stringify(err1));
            }
        });
        clearInterval(this.IntervalID)
    },
    joinAuthOk() {
        if(this.StatInfo.pinCode !=0){
            clearInterval(this.IntervalID)
            this.joinPin()
        }
    },
    bottonjoinAuthOk(){
        this.setUserOperation(0)
        this.IntervalID = setInterval(this.initStatue, 1000);
        this.status = 'join-auth'
    },
    /**
     * Cancel authorization
     */
    joinAuthCancel() {
        /*this.timing(1)*/
        this.setUserOperation(1)
        this.status = null
        this.dmpage = false
        this.exitDM()
    },
    /**
     * Enter a number with the keyboard
     * @param s
     * @return
     */
    mainInputPin(s) {
        if (this.pinNumb == 6) return
        if (this.pinNumb < 6) {
            this.pin[this.pinNumb] = s
            ++this.pinNumb
        }
        if (this.pinNumb == 6) {
            console.log("verifyAuthInfo ok")
            this.verifyAuthInfo(this.pin.join(''))
        }
    },
    joinAuthImageCancel() {
        this.setUserOperation(1)
        this.dmpage = false
    },
    /**
     * verify auth info, such as pin code.
     * @param pinCode
     * @return
     *///验证认证信息pin码

    verifyAuthInfo(pinCode) {
/*        console.info("in verifyAuthInfo:"+pinCode)
        console.info("verifyAuthInfo :" + JSON.stringify(this.GetPonken))*/
        devicemanager.verifyAuthInfo({
            "authType": 1,
            "token": '0',
            "extraInfo": {
                "pinCode": +pinCode
            }
        },{
            success(data0,data1){
                console.log("success verifyAuthInfo")
            },
            fail(err0) {
                console.log("fail verifyAuthInfo")
                console.log(JSON.stringify(err0))
            }
        });
/*        this.OndeviceStateChange()*/
    },
    /**
     * Keyboard delete number
     */
    mainInputPinBack() {
        if (this.pinNumb > 0) {
            --this.pinNumb
            this.pin[this.pinNumb] = ''
        }
    },
    cleanInputPin(){
        if (this.pinNumb > 0) {
            for(let i = 0; i < this.pinNumb; i++){
                this.pin[i] = ''
            }
            this.pinNumb = 0
        }
    },
    mainInputPinCancel() {
        this.setUserOperation(4)
        this.dmpage = false
        this.exitDM()
    },
    /**
     * Get authentication param
     */
    initStatue() {
        console.info('initStatue')
        let data = devicemanager.getAuthenticationParam()
        console.info('getAuthenticationParam:' + JSON.stringify(data))
        // Authentication type, 1 for pin code.
        //code ==1,pin码
        if (data && data.authType == 1) {
            this.StatInfo = {
                deviceName: data.extraInfo.packageName,
                appName: data.extraInfo.appName,
                appIcon: null,
                pinCode: data.extraInfo.pincode+'',
                pinToken: data.extraInfo.pinToken
            }
            // direction: 1(main)/0(join)
            if (data.extraInfo.direction == 1) {
                this.mainPin()
            } else if (data.appIcon) {
                //连接端授权，业务
                this.joinAuthImage()
            } else if (data.extraInfo.business == 0) {
                // business: 0(FA流转)/1(资源访问)
             /*   this.joinAuth()*/
            } else {
                this.joinAuthorize()
            }
        }
    },
    /**
     * Set user Operation from devicemanager Fa, this interface can only used by devicemanager Fa.
     *
     * @param operateAction User Operation Actions.
     * ACTION_ALLOW_AUTH = 0, allow authentication
     * ACTION_CANCEL_AUTH = 1, cancel authentication
     * ACTION_AUTH_CONFIRM_timeout = 2, user operation timeout for authentication confirm
     * ACTION_CANCEL_PINCODE_DISPLAY = 3, cancel pinCode display
     * ACTION_CANCEL_PINCODE_INPUT = 4, cancel pinCode input
     */
    setUserOperation(operation) {
        var data = devicemanager.setUserOperation(operation);
        console.info('setUserOperation result: ' + JSON.stringify(data))
    },
    joinAuth() {
        if( this.status == null)
        {
             this.dmpage = true
             this.status = 'join-auth'
        }
        //this.timing(2)
    },
    /**
     * Join end authorization, business(FA流转)/1(资源访问): 1, show application icon
     */
    joinAuthImage() {
        this.status = 'join-auth-image'
        //this.timing(2)
        /* router.back()*/
    },
    /**
     * Cancel authorization
     */
    joinAuthorizeCancel() {
        this.setUserOperation(1)
        this.dmpage = false
    },
    /**
     * Cancel authorization
     */
    joinPinCancel() {
        this.setUserOperation(3)
        this.exitDM()
        this.status = null
        this.dmpage = false
    },
    /**
     * Join end authorization, business(FA流转)/1(资源访问): 0
     */
    joinAuthorize() {
        this.status = 'join-authorize'
        //this.timing(2)
        /*  router.back()*/
    },
    /**
     * Confirm authorization
     */
    joinAuthorizeOk() {
        this.setUserOperation(0)
        this.joinPin()
    },
    /**
     * Input pinCode at the main control terminal
     */
    mainPin() {
        this.statusType = null
        this.status = 'main-pin'
    },
    joinPin() {
        this.status = 'join-pin'
      /*  this.OndeviceStateChange()*/
    },
    /**
     * Pure function countdown
     * @param operateAction User Operation Actions.
     */
    timing(numb){
        let  _this =this
        _this.timeout = setInterval(function(){
            _this.timeRemaining--;
             console.log(_this.timeRemaining)
            if(_this.timeRemaining ==0){
                console.log(_this.timeRemaining)
                _this.setUserOperation(numb)
                _this.dmpage = false
                /* _this.exitDM()
                clearInterval(_this.timeout)*/
            }
        },1000)
    },
    back() {
        this.dmpage = false
        this.statusType = null
        this.status = null
        this.exitDM()
    },
    changeCode(){
        this.AuthenticateDevice()
    },
    localCode(){
        this.status = null
    },
    BackToMusicPage(){
        this.dmpage = false
    },
    exitDM(){
        this.status = null
        this.dmpage = false
        this.GetPonken = null
        this.statusInfo = null
        this.subscribeId =0
        this.cleanInputPin()
        clearInterval(this.IntervalID)
    },
    OndeviceFound(){
        devicemanager.on('deviceFound', (data0,data1) => {
            if (data1 == null) {
                console.info("deviceFound error data=null")
                return;
            }
    /*        clearInterval(this.StartDiscoveryTimer)
            console.info("in deviceFound success");*/
            console.info("deviceFound:" + JSON.stringify(data0));
            console.info("deviceFound:" + JSON.stringify(data1));
            this.statusInfo = {
                deviceId: data1.deviceId,
                deviceName: data1.deviceName,
                deviceTypeId: data1.deviceTypeId
            }
        });
    },
    statmainInputPin(){
        this.status ='main-pin'
    },
   OndiscoverFail(){
       devicemanager.on('discoverFail', (data,data1) => {
/*           console.info('on discoverFail in')
           console.info("discoverFail on:" + JSON.stringify(data));
           console.info("discoverFail on:" + JSON.stringify(data1));*/
       });
   },
    OndeviceStateChange(){
        devicemanager.on('deviceStateChange', (data,data1) => {
            console.info('on deviceStateChange in')
            console.info("deviceStateChange data:" + JSON.stringify(data));
            if(data.action == 0){
                console.log('in data.action')
                clearInterval(this.IntervalID)
                this.status = null
                this.dmpage = false
            }
            console.info("deviceStateChange data1:" + JSON.stringify(data1));
        });
    },
    onserviceDie(){
        console.info('start serviceDie')
        devicemanager.on('serviceDie', (data) => {
            console.info('on serviceDie in')
            console.info("serviceDie:" + JSON.stringify(data));
        });
    },
    OndmFaCallback(){
        devicemanager.on('dmFaCallback', (data) => {
            console.info("on dmFaCallback data:" + JSON.stringify(data));
        });
    }
}
