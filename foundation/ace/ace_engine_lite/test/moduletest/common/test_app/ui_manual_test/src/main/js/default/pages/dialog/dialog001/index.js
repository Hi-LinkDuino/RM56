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
import {backPage, routePage, lifeCycleFunc} from "../../../common/js/general";
import prompt from "@system.prompt";

export default {
  backMain:backPage().backMain,
  ...lifeCycleFunc(),
  showDialog() {
    console.log("JS::show dialog called!");
    prompt.showDialog({title: '弹框标题',});
  },

  showDialog01() {
    console.log("JS::show dialog01 called!");
    prompt.showDialog({
    title: '弹框标题',
    message: '弹框显示内容01',
    buttons: [
      {
        text: '确认',
        color: '#666666',
      },
    ],
    success: function(data) {
      console.log('dialog success callback，click button : ' + data.index);
      switch(data.index)
      {
          case 0:
              console.log("JS::dialog confirm button clicked!");
              break;
      }
    },
    });
  },

    showDialog02() {
      console.log("JS::show dialog02 called!");
      prompt.showDialog({
      title: '弹框标题',
      message: '弹框显示内容02',
      buttons: [
        {
          text: '确认',
          color: '#666666',
        },
        {
          text: '取消',
          color: '#666666',
        },
      ],
      success: function(data) {
        console.log('dialog success callback，click button : ' + data.index);
        switch(data.index)
        {
            case 0:
                console.log("JS::dialog confirm button clicked!");
                break;
            case 1:
                console.log("JS::dialog cancel button clicked!");
                break;
        }
      },
    });
  },

  showDialog03() {
      console.log("JS::show dialog03 called!");
      prompt.showDialog({
      title: '弹框标题',
      message: '弹框显示内容03',
      buttons: [
        {
          text: '确认',
          color: '#666666',
        },
        {
          text: '取消',
          color: '#666666',
        },
        {
          text: '完成',
          color: '#666666',
        },
      ],
      success: function(data) {
        console.log('dialog success callback，click button : ' + data.index);
        switch(data.index)
        {
            case 0:
                console.log("JS::dialog confirm button index 0 clicked!");
                break;
            case 1:
                console.log("JS::dialog cancel button index 1 clicked!");
                break;
            case 2:
                console.log("JS::dialog complete button index 2 clicked!");
                break;
        }
      },
    });
  },

  showDialog04() {
      console.log("JS::show dialog04 called!");
      prompt.showDialog({
      title: '弹框标题',
      message: '弹框显示内容04',
      buttons: [
        {
          text: '确认',
          color: '#666666',
        },
        {
          text: '取消',
          color: '#666666',
        },
        {
          text: '完成',
          color: '#666666',
        },
        {
          text: 'button04',
          color: '#666666',
        },
      ],
      success: function(data) {
        console.log('dialog success callback，click button : ' + data.index);
        switch(data.index)
        {
            case 0:
                console.log("JS::dialog confirm button index 0 clicked!");
                break;
            case 1:
                console.log("JS::dialog cancel button clicked!");
                break;
            case 2:
                console.log("JS::dialog complete button clicked!");
                break;
            case 3:
                console.log("JS::dialog buttontest04 button clicked!");
                break;
        }
      },
    });
  },

      showDialog05() {
      var dialogContext = this;
      console.log("JS::show dialog05 called!");
      prompt.showDialog({
      title: '弹框标题',
      message: '弹框显示内容06',
      buttons: [
        {
          text: '确认',
          color: '#ff0000',
        },
        {
          text: '取消',
          color: '#00ff00',
        },
      ],
      success: function(data) {
        console.log('dialog success callback，click button : ' + data.index);
        switch(data.index)
        {
            case 0:
                console.log("JS::dialog confirm button clicked!");
                dialogContext.showDialog07();
                break;
            case 1:
                console.log("JS::dialog cancel button clicked!");
                break;
        }
      },
    });
    setTimeout(
      () => {
        this.showDialog06();
      }, 5 * 1000
    );

  },

  showDialog06() {
    console.log("JS::show dialog 06 called!");
    prompt.showDialog({message: 'only ShowMessage',});
  },

  showDialog07() {
    console.log("JS::show dialog 07 called !");
    prompt.showDialog({
      title: '弹框标题',
      message: 'show second dialog',
      buttons: [
        {
          text: '确认',
          color: '#0000ff',
        },
        {
          text: '取消',
          color: '#0000ff',
        },
      ],
      success: function(data) {
        console.log('dialog success callback，click button : ' + data.index);
        switch(data.index)
        {
            case 0:
                console.log("JS::dialog confirm button clicked!");
                break;
            case 1:
                console.log("JS::dialog cancel button clicked!");
                break;
        }
      },
    });
  },

};