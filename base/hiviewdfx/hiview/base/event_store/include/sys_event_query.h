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

#ifndef HIVIEW_BASE_EVENT_STORE_INCLUDE_SYS_EVENT_QUERY_H
#define HIVIEW_BASE_EVENT_STORE_INCLUDE_SYS_EVENT_QUERY_H

#ifndef DllExport
#define DllExport
#endif // DllExport

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "sys_event.h"

namespace OHOS {
namespace HiviewDFX {
class DataQuery;
namespace EventStore {
enum Op { NONE = 0, EQ = 1, NE, LT, LE, GT, GE, SW, NSW };

class EventCol {
public:
    static std::string DOMAIN;
    static std::string NAME;
    static std::string TYPE;
    static std::string TS;
    static std::string TZ;
    static std::string PID;
    static std::string TID;
    static std::string UID;
    static std::string TRACE_ID;
    static std::string TRACE_FLAG;
    static std::string SPAN_ID;
    static std::string PARENT_SPAN_ID;
    static std::string INFO;
};

class FieldValue {
public:
    enum ValueType { NONE = 0, INTEGER = 1, FLOAT = 2, DOUBLE = 3, STRING = 4 };
    FieldValue(int value): valueType_(INTEGER), iValue_(value), fValue_(0), dValue_(0) {}
    FieldValue(int64_t value): valueType_(INTEGER), iValue_(value), fValue_(0), dValue_(0) {}
    FieldValue(float value): valueType_(FLOAT), iValue_(0), fValue_(value), dValue_(0) {}
    FieldValue(double value): valueType_(DOUBLE), iValue_(0), fValue_(0), dValue_(value) {}
    FieldValue(const std::string &value): valueType_(STRING), iValue_(0), fValue_(0), dValue_(0), sValue_(value) {}
    ~FieldValue() {};

    bool IsInteger();
    bool IsFloat();
    bool IsDouble();
    bool IsString();
    int64_t GetInteger();
    float GetFloat();
    double GetDouble();
    const std::string GetString();
private:
    ValueType valueType_;
    int64_t iValue_;
    float fValue_;
    double dValue_;
    std::string sValue_;
};

class DllExport Cond {
public:
    Cond(): op_(NONE), fieldValue_(0) {};
    Cond(const std::string &col, Op op, int8_t value);
    Cond(const std::string &col, Op op, int16_t value);
    Cond(const std::string &col, Op op, int32_t value);
    Cond(const std::string &col, Op op, int64_t value);
    Cond(const std::string &col, Op op, float value);
    Cond(const std::string &col, Op op, double value);
    Cond(const std::string &col, Op op, const std::string &value);
    Cond(const std::string &col, Op op, const std::vector<int8_t> &ints);
    Cond(const std::string &col, Op op, const std::vector<int16_t> &ints);
    Cond(const std::string &col, Op op, const std::vector<int32_t> &ints);
    Cond(const std::string &col, Op op, const std::vector<int64_t> &longs);
    Cond(const std::string &col, Op op, const std::vector<float> &floats);
    Cond(const std::string &col, Op op, const std::vector<double> &doubles);
    Cond(const std::string &col, Op op, const std::vector<std::string> &strings);
    ~Cond();
    Cond &And(const std::string &col, Op op, const int8_t value);
    Cond &And(const std::string &col, Op op, const int16_t value);
    Cond &And(const std::string &col, Op op, const int32_t value);
    Cond &And(const std::string &col, Op op, const int64_t value);
    Cond &And(const std::string &col, Op op, const float value);
    Cond &And(const std::string &col, Op op, const double value);
    Cond &And(const std::string &col, Op op, const std::string &value);
    Cond &And(const Cond &cond);
    Cond &Or(const std::string &col, Op op, const int8_t value);
    Cond &Or(const std::string &col, Op op, const int16_t value);
    Cond &Or(const std::string &col, Op op, const int32_t value);
    Cond &Or(const std::string &col, Op op, const int64_t value);
    Cond &Or(const std::string &col, Op op, const float value);
    Cond &Or(const std::string &col, Op op, const double value);
    Cond &Or(const std::string &col, Op op, const std::string &value);
    Cond &Or(const Cond &cond);
private:
    friend class SysEventQuery;

private:
    static bool IsSimpleCond(Cond &cond);
    static void Traval(DataQuery &dataQuery, Cond &cond);
    static void GetCond(DataQuery &dataQuery, Cond &cond);
    static void GetCondEqualValue(DataQuery &dataQuery, Cond &cond);
    static void GetCondNotEqualValue(DataQuery &dataQuery, Cond &cond);
    static void GetCondLessThanValue(DataQuery &dataQuery, Cond &cond);
    static void GetCondLessEqualValue(DataQuery &dataQuery, Cond &cond);
    static void GetCondGreatThanValue(DataQuery &dataQuery, Cond &cond);
    static void GetCondGreatEqualValue(DataQuery &dataQuery, Cond &cond);
    static void GetCondStartWithValue(DataQuery &dataQuery, Cond &cond);
    static void GetCondNoStartWithValue(DataQuery &dataQuery, Cond &cond);

private:
    std::string col_;
    Op op_;
    FieldValue fieldValue_;
    std::vector<Cond> andConds_;
    std::vector<Cond> orConds_;
};  // Cond

class DllExport ResultSet {
public:
    ResultSet();
    ResultSet(ResultSet &&result);
    ResultSet& operator = (ResultSet &&result);
    ~ResultSet();

public:
    using RecordIter = std::vector<SysEvent>::iterator;
    int GetErrCode() const;
    bool HasNext() const;
    RecordIter Next();

private:
    friend class SysEventQuery;
    void Set(int code, bool has);
    std::vector<SysEvent> eventRecords_;
    RecordIter iter_;
    int code_;
    bool has_;
};  // ResultSet

using SysEeventCallBack = std::function<int (SysEvent &sysEvent)>;

class DllExport SysEventQuery {
public:
    SysEventQuery &Select();
    SysEventQuery &Select(const std::vector<std::string> &eventCols);
    SysEventQuery &Where(const std::string &col, Op op, const int8_t value);
    SysEventQuery &Where(const std::string &col, Op op, const int16_t value);
    SysEventQuery &Where(const std::string &col, Op op, const int32_t value);
    SysEventQuery &Where(const std::string &col, Op op, const int64_t value);
    SysEventQuery &Where(const std::string &col, Op op, const float value);
    SysEventQuery &Where(const std::string &col, Op op, const double value);
    SysEventQuery &Where(const std::string &col, Op op, const std::string &value);
    SysEventQuery &Where(const Cond &cond);
    SysEventQuery &And(const std::string &col, Op op, const int8_t value);
    SysEventQuery &And(const std::string &col, Op op, const int16_t value);
    SysEventQuery &And(const std::string &col, Op op, const int32_t value);
    SysEventQuery &And(const std::string &col, Op op, const int64_t value);
    SysEventQuery &And(const std::string &col, Op op, const float value);
    SysEventQuery &And(const std::string &col, Op op, const double value);
    SysEventQuery &And(const std::string &col, Op op, const std::string &value);
    SysEventQuery &And(const Cond &cond);
    SysEventQuery &And(const std::vector<Cond> &conds);
    SysEventQuery &Or(const std::string &col, Op op, const int8_t value);
    SysEventQuery &Or(const std::string &col, Op op, const int16_t value);
    SysEventQuery &Or(const std::string &col, Op op, const int32_t value);
    SysEventQuery &Or(const std::string &col, Op op, const int64_t value);
    SysEventQuery &Or(const std::string &col, Op op, const float value);
    SysEventQuery &Or(const std::string &col, Op op, const double value);
    SysEventQuery &Or(const std::string &col, Op op, const std::string &value);
    SysEventQuery &Or(const Cond &cond);
    SysEventQuery &Or(const std::string &col, Op op, const std::vector<int8_t> &ints);
    SysEventQuery &Or(const std::string &col, Op op, const std::vector<int16_t> &ints);
    SysEventQuery &Or(const std::string &col, Op op, const std::vector<int32_t> &ints);
    SysEventQuery &Or(const std::string &col, Op op, const std::vector<int64_t> &longs);
    SysEventQuery &Or(const std::string &col, Op op, const std::vector<float> &floats);
    SysEventQuery &Or(const std::string &col, Op op, const std::vector<double> &floats);
    SysEventQuery &Or(const std::string &col, Op op, const std::vector<std::string> &strings);
    SysEventQuery &Or(const std::vector<Cond> &conds);
    SysEventQuery &Order(const std::string &col, bool isAsc = true);
    ResultSet Execute(int limit = 100);
    int ExecuteWithCallback(SysEeventCallBack callback, int limit = 100);
    ~SysEventQuery();
private:
    friend class SysEventDao;
    SysEventQuery(const std::string &dbFile);

private:
    void GetDataQeury(DataQuery &dataQuery);
    void BuildDataQuery(DataQuery &dataQuery, int limit);

private:
    std::string dbFile_;
    std::vector<std::string> eventCols_;
    std::vector<std::pair<std::string, bool>> orderCols_;
    Cond cond_;
}; // SysEventQuery
} // EventStore
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_EVENT_STORE_INCLUDE_SYS_EVENT_QUERY_H