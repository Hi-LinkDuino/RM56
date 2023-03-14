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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_DATA_ADAPTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_DATA_ADAPTER_H

#include <array>
#include <functional>
#include <map>
#include <queue>
#include <vector>

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "base/utils/string_utils.h"
#include "bridge/codec/codec_data.h"
#include "core/components/calendar/calendar_theme.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

constexpr int32_t CALENDAR_MIN_DATE_YEAR = 1970;
constexpr int32_t CALENDAR_MIN_DATE_MONTH = 1;
constexpr int32_t CALENDAR_INVALID = -1;
constexpr int32_t CALENDAR_FIRST_DAY_NUM_OF_MONTH = 1;
constexpr int32_t CALENDAR_TOTAL_MONTH_OF_YEAR = 12;
constexpr int32_t CALENDAR_CACHE_PAGE = 3;

struct CalendarMonth {
    int32_t year = CALENDAR_MIN_DATE_YEAR;
    // calendar month value is from 0 to 11
    int32_t month = CALENDAR_MIN_DATE_MONTH;

    bool operator==(const CalendarMonth& calendarMonth) const
    {
        return calendarMonth.month == month && calendarMonth.year == year;
    }
    bool operator!=(const CalendarMonth& calendarMonth) const
    {
        return calendarMonth.month != month || calendarMonth.year != year;
    }
    bool operator>(const CalendarMonth& calendarMonth) const
    {
        return year > calendarMonth.year || (year == calendarMonth.year && month > calendarMonth.month);
    }
    bool operator<(const CalendarMonth& calendarMonth) const
    {
        return year < calendarMonth.year || (year == calendarMonth.year && month < calendarMonth.month);
    }

    static CalendarMonth GetNextMonth(const CalendarMonth& calendarMonth)
    {
        CalendarMonth nextMonth = calendarMonth;
        if (calendarMonth.month + 1 >= CALENDAR_TOTAL_MONTH_OF_YEAR) {
            nextMonth.month = 0;
            ++nextMonth.year;
        } else {
            ++nextMonth.month;
        }
        return nextMonth;
    }

    static CalendarMonth GetLastMonth(const CalendarMonth& calendarMonth)
    {
        CalendarMonth lastMonth = calendarMonth;
        if (calendarMonth.month - 1 < 0) {
            lastMonth.month = CALENDAR_TOTAL_MONTH_OF_YEAR - 1;
            --lastMonth.year;
        } else {
            --lastMonth.month;
        }
        return lastMonth;
    }

    std::string ToString() const
    {
        auto json = JsonUtil::Create(true);
        json->Put("year", year);
        json->Put("month", month);
        return json->ToString();
    }
};

struct HashFunc {
    std::size_t operator()(const CalendarMonth& key) const
    {
        return ((std::hash<uint32_t>()(key.year) ^ (std::hash<uint32_t>()(key.month) << 1)) >> 1);
    }
};

enum class MonthState {
    CUR_MONTH,
    PRE_MONTH,
    NEXT_MONTH
};

struct CalendarDay {
    int32_t index = 0;
    int32_t day = 0;
    bool weekend = false;
    bool today = false;
    bool focused = false;
    bool touched = false;
    bool isFirstOfLunar = false;
    bool hasSchedule = false;
    bool markLunarDay = false;
    std::string lunarMonth;
    std::string lunarDay;
    std::string dayMark;
    std::string dayMarkValue;
    CalendarMonth month;

    static constexpr char INDEX[] { "index" };
    static constexpr char DAY[] { "day" };
    static constexpr char MONTH[] { "month" };
    static constexpr char YEAR[] { "year" };
    static constexpr char LUNAR_MONTH[] { "lunarMonth" };
    static constexpr char LUNAR_DAY[] { "lunarDay" };
    static constexpr char DAY_MARK[] { "dayMark" };
    static constexpr char DAY_MARK_VALUE[] { "dayMarkValue" };

    std::string ToString() const
    {
        auto json = JsonUtil::Create(true);
        json->Put(DAY, day);
        json->Put(INDEX, index);
        json->Put(MONTH, month.month);
        json->Put(YEAR, month.year);
        json->Put(LUNAR_MONTH, lunarMonth.c_str());
        json->Put(LUNAR_DAY, lunarDay.c_str());
        return json->ToString();
    }
};

struct CalendarDaysOfMonth {
    CalendarMonth month;
    int32_t firstDayIndex = CALENDAR_INVALID;
    int32_t lastDayIndex = CALENDAR_INVALID;
    int32_t today = CALENDAR_INVALID;
    std::vector<CalendarDay> days;
};

struct CalendarDataRequest {
    CalendarDataRequest(const CalendarMonth& month, int32_t indexOfContainer)
        : month(month), indexOfContainer(indexOfContainer)
    {}
    ~CalendarDataRequest() = default;

    int32_t selected = 0;
    CalendarMonth month;
    MonthState state = MonthState::CUR_MONTH;
    int32_t indexOfContainer = 0;
};

struct CardCalendarAttr {
    int32_t startDayOfWeek = 0;
    bool isV2Component = false;
    bool showLunar = true;
    bool showHoliday = true;
    bool needSlide = false;
    bool cardCalendar = false;
    bool listenersReady = true;
    Axis axis = Axis::HORIZONTAL;
    std::string offDays;
    std::string holidays;
    std::string workDays;
    EventMarker requestData;
    RefPtr<CalendarTheme> calendarTheme;
    TextDirection textDirection = TextDirection::LTR;
    CalendarType type { CalendarType::NORMAL };
};

struct ObtainedMonth {
    int32_t year = 0;
    int32_t month = 0;
    int32_t firstDayIndex = CALENDAR_INVALID;
    std::vector<CalendarDay> days;
};

using PendingRequestQueue = std::queue<CalendarDataRequest>;
using RequestDataCompleteCallback = std::function<void()>;

class CalendarDataChangeListener : public AceType {
    DECLARE_ACE_TYPE(CalendarDataChangeListener, AceType);

public:
    virtual void OnDataChanged(const CalendarDaysOfMonth& daysOfMonth) = 0;
    virtual void OnSelectedDay(int32_t selected) = 0;
    virtual void OnFocusChanged(bool focusStatus) = 0;
    virtual void UpdateCardCalendarAttr(CardCalendarAttr&& attr) = 0;
    virtual void OnSwiperMove() = 0;
};

struct DataAdapterAction {
    std::string bundleName;
    std::string abilityName;
    int32_t messageCode = 0;
    std::string data;

    std::string GetAction()
    {
        return "{\"element\":{\"bundleName\":\"" + bundleName +
                "\",\"name\":\"" + abilityName + "\",\"type\":1},\"code\":" +
                std::to_string(messageCode) + "}";
    }
};

struct CalendarDataAdapterAction : public DataAdapterAction {
    std::string GetData(int32_t year, int32_t month)
    {
        return "{\"year\":" + std::to_string(year) + ",\"month\":" + std::to_string(month) + "}";
    }

    std::vector<Framework::CodecData> GetAction(int32_t year, int32_t month)
    {
        std::vector<Framework::CodecData> args;
        Framework::CodecData header(DataAdapterAction::GetAction());
        args.push_back(header);
        Framework::CodecData params(GetData(year, month));
        args.push_back(params);

        return args;
    }
};

class CalendarDataAdapter : public AceType {
    DECLARE_ACE_TYPE(CalendarDataAdapter, AceType);

public:
    explicit CalendarDataAdapter(
        const CalendarDataAdapterAction& dataAdapterAction, const WeakPtr<PipelineContext>& pipelineContext);
    ~CalendarDataAdapter() override = default;

    void RegisterDataListener(const RefPtr<CalendarDataChangeListener>& listener)
    {
        allListeners_.push_back(listener);
        if (allListeners_.size() == CALENDAR_CACHE_PAGE && !calendarAttr_.listenersReady) {
            calendarAttr_.listenersReady = true;
            for (const auto& listen : allListeners_) {
                listen->UpdateCardCalendarAttr(std::move(calendarAttr_));
            }
        }
    }

    void AddPendingRequest(const CalendarMonth& month, const int32_t indexOfContainer)
    {
        requestQueue_.emplace(month, indexOfContainer);
        indexMap_[indexOfContainer] = month;
    }

    void AddPendingRequest(const CalendarDataRequest& request)
    {
        requestQueue_.emplace(request);
        indexMap_[request.indexOfContainer] = request.month;
    }

    void RequestData(const CalendarDataRequest& request);

    void NotifySelectedChanged() const
    {
        int32_t listenersSize = static_cast<int32_t>(allListeners_.size());
        if (indexOfContainer_ >= 0 && indexOfContainer_ < listenersSize) {
            auto& listener = allListeners_[indexOfContainer_];
            listener->OnSelectedDay(selectedDay_);
        }
    }

    const CalendarDay& GetToday() const
    {
        return today_;
    }

    void SetToday(const CalendarDay& today)
    {
        today_.day = today.day;
        today_.month.month = today.month.month;
        today_.month.year = today.month.year;
    }

    const std::array<std::vector<std::string>, 3>& GetCalendarCache()
    {
        return calendarCache_;
    }

    void SetSelectedChanged(int32_t selected, int32_t indexOfContainer)
    {
        selectedDay_ = selected;
        indexOfContainer_ = indexOfContainer;
    }

    void NotifyFocusChanged(bool focusStatus) const
    {
        for (const auto& listener : allListeners_) {
            listener->OnFocusChanged(focusStatus);
        }
    }

    void SetRequestDataEvent(const EventMarker& requestData)
    {
        if (requestData_ != requestData.GetData().eventId) {
            requestData_ = requestData;
            requestDataEvent_ =
                AceAsyncEvent<void(const std::string&)>::Create(requestData, pipelineContext_);
        }
    }

    void SetCardCalendar(bool cardCalendar)
    {
        cardCalendar_ = cardCalendar;
    }

    bool IsCardCalendar() const
    {
        return cardCalendar_;
    }

    void SetStartDayOfWeek(int32_t indexOfWeek)
    {
        startDayOfWeek_ = indexOfWeek;
    }

    int32_t GetStartDayOfWeek() const
    {
        return startDayOfWeek_;
    }

    static void SetCachePath(const std::string& cachePath)
    {
        cachePath_ = cachePath;
    }

    const std::array<CalendarDaysOfMonth, CALENDAR_CACHE_PAGE>& GetDayOfMonthCache()
    {
        return dayOfMonthCache_;
    }

    bool ShowLunar() const
    {
        return showLunar_;
    }

    void SetCurrentMonth(const CalendarMonth& currentMonth)
    {
        currentMonth_ = currentMonth;
    }

    void SetOffDays(const std::string& offDays)
    {
        offDays_ = offDays;
    }

    const std::string& GetWorkDays() const
    {
        return workDays_;
    }

    const std::string& GetHolidays() const
    {
        return holidays_;
    }

    bool HasMoved() const
    {
        return hasMoved_;
    }

    void SetHasMoved(bool hasMoved)
    {
        hasMoved_ = hasMoved;
    }

    void SetCrossMonth(bool isCrossMonth)
    {
        isCrossMonth_ = isCrossMonth;
    }

    bool IsCrossMonth()
    {
        return isCrossMonth_;
    }

    void ParseCardCalendarData(const std::string& source);
    void ParseCalendarData(std::queue<ObtainedMonth>&& months);
    void UpdateCardCalendarAttr(CardCalendarAttr&& attr);
    void SetOffDays(CalendarDay& dayInfo);

private:
    bool ParseData(int32_t index, const std::string& source, CalendarDaysOfMonth& result);

    void SaveCacheData(const CalendarDataRequest& request, const std::string& result);

    bool GetCacheData(const CalendarDataRequest& request);

    void ParseMonthData(const std::unique_ptr<JsonValue>& monthData);

    void RequestDataInWatch(const CalendarDataRequest& request);

    void NotifyDataChanged(const CalendarDaysOfMonth& data, int32_t indexOfContainer);

    void FillMonthData(const CalendarDataRequest& request, CalendarDaysOfMonth& result);
    void FillPreMonthData(
        const CalendarMonth& currentMonth, int32_t indexOfContainer, int32_t& index, CalendarDaysOfMonth& result);
    void FillCurrentMonthData(
        const CalendarMonth& currentMonth, int32_t indexOfContainer, int32_t& index, CalendarDaysOfMonth& result);
    void FillNextMonthData(
        const CalendarMonth& currentMonth, int32_t indexOfContainer, int32_t& index, CalendarDaysOfMonth& result);

    void RequestNextData()
    {
        if (!requestQueue_.empty()) {
            CalendarDataRequest request = requestQueue_.front();
            RequestData(request);
            requestQueue_.pop();
        } else {
            NotifySelectedChanged();
        }
    }

    void HandleDataRequestResult(const CalendarDataRequest& request, const std::string& result)
    {
        CalendarDaysOfMonth daysOfMonth;
        daysOfMonth.month = request.month;
        if (request.indexOfContainer >= 0 && request.indexOfContainer < CALENDAR_CACHE_PAGE) {
            if (!ParseData(request.indexOfContainer, result, daysOfMonth)) {
                return;
            }
            NotifyDataChanged(daysOfMonth, request.indexOfContainer);
            RequestNextData();
        }
    }

    bool cardCalendar_ = false;
    bool showLunar_ = true;
    bool firstLoad_ = true;
    bool hasMoved_ = false;
    bool isV2Component_ = false;
    bool isCrossMonth_ = false;
    int32_t selectedDay_ = 0;
    int32_t indexOfContainer_ = 0;
    int32_t startDayOfWeek_ = 0;
    int32_t requestNextIndex_ = -1;
    std::string offDays_;
    std::string workDays_;
    std::string holidays_;
    CalendarDay today_;
    EventMarker requestData_;
    CalendarMonth currentMonth_;
    PendingRequestQueue requestQueue_;
    CalendarType type_ { CalendarType::NORMAL };
    CardCalendarAttr calendarAttr_;
    CalendarDataAdapterAction dataAdapterAction_;
    WeakPtr<PipelineContext> pipelineContext_;
    std::array<std::vector<std::string>, CALENDAR_CACHE_PAGE> calendarCache_;
    std::array<CalendarDaysOfMonth, CALENDAR_CACHE_PAGE> dayOfMonthCache_;
    std::vector<RefPtr<CalendarDataChangeListener>> allListeners_;
    std::function<void(const std::string&)> requestDataEvent_;
    std::unordered_map<int32_t, CalendarMonth> indexMap_;
    std::unordered_map<CalendarMonth, std::string, HashFunc> monthCache_;

    static std::string cachePath_;
};

namespace StringUtils {

inline bool StringToCalendarDay(const std::string& str, CalendarDay& day)
{
    std::vector<int> date;
    StringSpliter(str, '-', date);
    if (date.size() == 3) {
        day.month.year = date[0];
        day.month.month = date[1] - 1;
        day.day = date[2];
        return true;
    }
    return false;
}

} // namespace StringUtils
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_DATA_ADAPTER_H
