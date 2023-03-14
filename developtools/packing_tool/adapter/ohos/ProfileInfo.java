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

package ohos;

import java.util.HashMap;
import java.util.Map;

/**
 * App Profile info.
 *
 */
public class ProfileInfo {
    /**
     * Indicates the file name of the hap.
     */
    public String hapName = "";

    /**
     * Indicates the appInfo of app.
     */
    public AppInfo appInfo = null;

    /**
     * Indicates the deviceConfig of app.
     */
    public Map<String, DeviceConfig> deviceConfig = new HashMap<>();

    /**
     * Indicates the hapInfo of app.
     */
    public HapInfo hapInfo = null;
}
