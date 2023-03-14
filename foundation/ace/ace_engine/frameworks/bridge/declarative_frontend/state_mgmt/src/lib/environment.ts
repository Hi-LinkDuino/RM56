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

class Environment {
  private static Instance_: Environment = undefined;
  private static EnvBackend_: IEnvironmentBackend;
  private props_: Map<string, ObservedPropertyAbstract<any>>;

  private static GetOrCreate(): Environment {
    if (Environment.Instance_) {
      // already initialized
      return Environment.Instance_;
    }

    Environment.Instance_ = new Environment();
    return Environment.Instance_;
  }

  public static ConfigureBackend(envBackend: IEnvironmentBackend): void {
    Environment.EnvBackend_ = envBackend;
  }

  public static AboutToBeDeleted(): void {
    if (!Environment.Instance_) {
      return;
    }

    Environment.GetOrCreate().aboutToBeDeleted();
    Environment.Instance_ = undefined;
  }

  public static EnvProp<S>(key: string, value: S): boolean {
    return Environment.GetOrCreate().envProp<S>(key, value);
  }

  public static EnvProps(props: {
    key: string,
    defaultValue: any
  }[]
  ): void {
    Environment.GetOrCreate().envProps(props);
  }

  static Keys(): Array<string> {
    return Environment.GetOrCreate().keys();
  }

  private constructor() {
    this.props_ = new Map<string, ObservedPropertyAbstract<any>>();

    Environment.EnvBackend_.onValueChanged(this.onValueChanged.bind(this));
  }

  private envProp<S>(key: string, value: S): boolean {
    let prop = AppStorage.Prop(key);
    if (prop) {
      console.warn(`Environment: envProp '${key}': Property already exists in AppStorage. Not using environment property.`);
      return false;
    }
    let tmp;
    switch(key) {
      case "accessibilityEnabled":
        tmp = Environment.EnvBackend_ .getAccessibilityEnabled();
        break;
      case "colorMode":
        tmp = Environment.EnvBackend_ .getColorMode();
        break;
      case "fontScale":
        tmp = Environment.EnvBackend_ .getFontScale();
        break;
      case "fontWeightScale":
        tmp = Environment.EnvBackend_ .getFontWeightScale().toFixed(2);
        break;
      case "layoutDirection":
        tmp = Environment.EnvBackend_ .getLayoutDirection();
        break;
      case "languageCode":
        tmp = Environment.EnvBackend_ .getLanguageCode();
        break;
      default:
        tmp = value;
    }
    prop = AppStorage.SetAndProp(key, tmp);
    this.props_.set(key, prop);
    console.debug(`Environment: envProp for '${key}' done.`);
  }

  private envProps(properties: {
    key: string,
    defaultValue: any
  }[]): void {
    properties.forEach(property => {
      this.envProp(property.key, property.defaultValue);
      console.debug(`Environment: envProps for '${property.key}' done.`);
    });
  }

  keys(): Array<string> {
    let result = [];
    const it = this.props_.keys();
    let val = it.next();

    while (!val.done) {
      result.push(val.value);
      val = it.next();
    }

    return result;
  }

  onValueChanged(key: string, value: any): void {
    let ok = AppStorage.Set(key, value);
    if (ok) {
      console.debug(`Environment: onValueChanged: ${key} changed to ${value}`);
    } else {
      console.warn(`Environment: onValueChanged: error changing ${key}! See results above.`);
    }
  }

  aboutToBeDeleted() {
    this.props_.forEach((val, key, map) => {
      val.aboutToBeDeleted();
      AppStorage.Delete(key);
    });
  }
}
