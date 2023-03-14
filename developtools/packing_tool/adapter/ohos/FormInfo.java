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
 * Form info.
 *
 */
public class FormInfo {
    /**
     * Indicates the formEntity of formInfo.
     */
    public List<String> formEntity = new ArrayList<String>();

    /**
     * Indicates the minHeight of formInfo.
     */
    public String minHeight = "";

    /**
     * Indicates the defaultHeight of formInfo.
     */
    public String defaultHeight = "";

    /**
     * Indicates the minWidth of formInfo.
     */
    public String minWidth = "";

    /**
     * Indicates the defaultWidth of formInfo.
     */
    public String defaultWidth = "";
}