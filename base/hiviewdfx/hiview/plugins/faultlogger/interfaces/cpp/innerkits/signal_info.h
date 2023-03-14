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
#ifndef FAULTLOGGER_SIGNAL_INFO_H
#define FAULTLOGGER_SIGNAL_INFO_H
#include <csignal>
#include <sstream>
#include <string>
namespace OHOS {
namespace HiviewDFX {
class SignalInfo {
public:
    static void FormatSignalInfo(std::stringstream &ss, const siginfo_t &siginfo);

private:
    static bool IsSignalInfoAvaliable(int32_t signal);
    static bool IsSignalAddrAvaliable(int32_t signal);
    static bool IsSignalPidAvaliable(int32_t sigCode);
    static std::string FormatSignalName(int32_t signal);
    static std::string FormatCodeName(int32_t signal, int32_t signalCode);

    static std::string FormatSIGBUSCodeName(int32_t signalCode);
    static std::string FormatSIGILLCodeName(int32_t signalCode);
    static std::string FormatSIGFPECodeName(int32_t signalCode);
    static std::string FormatSIGSEGVCodeName(int32_t signalCode);
    static std::string FormatSIGTRAPCodeName(int32_t signalCode);
    static std::string FormatCommonSignalCodeName(int32_t signalCode);
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // FAULTLOGGER_SIGNAL_INFO_H