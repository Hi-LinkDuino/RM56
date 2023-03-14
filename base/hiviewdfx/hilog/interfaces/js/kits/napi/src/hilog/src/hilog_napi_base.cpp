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

#include "properties.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "../../common/napi/n_func_arg.h"
#include "../../common/napi/n_class.h"
#include "hilog/log.h"
#include "hilog/log_c.h"
#include "securec.h"
#include "../include/context/hilog_napi_base.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace OHOS {
namespace HiviewDFX {
using namespace std;
#define DEFAULT_LOG_TYPE LOG_CORE
const HiLogLabel LABEL = { LOG_CORE, 0xD002D00, "Hilog_JS" };
static constexpr int MIN_NUMBER = 3;
static constexpr int MAX_NUMBER = 100;
static constexpr int PUBLIC_LEN = 6;
static constexpr int PRIVATE_LEN = 7;
static constexpr int PROPERTY_POS = 2;

void ParseLogContent(string& formatStr, vector<string>& params, string& logContent)
{
    string& ret = logContent;
    if (params.empty()) {
        ret += formatStr;
        return;
    }
    auto size = params.size();
    auto len = formatStr.size();
    uint32_t pos = 0;
    uint32_t count = 0;
    bool debug = IsDebugOn();
    bool priv = (!debug) && IsPrivateSwitchOn();

    for (; pos < len; ++pos) {
        bool showPriv = false;
        if (count > size) {
            break;
        }
        if (formatStr[pos] == '%') {
            if (formatStr[pos + 1] == '{') {
                if (formatStr.substr(pos + PROPERTY_POS, PUBLIC_LEN) == "public") {
                    pos += (PUBLIC_LEN + PROPERTY_POS);
                }
                if (formatStr.substr(pos + PROPERTY_POS, PRIVATE_LEN) == "private") {
                    pos += (PRIVATE_LEN + PROPERTY_POS);
                    if (priv) {
                        showPriv = true;
                    }
                }
            }
            if (pos + 1 >= len) {
                break;
            }
            string privStr = "<private>";
            switch (formatStr[pos + 1]) {
                case 's':
                case 'j':
                case 'd':
                case 'O':
                case 'o':
                case 'i':
                case 'f':
                case 'c':
                    ret += showPriv ? privStr : params[count];
                    count++;
                    ++pos;
                    break;
                case '%':
                    ret += formatStr[pos];
                    ++pos;
                    break;
                default:
                    ret += formatStr[pos];
                    break;
            }
        } else {
            ret += formatStr[pos];
        }
    }
    if (pos < len) {
        ret += formatStr.substr(pos, len - pos);
    }
    return;
}

napi_value HilogNapiBase::isLoggable(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    int32_t domain;
    tie(succ, domain) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    int32_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    unique_ptr<char[]> tag;
    tie(succ, tag, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    bool res = HiLogIsLoggable(domain, tag.get(), static_cast<LogLevel>(level));
    return NVal::CreateBool(env, res).val_;
}

napi_value HilogNapiBase::debug(napi_env env, napi_callback_info info)
{
    return HilogImpl(env, info, LOG_DEBUG);
}

napi_value HilogNapiBase::info(napi_env env, napi_callback_info info)
{
    return HilogImpl(env, info, LOG_INFO);
}

napi_value HilogNapiBase::warn(napi_env env, napi_callback_info info)
{
    return HilogImpl(env, info, LOG_WARN);
}

napi_value HilogNapiBase::error(napi_env env, napi_callback_info info)
{
    return HilogImpl(env, info, LOG_ERROR);
}

napi_value HilogNapiBase::fatal(napi_env env, napi_callback_info info)
{
    return HilogImpl(env, info, LOG_FATAL);
}

napi_value HilogNapiBase::parseNapiValue(napi_env env, napi_callback_info info,
    napi_value element, vector<string>& params)
{
    bool succ = false;
    napi_valuetype type;
    napi_status typeStatus = napi_typeof(env, element, &type);
    if (typeStatus != napi_ok) {
        return nullptr;
    }
    if (type == napi_number) {
        napi_value elmString;
        napi_status objectStatus = napi_coerce_to_string(env, element, &elmString);
        if (objectStatus != napi_ok) {
            return nullptr;
        }
        unique_ptr<char[]> name;
        tie(succ, name, ignore) = NVal(env, elmString).ToUTF8String();
        if (!succ) {
            return nullptr;
        }
        params.emplace_back(name.get());
    } else if (type == napi_string) {
        unique_ptr<char[]> name;
        tie(succ, name, ignore) = NVal(env, element).ToUTF8String();
        if (!succ) {
            return nullptr;
        }
        params.emplace_back(name.get());
    } else if (type == napi_object) {
        napi_value elmString;
        napi_status objectStatus = napi_coerce_to_string(env, element, &elmString);
        if (objectStatus != napi_ok) {
            return nullptr;
        }
        unique_ptr<char[]> name;
        tie(succ, name, ignore) = NVal(env, elmString).ToUTF8String();
        if (!succ) {
            return nullptr;
        }
        params.emplace_back(name.get());
    } else {
        NAPI_ASSERT(env, false, "type mismatch");
    }
    return nullptr;
}

napi_value HilogNapiBase::HilogImpl(napi_env env, napi_callback_info info, int level)
{
    NFuncArg funcArg(env, info);
    funcArg.InitArgs(MIN_NUMBER, MAX_NUMBER);
    bool succ = false;
    int32_t domain;
    tie(succ, domain) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        HiLog::Info(LABEL, "%{public}s", "domain mismatch");
        return nullptr;
    }
    unique_ptr<char[]> tag;
    tie(succ, tag, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        HiLog::Info(LABEL, "%{public}s", "tag mismatch");
        return nullptr;
    }
    unique_ptr<char[]> fmt;
    tie(succ, fmt, ignore) = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
    if (!succ) {
        HiLog::Info(LABEL, "%{public}s", "Format mismatch");
        return nullptr;
    }
    string fmtString = fmt.get();
    bool res = false;
    napi_value array = funcArg[NARG_POS::FOURTH];
    napi_is_array(env, array, &res);
    string logContent;
    vector<string> params;
    if (!res) {
        for (size_t i = MIN_NUMBER; i < funcArg.GetArgc(); i++) {
            napi_value argsVal = funcArg[i];
            (void)parseNapiValue(env, info, argsVal, params);
        }
    } else {
        if (funcArg.GetArgc() != MIN_NUMBER + 1) {
            NAPI_ASSERT(env, false, "Argc mismatch");
            HiLog::Info(LABEL, "%{public}s", "Argc mismatch");
            return nullptr;
        }
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            (void)parseNapiValue(env, info, element, params);
        }
    }
    ParseLogContent(fmtString, params, logContent);
    HiLogPrint(DEFAULT_LOG_TYPE, static_cast<LogLevel>(level), domain, tag.get(),
        logContent.c_str(), "");
    return nullptr;
}
}  // namespace HiviewDFX
}  // namespace OHOS

#ifdef __cplusplus
}
#endif
