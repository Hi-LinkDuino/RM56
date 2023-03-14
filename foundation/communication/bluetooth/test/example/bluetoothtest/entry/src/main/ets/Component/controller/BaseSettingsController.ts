/**
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

import ISettingsController from './ISettingsController'

export default abstract class BaseSettingsController implements ISettingsController {
  protected getComponent: () => any;

/**
  * Bind component.
  */
  bindComponent(component: any): ISettingsController{
    this.getComponent = () => component;

// set default property values by component
    for (var key in this) {
      if (key in component) {
        this[key] = component[key];
      }
    }

    return this;
  }

/**
 * Bind component's properties, note that only basic types can be transported.
 * Type like Resource may meet unexpected error.
 * If you want to transport resource string or color, ets. AppStorage is suggested @StorageLink.
 */
  bindProperties(componentProperties: string[], controllerProperties?: string[]): ISettingsController {
    for (let i = 0; i < componentProperties.length; i++) {
      this.defineBoundProperty(componentProperties[i], controllerProperties ? controllerProperties[i] : componentProperties[i]);
    }
    return this;
  }

  /**
    * Initialize data.
    */
  initData(): ISettingsController {
    return this;
  };

  /**
    * Subscribe listeners.
    */
  subscribe(): ISettingsController {
    return this;
  };

  /**
    * Unsubscribe listeners.
    */
  unsubscribe(): ISettingsController {
    return this;
  };

  /**
    * Define bound properties.
    */
  private defineBoundProperty(componentProperty: string, controllerProperty: string): void {
    let __v = this[controllerProperty];

    Object.defineProperty(this, controllerProperty, {
      get: function () {
        return __v;
      },
      set: function (value) {
        __v = value;
        this.getComponent()[componentProperty] = value;
      }
    });
  }
}