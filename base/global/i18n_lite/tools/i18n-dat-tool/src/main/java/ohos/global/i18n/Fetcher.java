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

import com.ibm.icu.text.DateFormatSymbols;
import com.ibm.icu.text.DateTimePatternGenerator;
import com.ibm.icu.text.DecimalFormatSymbols;
import com.ibm.icu.text.NumberFormat;
import com.ibm.icu.text.NumberingSystem;
import com.ibm.icu.util.ULocale;
import com.ibm.icu.text.DateFormat;
import com.ibm.icu.text.SimpleDateFormat;
import com.ibm.icu.util.Calendar;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Objects;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import ohos.global.i18n.ResourceConfiguration.ConfigItem;
import ohos.global.i18n.ResourceConfiguration.Element;

/**
 * Fetcher is used to fetche a locale's specified data
 */
public class Fetcher implements Runnable, Comparable<Fetcher> {
    /** configuration extracted from resourec_items.json */
    private static ArrayList<ConfigItem> configItems = null;
    private static final Logger LOG = Logger.getLogger("Fetcher");
    private static int resourceCount = 0;
    private static HashMap<Integer, String> int2Str = new HashMap<>();
    private static HashMap<String, Integer> str2Int = new HashMap<>();
    private static boolean sStatusOk = true;

    /** Used to store data related to a locale */
    public ArrayList<String> datas = new ArrayList<>();

    /** All non-repeated strings will be put into idMap */
    public Map<String, Integer> idMap;

    /** Indicate whether this Fetcher is included in the final generation process of i18n.dat file */
    public boolean included = true;

    /** LanguageTag related to the locale */
    public String languageTag;

    private String lan; // language
    private ReentrantLock lock; // Lock used to synchronize dump operation
    private ULocale locale;
    private DateFormatSymbols formatSymbols;
    private DateTimePatternGenerator patternGenerator;
    private int status = 0;
    private String defaultHourString;

    static {
        configItems = ResourceConfiguration.parse();
        configItems.sort((ConfigItem first, ConfigItem second) -> first.getIndex() - second.getIndex());
        resourceCount = configItems.size();
    }

    /**
     * show whether resouce_items is loaded successfully
     *
     * @return true if status is right, otherwise false
     */
    public static boolean isFetcherStatusOk() {
        return sStatusOk;
    }

    /**
     * return the total resource number
     *
     * @return resourceCount
     */
    public static int getResourceCount() {
        return resourceCount;
    }

    /**
     * Methods to get int2Str
     *
     * @return Return int2Str
     */
    public static HashMap<Integer, String> getInt2Str() {
        return int2Str;
    }

    /**
     * Methods to get str2Int
     *
     * @return Return str2Int
     */
    public static HashMap<String, Integer> getStr2Int() {
        return str2Int;
    }

    public Fetcher(String tag, ReentrantLock lock, Map<String, Integer> idMap) {
        if (!Utils.isValidLanguageTag(tag)) {
            LOG.log(Level.SEVERE, String.format("wrong languageTag %s", tag));
            status = 1;
        }
        this.languageTag = tag;
        Objects.requireNonNull(lock);
        this.lock = lock;
        Objects.requireNonNull(idMap);
        this.idMap = idMap;
        this.lan = this.languageTag.split("-")[0];
        this.locale = ULocale.forLanguageTag(this.languageTag);
        formatSymbols = DateFormatSymbols.getInstance(locale);
        patternGenerator = DateTimePatternGenerator.getInstance(locale);
        defaultHourString = defaultHour();
    }

    /**
     * Check the status of the fetcher, normally a wrong language tag
     * can make the status wrong.
     *
     * @return the status
     */
    public boolean checkStatus() {
        return status == 0;
    }

    /**
     * Get all meta data defined in resource_items.json
     */
    public void getData() {
        int current = 0;
        Method method = null;
        for (ConfigItem item : configItems) {
            int index = item.getIndex();
            if (current != index) {
                throw new IllegalStateException();
            }
            String methodString = item.getMethod();
            try {
                method = Fetcher.class.getDeclaredMethod(methodString, ConfigItem.class);
                method.setAccessible(true);
                method.invoke(this, item);
            } catch(IllegalAccessException | NoSuchMethodException | InvocationTargetException e) {
                LOG.severe("get data failed for index " + current);
            }
            ++current;
        }
    }

    /**
     * Dump all datas in this locale to idMap
     */
    public void dump() {
        try {
            lock.lock();
            int size = this.idMap.size();
            for (int i = 0; i < datas.size(); i++) {
                String data = datas.get(i);
                if (!idMap.containsKey(data)) {
                    idMap.put(data, size);
                    size++;
                }
            }
        } finally {
            lock.unlock();
        }
    }

    /**
     * Equals function to determin whether two objs are equal
     *
     * @param obj Object to be compared
     * @return Return true if obj is equals to this Fetcher object, otherwise false
     */
    public boolean equals(Object obj) {
        if (!(obj instanceof Fetcher)) {
            return false;
        }
        Fetcher fetcher = (Fetcher) obj;
        if (datas.size() != fetcher.datas.size()) {
            return false;
        }
        for (int i = 0; i < datas.size(); i++) {
            if (!datas.get(i).equals(fetcher.datas.get(i))) {
                return false;
            }
        }
        return true;
    }

    /**
     * Returns hashcode
     *
     * @return HashCode of Fetcher object
     */
    @Override
    public int hashCode() {
        return datas.hashCode() + languageTag.hashCode();
    }

    /**
     * Override methods in Runnable
     */
    public void run() {
        getData();
        dump();
    }

    private String convertNoAscii(String str) {
        return str;
    }

    // Get month names
    private void getMonthNames(int formatType, int lengthType) {
        StringBuilder sb = new StringBuilder();
        String[] months = formatSymbols.getMonths(formatType, lengthType);
        for (int i = 0; i < months.length; i++) {
            sb.append(months[i]);
            if (i != months.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        datas.add(sb.toString());
    }

    // Get weekday names
    private void getWeekDayNames(int formatType, int lengthType) {
        StringBuilder sb = new StringBuilder();
        String[] weekdays = formatSymbols.getWeekdays(formatType, lengthType);
        String[] adjustWeekdays = new String[(weekdays.length - 1)];
        for (int i = 0; i < adjustWeekdays.length; i++) {
            adjustWeekdays[i] = weekdays[i + 1];
        }
        for (int i = 0; i < adjustWeekdays.length; i++) {
            sb.append(adjustWeekdays[i]);
            if (i != adjustWeekdays.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        this.datas.add(sb.toString());
    }

    private void getPatterns(ConfigItem config) {
        if (config.elements == null) {
            throw new IllegalArgumentException("no patterns defined in resource_items.json for index: " + config.index);
        }
        Element[] elements = config.elements;
        int current = 0;
        ArrayList<String> skeletons = new ArrayList<String>();
        for (Element ele : elements) {
            int index = ele.index;
            if (current != index) {
                throw new IllegalStateException("wrong index order in patterns for index: " + config.index);
            }
            ++current;
            skeletons.add(ele.skeleton);
        }
        StringBuilder sb = new StringBuilder();
        String[] outPatterns = new String[skeletons.size()];
        processPatterns(outPatterns, skeletons);
        for (int i = 0; i < skeletons.size(); i++) {
            sb.append(outPatterns[i]);
            if (i != outPatterns.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        datas.add(sb.toString());
    }

    private void processPatterns(String[] outPatterns, ArrayList<String> skeletons) {
        for (int i = 0; i < skeletons.size(); ++i) {
            switch (skeletons.get(i)) {
                case "FULL":
                case "MEDIUM":
                case "SHORT": {
                    outPatterns[i] = getFMSPattern(skeletons.get(i));
                    break;
                }
                default: {
                    processSpecialPattern(outPatterns, skeletons, i);
                }
            }
        }
    }

    private void processSpecialPattern(String[] outPatterns, ArrayList<String> skeletons, int i) {
        if ("en-US".equals(languageTag) && ("Ed".equals(skeletons.get(i)))) {
            outPatterns[i] = "EEE d";
            return;
        }
        if ("jm".equals(skeletons.get(i))) {
            if ("h".equals(defaultHourString)) {
                outPatterns[i] = patternGenerator.getBestPattern("hm");
            } else {
                outPatterns[i] = patternGenerator.getBestPattern("Hm");
            }
            return;
        }
        if ("jms".equals(skeletons.get(i))) {
            if ("h".equals(defaultHourString)) {
                outPatterns[i] = patternGenerator.getBestPattern("hms");
            } else {
                outPatterns[i] = patternGenerator.getBestPattern("Hms");
            }
            return;
        }
        outPatterns[i] = patternGenerator.getBestPattern(skeletons.get(i));
    }

    // Get FULL-MEDIUM_SHORT pattern
    private String getFMSPattern(String skeleton) {
        DateFormat formatter = null;
        try {
            Field patternField = DateFormat.class.getField(skeleton);
            int patternIndex = patternField.getInt(null);
            formatter = DateFormat.getDateInstance(patternIndex, locale);
        } catch (NoSuchFieldException | IllegalArgumentException | IllegalAccessException e ) {
            LOG.log(Level.SEVERE, "cannot get field " + skeleton);
        }
        if (formatter instanceof SimpleDateFormat) {
            return ((SimpleDateFormat)formatter).toPattern();
        } else {
            LOG.log(Level.SEVERE, "wrong type in getFMSPattern");
            return "";
        }
    }

    // 0. get format abbreviated month names
    private void getFormatAbbrMonthNames(ConfigItem config) {
        getMonthNames(DateFormatSymbols.FORMAT, DateFormatSymbols.ABBREVIATED);
    }

    // 1. get format abbreviated day names
    private void getFormatAbbrDayNames(ConfigItem config) {
        getWeekDayNames(DateFormatSymbols.FORMAT, DateFormatSymbols.ABBREVIATED);
    }

    // 4. get am pm markser
    private void getAmPmMarkers(ConfigItem config) {
        StringBuilder sb = new StringBuilder();
        String[] amPmStrings = formatSymbols.getAmPmStrings();
        for (int i = 0; i < amPmStrings.length; ++i) {
            sb.append(amPmStrings[i]);
            if (i != amPmStrings.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        this.datas.add(sb.toString());
    }

    // 5. get plural data
    private void getPluralRules(ConfigItem config) {
        String str = PluralFetcher.getInstance().get(this.lan);
        if (str == null) {
            str = "";
        }
        this.datas.add(str);
    }

    private void getDecimalPluralRules(ConfigItem config) {
        String str = PluralFetcher.getInstance().getDecimal(this.lan);
        if (str == null) {
            str = "";
        }
        this.datas.add(str);
    }

    // 6. get number format data
    @SuppressWarnings("Deprecation")
    private void getNumberFormat(ConfigItem config) {
        String pattern = NumberFormat.getPatternForStyle(locale, NumberFormat.NUMBERSTYLE);
        String percentPattern = NumberFormat.getPatternForStyle(locale, NumberFormat.PERCENTSTYLE);
        DecimalFormatSymbols decimalFormatSymbols = new DecimalFormatSymbols(locale);
        String percent = decimalFormatSymbols.getPercentString();
        String  groupingSeparator = decimalFormatSymbols.getGroupingSeparatorString();
        String decimalSeparator = decimalFormatSymbols.getDecimalSeparatorString();
        StringBuilder sb = new StringBuilder();
        sb.append(pattern);
        sb.append(FileConfig.SEP);
        sb.append(percentPattern);
        sb.append(FileConfig.SEP);
        sb.append(convertNoAscii(decimalSeparator));
        sb.append(FileConfig.SEP);
        sb.append(convertNoAscii(groupingSeparator));
        sb.append(FileConfig.SEP);
        sb.append(convertNoAscii(percent));
        datas.add(sb.toString());
    }

    // 7. get number digits
    private void getNumberDigits(ConfigItem config) {
        NumberingSystem numberSystem = NumberingSystem.getInstance(locale);
        String description = numberSystem.getDescription();
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < description.length(); i++) {
            sb.append(String.valueOf(description.charAt(i)));
            if (i != description.length() - 1) {
                sb.append(FileConfig.NUMBER_SEP);
            }
        }
        datas.add(sb.toString());
    }

    // 8. get time separtor
    @SuppressWarnings("Deprecation")
    private void getTimeSeparator(ConfigItem config) {
        datas.add(formatSymbols.getTimeSeparatorString());
    }

    // 9. get default hour
    private void getDefaultHour(ConfigItem config) {
        datas.add(defaultHourString);
    }

    // 10.get standalone abbreviated month
    private void getStandAloneAbbrMonthNames(ConfigItem config) {
        getMonthNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.ABBREVIATED);
    }

    // 11. get standalone abbreviated weekday
    private void getStandAloneAbbrWeekDayNames(ConfigItem config) {
        getWeekDayNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.ABBREVIATED);
    }

    // 12. get format wide month
    private void getFormatWideMonthNames(ConfigItem config) {
        getMonthNames(DateFormatSymbols.FORMAT, DateFormatSymbols.WIDE);
    }

    // 13. get format wide days
    private void getFormatWideWeekDayNames(ConfigItem config) {
        getWeekDayNames(DateFormatSymbols.FORMAT, DateFormatSymbols.WIDE);
    }

    // 14. get standalone wide days
    private void getStandAloneWideWeekDayNames(ConfigItem config) {
        getWeekDayNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.WIDE);
    }

    // 15. get standalone wide month
    private void getStandAloneWideMonthNames(ConfigItem config) {
        getMonthNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.WIDE);
    }

    private String defaultHour() {
        DateFormat tempFormat = DateFormat
            .getTimeInstance(DateFormat.SHORT, ULocale.forLanguageTag(languageTag));
        SimpleDateFormat timeInstance = null;
        if (tempFormat instanceof SimpleDateFormat) {
            timeInstance = (SimpleDateFormat) tempFormat;
        }
        String shortDateTimePattern = (timeInstance == null) ? "" : timeInstance.toPattern();
        if (shortDateTimePattern.contains("H")) {
            return "H";
        } else {
            return "h";
        }
    }

    private void getWeekdata(ConfigItem config) {
        Calendar cal = Calendar.getInstance(ULocale.forLanguageTag(languageTag));
        Calendar.WeekData weekdata = cal.getWeekData();
        StringBuilder sb = new StringBuilder();
        sb.append(weekdata.firstDayOfWeek);
        sb.append(FileConfig.NUMBER_SEP);
        sb.append(weekdata.minimalDaysInFirstWeek);
        sb.append(FileConfig.NUMBER_SEP);
        sb.append(weekdata.weekendOnset);
        sb.append(FileConfig.NUMBER_SEP);
        sb.append(weekdata.weekendCease);
        datas.add(sb.toString());
    }

    private void getMinusSign(ConfigItem config) {
        NumberFormat formatter = NumberFormat.getNumberInstance(locale);
        String formatValue = formatter.format(-1);
        NumberingSystem numberSystem = NumberingSystem.getInstance(locale);
        String description = numberSystem.getDescription();
        String temp = formatValue.substring(0, formatValue.indexOf(description.charAt(1)));
        datas.add(temp);
    }

    public @Override int compareTo(Fetcher other) {
        if (languageTag == null && other.languageTag == null) {
            return 0;
        }
        if (languageTag == null) {
            return -1;
        } else if (other.languageTag == null) {
            return 1;
        } else {
            return languageTag.compareTo(other.languageTag);
        }
    }
}
