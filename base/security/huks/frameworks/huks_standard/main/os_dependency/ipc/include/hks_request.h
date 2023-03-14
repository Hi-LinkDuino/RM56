/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HKS_REQUEST_H
#define HKS_REQUEST_H

#include "hks_type_inner.h"

enum HksMessage {
#ifndef _HKS_L1_TEE_
    HKS_MSG_BASE = 0x3a400, /* range of message value defined by router. globally unique */
#else
    HKS_MSG_BASE = 1000, /* range of message value defined by SmartLock. Max 65535 */
#endif
    HKS_MSG_GEN_KEY = HKS_MSG_BASE,
    HKS_MSG_IMPORT_KEY,
    HKS_MSG_EXPORT_PUBLIC_KEY,
    HKS_MSG_DELETE_KEY,
    HKS_MSG_GET_KEY_PARAMSET,
    HKS_MSG_KEY_EXIST,
    HKS_MSG_GENERATE_RANDOM,
    HKS_MSG_SIGN,
    HKS_MSG_VERIFY,
    HKS_MSG_ENCRYPT,
    HKS_MSG_DECRYPT,
    HKS_MSG_AGREE_KEY,
    HKS_MSG_DERIVE_KEY,
    HKS_MSG_MAC,
    HKS_MSG_GET_KEY_INFO_LIST,
    HKS_MSG_ATTEST_KEY,
    HKS_MSG_GET_CERTIFICATE_CHAIN,
    HKS_MSG_WRAP_KEY,
    HKS_MSG_UNWRAP_KEY,
    HKS_MSG_PROVISION,
    HKS_MSG_PROVISION_VERIFY,
    HKS_MSG_EXPORT_TRUST_CERTS,
    HKS_MSG_INIT,
    HKS_MSG_UPDATE,
    HKS_MSG_FINISH,
    HKS_MSG_ABORT,
    HKS_MSG_MAX, /* new cmd type must be added before HKS_MSG_MAX */
};

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SendRequest - Send the request message to target module by function call or ipc or other ways.
 * @type:        the request message type.
 * @inBlob:      the input serialized data blob.
 * @outBlob:     the output serialized data blob, can be null.
 */
int32_t HksSendRequest(enum HksMessage type, const struct HksBlob *inBlob, struct HksBlob *outBlob,
    const struct HksParamSet *paramSet);

#ifdef __cplusplus
}
#endif

#endif /* HKS_REQUEST_H */
