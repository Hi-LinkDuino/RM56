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

import com.ibm.icu.util.ULocale;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Iterator;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import ohos.global.i18n.ResourceConfiguration.ConfigItem;
import ohos.global.i18n.ResourceConfiguration.Element;

/**
 * utils class.
 */
public class Utils {
    private Utils() {}

    private static final String AVAILABLE_LINE = "enum AvailableDateTimeFormatPattern {";
    private static final String AVAILABLE_END_LINE = "};";
    private static final int TYPE_SHIFT = 16;
    private static final String PATTERN_INDEX_MASK = "#define PATTERN_INDEX_MASK = 0x0000ffff";
    private static final String I18N_MACROS_BEGIN = "// this file should only be included by date_time_format_impl.cpp";
    private static final int MAX_CASE_NUMBER = 14;

    /**
     * Get a locale's fallback, locale is specified with languageTag
     *
     * @param languageTag Use this languageTag to compute the fallback
     * @return Fallback languageTag
     */
    public static String getFallback(String languageTag) {
        if ("".equals(languageTag)) {
            return "en-US";
        }
        String[] split = languageTag.split("-");
        if ("en-US".equals(languageTag) || split.length == 1) {
            return "en-US";
        }
        if (split.length != 2) {
            return split[0] + "-" + split[1];
        }
        if ((split[1].length() != 4) && (!"en".equals(split[0]))) {
            return split[0];
        }
        return "en-US";
    }

    /**
     * Determines whether a languageTag is valid.
     *
     * @param languageTag tag to be checked
     * @return returns true if languageTag is valid, otherwise false.
     */
    public static boolean isValidLanguageTag(String languageTag) {
        if (languageTag == null) {
            return false;
        }
        String[] items = languageTag.split("-");
        switch (items.length) {
            case 1: {
                return checkLanguage(items[0]);
            }
            case 2: {
                if (!checkLanguage(items[0])) {
                    return false;
                }
                // script
                if (items[1].length() == 4) {
                    if (checkScript(items[1])) {
                        return true;
                    }
                } else if (items[1].length() == 2) {
                    if (checkRegion(items[1])) {
                        return true;
                    }
                } else {
                    return false;
                }
                return false;
            }
            case 3: {
                return checkLanguage(items[0]) && checkScript(items[1]) && checkRegion(items[2]);
            }
            default: {
                return false;
            }
        }
    }

    private static boolean checkLanguage(String lan) {
        if (lan == null) {
            return false;
        }
        int length = lan.length();
        if (length > 3 || length < 2) {
            return false;
        }
        for (int i = 0; i < length; ++i) {
            if ((int) lan.charAt(i) > 255) {
                return false;
            }
        }
        return true;
    }

    // script is a 4 character string, started with a uppercase letter
    private static boolean checkScript(String script) {
        int length = script.length();
        if (length != 4) {
            return false;
        }
        for (int i = 0; i < length; ++i) {
            if (i == 0 ) {
                if (!Character.isUpperCase(script.charAt(0))) {
                    return false;
                }
            } else {
                char cur = script.charAt(i);
                if ('a' > cur || 'z' < cur) {
                    return false;
                }
            }
        }
        return true;
    }

    private static boolean checkRegion(String region) {
        int length = region.length();
        if (length != 2) {
            return false;
        }
        for (int i = 0; i < length; ++i) {
            char cur = region.charAt(i);
            if ('A' > cur || 'Z' < cur) {
                return false;
            }
        }
        return true;
    }

    /**
     * Write i18n.dat's Header to DataOutputStream
     *
     * @param out data will be writen into the stream
     * @param hashCode reserved for future use
     * @param localesCount valid locales in total
     * @param metaCount all metaData in total
     * @throws IOException
     */
    public static void writeHeader(DataOutputStream out, int hashCode, int localesCount,
        int metaCount) throws IOException {
        out.writeInt(hashCode); // reserved hashcode
        out.writeByte(FileConfig.FILE_VERSION);
        out.writeByte(0); // reserved
        out.writeChar(0);
        out.writeChar(0); // reserved
        out.writeChar(FileConfig.HEADER_SIZE + 8 * localesCount);
        out.writeChar(localesCount);
        out.writeChar(FileConfig.HEADER_SIZE + 8 * localesCount  + metaCount * 6);
        out.flush();
    }

    /**
     * Get mask of a locale
     *
     * @param locale Indicates the specified locale related to the output mask
     * @param maskOut The value of mask will be stored in the first element of maskOut
     * @return The text representation of mask in hex format
     * @throws UnsupportedEncodingException if getBytes function failed
     */
    public static String getMask(ULocale locale, long[] maskOut) throws UnsupportedEncodingException {
        long mask = 0;
        byte[] langs;
        // Deal with "fil" and "mai" these 3-leters language
        if ("fil".equals(locale.getLanguage())) {
            langs = "tl".getBytes("utf-8");
        } else if ("mai".equals(locale.getLanguage())) {
            langs = "md".getBytes("utf-8");
        } else {
            langs = locale.getLanguage().getBytes("utf-8");
        }
        mask = mask | ((long)(langs[0] - 48)) << 25 | ((long)(langs[1] - 48)) << 18;
        int temp = 0;
        if ("Latn".equals(locale.getScript())) {
            temp = 1;
        } else if ("Hans".equals(locale.getScript())) {
            temp = 2;
        } else if ("Hant".equals(locale.getScript())) {
            temp = 3;
        } else if ("Qaag".equals(locale.getScript())) {
            temp = 4;
        } else if ("Cyrl".equals(locale.getScript())) {
            temp = 5;
        } else if ("Deva".equals(locale.getScript())) {
            temp = 6;
        } else {
            temp = "Guru".equals(locale.getScript()) ? 7 : 0;
        }
        mask = mask | ((long)temp << 14);
        if (locale.getCountry() != null && locale.getCountry().length() == 2) {
            byte[] ret = locale.getCountry().getBytes("utf-8");
            mask = mask | ((long) (ret[0] - 48) << 7) | ((long)(ret[1] - 48));
        }
        maskOut[0] = mask;
        String ret = "0x" + Long.toHexString(mask);
        return ret;
    }

    /**
     * Generate the types.h in interfaces
     *
     * @param src the original types.h file
     * @param dst the generated types.h file
     * @param configItems ConfigItems extracted from resource_items.json
     */
    public static void generateTypesFile(File src, File dst, ArrayList<ConfigItem> configItems) {
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(src)));
            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(dst)))) {
            String line = null;
            boolean found = false;
            while ((line = reader.readLine()) != null) {
                if (!found) {
                    writer.write(line + System.lineSeparator());
                }
                if (AVAILABLE_LINE.equals(line)) {
                    found = true;
                    writer.write(generateAvailableDateTimeFormatPattern(configItems));
                    continue;
                }
                if (found && AVAILABLE_END_LINE.equals(line)) {
                    writer.write(line + System.lineSeparator());
                    found = false;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static String generateAvailableDateTimeFormatPattern(ArrayList<ConfigItem> configItems) {
        StringBuilder sb = new StringBuilder();
        ArrayList<Element> adjust = new ArrayList<>();
        for (ConfigItem item : configItems) {
            if ("true".equals(item.pub) && item.elements != null) {
                for (Element ele : item.elements) {
                    adjust.add(ele);
                }
            }
        }
        adjust.sort(new Comparator<Element>() {
            @Override
            public int compare(Element first, Element second) {
                if (first.enumIndex < second.enumIndex) {
                    return -1;
                } else if (first.enumIndex > second.enumIndex) {
                    return 1;
                } else {
                    return 0;
                }
            }
        });
        for (int i = 0; i < adjust.size(); ++i) {
            sb.append("\t");
            sb.append(adjust.get(i).getAvailableFormat());
            if (i != adjust.size() - 1) {
                sb.append(",");
            }
            sb.append(System.lineSeparator());
        }
        return sb.toString();
    }

    private static String generateI18nPatternMacros(ArrayList<ConfigItem> configItems) {
        StringBuilder sb = new StringBuilder();
        ArrayList<ConfigItem> adjust = new ArrayList<>();
        for (ConfigItem item : configItems) {
            if (item.elements != null) {
                adjust.add(item);
            }
        }
        adjust.sort(new Comparator<ConfigItem>() {
            @Override
            public int compare(ConfigItem first, ConfigItem second) {
                if (first.index < second.index) {
                    return -1;
                } else if (first.index > second.index) {
                    return 1;
                } else {
                    return 0;
                }
            }
        });
        int current = 0;
        for (ConfigItem item : adjust) {
            int type = current++;
            int innerIndex = 0;
            for (Element ele : item.elements) {
                if (innerIndex++ != ele.index) {
                    throw new IllegalStateException("not consecutive index in resourceItem " + item.index);
                }
                sb.append("#define " + ele.getAvailableFormat() + "_INDEX " + getHexIndexString(type, ele.index) +
                    System.lineSeparator());
            }
        }
        sb.append(PATTERN_INDEX_MASK + System.lineSeparator());
        sb.append("#define TYPE_SHIFT " + TYPE_SHIFT + System.lineSeparator());
        return sb.toString();
    }

    /**
     * Generate the i18n_pattern.h in frameworks
     *
     * @param src the original i18n_pattern.h file path
     * @param dst the generated i18n_pattern.h file path
     * @param items ConfigItems extracted from resource_items.json
     */
    public static void generateI18nPatternFile(File src, File dst, ArrayList<ConfigItem> items) {
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(src)));
            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(dst)))) {
            String line = null;
            boolean found = false;
            while ((line = reader.readLine()) != null) {
                if (found && ("} // I18N".equals(line))) {
                    found = false;
                }
                if (!found) {
                    writer.write(line + System.lineSeparator());
                }
                if (I18N_MACROS_BEGIN.equals(line)) {
                    found = true;
                    writer.write(generateI18nPatternMacros(items));
                    writer.write(System.lineSeparator());
                    writer.write("namespace OHOS{" + System.lineSeparator());
                    writer.write("namespace I18N{" + System.lineSeparator());
                    writer.write(getPatternTypeEnum(items));
                    writer.write(System.lineSeparator());
                    writer.write(getGetPatternFromIndexCode(items));
                    writer.write(System.lineSeparator());
                    writer.write(getGetStringFromPattern(items));
                    continue;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static String getGetStringFromPattern(ArrayList<ConfigItem> configItems) {
        ArrayList<Element> eles = new ArrayList<>();
        for (ConfigItem item : configItems) {
            if (item.elements == null) {
                continue;
            }
            for (Element ele : item.elements) {
                if (ele.enumIndex >= 0) {
                    eles.add(ele);
                }
            }
        }
        int size = eles.size();
        // every GetStringFromPattern function can only have 14 cases;
        int functionSize = 1;
        if (size >= (MAX_CASE_NUMBER + 1)) {
            if (1 == size % MAX_CASE_NUMBER) {
                functionSize = size / MAX_CASE_NUMBER;
            } else {
                functionSize = size / MAX_CASE_NUMBER + 1;
            }
        }
        int currentFunction = 1;
        String[] temp = new String[functionSize];
        StringBuilder sb = new StringBuilder();
        while (currentFunction <= functionSize) {
            temp[currentFunction - 1] = getGetStringFromPattern(currentFunction, eles);
            ++currentFunction;
        }
        for (int i = functionSize - 1; i >= 0; --i) {
            sb.append(temp[i]);
            if (i != 0) {
                sb.append(System.lineSeparator());
            }
        }
        return sb.toString();
    }

    private static String getGetStringFromPattern(int functionIndex, ArrayList<Element> left) {
        StringBuilder sb = new StringBuilder();
        if (functionIndex == 1) {
            sb.append("std::string GetStringFromPattern(const AvailableDateTimeFormatPattern &requestPattern," +
                "const DateTimeData* const data)");
        } else {
            sb.append("std::string GetStringFromPattern" + functionIndex + "(const AvailableDateTimeFormatPattern" +
                "&requestPattern, const DateTimeData* const data)");
        }
        sb.append(System.lineSeparator() + "{" + System.lineSeparator());
        sb.append("    switch (requestPattern) {" + System.lineSeparator());
        int totalLength = 0;
        boolean hasRemainingFunction = true;
        if (left.size() <= (MAX_CASE_NUMBER + 1)) {
            totalLength = left.size();
            hasRemainingFunction = false;
        } else {
            totalLength = MAX_CASE_NUMBER;
        }
        Iterator<Element> iter = left.iterator();
        while (iter.hasNext() && (totalLength-- > 0)) {
            Element ele = iter.next();
            if (totalLength == 0 && !hasRemainingFunction) {
                sb.append("        default: {" + System.lineSeparator());
            } else {
                sb.append("        case " + ele.getAvailableFormat() + ": {" + System.lineSeparator());
            }
            sb.append("            return GetPatternFromIndex(" + ele.getAvailableFormat() + "_INDEX, data);" +
                System.lineSeparator());
            sb.append("        }" + System.lineSeparator());
            iter.remove();
        }
        if (hasRemainingFunction) {
            sb.append("        default: {" + System.lineSeparator());
            sb.append("            return GetPatternFromIndex" + (functionIndex + 1) + "(requestPattern, data);" +
                System.lineSeparator());
            sb.append("        }" + System.lineSeparator());
        }
        sb.append("    }" + System.lineSeparator());
        sb.append("}" + System.lineSeparator());
        return sb.toString();
    }

    private static String getGetPatternFromIndexCode(ArrayList<ConfigItem> configItems) {
        StringBuilder sb = new StringBuilder();
        sb.append("std::string GetPatternFromIndex(uint32_t index, const DateTimeData * const data)" +
            System.lineSeparator());
        sb.append("{" + System.lineSeparator());
        sb.append("    uint32_t type = index >> PATTERN_TYPE_SHIFT;" + System.lineSeparator());
        sb.append("    if (type > PatternType::PATTERN_TYPE_END) {" + System.lineSeparator());
        sb.append("         return \"\";" + System.lineSeparator());
        sb.append("    }" + System.lineSeparator());
        sb.append("    uint32_t ind = index & PATTERN_INDEX_MASK;" + System.lineSeparator());
        sb.append("    PatternType patternType = static_cast<PatternType>(type);" + System.lineSeparator());
        sb.append("    switch (patternType) {" + System.lineSeparator());
        ArrayList<ConfigItem> adjust = new ArrayList<>();
        for (ConfigItem item : configItems) {
            if (item.type != null) {
                adjust.add(item);
            }
        }
        for (int i = 0; i < adjust.size(); ++i) {
            if ( i != adjust.size() - 1) {
                sb.append("        case " + adjust.get(i).type + ": {" + System.lineSeparator());
            } else {
                sb.append("        default: {" + System.lineSeparator());
            }
            sb.append("            return Parse(data->" + adjust.get(i).pointer + " , ind);" + System.lineSeparator());
            sb.append("        }" + System.lineSeparator());
        }
        sb.append("    }" + System.lineSeparator());
        sb.append("}" + System.lineSeparator());
        return sb.toString();
    }

    private static String getPatternTypeEnum(ArrayList<ConfigItem> configItems) {
        StringBuilder sb = new StringBuilder();
        sb.append("enum PatternType {" + System.lineSeparator());
        sb.append("    PATTERN_TYPE_BEGIN = 0," + System.lineSeparator());
        for (int i = 0; i < configItems.size(); ++i) {
            if (configItems.get(i).type == null) {
                continue;
            }
            if ( i == 0) {
                sb.append("    " + configItems.get(i).type + " = PATTERN_TYPE_BEGIN," + System.lineSeparator());
            } else {
                sb.append("    " + configItems.get(i).type + "," + System.lineSeparator());
            }
        }
        sb.append("    PATTERN_TYPE_END" + System.lineSeparator());
        sb.append("};" + System.lineSeparator());
        return sb.toString();
    }

    private static String getHexIndexString(int type, int index) {
        if (type < 0 || index < 0) {
            return "";
        }
        return "0x" + Integer.toHexString((type << TYPE_SHIFT) + index);
    }
}
