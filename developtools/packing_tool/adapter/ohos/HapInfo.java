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

import java.util.ArrayList;
import java.util.List;

/**
 * Hap info.
 *
 */
public class HapInfo {
    /**
     * Indicates the package of HapInfo.
     */
    public String packageStr = "";

    /**
     * Indicates the name of HapInfo.
     */
    public String name = "";

    /**
     * Indicates the description of HapInfo.
     */
    public String description = "";

    /**
     * Indicates the supportedModes of HapInfo.
     */
    public List<String> supportedModes = new ArrayList<String>();

    /**
     * Indicates the abilities of HapInfo.
     */
    public List<AbilityInfo> abilities = new ArrayList<AbilityInfo>();

    /**
     * Indicates the defPermissions of HapInfo.
     */
    public List<DefPermission> defPermissions = new ArrayList<DefPermission>();

    /**
     * Indicates the defPermissionsGroups of HapInfo.
     */
    public List<DefPermissionGroup> defPermissionsGroups = new ArrayList<DefPermissionGroup>();

    /**
     * Indicates the distro of HapInfo.
     */
    public Distro distro = null;

    /**
     * Indicates the reqCapabilities of HapInfo.
     */
    public List<String> reqCapabilities = new ArrayList<String>();

    /**
     * Indicates the deviceType of HapInfo.
     */
    public List<String> deviceType = new ArrayList<String>();

    /**
     * Indicates the metaData of HapInfo.
     */
    public MetaData metaData = new MetaData();

    /**
     * Indicates the HapInfo is Js app.
     */
    public boolean isJs = false;

    /**
     * Indicates the reqPermissions of HapInfo.
     */
    public List<ReqPermission> reqPermissions = new ArrayList<ReqPermission>();

    /**
     * Indicates the commonEvents of HapInfo.
     */
    public List<CommonEvent> commonEvents = new ArrayList<CommonEvent>();

    /**
     * Indicates the shortcuts of HapInfo.
     */
    public List<Shortcut> shortcuts = new ArrayList<Shortcut>();

    /**
     * Indicates the DistroFilter of HapInfo
     */
    public DistroFilter distroFilter = new DistroFilter();

    /**
     * get the customize Data value defined in this module.
     */
    public String getCustomizeDataValue(String customizeDataName) {
        for (CustomizeData data : metaData.customizeDatas) {
            if (customizeDataName.equals(data.name)) {
                return data.value;
            }
        }
        return "";
    }
}
