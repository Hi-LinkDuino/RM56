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
#include "sys_event_query.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "data_query.h"
#include "hiview_global.h"
#include "store_mgr_proxy.h"
namespace OHOS {
namespace HiviewDFX {
namespace EventStore {
std::string EventCol::DOMAIN = "domain_";
std::string EventCol::NAME = "name_";
std::string EventCol::TYPE = "type_";
std::string EventCol::TS = "time_";
std::string EventCol::TZ = "tz_";
std::string EventCol::PID = "pid_";
std::string EventCol::TID = "tid_";
std::string EventCol::UID = "uid_";
std::string EventCol::TRACE_ID = "traceid_";
std::string EventCol::TRACE_FLAG = "trace_flag_";
std::string EventCol::SPAN_ID = "spanid_";
std::string EventCol::PARENT_SPAN_ID = "pspanid_";
std::string EventCol::INFO = "info_";

bool FieldValue::IsInteger()
{
    return (valueType_ == INTEGER);
}

bool FieldValue::IsFloat()
{
    return (valueType_ == FLOAT);
}

bool FieldValue::IsDouble()
{
    return (valueType_ == DOUBLE);
}

bool FieldValue::IsString()
{
    return (valueType_ == STRING);
}

int64_t FieldValue::GetInteger()
{
    return iValue_;
}

float FieldValue::GetFloat()
{
    return fValue_;
}

double FieldValue::GetDouble()
{
    return dValue_;
}

const std::string FieldValue::GetString()
{
    return sValue_;
}

Cond::Cond(const std::string &col, Op op, int8_t value): col_(col), op_(op), fieldValue_(value)
{
}

Cond::Cond(const std::string &col, Op op, int16_t value): col_(col), op_(op), fieldValue_(value)
{
}

Cond::Cond(const std::string &col, Op op, int32_t value): col_(col), op_(op), fieldValue_(value)
{
}

Cond::Cond(const std::string &col, Op op, int64_t value): col_(col), op_(op), fieldValue_(value)
{
}

Cond::Cond(const std::string &col, Op op, float value): col_(col), op_(op), fieldValue_(value)
{
}

Cond::Cond(const std::string &col, Op op, double value): col_(col), op_(op), fieldValue_(value)
{
}

Cond::Cond(const std::string &col, Op op, const std::string &value): col_(col), op_(op), fieldValue_(value)
{
}

Cond::Cond(const std::string &col, Op op, const std::vector<int8_t> &ints): op_(NONE), fieldValue_(0)
{
    for (int8_t value : ints) {
        orConds_.emplace_back(Cond(col, op, value));
    }
}

Cond::Cond(const std::string &col, Op op, const std::vector<int16_t> &ints): op_(NONE), fieldValue_(0)
{
    for (int16_t value : ints) {
        orConds_.emplace_back(Cond(col, op, value));
    }
}

Cond::Cond(const std::string &col, Op op, const std::vector<int32_t> &ints): op_(NONE), fieldValue_(0)
{
    for (int32_t value : ints) {
        orConds_.emplace_back(Cond(col, op, value));
    }
}
Cond::Cond(const std::string &col, Op op, const std::vector<int64_t> &longs): op_(NONE), fieldValue_(0)
{
    for (int64_t value : longs) {
        orConds_.emplace_back(Cond(col, op, value));
    }
}
Cond::Cond(const std::string &col, Op op, const std::vector<float> &floats): op_(NONE), fieldValue_(0)
{
    for (float value : floats) {
        orConds_.emplace_back(Cond(col, op, value));
    }
}
Cond::Cond(const std::string &col, Op op, const std::vector<double> &doubles): op_(NONE), fieldValue_(0)
{
    for (double value : doubles) {
        orConds_.emplace_back(Cond(col, op, value));
    }
}
Cond::Cond(const std::string &col, Op op, const std::vector<std::string> &strings): op_(NONE), fieldValue_(0)
{
    for (std::string value : strings) {
        orConds_.emplace_back(Cond(col, op, value));
    }
}

Cond::~Cond()
{
}

Cond &Cond::And(const std::string &col, Op op, const int8_t value)
{
    andConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::And(const std::string &col, Op op, const int16_t value)
{
    andConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::And(const std::string &col, Op op, const int32_t value)
{
    andConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::And(const std::string &col, Op op, const int64_t value)
{
    andConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::And(const std::string &col, Op op, const float value)
{
    andConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::And(const std::string &col, Op op, const double value)
{
    andConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::And(const std::string &col, Op op, const std::string &value)
{
    andConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::And(const Cond &cond)
{
    andConds_.emplace_back(cond);
    return *this;
}

Cond &Cond::Or(const std::string &col, Op op, const int8_t value)
{
    orConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::Or(const std::string &col, Op op, const int16_t value)
{
    orConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::Or(const std::string &col, Op op, const int32_t value)
{
    orConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::Or(const std::string &col, Op op, const int64_t value)
{
    orConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::Or(const std::string &col, Op op, const float value)
{
    orConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::Or(const std::string &col, Op op, const double value)
{
    orConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::Or(const std::string &col, Op op, const std::string &value)
{
    orConds_.emplace_back(Cond(col, op, value));
    return *this;
}

Cond &Cond::Or(const Cond &cond)
{
    orConds_.emplace_back(cond);
    return *this;
}

void Cond::GetCondEqualValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsInteger()) {
        dataQuery.EqualTo(cond.col_, cond.fieldValue_.GetInteger());
        return;
    }
    if (cond.fieldValue_.IsFloat()) {
        dataQuery.EqualTo(cond.col_, cond.fieldValue_.GetFloat());
        return;
    }
    if (cond.fieldValue_.IsDouble()) {
        dataQuery.EqualTo(cond.col_, cond.fieldValue_.GetDouble());
        return;
    }
    if (cond.fieldValue_.IsString()) {
        dataQuery.EqualTo(cond.col_, cond.fieldValue_.GetString());
        return;
    }
    return;
}

void Cond::GetCondNotEqualValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsInteger()) {
        dataQuery.NotEqualTo(cond.col_, cond.fieldValue_.GetInteger());
        return;
    }
    if (cond.fieldValue_.IsFloat()) {
        dataQuery.NotEqualTo(cond.col_, cond.fieldValue_.GetFloat());
        return;
    }
    if (cond.fieldValue_.IsDouble()) {
        dataQuery.NotEqualTo(cond.col_, cond.fieldValue_.GetDouble());
        return;
    }
    if (cond.fieldValue_.IsString()) {
        dataQuery.NotEqualTo(cond.col_, cond.fieldValue_.GetString());
        return;
    }
    return;
}

void Cond::GetCondLessThanValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsInteger()) {
        dataQuery.LessThan(cond.col_, cond.fieldValue_.GetInteger());
        return;
    }
    if (cond.fieldValue_.IsFloat()) {
        dataQuery.LessThan(cond.col_, cond.fieldValue_.GetFloat());
        return;
    }
    if (cond.fieldValue_.IsDouble()) {
        dataQuery.LessThan(cond.col_, cond.fieldValue_.GetDouble());
        return;
    }
    if (cond.fieldValue_.IsString()) {
        dataQuery.LessThan(cond.col_, cond.fieldValue_.GetString());
        return;
    }
}

void Cond::GetCondLessEqualValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsInteger()) {
        dataQuery.LessThanOrEqualTo(cond.col_, cond.fieldValue_.GetInteger());
        return;
    }
    if (cond.fieldValue_.IsFloat()) {
        dataQuery.LessThanOrEqualTo(cond.col_, cond.fieldValue_.GetFloat());
        return;
    }
    if (cond.fieldValue_.IsDouble()) {
        dataQuery.LessThanOrEqualTo(cond.col_, cond.fieldValue_.GetDouble());
        return;
    }
    if (cond.fieldValue_.IsString()) {
        dataQuery.LessThanOrEqualTo(cond.col_, cond.fieldValue_.GetString());
        return;
    }
}

void Cond::GetCondGreatThanValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsInteger()) {
        dataQuery.GreaterThan(cond.col_, cond.fieldValue_.GetInteger());
        return;
    }
    if (cond.fieldValue_.IsFloat()) {
        dataQuery.GreaterThan(cond.col_, cond.fieldValue_.GetFloat());
        return;
    }
    if (cond.fieldValue_.IsDouble()) {
        dataQuery.GreaterThan(cond.col_, cond.fieldValue_.GetDouble());
        return;
    }
    if (cond.fieldValue_.IsString()) {
        dataQuery.GreaterThan(cond.col_, cond.fieldValue_.GetString());
        return;
    }
}

void Cond::GetCondGreatEqualValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsInteger()) {
        dataQuery.GreaterThanOrEqualTo(cond.col_, cond.fieldValue_.GetInteger());
        return;
    }
    if (cond.fieldValue_.IsFloat()) {
        dataQuery.GreaterThanOrEqualTo(cond.col_, cond.fieldValue_.GetFloat());
        return;
    }
    if (cond.fieldValue_.IsDouble()) {
        dataQuery.GreaterThanOrEqualTo(cond.col_, cond.fieldValue_.GetDouble());
        return;
    }
    if (cond.fieldValue_.IsString()) {
        dataQuery.GreaterThanOrEqualTo(cond.col_, cond.fieldValue_.GetString());
        return;
    }
}

void Cond::GetCondStartWithValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsString()) {
        dataQuery.StartWith(cond.col_, cond.fieldValue_.GetString());
        return;
    }
}

void Cond::GetCondNoStartWithValue(DataQuery &dataQuery, Cond &cond)
{
    if (cond.fieldValue_.IsString()) {
        dataQuery.NotStartWith(cond.col_, cond.fieldValue_.GetString());
        return;
    }
}

void Cond::GetCond(DataQuery &dataQuery, Cond &cond)
{
    switch (cond.op_) {
        case EQ:
            GetCondEqualValue(dataQuery, cond);
            break;
        case NE:
            GetCondNotEqualValue(dataQuery, cond);
            break;
        case GT:
            GetCondGreatThanValue(dataQuery, cond);
            break;
        case GE:
            GetCondGreatEqualValue(dataQuery, cond);
            break;
        case LT:
            GetCondLessThanValue(dataQuery, cond);
            break;
        case LE:
            GetCondLessEqualValue(dataQuery, cond);
            break;
        case SW:
            GetCondStartWithValue(dataQuery, cond);
            break;
        case NSW:
            GetCondNoStartWithValue(dataQuery, cond);
            break;
        default:
            break;
    }
}

bool Cond::IsSimpleCond(Cond &cond)
{
    if (!cond.col_.empty() && (!cond.andConds_.empty() || !cond.orConds_.empty())) {
        return false;
    }
    if (cond.andConds_.size() > 1) {
        return false;
    }
    if (cond.orConds_.size() > 1) {
        return false;
    }
    return true;
}

void Cond::Traval(DataQuery &dataQuery, Cond &cond)
{
    if (!cond.col_.empty()) {
        GetCond(dataQuery, cond);
    }
    if (!cond.andConds_.empty()) {
        if (!cond.col_.empty()) {
            dataQuery.And();
        }
        bool isFirst = true;
        for (auto it = cond.andConds_.begin(); it != cond.andConds_.end(); it++) {
            if (!isFirst) {
                dataQuery.And();
            }
            isFirst = false;
            if (IsSimpleCond(*it)) {
                Traval(dataQuery, *it);
            } else {
                dataQuery.BeginGroup();
                Traval(dataQuery, *it);
                dataQuery.EndGroup();
            }
        }
    }
    if (!cond.orConds_.empty()) {
        if (!cond.col_.empty() || !cond.andConds_.empty()) {
            dataQuery.Or();
        }
        bool isFirst = true;
        for (auto it = cond.orConds_.begin(); it != cond.orConds_.end(); it++) {
            if (!isFirst) {
                dataQuery.Or();
            }
            isFirst = false;
            if (IsSimpleCond(*it)) {
                Traval(dataQuery, *it);
            } else {
                dataQuery.BeginGroup();
                Traval(dataQuery, *it);
                dataQuery.EndGroup();
            }
        }
    }
}

ResultSet::ResultSet(): iter_(eventRecords_.begin()), code_(0), has_(false)
{
}

ResultSet::~ResultSet()
{
}

ResultSet::ResultSet(ResultSet &&result)
{
    eventRecords_ = move(result.eventRecords_);
    code_ = result.code_;
    has_ = result.has_;
    iter_ = result.iter_;
}

ResultSet& ResultSet::operator = (ResultSet &&result)
{
    eventRecords_ = move(result.eventRecords_);
    code_ = result.code_;
    has_ = result.has_;
    iter_ = result.iter_;
    return *this;
}

int ResultSet::GetErrCode() const
{
    return code_;
}

bool ResultSet::HasNext() const
{
    return has_;
}

ResultSet::RecordIter ResultSet::Next()
{
    if (!has_) {
        return eventRecords_.end();
    }

    auto tempIter = iter_;
    iter_++;
    if (iter_ == eventRecords_.end()) {
        has_ = false;
    }

    return tempIter;
}

void ResultSet::Set(int code, bool has)
{
    code_ = code;
    has_ = has;
    if (eventRecords_.size() > 0) {
        iter_ = eventRecords_.begin();
    }
}

SysEventQuery::SysEventQuery(const std::string &dbFile): dbFile_(dbFile)
{
}

SysEventQuery::~SysEventQuery()
{
}

SysEventQuery &SysEventQuery::Select()
{
    return *this;
}

SysEventQuery &SysEventQuery::Select(const std::vector<std::string> &eventCols)
{
    for (std::string col : eventCols) {
        auto it = std::find(eventCols_.begin(), eventCols_.end(), col);
        if (it != eventCols_.end()) {
            continue;
        }
        eventCols_.emplace_back(col);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Where(const std::string &col, Op op, const int8_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Where(const std::string &col, Op op, const int16_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Where(const std::string &col, Op op, const int32_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Where(const std::string &col, Op op, const int64_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Where(const std::string &col, Op op, const float value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Where(const std::string &col, Op op, const double value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Where(const std::string &col, Op op, const std::string &value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Where(const Cond &cond)
{
    cond_.And(cond);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::string &col, Op op, const int8_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::string &col, Op op, const int16_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::string &col, Op op, const int32_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::string &col, Op op, const int64_t value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::string &col, Op op, const float value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::string &col, Op op, const double value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::string &col, Op op, const std::string &value)
{
    cond_.And(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::And(const Cond &cond)
{
    cond_.And(cond);
    return *this;
}

SysEventQuery &SysEventQuery::And(const std::vector<Cond> &conds)
{
    for (Cond cond : conds) {
        cond_.And(cond);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const int8_t value)
{
    cond_.Or(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const int16_t value)
{
    cond_.Or(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const int32_t value)
{
    cond_.Or(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const int64_t value)
{
    cond_.Or(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const float value)
{
    cond_.Or(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const double value)
{
    cond_.Or(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::string &value)
{
    cond_.Or(col, op, value);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const Cond &cond)
{
    cond_.Or(cond);
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::vector<int8_t> &ints)
{
    for (auto value : ints) {
        cond_.Or(col, op, value);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::vector<int16_t> &ints)
{
    for (auto value : ints) {
        cond_.Or(col, op, value);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::vector<int32_t> &ints)
{
    for (auto value : ints) {
        cond_.Or(col, op, value);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::vector<int64_t> &longs)
{
    for (auto value : longs) {
        cond_.Or(col, op, value);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::vector<float> &floats)
{
    for (auto value : floats) {
        cond_.Or(col, op, value);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::vector<double> &doubles)
{
    for (auto value : doubles) {
        cond_.Or(col, op, value);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::string &col, Op op, const std::vector<std::string> &strings)
{
    for (auto value : strings) {
        cond_.Or(col, op, value);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Or(const std::vector<Cond> &conds)
{
    for (auto cond : conds) {
        cond_.Or(cond);
    }
    return *this;
}

SysEventQuery &SysEventQuery::Order(const std::string &col, bool isAsc)
{
    orderCols_.emplace_back(std::make_pair<>(col, isAsc));
    return *this;
}

void SysEventQuery::BuildDataQuery(DataQuery &dataQuery, int limit)
{
    dataQuery.Select(eventCols_);
    Cond::Traval(dataQuery, cond_);
    for (auto it = orderCols_.begin(); it != orderCols_.end(); it++) {
        if (it->second) {
            dataQuery.OrderByAsc(it->first);
        } else {
            dataQuery.OrderByDesc(it->first);
        }
    }
    dataQuery.Limit(limit);
}

ResultSet SysEventQuery::Execute(int limit)
{
    DataQuery dataQuery;
    BuildDataQuery(dataQuery, limit);

    ResultSet resultSet;
    std::vector<Entry> entries;
    std::shared_ptr<DocStore> docStore = StoreMgrProxy::GetInstance().GetDocStore(dbFile_);
    int retCode = docStore->GetEntriesWithQuery(dataQuery, entries);
    if (retCode != 0) {
        resultSet.Set(retCode, false);
        return resultSet;
    }
    if (entries.empty()) {
        resultSet.Set(0, false);
        return resultSet;
    }
    for (auto it = entries.begin(); it != entries.end(); it++) {
        SysEvent sysEvent("", nullptr, it->value);
        sysEvent.PaserJson();
        sysEvent.SetSeq(it->id);
        resultSet.eventRecords_.emplace_back(sysEvent);
    }
    resultSet.Set(0, true);
    return resultSet;
}

void SysEventQuery::GetDataQeury(DataQuery &dataQuery)
{
    Cond::Traval(dataQuery, cond_);
}

int SysEventQuery::ExecuteWithCallback(SysEeventCallBack callback, int limit)
{
    DataQuery dataQuery;
    BuildDataQuery(dataQuery, limit);

    std::vector<Entry> entries;
    std::shared_ptr<DocStore> docStore = StoreMgrProxy::GetInstance().GetDocStore(dbFile_);
    std::function<int (int, const Entry&)> c = [&](int cnt, const Entry &entry) -> int {
        SysEvent sysEvent("", nullptr, entry.value);
        sysEvent.PaserJson();
        sysEvent.SetSeq(entry.id);
        return callback(sysEvent);
    };
    docStore->GetEntryDuringQuery(dataQuery, c);
    return 0;
}
} // EventStore
} // namespace HiviewDFX
} // namespace OHOS