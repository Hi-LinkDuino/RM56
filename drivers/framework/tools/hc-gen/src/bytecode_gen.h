/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_BYTECODE_GEN_H
#define HC_GEN_BYTECODE_GEN_H

#include <fstream>
#include "generator.h"

namespace OHOS {
namespace Hardware {

struct OpCode {
    OpCode() : opCode(0), size(0) {}

    OpCode(uint8_t code, uint32_t s, std::string str) : opCode(code), size(s), opStr(std::move(str)) {}

    ~OpCode() = default;

    uint8_t opCode;
    uint32_t size;
    const std::string opStr;
};

class ByteCodeGen : public Generator {
public:
    explicit ByteCodeGen(std::shared_ptr<Ast> ast);

    ~ByteCodeGen() override = default;

    bool Output() override;

private:
    bool Initialize();

    bool ByteCodeConvert();

    uint32_t Align(uint32_t size) const;

    void CalculateSize(const std::shared_ptr<AstObject> &object);

    bool ByteCodeWrite(bool dummy);

    bool ByteCodeWriteWalk();

    template <typename T>
    void Write(T &data);

    void Write(const char *data, uint32_t size);

    void Write(const std::string &data);

    void FsWrite(const char *data, uint32_t size);

    static const OpCode &ToOpCode(uint32_t objectType);

    bool WriteBad();

    bool HexdumpInitialize(FILE *&in, FILE *&out);

    static bool HexdumpOutput(FILE *in, FILE *out);

    bool Hexdump();

    bool needAlign_;
    std::ofstream ofs_;
    std::string outFileName_;
    bool dummyOutput_;
    uint32_t writeSize_;
};
} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_BYTECODE_GEN_H
