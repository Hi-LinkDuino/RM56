/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import BaseModel from '../model/BaseModel.ets';
import LogUtil from './LogUtil.ets';
import Storage from '@ohos.data.storage';

const PREFERENCES_PATH = '/data/accounts/account_0/appdata/com.ohos.wifidirectdemo/sharedPreference/WifiDirectPreference';

let mPreferences = null;
export class StorageUtil extends BaseModel {
    constructor() {
        super();
        mPreferences = Storage.getStorageSync(PREFERENCES_PATH);
    }

    isPreferencesExist(key) {
        if (mPreferences && mPreferences.hasSync(key)) {
            return true;
        }
        return false;
    }
    /**
     * 获取数据并指定默认
     */
    getDataToDef(key, def) {
        let data;
        if (mPreferences && mPreferences.hasSync(key)) {
            data = mPreferences.getSync(key, def);
        } else {
            data = def;
        }
        LogUtil.info('getDataToDef  key == ' + key + '  data  ==  ' + data);
        return data;
    }
    /**
    * 同步保存原始数据
    */
    putObjData(key, value) {
        LogUtil.info('putObjData  key == ' + key + '  value  ==  ' + value);
        mPreferences.putSync(key, value);
        mPreferences.flush();
    }
    /**
     * 删除指定键数据
     */
    delObjData(key) {
        LogUtil.info('delObjData  key == ' + key);
        mPreferences.delete(key);
    }
}

let mStorageUtil = new StorageUtil();

export default mStorageUtil as StorageUtil;
