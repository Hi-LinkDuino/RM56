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

#include "signal_info.h"

#include <csignal>
#include <iomanip>
#include <sstream>

namespace OHOS {
namespace HiviewDFX {
void SignalInfo::FormatSignalInfo(std::stringstream &ss, const siginfo_t &siginfo)
{
    int32_t signo = siginfo.si_signo;
    int32_t code = siginfo.si_code;
    ss << "Signal:" << FormatSignalName(signo) << "(" << FormatCodeName(signo, code) << ")";
    if (IsSignalAddrAvaliable(signo)) {
        ss << "@" << std::setw(8) << std::setfill('0') << std::hex << siginfo.si_addr << " "; // 8 : bit width
    }

    if (SI_FROMUSER(&siginfo) && (siginfo.si_pid != 0)) {
        ss << "form:" << std::to_string(siginfo.si_pid) << ":" << std::to_string(siginfo.si_uid);
    }
}

bool SignalInfo::IsSignalInfoAvaliable(int32_t signal)
{
    struct sigaction previousAction;
    if (sigaction(signal, nullptr, &previousAction) < 0) {
        return false;
    }
    return (static_cast<uint32_t>(previousAction.sa_flags) & SA_SIGINFO) != 0;
}

bool SignalInfo::IsSignalAddrAvaliable(int32_t signal)
{
    return ((signal == SIGABRT) || (signal == SIGBUS) || (signal == SIGILL)
        || (signal == SIGSEGV) || (signal == SIGTRAP));
}

bool SignalInfo::IsSignalPidAvaliable(int32_t sigCode)
{
    switch (sigCode) {
        case SI_USER:
        case SI_QUEUE:
        case SI_TIMER:
        case SI_ASYNCIO:
        case SI_MESGQ:
            return true;
        default:
            return false;
    }
}

std::string SignalInfo::FormatSignalName(int32_t signal)
{
    switch (signal) {
        case SIGABRT:
            return "SIGABRT";
        case SIGALRM:
            return "SIGALRM";
        case SIGBUS:
            return "SIGBUS";
        case SIGFPE:
            return "SIGFPE";
        case SIGILL:
            return "SIGILL";
        case SIGSEGV:
            return "SIGSEGV";
        case SIGSYS:
            return "SIGSYS";
        case SIGTRAP:
            return "SIGTRAP";
        default:
            return "Uncare Signal";
    }
}

std::string SignalInfo::FormatSIGBUSCodeName(int32_t signalCode)
{
    std::stringstream codeName;
    switch (signalCode) {
        case BUS_ADRALN:
            codeName << "BUS_ADRALN";
            break;
        case BUS_ADRERR:
            codeName << "BUS_ADRERR";
            break;
        case BUS_OBJERR:
            codeName << "BUS_OBJERR";
            break;
        case BUS_MCEERR_AR:
            codeName << "BUS_MCEERR_AR";
            break;
        case BUS_MCEERR_AO:
            codeName << "BUS_MCEERR_AO";
            break;
        default:
            codeName << FormatCommonSignalCodeName(signalCode);
            break;
    }
    return codeName.str();
}

std::string SignalInfo::FormatSIGILLCodeName(int32_t signalCode)
{
    std::stringstream codeName;
    switch (signalCode) {
        case ILL_ILLOPC:
            codeName << "ILL_ILLOPC";
            break;
        case ILL_ILLOPN:
            codeName << "ILL_ILLOPN";
            break;
        case ILL_ILLADR:
            codeName << "ILL_ILLADR";
            break;
        case ILL_ILLTRP:
            codeName << "ILL_ILLTRP";
            break;
        case ILL_PRVOPC:
            codeName << "ILL_PRVOPC";
            break;
        case ILL_PRVREG:
            codeName << "ILL_PRVREG";
            break;
        case ILL_COPROC:
            codeName << "ILL_COPROC";
            break;
        case ILL_BADSTK:
            codeName << "ILL_BADSTK";
            break;
        default:
            codeName << FormatCommonSignalCodeName(signalCode);
            break;
    }
    return codeName.str();
}

std::string SignalInfo::FormatSIGFPECodeName(int32_t signalCode)
{
    std::stringstream codeName;
    switch (signalCode) {
        case FPE_INTDIV:
            codeName << "FPE_INTDIV";
            break;
        case FPE_INTOVF:
            codeName << "FPE_INTOVF";
            break;
        case FPE_FLTDIV:
            codeName << "FPE_FLTDIV";
            break;
        case FPE_FLTOVF:
            codeName << "FPE_FLTOVF";
            break;
        case FPE_FLTUND:
            codeName << "FPE_FLTUND";
            break;
        case FPE_FLTRES:
            codeName << "FPE_FLTRES";
            break;
        case FPE_FLTINV:
            codeName << "FPE_FLTINV";
            break;
        case FPE_FLTSUB:
            codeName << "FPE_FLTSUB";
            break;
        default:
            codeName << FormatCommonSignalCodeName(signalCode);
            break;
    }
    return codeName.str();
}

std::string SignalInfo::FormatSIGSEGVCodeName(int32_t signalCode)
{
    std::stringstream codeName;
    switch (signalCode) {
        case SEGV_MAPERR:
            codeName << "SEGV_MAPERR";
            break;
        case SEGV_ACCERR:
            codeName << "SEGV_ACCERR";
            break;
        default:
            codeName << FormatCommonSignalCodeName(signalCode);
            break;
    }
    return codeName.str();
}

std::string SignalInfo::FormatSIGTRAPCodeName(int32_t signalCode)
{
    std::stringstream codeName;
    switch (signalCode) {
        case TRAP_BRKPT:
            codeName << "TRAP_BRKPT";
            break;
        case TRAP_TRACE:
            codeName << "TRAP_TRACE";
            break;
        case TRAP_BRANCH:
            codeName << "TRAP_BRANCH";
            break;
        case TRAP_HWBKPT:
            codeName << "TRAP_HWBKPT";
            break;
        default:
            codeName << FormatCommonSignalCodeName(signalCode);
            break;
    }
    return codeName.str();
}

std::string SignalInfo::FormatCommonSignalCodeName(int32_t signalCode)
{
    std::stringstream codeName;
    switch (signalCode) {
        case SI_USER:
            codeName << "SI_USER";
            break;
        case SI_KERNEL:
            codeName << "SI_KERNEL";
            break;
        case SI_QUEUE:
            codeName << "SI_QUEUE";
            break;
        case SI_TIMER:
            codeName << "SI_TIMER";
            break;
        case SI_MESGQ:
            codeName << "SI_MESGQ";
            break;
        case SI_ASYNCIO:
            codeName << "SI_ASYNCIO";
            break;
        case SI_SIGIO:
            codeName << "SI_SIGIO";
            break;
        case SI_TKILL:
            codeName << "SI_TKILL";
            break;
        default:
            codeName << "Unknown Code:" << signalCode;
            break;
    }
    return codeName.str();
}

std::string SignalInfo::FormatCodeName(int32_t signal, int32_t signalCode)
{
    switch (signal) {
        case SIGILL:
            return FormatSIGILLCodeName(signalCode);
        case SIGBUS:
            return FormatSIGBUSCodeName(signalCode);
        case SIGFPE:
            return FormatSIGFPECodeName(signalCode);
        case SIGSEGV:
            return FormatSIGSEGVCodeName(signalCode);
        case SIGTRAP:
            return FormatSIGTRAPCodeName(signalCode);
        default:
            break;
    }
    return FormatCommonSignalCodeName(signalCode);
}
}  // namespace HiviewDFX
}  // namespace OHOS
