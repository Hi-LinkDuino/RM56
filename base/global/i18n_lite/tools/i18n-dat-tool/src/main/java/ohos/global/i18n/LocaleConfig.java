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

package ohos.global.i18n;

/**
 * Represents a meta data config in dat file
 */
public class LocaleConfig {
    /** Meata data's name for example calendar/gregorian/monthNames/format/abbreviated */
    public String name;

    /** Meta data's index in str2Int */
    public int nameId;

    /** Meta data's index in sIdMap */
    public int stringId;

    public LocaleConfig(String str, int nameId, int stringId) {
        this.name = str;
        this.nameId = nameId;
        this.stringId = stringId;
    }
}
