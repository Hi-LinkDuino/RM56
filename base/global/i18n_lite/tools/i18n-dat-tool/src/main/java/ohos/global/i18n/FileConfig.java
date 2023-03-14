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
 * This class encapsulate some configurations
 */
public class FileConfig {
    /**
     * Every config needs 6 bytes
     */
    public static final int CONFIG_SIZE = 6;

    /**
     * Version number
     */
    public static final int FILE_VERSION = 1;
    /**
     * Dat file's header size is 16 bytes
     */
    public static final int HEADER_SIZE = 16;
    /**
     * Every locale need 8 bytes
     */
    public static final int LOCALE_MASK_ITEM_SIZE = 8;
    /**
     * Locales mask is a 4-byte-length unsigned integer
     */
    public static final int LOCALE_MASK_SIZE = 4;
    /**
     * Separator used to separate numbers
     */
    public static final String NUMBER_SEP = ";";
    /**
     * Separator used to separate (except numbers)
     */
    public static final String SEP = "_";
}
