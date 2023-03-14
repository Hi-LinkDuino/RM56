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

#ifndef HILOGTOOL_H
#define HILOGTOOL_H

namespace OHOS {
namespace HiviewDFX {
#define DOMAIN_NUMBER_BASE (16)
#define DOMAIN_MAX_SCOPE 0xDFFFFFF

using HilogArgs = struct {
    uint16_t noBlockMode;
    uint8_t nPid;
    uint8_t nNoPid;
    uint8_t nDomain;
    uint8_t nNoDomain;
    uint8_t nTag;
    uint8_t nNoTag;
    uint16_t types;
    uint16_t levels;
    uint16_t headLines;
    uint16_t tailLines;
    std::string domain; // domain recv
    std::string tag; // tag recv
    std::string pids[MAX_PIDS];
    std::string domains[MAX_DOMAINS]; // domains send
    std::string tags[MAX_TAGS]; // tags send
    uint16_t noTypes;
    uint16_t noLevels;
    std::string noPids[MAX_PIDS];
    std::string noDomains[MAX_DOMAINS];
    std::string noTags[MAX_TAGS];
    std::string regexArgs;
    std::string buffSizeArgs;
    std::string logFileCtrlArgs;
    std::string fileSizeArgs;
    std::string fileNumArgs;
    std::string filePathArgs;
    std::string fileNameArgs;
    std::string jobIdArgs;
    std::string personalArgs;
    std::string kmsgArgs;
    std::string logClearArgs;
    std::string logTypeArgs;
    std::string domainArgs;
    std::string statisticArgs;
    std::string tagArgs;
    std::string logLevelArgs;
    std::string flowSwitchArgs;
    std::string flowQuotaArgs;
    std::string pidArgs;
    std::string algorithmArgs;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
