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
 * Ability info.
 *
 */
public class AbilityInfo {
    /**
     * Indicates the name of ability.
     */
    public String name = "";

    /**
     * Indicates the description of ability.
     */
    public String description = "";

    /**
     * Indicates the description resource of ability.
     */
    public String descriptionRes = "";

    /**
     * Indicates the icon of ability.
     */
    public String icon = "";

    /**
     * Indicates the icon path of ability.
     */
    public String iconPath = "";

    /**
     * Indicates the label of ability.
     */
    public String label = "";

    /**
     * Indicates the label resource of ability.
     */
    public String labelRes = "";

    /**
     * Indicates the type of ability.
     */
    public String type = "";

    /**
     * Indicates the formEnabled of ability.
     */
    public boolean formEnabled = false;

    /**
     * Indicates the formInfo of ability.
     */
    public FormInfo formInfo = null;

    /**
     * Indicates the uri of ability.
     */
    public String uri = "";

    /**
     * Indicates the launchType of ability.
     */
    public String launchType = "";

    /**
     * Indicates the metaData of ability.
     */
    public MetaData metaData = new MetaData();

    /**
     * Indicates the orientation of ability.
     */
    public String orientation = "";

    /**
     * Indicates the permissions of ability.
     */
    public List<String> permissions = new ArrayList<String>();

    /**
     * Indicates the skills of ability.
     */
    public List<SkillInfo> skills = new ArrayList<SkillInfo>();

    /**
     * Indicates the backgroundModes of ability.
     */
    public List<String> backgroundModes = new ArrayList<String>();

    /**
     * Indicates the visible of ability.
     */
    public boolean visible = false;

    /**
     * Indicates the grantPermission of ability.
     */
    public boolean grantPermission = false;

    /**
     * Indicates the readPermission of ability.
     */
    public String readPermission = "";

    /**
     * Indicates the writePermission of ability.
     */
    public String writePermission = "";

    /**
     * Indicates the uriPermission mode of ability.
     */
    public String uriPermissionMode = "";

    /**
     * Indicates the uriPermission path of ability.
     */
    public String uriPermissionPath = "";

    /**
     * Indicates the configChanges of ability.
     */
    public List<String> configChanges = new ArrayList<String>();

    /**
     * Indicates the directLaunch of ability.
     */
    public boolean directLaunch = false;

    /**
     * Indicates the mission of ability.
     */
    public String mission = "";

    /**
     * Indicates the targetAbility of ability.
     */
    public String targetAbility = "";

    /**
     * Indicates the multiUserShared of ability.
     */
    public boolean multiUserShared = false;

    /**
     * Indicates the supportPipMode of ability.
     */
    public boolean supportPipMode = false;

    /**
     * Indicates the forms of ability.
     */
    public List<AbilityFormInfo> formInfos = new ArrayList<AbilityFormInfo>();

    /**
     * get the customize Data value defined in this ability.
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