/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef TICKET_VERIFY_H
#define TICKET_VERIFY_H

#include "common/export_define.h"
#include "common/hap_byte_buffer.h"
#include "common/random_access_file.h"

#include "provision/provision_info.h"
#include "provision/provision_verify.h"

namespace OHOS {
namespace Security {
namespace Verify {
enum TicketVerifyResultCode {
    TICKET_VERIFY_SUCCESS = 0,
    OPEN_TICKET_FILE_ERROR = -1,
    SIGNATURE_VERIFY_FAIL = -2,
    TICKET_OK = -3,
    TICKET_NOT_MATCH = -4,
    TICKET_DEVICE_INVALID = -5,
    TICKET_PARSE_FAIL = -6,
    TICKET_PERMISSION_ERROR = -7,
    TICKET_READ_FAIL = -8,
};

/**
 * @brief Parse and verify the ticket
 * @param profileInfo input param, the parsed profile structure.
 * @return ticket verify result.
 */
DLL_EXPORT bool CheckTicketSource(const ProvisionInfo& profileInfo);
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // TICKET_VERIFY_H
