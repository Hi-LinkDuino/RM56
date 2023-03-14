/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "decompile.h"
#include <cctype>
#include "decompile_gen.h"
#include "logger.h"
#include "opcode.h"

using namespace OHOS::Hardware;

Decompile::Decompile(std::string fileName) : isAlign_(false), fileName_(std::move(fileName)) {}

bool Decompile::InitDecompileFile()
{
    file_.open(fileName_.data(), std::ios::binary);
    if (!file_.is_open()) {
        Logger().Error() << "Failed to open decompile file: " << fileName_;
        return false;
    }
    return true;
}

bool Decompile::ReadFile(char *buffer, size_t readSize)
{
    if (!file_.read(buffer, static_cast<std::streamsize>(readSize))) {
        Logger().Error() << "read file failed, read size is: " << readSize;
        return false;
    }
    return true;
}

void Decompile::SetAlign(bool isAlign)
{
    isAlign_ = isAlign;
}

bool Decompile::VerifyDecompileFile()
{
    HcbHeader header {0};
    if (!ReadFile(reinterpret_cast<char *>(&header), sizeof(header))) {
        Logger().Error() << "read header failed";
        return false;
    }
    Logger().Debug() << "read Header: magic is: " << header.magicNumber << " version major: " << header.versionMajor
                     << " version minor: " << header.versionMinor << " checksum: " << header.checkSum
                     << " totalSize: " << header.totalSize;
    if (header.magicNumber != HCB_MAGIC_NUM) {
        Logger().Error() << "magic number is: " << header.magicNumber << ", check failed!";
        return false;
    }
    if (header.totalSize < 0) {
        SetAlign(true);
        header.totalSize = -header.totalSize;
    }
    return true;
}

bool Decompile::ReadUint32(uint32_t &value)
{
    return ReadFile(reinterpret_cast<char *>(&value), sizeof(uint32_t));
}

bool Decompile::ReadUint8(uint8_t &value)
{
    if (GetAlignSize(sizeof(uint8_t)) != sizeof(uint8_t)) {
        uint32_t readValue = 0;
        if (!ReadUint32(readValue)) {
            return false;
        }
        value = static_cast<uint8_t>(readValue);
        return true;
    }
    return ReadFile(reinterpret_cast<char *>(&value), sizeof(uint8_t));
}

bool Decompile::ReadUint16(uint16_t &value)
{
    if (GetAlignSize(sizeof(uint16_t)) != sizeof(uint16_t)) {
        uint32_t readValue = 0;
        if (!ReadUint32(readValue)) {
            return false;
        }
        value = static_cast<uint16_t>(readValue);
        return true;
    }
    return ReadFile(reinterpret_cast<char *>(&value), sizeof(uint16_t));
}

bool Decompile::ReadUint64(uint64_t &value)
{
    return ReadFile(reinterpret_cast<char *>(&value), sizeof(uint64_t));
}

bool Decompile::ReadString(std::string &value)
{
    value.clear();
    char c;
    while (ReadFile(&c, sizeof(c))) {
        if (c == '\0') {
            break;
        }
        if (value.length() > NUMBER) {
            return false;
        }
        value += c;
    }
    uint32_t alignSize = GetAlignSize(value.length() + 1) - (value.length() + 1);
    if (alignSize > 0) {
        char alignReadBuff[4];
        if (!ReadFile(alignReadBuff, alignSize)) {
            return false;
        }
    }
    return true;
}

bool Decompile::GetNextByteCode(uint32_t &byteCode)
{
    if (GetAlignSize(OPCODE_BYTE_WIDTH) == OPCODE_BYTE_WIDTH) {
        uint8_t value = 0;
        bool ret = ReadUint8(value);
        byteCode = value;
        return ret;
    } else {
        return ReadUint32(byteCode);
    }
}

std::shared_ptr<AstObject> Decompile::RebuildNode()
{
    uint32_t nodeHash = static_cast<uint32_t>(file_.tellg()) - GetAlignSize(OPCODE_BYTE_WIDTH);
    std::string nodeName;
    if (!ReadString(nodeName)) {
        return nullptr;
    }

    auto node = std::make_shared<ConfigNode>(nodeName, NODE_NOREF, "");
    uint32_t nodeSize = 0;
    if (!ReadUint32(nodeSize)) {
        return nullptr;
    }
    node->SetSize(nodeSize);
    node->SetHash(nodeHash);
    Logger().Debug() << "node name is: " << node->Name() << ", size is: " << nodeSize << ", hash is: " << nodeHash;
    int32_t pos = file_.tellg();
    int32_t nodeEnd = pos + nodeSize;
    while (pos < nodeEnd) {
        uint32_t childOpCode;
        if (!GetNextByteCode(childOpCode)) {
            Logger().Error() << "Rebuild node failed, get next byte code failed";
            return nullptr;
        }
        auto child = RebuildObject(childOpCode);
        if (child == nullptr) {
            Logger().Error() << "Rebuild node failed, get child failed";
            return nullptr;
        }
        if (!node->AddChild(child)) {
            Logger().Error() << "Rebuild node failed, add child failed";
            return nullptr;
        }
        pos = file_.tellg();
    }
    return node;
}

std::shared_ptr<AstObject> Decompile::RebuildTerm()
{
    std::string termName;
    if (!ReadString(termName)) {
        return nullptr;
    }
    uint32_t childOpCode;
    if (!GetNextByteCode(childOpCode)) {
        return nullptr;
    }
    auto value = RebuildObject(childOpCode);
    if (value == nullptr) {
        return nullptr;
    }
    return std::make_shared<ConfigTerm>(termName, value);
}

std::shared_ptr<AstObject> Decompile::RebuildNodeRefObject()
{
    uint32_t refNodeHash = 0;
    if (!ReadUint32(refNodeHash)) {
        return nullptr;
    }
    Logger().Debug() << "Ref object value is: " << refNodeHash;
    return std::make_shared<AstObject>(std::string(), PARSEROP_NODEREF, refNodeHash);
}

std::shared_ptr<AstObject> Decompile::RebuildNumberObject(uint8_t opCode)
{
    uint8_t u8Value = 0;
    uint16_t u16Value = 0;
    uint32_t u32Value = 0;
    uint64_t u64Value = 0;
    switch (opCode) {
        case HCS_BYTE_OP:
            if (!ReadUint8(u8Value)) {
                return nullptr;
            }
            return std::make_shared<AstObject>(std::string(), PARSEROP_UINT8, u8Value);
        case HCS_WORD_OP:
            if (!ReadUint16(u16Value)) {
                return nullptr;
            }
            return std::make_shared<AstObject>(std::string(), PARSEROP_UINT16, u16Value);
        case HCS_DWORD_OP:
            if (!ReadUint32(u32Value)) {
                return nullptr;
            }
            return std::make_shared<AstObject>(std::string(), PARSEROP_UINT32, u32Value);
        case HCS_QWORD_OP:
            if (!ReadUint64(u64Value)) {
                return nullptr;
            }
            return std::make_shared<AstObject>(std::string(), PARSEROP_UINT64, u64Value);
        default:
            return nullptr;
    }
}

std::shared_ptr<AstObject> Decompile::RebuildArray()
{
    uint16_t arraySize = 0;
    if (!ReadUint16(arraySize)) {
        return nullptr;
    }
    auto array = std::make_shared<AstObject>(std::string(), PARSEROP_ARRAY, 0);
    if (array == nullptr) {
        return nullptr;
    }
    for (uint16_t i = 0; i < arraySize; i++) {
        uint32_t opCode = 0;
        if (!GetNextByteCode(opCode)) {
            return nullptr;
        }
        auto element = RebuildObject(opCode);
        if (element == nullptr) {
            return nullptr;
        }
        if (!array->AddChild(element)) {
            return nullptr;
        }
    }
    return array;
}

std::shared_ptr<AstObject> Decompile::RebuildStringObject()
{
    std::string strValue;
    if (!ReadString(strValue)) {
        return nullptr;
    }
    return std::make_shared<AstObject>(std::string(), PARSEROP_STRING, strValue);
}

std::shared_ptr<AstObject> Decompile::RebuildObject(uint8_t opCode)
{
    switch (opCode) {
        case HCS_NODE_OP:
            return RebuildNode();
        case HCS_TERM_OP:
            return RebuildTerm();
        case HCS_NODEREF_OP:
            return RebuildNodeRefObject();
        case HCS_BYTE_OP:
        case HCS_WORD_OP:
        case HCS_DWORD_OP:
        case HCS_QWORD_OP:
            return RebuildNumberObject(opCode);
        case HCS_ARRAY_OP:
            return RebuildArray();
        case HCS_STRING_OP:
            return RebuildStringObject();
        default:
            Logger().Error() << "Rebuild object failed, unknown OpCode is: " << opCode;
            break;
    }
    return nullptr;
}

std::shared_ptr<Ast> Decompile::RebuildAst()
{
    uint32_t currByteCode = 0;
    if (!GetNextByteCode(currByteCode) || currByteCode != HCS_NODE_OP) {
        Logger().Error() << "Rebuild Ast failed, miss root node!";
        return nullptr;
    }
    auto rootObject = RebuildObject(currByteCode);
    if (rootObject == nullptr) {
        Logger().Error() << "Rebuild Ast failed, rebuild object failed!";
        return nullptr;
    }
    auto ast = std::make_shared<Ast>(rootObject);
    ast->Dump();
    return ast;
}

bool Decompile::DoDecompile()
{
    if (!InitDecompileFile()) {
        return false;
    }

    if (!VerifyDecompileFile()) {
        Logger().Error() << "Verify decompile file failed!";
        return false;
    }
    auto ast = RebuildAst();
    if (ast == nullptr) {
        Logger().Error() << "Rebuild ast failed!";
        return false;
    }
    std::string outPutFileName = Option::Instance().GetOutputName();
    if (outPutFileName.empty()) {
        outPutFileName = Option::Instance().GetSourceName();
    } else if (!isalpha(outPutFileName[outPutFileName.length() - 1])) {
        outPutFileName.append(Option::Instance().GetSourceNameBase());
    }
    DecompileGen decompileGen(ast, outPutFileName);
    return decompileGen.OutPut();
}
