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
 * Skill info.
 *
 */
public class SkillInfo {
    /**
     * Indicates the actions of SkillInfo.
     */
    public List<String> actions = new ArrayList<String>();

    /**
     * Indicates the entities of SkillInfo.
     */
    public List<String> entities = new ArrayList<String>();

    /**
     * Indicates the uris of SkillInfo.
     */
    public List<String> uris = new ArrayList<String>();
}