
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
// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index"
import missionManager  from '@ohos.application.missionManager';
import featureAbility from '@ohos.ability.featureAbility';
import bundleManager from '@ohos.bundle';
import appManager from "@ohos.application.appManager"
import commonEvent from '@ohos.commonEvent';
import {onDestroys} from "../MainAbility2/app.js"
import abilityManager from '@ohos.application.abilityManager'
import ConfigurationConstant from "@ohos.application.ConfigurationConstant";

function startAility(){
    setTimeout(()=>{
        featureAbility.startAbility({ want: {
            bundleName: "com.example.abilitydisable",
            abilityName: "com.example.abilitydisable.MainAbility2"
        } }, (error, data) => {
            console.log('SUB_AA_OpenHarmony_Clear_0400 - startMainAbility2: '
            + JSON.stringify(error) + ", " + JSON.stringify(data))
        });
    },100)
    setTimeout(()=>{
        featureAbility.startAbility({ want: {
            bundleName: "com.example.abilitydisable",
            abilityName: "com.example.abilitydisable.MainAbility3"
        } }, (error, data) => {
            console.log('SUB_AA_OpenHarmony_Clear_0400 - startMainAbility3: '
            + JSON.stringify(error) + ", " + JSON.stringify(data));
        });
    },400)
}
function lockMissions(titles,missionId,times){
        setTimeout(()=>{
            console.log(titles+" lockMission missionIds[1]+++++123456::::"+missionId + typeof missionId)
            for(var i=0;i<missionId.length;i++){
                missionManager.lockMission(parseInt(missionId[i]),(err,data)=>{
                console.log(titles+'lockMission1 errCode:' + JSON.stringify(err) + " data: " + JSON.stringify(data));
                });
            };
        },times);
}
function clearAllMissions(){
        missionManager.clearAllMissions((err,data)=>{
            console.log('clearAllMissions errCode: ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
        })
}

function fns(){
        setTimeout(()=>{
            featureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility2"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 - startMainAbility2: '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
                expect(err.code == 0).assertTrue();
            });
        },100)
        setTimeout(()=>{
            featureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility4"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 - startMainAbility4: '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
                expect(err.code == 0).assertTrue();
            });
        },300)
        setTimeout(()=>{
            featureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility3"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 - startMainAbility3:  '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
                expect(err.code == 0).assertTrue();
            });
        },500)
        setTimeout(()=>{
        missionManager.getMissionInfos("", 20, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Clear_0100 getMissionInfos errCode : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            for (var i = 0;i < data.length; i++) {
                console.log("bundleName："+data[i].want["abilityName"])
                if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility3"
                || data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility4"
                ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility"
                ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility2"){
                    console.log("data[i]['lockedState']::::"+data[i]['lockedState']);
                    console.log("data[i].missionId" + data[i].missionId);
                    console.log("SUB_AA_OpenHarmony_Clear_0100, missionId:" + data[i].missionId);
                    missionIds.push(data[i].missionId);
                }
            }
            console.log('SUB_AA_OpenHarmony_Clear_0100 missionId：'+ missionIds +
            typeof missionIds + ":::::"+missionIds[1] + typeof missionIds[1]);
        })
        console.log("SUB_AA_OpenHarmony_Clear_0100" + JSON.stringify(missionIds));
        },800)
        setTimeout(()=>{
            missionManager.lockMission(parseInt(missionIds[1]),(err,data)=>{
            console.log('SUB_AA_OpenHarmony_Clear_0100 lockMission1 missionIds[1] : ' +
            JSON.stringify(missionIds[1]), + parseInt(missionIds[1]));
            console.log('SUB_AA_OpenHarmony_Clear_0100 lockMission1 errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
        })
    },1200)
        setTimeout(()=>{
            missionManager.lockMission(parseInt(missionIds[2]),(err,data)=>{
            console.log('SUB_AA_OpenHarmony_Clear_0100 lockMission2 errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
        })
    },1500)
        setTimeout(()=>{
            missionManager.getMissionInfos("", 20, (err, data) => {
            console.log("SUB_AA_OpenHarmony_Clear_0100, twosgetMissionInfos:" +
            JSON.stringify(data));
            for (var i = 0;i < data.length; i++) {
                console.log("data[i].missionId：missionIds[2] news missionIds[1]" +
                data[i].missionId + missionIds[2] + missionIds[1])
                if(data[i].missionId == missionIds[1] || data[i].missionId == missionIds[2]){
                    console.log("data[i]['lockedState']222:::::::"+data[i]['lockedState']);
                    expect(data[i]['lockedState'] == true).assertTrue();
                }
            }
        })
        console.log("SUB_AA_OpenHarmony_Clear_0100" + JSON.stringify(missionIds));
    },3500)
    setTimeout(()=>{
        missionManager.getMissionInfos("", 20, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Clear_0100 getMissionInfos errCode : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
//            var terminateSelf =
            for (var i = 0;i < data.length; i++) {
                console.log("bundleName："+data[i].want["abilityName"])
                if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility3"
                || data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility4"
                ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility"
                ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility2"){
                    console.log("data[i]['lockedState']::::"+data[i]['lockedState']);
                    expect(data[i].lockedState == false).assertTrue();
                }
            }
            console.log('SUB_AA_OpenHarmony_Clear_0100 missionId：'+ missionIds +
            typeof missionIds + ":::::"+missionIds[1] + typeof missionIds[1]);
        })
        console.log("SUB_AA_OpenHarmony_Clear_0100" + JSON.stringify(missionIds));
    },4000)
    setTimeout(()=>{
        missionManager.moveMissionToFront(parseInt(missionIds[2]),(err,data)=>{
            console.log('SUB_AA_OpenHarmony_Clear_0100 moveMissionToFront errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
        })
    },4500)
    setTimeout(()=>{
        missionManager.clearAllMissions((err,data)=>{
            console.log('SUB_AA_OpenHarmony_Clear_0100 clearAllMissions errCode: ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));

        })
    },5000)
    setTimeout(()=>{
        missionManager.getMissionInfos("", 20, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Clear_0100 getMissionInfos errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            for (var i = 0;i < data.length; i++) {
                console.log("bundleName："+data[i].want["abilityName"])
                if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility3" ||
                data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility4" ||
                data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility"||
                data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility2"){
                    console.log("data[i]['lockedState']end::::"+data[i]['lockedState']);
                    expect(data[i]['lockedState'] == true).assertTrue();
                }
            }
            console.log('SUB_AA_OpenHarmony_Clear_0100 missionId：'+
            missionIds + typeof missionIds + ":::::"+missionIds[1] + typeof missionIds[1]);
        })
        console.log("SUB_AA_OpenHarmony_Clear_0100" +
        JSON.stringify(missionIds));
    },8000)
    expect(lists.indexOf(JSON.stringify("singletonEntryAbulity2_onShow"))!=-1).assertTrue();

}
describe('AbilityDisableTests', function () {
    let mainAbility1ListS =  new Array()
    let lists = new Array()
    function subscriberCallBackMainAbility2Destory(err,data){
        console.log("Subscribe2 CallBack data:" + JSON.stringify(data));
        lists.push(data["event"]);
        console.log("subscriberCallBackMainAbility2Destory lists:"+lists);
    };
    function subscriberCallBack_mainAbility1_Destory(err,data){
        console.log("Subscribe3 CallBack data:" + JSON.stringify(data));
        mainAbility1ListS.push(data["event"])
        console.log("subscriberCallBack_mainAbility3_Destory mainAbility1ListS:"+mainAbility1ListS);
    }
    console.log("AbilityDisableTests --- start");
    var ability2LifeEvents = {
        events:["ApplicationMainAbility2_onDestroy",
                "singletonEntryAbulity2_onHide",
                "singletonEntryAbulity2_onActive",
                "singletonEntryAbulity2_onInactive",
                "singletonEntryAbility2_onDestroy",
                "singletonEntryAbulity2_onShow",
                "ApplicationMainAbility2_onCreate",]
    };
    var ability1LifeEvents = {
        events:["singletonEntryAbulity3_onHide",
        "singletonEntryAbulity3_onActive",
        "singletonEntryAbulity3_onInactive",
        "singletonEntryAbility3_onDestroy",
        "singletonEntryAbulity3_onShow",
        "ApplicationMainAbility3_onCreate",
        "ApplicationMainAbility3_onDestroy",
        ]
    };

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2900
     * @tc.name    Verify the full-screen switching application
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2900', 0, async function (done) {

        var Subscriber2 = [];
        var Subscriber1 = [];
        var mainAbility1missinds = []
        console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2900 --- start")
        commonEvent.createSubscriber(ability2LifeEvents).then(async (data)=>{
            console.log("Create Subscriber2=======>："+JSON.stringify(data));
            Subscriber2 = data;
            console.log("data2 is:" + JSON.stringify(Subscriber2));
            await commonEvent.subscribe(Subscriber2,subscriberCallBackMainAbility2Destory)
        });
        commonEvent.createSubscriber(ability1LifeEvents).then(async (data)=>{
            console.log("Create Subscriber1=======>："+JSON.stringify(data));
            Subscriber1 = data;
            console.log("data2 is:" + JSON.stringify(Subscriber1));
            await commonEvent.subscribe(Subscriber1,subscriberCallBack_mainAbility1_Destory)
        });
        setTimeout(()=>{
            featureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility2"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_2900 - startMainAbility2: '
                + JSON.stringify(error) + ", " + JSON.stringify(data));
            });
        },500)
        setTimeout(()=>{
            featureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility3"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_2900 - startMainAbility3: '
                + JSON.stringify(error) + ", " + JSON.stringify(data));
            });
        },1000);
        setTimeout(()=>{
            console.log("1111subscriberCallBackMainAbility2Destory lists:"+lists);
            expect(lists.indexOf(JSON.stringify("singletonEntryAbulity2_onShow"))!=-1).assertTrue();
            expect(lists.indexOf(JSON.stringify("ApplicationMainAbility2_onCreate"))!=-1).assertTrue();
        },2500)
        setTimeout(()=>{
            missionManager.getMissionInfos("", 100, (err, data) => {
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_2900 getMissionInfos errCode: '+ 
		JSON.stringify(err) + " data: " + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility2"){
                        mainAbility1missinds.push(data[i]["missionId"])
                    }

                }
                console.log("mainAbility1missinds:"+mainAbility1missinds);
            });
        },3000)
        setTimeout(()=>{
            console.log('SUB_AA_OpenHarmony_MoveMissionToFront_2900 moveMissionToFront dataCode:++++++');
            missionManager.moveMissionToFront(parseInt(mainAbility1missinds[0]),{
                parameters:{ "windowMode":1 }}).then((data)=>{
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_2900 moveMissionToFront dataCode:'+ " data:"
                + JSON.stringify(data));
                expect().assertFail();
            }).catch((err)=>{
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_2900 moveMissionToFront errCode:'+ " err:"
                + JSON.stringify(err));
            });
        },3888);
        setTimeout(()=>{
            console.log("1111subscriberCallBack_mainAbility1_Destory lists:"+ mainAbility1ListS);
            expect(mainAbility1ListS.indexOf(JSON.stringify("singletonEntrymainAb" +
            "ility1ListSAbulity1_onInactive"))!=-1).assertTrue();
            expect(mainAbility1ListS.indexOf(JSON.stringify("singletonEntryA" +
            "bulity1_onHide"))!=-1).assertTrue();
        },3999);
        done()
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_3000
     * @tc.name    Verify the left split screen switching application
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_3000', 0, async function (done) {
        var Subscriber2 = "";
        var Subscriber1 = "";
        var mainAbility1missinds = []
        console.log("SUB_AA_OpenHarmony_MoveMissionToFront_3000 --- start")
        commonEvent.createSubscriber(ability2LifeEvents).then(async (data)=>{
            console.log("Create Subscriber2=======>："+JSON.stringify(data));
            Subscriber2 = data;
            console.log("data2 is:" + JSON.stringify(Subscriber2));
            await commonEvent.subscribe(Subscriber2,subscriberCallBackMainAbility2Destory)
        });
        commonEvent.createSubscriber(ability1LifeEvents).then(async (data)=>{
            console.log("Create Subscriber1=======>："+JSON.stringify(data));
            Subscriber1 = data;
            console.log("data2 is:" + JSON.stringify(Subscriber1));
            await commonEvent.subscribe(Subscriber1,subscriberCallBack_mainAbility1_Destory)
        });

        setTimeout(()=>{
            featureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility2"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_3000 - startMainAbility2: '
                + JSON.stringify(error) + ", " + JSON.stringify(data));
            });
        },500);
        setTimeout(()=>{
            featureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility3"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_3000 - startMainAbility3: '
                + JSON.stringify(error) + ", " + JSON.stringify(data));
            });
        },1000);
        setTimeout(()=>{
            console.log("1111subscriberCallBack_mainAbility2_lists:
	singletonEntryAbulity2_onShow:"+lists);
            console.log("+++++++++++++++++"+lists.indexOf("singletonEntryAbulity2_onShow")
	+"+++++++"+lists.indexOf("ApplicationMainAbility2_onCreate")+lists)
            expect(lists.indexOf(JSON.stringify("singletonEntryAbulity2_onShow"))!=-1).assertTrue();
            expect(lists.indexOf(JSON.stringify("ApplicationMainAbility2_onCreate"))!=-1).assertTrue();
        },2500);
        setTimeout(()=>{
            missionManager.getMissionInfos("", 100, (err, data) => {
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_3000 
		getMissionInfos errCode: '+ 
	JSON.stringify(err) + " data: " + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    console.log("bundleName222222："+data[i].want["abilityName"]+
			"data[i]['lockedState']::::"+data[i]['lockedState']);
                    if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility3"){
                        console.log('data[i]["missionId"]'+data[i]["missionId"] + "::::"+
			data[i].want["abilityName"]);
                        mainAbility1missinds.push(data[i]["missionId"])
                    }
                }
                console.log("mainAbility1missinds:"+mainAbility1missinds);
            });
        },3000);
        setTimeout(()=>{
            console.log('SUB_AA_OpenHarmony_MoveMissionToFront_3000 
		moveMissionToFront dataCode:++++++');
            missionManager.moveMissionToFront(parseInt(mainAbility1missinds[0]),{
                parameters:{ "windowMode":100 }}).then((data)=>{
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_3000 
		moveMissionToFront dataCode:'+ 
		" data:" + JSON.stringify(data));
            }).catch((err)=>{
                console.log('SUB_AA_OpenHarmony_MoveMissionToFront_3000 
		moveMissionToFront errCode:'+ 
		" err:" + JSON.stringify(err));
            });
        },3888);
        setTimeout(()=>{
            console.log("1111subscriberCallBack_mainAbility1_Destory lists:"+ mainAbility1ListS);
            console.log("+++++++++++++++++"+mainAbility1ListS.indexOf("mainAbility1ListS_onShow")
		+"+++++++"+mainAbility1ListS.indexOf("ApplicationMainAbility2_onCreate")
		+mainAbility1ListS)
            expect(mainAbility1ListS.indexOf(JSON.stringify("singletonEntryAbulity1_onInactive"))==-1).assertTrue();
            expect(mainAbility1ListS.indexOf(JSON.stringify("singletonEntryAbulity1_onHide"))==-1).assertTrue();
        },3999);
        done();
    });
})
