/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "bytecode_gen.h"
#include <string>
#include "file.h"
#include "logger.h"
#include "opcode.h"

using namespace OHOS::Hardware;

ByteCodeGen::ByteCodeGen(std::shared_ptr<Ast> ast) :
    Generator(ast),
    needAlign_(false),
    dummyOutput_(false),
    writeSize_(0)
{
}

bool ByteCodeGen::Output()
{
    if (!Initialize()) {
        return false;
    }

    if (!ByteCodeConvert()) {
        return false;
    }

    if (!ByteCodeWrite(true)) {
        return false;
    }

    if (!ByteCodeWrite(false)) {
        return false;
    }

    if (Option::Instance().ShouldGenHexDump()) {
        return Hexdump();
    }

    return true;
}

bool ByteCodeGen::Initialize()
{
    auto opt = Option::Instance();
    std::string outFileName = Util::File::StripSuffix(opt.GetOutputName());
    if (outFileName.empty()) {
        outFileName = opt.GetSourceNameBase() + ".hcb";
    }

    if (outFileName.find(".hcb") == std::string::npos) {
        outFileName.append(".hcb");
    }

    ofs_.open(outFileName, std::ofstream::out | std::ofstream::binary);
    if (!ofs_.is_open()) {
        Logger().Error() << "failed to open output file: " << outFileName;
        return false;
    }
    Logger().Debug() << "output: " << outFileName;

    needAlign_ = opt.ShouldAlign();
    outFileName_ = std::move(outFileName);
    return true;
}

bool ByteCodeGen::ByteCodeConvert()
{
    return ast_->WalkBackward([this](std::shared_ptr<AstObject> &object, int32_t depth) {
        if (object->IsNode() && ConfigNode::CastFrom(object)->GetNodeType() == NODE_TEMPLATE) {
            object->Separate();
            return NOERR;
        }
        auto opcode = ToOpCode(object->Type());
        if (opcode.opCode == 0) {
            Logger().Error() << object->SourceInfo() << "cannot covert type " << object->Type() << " to opcode";
            return EINVALF;
        }
        object->SetOpCode(opcode.opCode);
        CalculateSize(object);
        return NOERR;
    });
}

uint32_t ByteCodeGen::Align(uint32_t size) const
{
    return needAlign_ ? ((size + ALIGN_SIZE - 1) & (~(ALIGN_SIZE - 1))) : size;
}

const OpCode &ByteCodeGen::ToOpCode(uint32_t objectType)
{
    static std::map<uint32_t, OpCode> byteCodeMap = {
        {PARSEROP_UINT8,    {HCS_BYTE_OP, BYTE_SIZE, "Uint8"}      },
        {PARSEROP_UINT16,   {HCS_WORD_OP, WORD_SIZE, "Uint16"}     },
        {PARSEROP_UINT32,   {HCS_DWORD_OP, DWORD_SIZE, "Uint32"}   },
        {PARSEROP_UINT64,   {HCS_QWORD_OP, QWORD_SIZE, "Uint64"}   },
        {PARSEROP_STRING,   {HCS_STRING_OP, 0, "String"}           },
        {PARSEROP_ARRAY,    {HCS_ARRAY_OP, WORD_SIZE, "Array"}     }, /* ElementCount - WORD */
        {PARSEROP_CONFNODE, {HCS_NODE_OP, DWORD_SIZE, "ConfigNode"}}, /* SubSize - DWORD */
        {PARSEROP_CONFTERM, {HCS_TERM_OP, 0, "ConfigTerm"}         },
        {PARSEROP_NODEREF,  {HCS_NODEREF_OP, DWORD_SIZE, "NodeRef"}}, /* RefHashCode - DWORD */
    };
    return byteCodeMap[objectType];
}

void ByteCodeGen::Write(const std::string &data)
{
    Write(data.c_str(), static_cast<uint32_t>(data.size() + 1));
}

template <typename T>
void ByteCodeGen::Write(T &data)
{
    auto p = &data;
    uint32_t size = sizeof(data);
    auto d = reinterpret_cast<const char *>(p);
    Write(d, size);
}

void ByteCodeGen::Write(const char *data, uint32_t size)
{
    FsWrite(data, size);
    static char stubData[ALIGN_SIZE] = {0};
    auto alignSize = Align(size);
    auto stubSize = alignSize - size;

    if (stubSize != 0) {
        FsWrite(stubData, stubSize);
    }

    writeSize_ += alignSize;
}

void ByteCodeGen::CalculateSize(const std::shared_ptr<AstObject> &object)
{
    uint32_t size = Align(OPCODE_BYTE_WIDTH) + Align(ToOpCode(object->Type()).size);
    switch (object->OpCode()) {
        case HCS_NODE_OP: /* fall-through */
        case HCS_TERM_OP:
            /* name string */
            size += Align(object->Name().size() + 1); // add 1 for '\0'
            break;
        case HCS_STRING_OP:
            size += Align(object->StringValue().size() + 1);
            break;
        default:
            break;
    }

    auto child = object->Child();
    uint32_t subSize = 0;
    while (child != nullptr) {
        subSize += child->GetSize();
        child = child->Next();
    }

    object->SetSize(subSize + size);
    object->SetSubSize(subSize);
}

bool ByteCodeGen::ByteCodeWrite(bool dummy)
{
    dummyOutput_ = dummy;
    writeSize_ = 0;

    HcbHeader header = {
        .magicNumber = HCB_MAGIC_NUM,
        .versionMajor = 0,
        .versionMinor = 0,
        .checkSum = 0,
        .totalSize = static_cast<int32_t>(
            Option::Instance().ShouldAlign() ? -ast_->GetAstRoot()->GetSize() : ast_->GetAstRoot()->GetSize()),
    };
    Option::Instance().GetVersion(header.versionMinor, header.versionMajor);
    Write(header);
    if (WriteBad()) {
        return false;
    }

    return ByteCodeWriteWalk();
}

bool ByteCodeGen::ByteCodeWriteWalk()
{
    return ast_->WalkForward([this](std::shared_ptr<AstObject> &current, int32_t depth) {
        current->SetHash(writeSize_);
        auto opcode = current->OpCode();
        Write(opcode);
        switch (current->OpCode()) {
            case HCS_BYTE_OP:
            case HCS_WORD_OP:
            case HCS_DWORD_OP:
            case HCS_QWORD_OP: {
                auto value = current->IntegerValue();
                Write(reinterpret_cast<const char *>(&value), ToOpCode(current->Type()).size);
                break;
            }
            case HCS_STRING_OP:
                Write(current->StringValue());
                break;
            case HCS_TERM_OP:
                Write(current->Name());

                break;
            case HCS_NODE_OP: {
                Write(current->Name());
                auto subSize = current->GetSubSize();
                Write(subSize);
                break;
            }
            case HCS_ARRAY_OP: {
                uint16_t arraySize = ConfigArray::CastFrom(current)->ArraySize();
                Write(arraySize);
                break;
            }
            case HCS_NODEREF_OP: {
                auto term = ConfigTerm::CastFrom(current->Parent());
                uint32_t hashCode = term->RefNode().lock()->GetHash();
                Write(hashCode);
                break;
            }
            default:
                break;
        }
        if (WriteBad()) {
            return EOUTPUT;
        }
        return NOERR;
    });
}

void ByteCodeGen::FsWrite(const char *data, uint32_t size)
{
    if (dummyOutput_)
        return;

    ofs_.write(data, size);
}

bool ByteCodeGen::WriteBad()
{
    if (ofs_.bad()) {
        Logger().Error() << "failed to write file " << outFileName_;
        return true;
    }
    return false;
}

bool ByteCodeGen::HexdumpInitialize(FILE *&in, FILE *&out)
{
    ofs_.close();
    std::string hexdumpOutName = Util::File::StripSuffix(outFileName_).append("_hex.c");

    in = fopen(outFileName_.data(), "rb");
    if (in == nullptr) {
        Logger().Error() << "failed to open " << outFileName_;
        return false;
    }

    out = fopen(hexdumpOutName.data(), "wb");
    if (out == nullptr) {
        fclose(in);
        in = nullptr;
        Logger().Error() << "failed to open " << hexdumpOutName;
        return false;
    }
    return true;
}

bool ByteCodeGen::Hexdump()
{
    FILE *in = nullptr;
    FILE *out = nullptr;
    if (!HexdumpInitialize(in, out)) {
        return false;
    }

    auto ret = HexdumpOutput(in, out);
    fclose(in);
    fclose(out);

    return ret;
}

bool ByteCodeGen::HexdumpOutput(FILE *in, FILE *out)
{
    constexpr const char *HCS_HEXDUMP_ENTRY_SYMBOL = "hdfConfigEntrySymbol";
    constexpr const int PRINT_SKIP_STEP = 2;
    constexpr const int NUMS_PER_LINE = 16;
    std::string prefix = Option::Instance().GetSymbolPrefix();
    if (fprintf(out, "static const unsigned char g_%s%s[] = {\n", prefix.data(), HCS_HEXDUMP_ENTRY_SYMBOL) < 0) {
        return false;
    }
    uint32_t writeCount = 0;
    int32_t byte;
    while ((byte = getc(in)) != EOF) {
        if (fprintf(out, "%s0x%02x", (writeCount % NUMS_PER_LINE) ? ", " : &",\n    "[PRINT_SKIP_STEP * !writeCount],
                    byte) < 0) {
            return false;
        }
        writeCount++;
    }
    if (fprintf(out, "\n};\n") < 0) {
        return false;
    }

    if (fprintf(out, "static const unsigned int g_%sLen = %u;\n", HCS_HEXDUMP_ENTRY_SYMBOL, writeCount) < 0) {
        return false;
    }
    if (fprintf(out,
                "void HdfGetBuildInConfigData(const unsigned char** data, unsigned int* size)\n"
                "{\n"
                "    *data = g_%s%s;\n"
                "    *size = g_%s%sLen;\n"
                "}",
                prefix.data(), HCS_HEXDUMP_ENTRY_SYMBOL, prefix.data(), HCS_HEXDUMP_ENTRY_SYMBOL) < 0) {
        return false;
    }
    return true;
}
