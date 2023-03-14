/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "install_result.h"

#include "app_log_wrapper.h"
#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_RESULT_TRANSACTID = "transactId";
const std::string JSON_KEY_RESULT_RESULTMSG = "resultMsg";
const std::string JSON_KEY_RESULT_RETCODE = "retCode";
const std::string JSON_KEY_PROGRESS_DOWNLOADSIZE = "downloadSize";
const std::string JSON_KEY_PROGRESS_TOTALSIZE = "totalSize";
const std::string JSON_KEY_INSTALLRESULT_VERSION = "version";
const std::string JSON_KEY_INSTALLRESULT_RESULT = "result";
const std::string JSON_KEY_INSTALLRESULT_PROGRESS = "progress";
}  //  namespace

void to_json(nlohmann::json &jsonObject, const Result &result)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_RESULT_TRANSACTID, result.transactId},
        {JSON_KEY_RESULT_RESULTMSG, result.resultMsg},
        {JSON_KEY_RESULT_RETCODE, result.retCode}
    };
}

void from_json(const nlohmann::json &jsonObject, Result &result)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_RESULT_TRANSACTID,
        result.transactId,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_RESULT_RESULTMSG,
        result.resultMsg,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_RESULT_RETCODE,
        result.retCode,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void to_json(nlohmann::json &jsonObject, const Progress &progress)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_PROGRESS_DOWNLOADSIZE, progress.downloadSize},
        {JSON_KEY_PROGRESS_TOTALSIZE, progress.totalSize}
    };
}

void from_json(const nlohmann::json &jsonObject, Progress &progress)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PROGRESS_DOWNLOADSIZE,
        progress.downloadSize,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PROGRESS_TOTALSIZE,
        progress.totalSize,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void to_json(nlohmann::json &jsonObject, const InstallResult &installResult)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_INSTALLRESULT_VERSION, installResult.version},
        {JSON_KEY_INSTALLRESULT_RESULT, installResult.result},
        {JSON_KEY_INSTALLRESULT_PROGRESS, installResult.progress}
    };
}

void from_json(const nlohmann::json &jsonObject, InstallResult &installResult)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_INSTALLRESULT_VERSION,
        installResult.version,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Result>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_INSTALLRESULT_RESULT,
        installResult.result,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Progress>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_INSTALLRESULT_PROGRESS,
        installResult.progress,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

bool Result::ReadFromParcel(Parcel &parcel)
{
    transactId = Str16ToStr8(parcel.ReadString16());
    resultMsg = Str16ToStr8(parcel.ReadString16());
    retCode = parcel.ReadInt32();
    return true;
}

bool Result::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(transactId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(resultMsg));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, retCode);
    return true;
}

Result *Result::Unmarshalling(Parcel &parcel)
{
    Result *result = new (std::nothrow) Result();
    if (result && !result->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete result;
        result = nullptr;
    }
    return result;
}

bool Progress::ReadFromParcel(Parcel &parcel)
{
    downloadSize = parcel.ReadInt32();
    totalSize = parcel.ReadInt32();
    return true;
}

bool Progress::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, downloadSize);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, totalSize);
    return true;
}

Progress *Progress::Unmarshalling(Parcel &parcel)
{
    Progress *progress = new (std::nothrow) Progress();
    if (progress && !progress->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete progress;
        progress = nullptr;
    }
    return progress;
}

bool InstallResult::ReadFromParcel(Parcel &parcel)
{
    version = Str16ToStr8(parcel.ReadString16());
    auto paramsResult = parcel.ReadParcelable<Result>();
    if (paramsResult != nullptr) {
        result = *paramsResult;
        delete paramsResult;
        paramsResult = nullptr;
    } else {
        return false;
    }

    auto paramsProgress = parcel.ReadParcelable<Progress>();
    if (paramsProgress != nullptr) {
        progress = *paramsProgress;
        delete paramsProgress;
        paramsProgress = nullptr;
    } else {
        return false;
    }
    return true;
}

bool InstallResult::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(version));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &result);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &progress);
    return true;
}

InstallResult *InstallResult::Unmarshalling(Parcel &parcel)
{
    InstallResult *installResult = new (std::nothrow) InstallResult();
    if (installResult && !installResult->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete installResult;
        installResult = nullptr;
    }
    return installResult;
}
}  //  namespace AppExecFwk
}  //  namespace OHOS