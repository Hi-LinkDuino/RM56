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

const testViewAppStorage = tsuite("View AppStorge", () => {

  // @Entry
  class Player extends View {

    // declare vars:

    /* @StorageLink("isPlaying") playCount : number = 0;  */
    private __playCount: SubscribedAbstractProperty<number>
      = as.setAndLink<number>("playCount", /* default value */ 0, /* subscriber */ this);

    public get playCount(): number {
      return this.__playCount.get();
    }
    public set playCount(newValue: number) {
      this.__playCount.set(newValue);
    }

    // generated constructor
    constructor(compilerAssignedUniqueChildId: string, parent: View,
      params: {}) {
      super(compilerAssignedUniqueChildId, parent);

      // tsc will add initialization of @State and regular variables with defauilt values below super() and above this comment.
      this.updateWithValueParams(params);
    }


    // will be called on 2nd render (generated)
    public updateWithValueParams(params: {}): void {
      console.debug(`${this.id__()}:${this.constructor.name}: updateWithValueParams start`);
      console.debug(`${this.id__()}:${this.constructor.name}: updateWithValueParams done`);
    }


    // will be called when View is no longer used (like destructor, generated)
    public aboutToBeDeleted() {
      console.debug(`${this.id__()}:${this.constructor.name}: aboutToBeDeleted`);
      this.__playCount.aboutToBeDeleted();
      SubscriberManager.Get().delete(this.id__());
    }

    render() {
      console.debug(`${this.id__()}:${this.constructor.name}: render playCount ${this.playCount}`);
    }
  }

  // simulate the process
  console.debug("create Player ...")
  let playerView: Player;

  tcase("create View and read back", () => {
    playerView = new Player("0", undefined, {});
    test("readback following creation, playCount has correct value", Array.from(as.keys()).toString() == ["playCount"].toString());
    test("readback following creation, playCount has correct value", as.get<number>("playCount") == 0);
  });


  let linkToplayCount: SubscribedAbstractProperty<number>;

  tcase("Simulate an event handler mutating parent's @StorageLink", () => {

    let spyPlayerPropertyHasChanged = spyOn(playerView, "propertyHasChanged");

    linkToplayCount = as.link("playCount" /* no subscriber */)

    playerView.playCount = 1;

    test(`playerView.aPropertyHasChanged was called`, spyPlayerPropertyHasChanged.called);
    test("readback from AppStorage, playCount has correct value", as.get<number>("playCount") == 1);
    test("readback from link variable, playCount has correct value", linkToplayCount.get() == 1);
  });

  tcase("Simulate an event handler mutating link to AppStorage prop.", () => {

    let spyPlayerPropertyHasChanged = spyOn(playerView, "propertyHasChanged");

    linkToplayCount.set(7);

    test("readback from AppStorage, playCount has correct value", as.get<number>("playCount") == 7);
    test(`playerView.aPropertyHasChanged was called`, spyPlayerPropertyHasChanged.called);
    test(`playerView.aPropertyHasChanged param correct`, spyPlayerPropertyHasChanged.args[0] == "playCount");
    test("readback from link variable, playCount has correct value", playerView.playCount == 7);
  });
});
