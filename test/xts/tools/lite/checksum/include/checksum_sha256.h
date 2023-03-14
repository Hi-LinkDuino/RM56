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

/*
 * Description: 计算文件SHA256校验和的基本方法
 */

#ifndef CHECKSUM_SHA256_H
#define CHECKSUM_SHA256_H

// 信息摘要结构体
typedef struct {
    unsigned char data[64]; // 消息数据块
    unsigned int dataLen; // 消息数据块长度
    unsigned long long bitLen; // 信息的字节长度
    unsigned int hash[8]; // 哈希值
} MesgDigest;

void InitSha256(MesgDigest *md);
void CalcSha256(MesgDigest *md, unsigned char *mesg);

#endif // CHECKSUM_SHA256_H
