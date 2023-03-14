/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

const testViewState = tsuite("View State", () => {

  // instead of a register, use glbal variables
  let childView: Child;

  class Parent extends View {
    // generated from
    // @State private forLink_ : boolean = 1
    // Info is the public property not the name of the prop in backing store
    /* private, for test purposes: */ public __forLink_: ObservedPropertySimple<boolean>
      = new ObservedPropertySimple<boolean>(true, this, "forLink_");
    public get forLink_(): boolean {
      return this.__forLink_.get();
    }
    public set forLink_(newValue: boolean) {
      this.__forLink_.set(newValue);
    }

    // generated from
    // @State private forProp_ : string
    // Info is the public property not the name of the prop in backing store
    /* private, for test purposes: */ public __forProp_: ObservedPropertySimple<string>
      = new ObservedPropertySimple<string>("ForProp Value OK", this, "forProp_");
    public get forProp_(): string {
      return this.__forProp_.get();
    }
    public set forProp_(newValue: string) {
      this.__forProp_.set(newValue);
    }

    // generated from
    // private forReg_ : string = "forReg Orig";
     /* private, for test purposes: */ public forReg_: string = "forReg Orig";

    // generated based on state definitions
    constructor(compilerAssignedUniqueChildId: string, parent: View,
      params: {
        forLink_?: boolean,
        forProp_?: string,
        forReg_?: string;
      },
      uri : string) {
      super(compilerAssignedUniqueChildId, parent);
      // tsc will add initialization of forLink_ and forProp_ with defauilt values here
      this.updateWithValueParams(params);
    }

    // generated based on state definitions
    public updateWithValueParams(params: {
      forLink_?: boolean,
      forProp_?: string,
      forReg_?: string
    }): void {
      console.debug(`${this.id__()}:${this.constructor.name}: updateWithValueParams ...`);
      if (params.forLink_) {
        this.forLink_ = params.forLink_;
      }
      if (params.forProp_) {
        this.forProp_ = params.forProp_;
      }
      if (params.forReg_) {
        this.forReg_ = params.forReg_;
      }
      console.debug(`${this.id__()}:${this.constructor.name}: updateWithValueParams done`);
    }

    // generated based on state definitions
    public aboutToBeDeleted() {
      console.debug(`${this.id__()}:${this.constructor.name}: abouttoBeDeleted`);
      // tell the ObservedProperty this View is to be deleted
      this.__forLink_.aboutToBeDeleted(this);
      this.__forProp_.aboutToBeDeleted(this);
      SubscriberManager.Get().delete(this.id__());
    }

    // added just to make things work, not in production when this func is implemented in C++
    findChildById(compilerAssignedUniqueChildId: string): View {
      console.log(`${this.id__()}:${this.constructor.name}: findChildById ${compilerAssignedUniqueChildId}. Will not work!`);
      return childView;
    }

    public render() {
      // dump state
      console.debug(`${this.id__()}:${this.constructor.name}: render: state is: forLink_: ${this.forLink_}, forProp_: ${this.forProp_}`);

      const compilerAssignedChildId = "1";
            /* global var */ childView = this.findChildById(compilerAssignedChildId) as Child;
      if (!childView) {
        childView = new Child(compilerAssignedChildId, this, {
          link_: this.__forLink_,
          prop_: this.__forProp_,
          reg_: this.forReg_
        });
      } else {
        childView.updateWithValueParams({ reg_: this.forReg_ });
      }
    }
  }

  // Child ----------------------------------------
  class Child extends View
    implements IPropertySubscriber {

    // generated from
    // @State private num_ : number = 1;
    private __num_: ObservedPropertySimple<number>
      = new ObservedPropertySimple<number>(1, this, "__num_");
    /* private changed to for testing: */ public get num_(): number {
      return this["__num_"].get();
    }
    /* private changed to for testing: */ public set num_(newValue: number) {
      this["__num_"].set(newValue);
    }

    // generated from
    // @Link private link_ : boolean
     /* private changed to for testing: */ __link_: ObservedPropertySimpleAbstract<boolean>;
    /* private changed to for testing: */ public get link_(): boolean {
      return this.__link_.get();
    }
    /* private changed to for testing: */ public set link_(newValue: boolean) {
      this.__link_.set(newValue);
    }

    // generated from
    // @Prop private prop_ : string
     /* private changed to for testing: */ __prop_: ObservedPropertySimpleAbstract<string>;
    /* private changed to for testing: */ public get prop_(): string {
      return this.__prop_.get();
    }
    /* private changed to for testing: */ public set prop_(newValue: string) {
      this.__prop_.set(newValue);
    }

    // eDSL compiler makes no change
    /* private changed to for testing: */ public reg_: string = "initial value"

    constructor(compilerAssignedUniqueChildId: string, parent: View,
      params: {
        num_?: number,
        link_: ObservedPropertySimpleAbstract<boolean>,
        prop_: ObservedPropertySimpleAbstract<string>,
        reg_?: string
      }) {
      super(compilerAssignedUniqueChildId, parent);
      // tsc will add initialization of num_ and reg_ with defauilt values here
      this.__link_ = params.link_.createLink(this, "__link_");
      this.__prop_ = params.prop_.createProp(this, "__prop_");
      this.updateWithValueParams(params);
    }

    // generated
    public updateWithValueParams(params: { num_?: number, reg_?: string }): void {
      console.debug(`${this.id__()}:${this.constructor.name}: updateWithValueParams start`);
      if (params.num_) {
        this.num_ = params.num_;
      }
      if (params.reg_) {
        this.reg_ = params.reg_;
      }
      console.debug(`${this.constructor.name}: updateWithValueParams done`);
    }

    // generated based on state definitions
    // notify all SubscribedAbstractProperty objects they will be deleted soon
    public aboutToBeDeleted() {
      console.debug(`${this.id__()}:${this.constructor.name}: aboutToBeDeleted`);
      // notify @State
      // this View unsubscribes
      this.__num_.aboutToBeDeleted(this);

      // notify for @Link and @Prop
      // to allow them to unsubscribe from their source properties
      // this View unsubscribes as well
      this.__link_.aboutToBeDeleted();
      this.__prop_.aboutToBeDeleted();

      SubscriberManager.Get().delete(this.id__());
    }


    // transpiled from build()
    public render() {
      // dump state
      console.debug(`${this.id__()}:${this.constructor.name}: render: state is: num_: ${this.num_}, link_: ${this.link_}, prop_: ${this.prop_}, reg_=${this.reg_}`);
    }
  }


  // simulate the process

  console.debug("create LocalStorage ...")
  const uri = "stateMgmt/src/utest/view_test.ts";
  LocalStorageLookup.GetOrCreate(uri, {});

  console.debug("create Parent ...")
  let parentView: Parent = new Parent("0", undefined, {}, uri);


  tcase("Parent.render #1 ...", () => {

    // to test the recording of get access / View.propsNeededForRender
    parentView.aboutToRender();
    parentView.render();
    parentView.onRenderDone();

    test(`get access recording during parent render, recorded: ${JSON.stringify(Array.from(parentView.propertiesNeededToRender()))}`,
      eqSet(parentView.propertiesNeededToRender(), new Set<string>(["forLink_", "forProp_"])));

    childView.render();

    test(`parentView.forLink_ value ok`, parentView.forLink_ == true);
    test(`parentView.forProp_ value ok`, parentView.forProp_ == "ForProp Value OK");
    test(`parentView.forReg_ value ok`, parentView.forReg_ == "forReg Orig");

    test(`childView.num_ value ok`, childView.num_ == 1);
    test(`childView.link_ value ok`, childView.link_ == true);
    test(`childView.prop_ value ok`, childView.prop_ == "ForProp Value OK");
    test(`childView.reg_ value ok`, childView.reg_ == "forReg Orig");

    test(`Check @Link and @State subscribed ok: parent.__forLink_ subscribers`, parentView.__forLink_.numberOfSubscrbers() == 2);
    test(`Check @Link and @State subscribed ok: parent.__forProp_ subscribers`, parentView.__forProp_.numberOfSubscrbers() == 2);

    SubscriberManager.Get().dumpSubscriberInfo();
    // parent view
    // parent view 2 state vard
    // child view
    // child view 3 state vars
    // 7 subscribers total
    test(`SubscriberManager num of subscribers, is ${SubscriberManager.Get().numberOfSubscrbers()} should be 7`, SubscriberManager.Get().numberOfSubscrbers() == 7);
  });

  tcase("Simulate an event handler mutating parent's @State props", () => {

    let spyParentPropertyHasChanged = spyOn(parentView, "propertyHasChanged");
    let spyChildPropertyHasChanged = spyOn(childView, "propertyHasChanged");

    let spyParentPropertyHasBeenRead = spyOn(parentView, "propertyRead");
    let spyChildPropertyHasBeenRead = spyOn(childView, "propertyRead");

    parentView.forLink_ = false;
    parentView.forProp_ = "_forProp changed value OK";
    // render will do the updating of the prop value:
    childView.prop_ = parentView.forProp_;

    test(`parentView.forLink_ value ok`, parentView.forLink_ == false);
    test(`parentView.forProp_ value ok`, parentView.forProp_ == "_forProp changed value OK");
    test(`parentView.forReg_ value ok`, parentView.forReg_ == "forReg Orig");

    test(`childView.num_ unchanged`, childView.num_ == 1);
    test(`childView.link_ updated ok`, childView.link_ == false);
    test(`childView.prop_ updated ok`, childView.prop_ == "_forProp changed value OK");
    test(`childView.reg_ unchanged`, childView.reg_ == "forReg Orig");

    test(`parentView.aPropertyHasChanged was called`, spyParentPropertyHasChanged.called)
    test(`childView.aPropertyHasChanged was called`, spyChildPropertyHasChanged.called)

    test(`parentView.aPropertyHasBeenRead was called`, spyParentPropertyHasBeenRead.called)
    test(`childView.aPropertyHasBeenRead was called`, spyChildPropertyHasBeenRead.called)
  });

  tcase("Parent.render #2 ...", () => {

    parentView.render();
    childView.render();

    test(`childView.num_ unchanged`, childView.num_ == 1);
    test(`childView.link_ unchanged`, childView.link_ == false);
    test(`childView.prop_ unchanged`, childView.prop_ == "_forProp changed value OK");
    test(`childView.reg_ unchanged`, childView.reg_ == "forReg Orig");
  });

  tcase("Simulate an event handler mutating parent's regular (unobserved) variable", () => {

    let spyParentPropertyHasChanged = spyOn(parentView, "propertyHasChanged");

    parentView.forReg_ = "forReg_ chanaged";
    test(`parentView.forReg_ value ok`, parentView.forReg_ == "forReg_ chanaged");
    test(`childView.reg_ unchanged`, childView.reg_ == "forReg Orig");
    test(`parentView.propertyHasChanged was NOT called`, !spyParentPropertyHasChanged.called)
  });

  tcase("Parent.render #3 ...", () => {

    parentView.render();
    childView.render();

    let spyChildaPropertyHasChanged = spyOn(childView, "aPropertyHasChanged");

    test(`parentView.forReg_ value ok`, parentView.forReg_ == "forReg_ chanaged");
    test(`childView.reg_ changed`, childView.reg_ == "forReg_ chanaged");
    test(`childView.aPropertyHasChanged was NOT called`, !spyChildaPropertyHasChanged.called)
  });

  tcase("Notify about deletion, first child ...", () => {

    let spyChildLinkAboutToBeDeleted = spyOn(childView.__prop_, "aboutToBeDeleted");
    let spyChildPropAboutToBeDeleted = spyOn(childView.__link_, "aboutToBeDeleted");

    childView.aboutToBeDeleted();
    test(`aboutToBeDeleted was called on childView.__link`, spyChildLinkAboutToBeDeleted.called);
    test(`aboutToBeDeleted was called on childView.__prop`, spyChildPropAboutToBeDeleted.called);
    test(`parent.__forLink_ subscribers`, parentView.__forLink_.numberOfSubscrbers() == 1);

    SubscriberManager.Get().dumpSubscriberInfo();
    // parent view
    // parent view forLink_, forProp_ state vars
    test(`SubscriberManager num of subscribers is ${SubscriberManager.Get().numberOfSubscrbers()} should be 3`, SubscriberManager.Get().numberOfSubscrbers() == 3);
  });

  tcase("Notify about deletion, second parent ...", () => {
    parentView.aboutToBeDeleted();
    test(`parent.__forLink_ subscribers`, parentView.__forLink_.numberOfSubscrbers() == 0);

    SubscriberManager.Get().dumpSubscriberInfo();

    test(`SubscriberManager num of subscribers is ${SubscriberManager.Get().numberOfSubscrbers()} should be 0 .`, SubscriberManager.Get().numberOfSubscrbers() == 0);
  });

});
