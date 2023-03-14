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

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.logging.Logger;
import java.util.logging.Level;

import com.ibm.icu.util.ULocale;

/**
 * Represent the mask value of a locale
 */
public class LocaleMaskItem implements Comparable<LocaleMaskItem> {
    private static final Logger logger = Logger.getLogger("LocaleMaskItem");
    private String languageTag;
    private String maskString;
    private long mask;
    private int rank = -1; // Mask index
    private int offset = -1;
    private ArrayList<LocaleConfig> configs;

    public LocaleMaskItem(String languageTag, ArrayList<LocaleConfig> configs) {
        this.languageTag = languageTag;
        this.configs = configs;
        configs.sort(new Comparator<LocaleConfig>() {
            @Override
            public int compare(LocaleConfig first, LocaleConfig second) {
                if (first.nameId ==  second.nameId) {
                    return 0;
                } else if (first.nameId < second.nameId) {
                    return -1;
                } else {
                    return 1;
                }
            }
        });
        try {
            ULocale locale = ULocale.forLanguageTag(languageTag);
            long[] temp = new long[1];
            this.maskString = Utils.getMask(locale, temp);
            this.mask = temp[0];
        } catch (UnsupportedEncodingException e) {
            logger.log(Level.SEVERE, "Get mask failed");
        }
    }


    public int getRank() {
        return rank;
    }

    public String getLanguageTag() {
        return languageTag;
    }

    public long getMask() {
        return mask;
    }

    public String getMaskString() {
        return maskString;
    }

    public void setRank(int rank) {
        this.rank = rank;
    }

    public void setOffset(int offset) {
        this.offset = offset;
    }

    public int getOffset() {
        return offset;
    }

    public ArrayList<LocaleConfig> getConfigs() {
        return configs;
    }

    @Override
    public int compareTo(LocaleMaskItem another) {
        if (mask == another.getMask()) {
            return 0;
        } else if (mask < another.getMask()) {
            return -1;
        } else {
            return 1;
        }
    }
}
