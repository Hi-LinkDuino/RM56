/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_DECOMPILE_H
#define HC_GEN_DECOMPILE_H

#include <fstream>
#include <string>

#include "ast.h"

namespace OHOS {
namespace Hardware {
class Decompile {
public:
    explicit Decompile(std::string fileName);

    ~Decompile() = default;

    bool DoDecompile();

private:
    bool InitDecompileFile();

    bool ReadFile(char *buffer, size_t readSize);

    uint32_t GetAlignSize(uint32_t size)
    {
        if (isAlign_) {
            return (size + ALIGN_SIZE - 1) & (~(ALIGN_SIZE - 1));
        } else {
            return size;
        }
    }

    bool ReadUint8(uint8_t &value);

    bool ReadUint16(uint16_t &value);

    bool ReadUint32(uint32_t &value);

    bool ReadUint64(uint64_t &value);

    bool ReadString(std::string &value);

    void SetAlign(bool isAlign);

    bool VerifyDecompileFile();

    bool GetNextByteCode(uint32_t &byteCode);

    std::shared_ptr<AstObject> RebuildObject(uint8_t opCode);

    std::shared_ptr<AstObject> RebuildNode();

    std::shared_ptr<AstObject> RebuildTerm();

    std::shared_ptr<AstObject> RebuildNodeRefObject();

    std::shared_ptr<AstObject> RebuildNumberObject(uint8_t opCode);

    std::shared_ptr<AstObject> RebuildArray();

    std::shared_ptr<AstObject> RebuildStringObject();

    std::shared_ptr<Ast> RebuildAst();

    bool isAlign_;
    std::string fileName_;
    std::ifstream file_;
};
} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_DECOMPILE_H
