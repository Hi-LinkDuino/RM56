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
import {backPage,routePage} from "../../../common/js/general";

export default {
  data: {
      photoUri: "",
      videoSrc:"/sdcard/video/test_720p_no_audio.LRV",
      showControls: true,
      startTime:"00:00",
      endTime:"00:00",
      index : 1,
  },
  startPlayVideo: function () {
    console.log("JS::startPlayVideo called!");
    var video = this.$refs.vid;
    video.start();
  },
  stopPlayVideo: function () {
    console.log("JS::pausePlayVideo called!");
    var video = this.$refs.vid;
    video.pause();
  },
  setCurrentTime: function (val) {
    console.log("JS::setCurrentTime called!");
    var video = this.$refs.vid;
    video.setCurrentTime(10);
  },

  playNext: function () {
    console.log("JS::playNext video!");
  if (this.index % 2 == 0) {
      console.log("click change to play image startPlayVideo called!");
      this.videoSrc = "/sdcard/video/test_720p_no_audio.LRV";
    } else {
      console.log("click change to stop image stopPlayVideo called!");
      this.videoSrc = "/sdcard/video/test.mp4";
    }
  this.index ++;
  },

  valueChange(value) {
    var video = this.$refs.vid;
    video.setCurrentTime(value.progress);
    this.startTime = value.progress;
  },

  preparedfn: function() {
    console.log('js video preparedfn called');
  },

  startfunc: function() {
    console.log('js video startfunc called');
  },

  pausefn: function() {
    console.log('js video pausefn called');
  },

  finishfn: function() {
    console.log('js video finishfn called');
  },

  error: function () {
    console.log('onerror called');
  },

  seekingfn: function(e) {
    console.log('js video seeking called:' + e.currenttime);
  },
  seekedfn: function(e) {
    console.log('js video seeked called' + e.currenttime);
  },
  timeupdatefn: function(e) {
    console.log('js video timeUpdate called' + e.currenttime);
  },

//  *************************  pages  *********************************
  backMain:backPage().backMain

}
