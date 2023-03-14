// @ts-nocheck
/**
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
import eventsEmitter from '@ohos.emitter';

@Entry
@Component
struct VideoCreateComponent {
  @State srcs: string = "/images/trailer.mp4";
  @State previewUris: string = "/images/img.jpeg";
  @State currentProgressRates: number = 1;
  @State autoPlays: boolean = true;
  @State controlsss: boolean = true;
  myVideoController: VideoController = new VideoController();
  @State startStaus: boolean = true;

  onPageShow() {
    console.info('[list] page show called');
    var stateChangeEvent = {
      eventId: 235,
      priority: eventsEmitter.EventPriority.LOW
    }
    eventsEmitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[stateChangCallBack] video stateChangCallBack");
    if (eventData != null) {
      console.info("[stateChangCallBack] state change called:" + JSON.stringify(eventData));
      if (eventData.data.progressRates != null) {
        this.currentProgressRates = eventData.data.progressRates;
        console.info("currentProgressRates " + this.currentProgressRates)
      }
      if (eventData.data.isStared != null) {
        this.myVideoController.start();
      }
      if (eventData.data.isStopped != null) {
        this.myVideoController.stop();
      }
    }
  }

  build() {
    Column() {
      Video({
        src: this.srcs,
        previewUri: this.previewUris,
        currentProgressRate: this.currentProgressRates,
        controller: this.myVideoController
      })
        .key("video")
        .width(700)
        .height(500)
        .autoPlay(this.autoPlays)
        .controls(this.controlsss)

      Row() {
        Button("start").onClick(() => {
          this.myVideoController.start();
        });
        Button("pause").onClick(() => {
          this.myVideoController.pause();
        });
        Button("stop").onClick(() => {
          this.myVideoController.stop();
        });
      }

      Row() {
        Button("setCurrentTime").onClick(() => {
          this.myVideoController.setCurrentTime(9, SeekMode.Accurate);
        });
      }
    }
  }
}
