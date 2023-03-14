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

#include "parcel.h"
#include "securec.h"
#include "utils_log.h"

namespace OHOS {

static const size_t DEFAULT_CPACITY = 204800; // 200K
static const size_t CAPACITY_THRESHOLD = 4096; // 4k

Parcelable::Parcelable() : Parcelable(false)
{}

Parcelable::Parcelable(bool asRemote)
{
    asRemote_ = asRemote;
    behavior_ = 0;
}

Parcel::Parcel(Allocator *allocator)
{
    if (allocator != nullptr) {
        allocator_ = allocator;
    } else {
        allocator_ = new DefaultAllocator();
    }

    writeCursor_ = 0;
    readCursor_ = 0;

    data_ = nullptr;
    dataSize_ = 0;
    dataCapacity_ = 0;

    maxDataCapacity_ = DEFAULT_CPACITY;
    objectOffsets_ = nullptr;
    objectCursor_ = 0;
    objectsCapacity_ = 0;
}

Parcel::Parcel() : Parcel(new DefaultAllocator())
{}

Parcel::~Parcel()
{
    FlushBuffer();
    delete allocator_;
}

size_t Parcel::GetWritableBytes() const
{
    if (dataCapacity_ > writeCursor_) {
        return dataCapacity_ - writeCursor_;
    }

    return 0;
}

size_t Parcel::GetReadableBytes() const
{
    if (dataSize_ > readCursor_) {
        return dataSize_ - readCursor_;
    }

    return 0;
}

size_t Parcel::CalcNewCapacity(size_t minNewCapacity)
{
    size_t threshold = CAPACITY_THRESHOLD;

    if (minNewCapacity == threshold) {
        return threshold;
    }

    // If over threshold, step by threshold.
    if (minNewCapacity > threshold) {
        size_t newCapacity = minNewCapacity / threshold * threshold;

        if ((maxDataCapacity_ > 0) && (newCapacity > maxDataCapacity_ - threshold)) {
            newCapacity = maxDataCapacity_;
        } else {
            newCapacity += threshold;
        }

        return newCapacity;
    }

    // Not over threshold. Double it.
    size_t newCapacity = 64;

    while (newCapacity < minNewCapacity) {
        newCapacity = newCapacity * 2;
    }

    if ((maxDataCapacity_ > 0) && (newCapacity > maxDataCapacity_)) {
        newCapacity = maxDataCapacity_;
    }

    return newCapacity;
}

bool Parcel::EnsureWritableCapacity(size_t desireCapacity)
{
    if (!writable_) {
        UTILS_LOGW("this parcel data is alloc by driver, which is can not be writen");
        return false;
    }
    if (desireCapacity <= GetWritableBytes()) {
        return true;
    }

    size_t minNewCapacity = desireCapacity + writeCursor_;
    size_t newCapacity = CalcNewCapacity(minNewCapacity);
    if ((newCapacity <= dataCapacity_) || (newCapacity < minNewCapacity)) {
        UTILS_LOGW("Failed to ensure parcel capacity, newCapacity = %{public}zu, dataCapacity_ = %{public}zu, "
                   "minNewCapacity = %{public}zu",
                   newCapacity, dataCapacity_, minNewCapacity);
        return false;
    }

    if (allocator_ != nullptr) {
        void *newData = allocator_->Realloc(data_, newCapacity);
        if (newData != nullptr) {
            data_ = reinterpret_cast<uint8_t *>(newData);
            dataCapacity_ = newCapacity;
            return true;
        }
        UTILS_LOGW("Failed to realloc parcel capacity, newCapacity = %{public}zu, dataCapacity_ = %{public}zu",
                   newCapacity, dataCapacity_);
    }

    return false;
}

size_t Parcel::GetDataSize() const
{
    return dataSize_;
}

uintptr_t Parcel::GetData() const
{
    return reinterpret_cast<uintptr_t>(data_);
}

binder_size_t Parcel::GetObjectOffsets() const
{
    return reinterpret_cast<binder_size_t>(objectOffsets_);
}

size_t Parcel::GetOffsetsSize() const
{
    return objectCursor_;
}

size_t Parcel::GetDataCapacity() const
{
    return dataCapacity_;
}

bool Parcel::SetMaxCapacity(size_t maxCapacity)
{
    if (maxCapacity > maxDataCapacity_) {
        maxDataCapacity_ = maxCapacity;
        return true;
    }

    return false;
}

bool Parcel::SetAllocator(Allocator *allocator)
{
    if ((allocator == nullptr) || (allocator_ == allocator)) {
        return false;
    }

    if ((data_ != nullptr) && (dataSize_ > 0)) {
        if (allocator_ == nullptr) {
            return false;
        }

        void *newData = allocator->Alloc(dataSize_);
        if (newData == nullptr) {
            UTILS_LOGE("Failed to alloc parcel size, dataSize_ = %{public}zu", dataSize_);
            return false;
        }

        if (memcpy_s(newData, dataSize_, data_, dataSize_) != EOK) {
            allocator->Dealloc(newData);
            return false;
        }
        allocator_->Dealloc(data_);
        data_ = reinterpret_cast<uint8_t *>(newData);
        dataCapacity_ = dataSize_;
    }

    delete allocator_;
    allocator_ = allocator;
    return true;
}

bool Parcel::CheckOffsets()
{
    size_t readPos = readCursor_;
    if ((readPos + sizeof(parcel_flat_binder_object)) > dataSize_) {
        UTILS_LOGW("CheckOffsets Invalid obj, obj size overflow. objSize:%{public}zu, dataSize:%{public}zu",
            readPos + sizeof(parcel_flat_binder_object), dataSize_);
        return false;
    }

    size_t objSize = objectCursor_;
    binder_size_t *objects = objectOffsets_;
    size_t objCount = 0;
    while (objCount < objSize) {
        if (objects[objCount] == readPos) {
            return true;
        }
        objCount++;
    }
    UTILS_LOGW("CheckOffsets Invalid obj: obj not found.");
    return false;
}

void Parcel::InjectOffsets(binder_size_t offsets, size_t offsetSize)
{
    if (offsetSize <= 0) {
        return;
    }

    auto *newObjectOffsets = reinterpret_cast<binder_size_t *>(offsets);
    for (size_t index = 0; index < offsetSize; index++) {
        if (EnsureObjectsCapacity()) {
            WriteObjectOffset(newObjectOffsets[index]);
        }
    }
}

void Parcel::FlushBuffer()
{
    if (allocator_ == nullptr) {
        return;
    }

    if (data_ != nullptr) {
        allocator_->Dealloc(data_);
        dataSize_ = 0;
        writeCursor_ = 0;
        readCursor_ = 0;
        dataCapacity_ = 0;
        data_ = nullptr;
    }

    if (objectOffsets_) {
        objectHolder_.clear();
        free(objectOffsets_);
        objectCursor_ = 0;
        objectOffsets_ = nullptr;
        objectsCapacity_ = 0;
    }
}

bool Parcel::SetDataCapacity(size_t newCapacity)
{
    if (allocator_ == nullptr || dataSize_ >= newCapacity) {
        return false;
    }

    void *newData = allocator_->Realloc(data_, newCapacity);
    if (newData != nullptr) {
        data_ = reinterpret_cast<uint8_t *>(newData);
        dataCapacity_ = newCapacity;
        return true;
    }
    return false;
}

bool Parcel::SetDataSize(size_t dataSize)
{
    if (dataSize > dataCapacity_) {
        return false;
    }

    dataSize_ = dataSize;
    return true;
}

bool Parcel::WriteDataBytes(const void *data, size_t size)
{
    void *dest = data_ + writeCursor_;
    size_t writableBytes = GetWritableBytes();
    if (memcpy_s(dest, writableBytes, data, size) != EOK) {
        return false;
    }
    writeCursor_ += size;
    dataSize_ += size;
    return true;
}

void Parcel::WritePadBytes(size_t padSize)
{
    uint8_t *dest = data_ + writeCursor_;
    static const int MAX_MASK_NUM = 4;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    static const size_t mask[MAX_MASK_NUM] = { 0xFFFFFFFF, 0x00ffffff, 0x0000ffff, 0x000000ff };
#else
    static const size_t mask[MAX_MASK_NUM] = { 0xFFFFFFFF, 0xffffff00, 0xffff0000, 0xff000000 };
#endif
    *reinterpret_cast<uint32_t *>(dest + padSize - MAX_MASK_NUM) &= mask[padSize];
    writeCursor_ += padSize;
    dataSize_ += padSize;
}

bool Parcel::WriteBuffer(const void *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    size_t padSize = GetPadSize(size);
    size_t desireCapacity = size + padSize;

    // in case of desireCapacity overflow
    if (desireCapacity < size || desireCapacity < padSize) {
        return false;
    }

    if (EnsureWritableCapacity(desireCapacity)) {
        if (!WriteDataBytes(data, size)) {
            return false;
        }
        WritePadBytes(padSize);
        return true;
    }

    return false;
}

bool Parcel::WriteBufferAddTerminator(const void *data, size_t size, size_t typeSize)
{
    if (data == nullptr || size < typeSize) {
        return false;
    }

    size_t padSize = GetPadSize(size);
    size_t desireCapacity = size + padSize;

    // in case of desireCapacity overflow
    if (desireCapacity < size || desireCapacity < padSize) {
        return false;
    }

    if (EnsureWritableCapacity(desireCapacity)) {
        if (!WriteDataBytes(data, size - typeSize)) {
            return false;
        }

        // Reserved for 32 bits
        const char terminator[] = {0, 0, 0, 0};
        if (!WriteDataBytes(terminator, typeSize)) {
            return false;
        }
        WritePadBytes(padSize);
        return true;
    }

    return false;
}

bool Parcel::WriteUnpadBuffer(const void *data, size_t size)
{
    return WriteBuffer(data, size);
}

template <typename T>
bool Parcel::Write(T value)
{
    size_t desireCapacity = sizeof(T);

    if (EnsureWritableCapacity(desireCapacity)) {
        *reinterpret_cast<T *>(data_ + writeCursor_) = value;
        writeCursor_ += desireCapacity;
        dataSize_ += desireCapacity;
        return true;
    }

    return false;
}

bool Parcel::WriteBool(bool value)
{
    return Write<int32_t>(static_cast<int32_t>(value));
}

bool Parcel::WriteBoolUnaligned(bool value)
{
    return Write<bool>(value);
}

bool Parcel::WriteInt8(int8_t value)
{
    return Write<int32_t>(static_cast<int32_t>(value));
}

bool Parcel::WriteInt8Unaligned(int8_t value)
{
    return Write<int8_t>(value);
}

bool Parcel::WriteInt16(int16_t value)
{
    return Write<int32_t>(static_cast<int32_t>(value));
}

bool Parcel::WriteInt16Unaligned(int16_t value)
{
    return Write<int16_t>(value);
}

bool Parcel::WriteInt32(int32_t value)
{
    return Write<int32_t>(value);
}

bool Parcel::WriteInt64(int64_t value)
{
    return Write<int64_t>(value);
}

bool Parcel::WriteUint8(uint8_t value)
{
    return Write<uint32_t>(static_cast<uint32_t>(value));
}

bool Parcel::WriteUint8Unaligned(uint8_t value)
{
    return Write<uint8_t>(value);
}

bool Parcel::WriteUint16(uint16_t value)
{
    return Write<uint32_t>(static_cast<uint32_t>(value));
}

bool Parcel::WriteUint16Unaligned(uint16_t value)
{
    return Write<uint16_t>(value);
}

bool Parcel::WriteUint32(uint32_t value)
{
    return Write<uint32_t>(value);
}

bool Parcel::WriteUint64(uint64_t value)
{
    return Write<uint64_t>(value);
}

bool Parcel::WriteFloat(float value)
{
    return Write<float>(value);
}

bool Parcel::WriteDouble(double value)
{
    return Write<double>(value);
}

bool Parcel::WritePointer(uintptr_t value)
{
    return Write<binder_uintptr_t>(value);
}

bool Parcel::WriteCString(const char *value)
{
    if (value == nullptr) {
        return false;
    }
    int32_t dataLength = strlen(value);
    int32_t desireCapacity = (dataLength + 1) * sizeof(char);
    return WriteBuffer(value, desireCapacity);
}

bool Parcel::WriteString(const std::string &value)
{
    if (value.data() == nullptr) {
        return WriteInt32(-1);
    }

    int32_t dataLength = value.length();
    int32_t typeSize = sizeof(char);
    int32_t desireCapacity = dataLength + typeSize;

    if (!Write<int32_t>(dataLength)) {
        return false;
    }

    return WriteBufferAddTerminator(value.data(), desireCapacity, typeSize);
}

bool Parcel::WriteString16(const std::u16string &value)
{
    if (value.data() == nullptr) {
        return WriteInt32(-1);
    }

    int32_t dataLength = value.length();
    int32_t typeSize = sizeof(char16_t);
    int32_t desireCapacity = (dataLength + 1) * typeSize;

    if (!Write<int32_t>(dataLength)) {
        return false;
    }

    return WriteBufferAddTerminator(value.data(), desireCapacity, typeSize);
}

bool Parcel::WriteString16WithLength(const char16_t *value, size_t len)
{
    if (!value) {
        return WriteInt32(-1);
    }

    int32_t dataLength = len;
    int32_t typeSize = sizeof(char16_t);
    int32_t desireCapacity = (dataLength + 1) * typeSize;
    std::u16string u16str(reinterpret_cast<const char16_t *>(value), len);

    if (!Write<int32_t>(dataLength)) {
        return false;
    }

    return WriteBufferAddTerminator(u16str.data(), desireCapacity, typeSize);
}

bool Parcel::WriteString8WithLength(const char *value, size_t len)
{
    if (!value) {
        return WriteInt32(-1);
    }

    int32_t dataLength = len;
    int32_t typeSize = sizeof(char);
    int32_t desireCapacity = (dataLength + 1) * typeSize;

    if (!Write<int32_t>(dataLength)) {
        return false;
    }

    return WriteBufferAddTerminator(value, desireCapacity, typeSize);
}

bool Parcel::EnsureObjectsCapacity()
{
    if ((objectsCapacity_ - objectCursor_) >= 1) {
        return true;
    }

    if (allocator_ == nullptr) {
        return false;
    }

    const int NEW_CAPACITY_ADD = 2;
    const int NEW_CAPACITY_MULTI = 3;
    const int NEW_CAPACITY_DIV = 2;
    size_t newCapacity = ((objectsCapacity_ + NEW_CAPACITY_ADD) * NEW_CAPACITY_MULTI) / NEW_CAPACITY_DIV;
    size_t newBytes = newCapacity * sizeof(binder_size_t);

    void *newOffsets = realloc(objectOffsets_, newBytes);
    if (newOffsets == nullptr) {
        return false;
    }

    objectOffsets_ = reinterpret_cast<binder_size_t *>(newOffsets);
    objectsCapacity_ = newCapacity;
    return true;
}

bool Parcel::WriteObjectOffset(binder_size_t offset)
{
    if (offset > dataSize_) {
        return false;
    }

    for (size_t index = 0; index < objectCursor_; index++) {
        if (objectOffsets_[index] == offset) {
            return false;
        }
    }

    objectOffsets_[objectCursor_] = offset;
    objectCursor_++;
    return true;
}

bool Parcel::WriteRemoteObject(const Parcelable *object)
{
    size_t placeholder = writeCursor_;
    // Parcelable is nullptr
    if ((object == nullptr) || (!object->asRemote_)) {
        return false;
    }

    if (!EnsureObjectsCapacity()) {
        return false;
    }

    if (!object->Marshalling(*this)) {
        return false;
    }

    WriteObjectOffset(placeholder);

    if (object->TestBehavior(Parcelable::BehaviorFlag::HOLD_OBJECT)) {
        objectHolder_.push_back(const_cast<Parcelable *>(object));
    }

    return true;
}

bool Parcel::WriteParcelable(const Parcelable *object)
{
    size_t placeholder = writeCursor_;
    size_t restorSize = dataSize_;

    // Parcelable is nullptr
    if (object == nullptr) {
        // write the meta data to indicate pass an null object.
        return WriteInt32(0);
    }

    if (!object->asRemote_) {
        // meta data indicate we have an parcelable object.
        if (!WriteInt32(1)) {
            return false;
        }

        return object->Marshalling(*this);
    }

    // Write the remote object flag
    if (!WriteInt32(1)) {
        return false;
    }

    if (WriteRemoteObject(const_cast<Parcelable*>(object))) {
        return true;
    }

    // rollback the write position.
    writeCursor_ = placeholder;
    dataSize_ = restorSize;
    return false;
}

bool Parcel::WriteStrongParcelable(const sptr<Parcelable> &object)
{
    if (object == nullptr) {
        WriteInt32(0);
        return true;
    }

    object->SetBehavior(Parcelable::BehaviorFlag::HOLD_OBJECT);
    return WriteParcelable(object.GetRefPtr());
}

template <typename T>
bool Parcel::Read(T &value)
{
    size_t desireCapacity = sizeof(T);

    if (desireCapacity <= GetReadableBytes()) {
        const void *data = data_ + readCursor_;
        readCursor_ += desireCapacity;
        value = *reinterpret_cast<const T *>(data);
        return true;
    }

    return false;
}

template <typename T>
T Parcel::Read()
{
    T lvalue {};
    return Read<T>(lvalue) ? lvalue : 0;
}

bool Parcel::ParseFrom(uintptr_t data, size_t size)
{
    if (data_ != nullptr) {
        return false;
    }

    data_ = reinterpret_cast<uint8_t *>(data);
    dataCapacity_ = size;
    dataSize_ = size;
    /* data is alloc by driver, can not write again */
    writable_ = false;
    return true;
}

const uint8_t *Parcel::ReadBuffer(size_t length)
{
    if (GetReadableBytes() >= length) {
        uint8_t *buffer = data_ + readCursor_;
        readCursor_ += length;
        return buffer;
    }

    return nullptr;
}

const uint8_t *Parcel::ReadUnpadBuffer(size_t length)
{
    if (GetReadableBytes() >= length) {
        uint8_t *buffer = data_ + readCursor_;
        readCursor_ += length;
        SkipBytes(GetPadSize(length));
        return buffer;
    }

    return nullptr;
}

void Parcel::SkipBytes(size_t bytes)
{
    if (GetReadableBytes() >= bytes) {
        readCursor_ += bytes;
    } else if (readCursor_ < dataCapacity_) {
        readCursor_ = dataCapacity_;
    }
}

size_t Parcel::GetReadPosition()
{
    return readCursor_;
}

bool Parcel::RewindRead(size_t newPosition)
{
    if (newPosition > dataSize_) {
        return false;
    }
    readCursor_ = newPosition;
    return true;
}

size_t Parcel::GetWritePosition()
{
    return writeCursor_;
}

bool Parcel::RewindWrite(size_t newPosition)
{
    if (newPosition > dataSize_) {
        return false;
    }
    writeCursor_ = newPosition;
    dataSize_ = newPosition;
    return true;
}

bool Parcel::ReadBool()
{
    int32_t temp = Read<int32_t>();
    return (temp != 0);
}

bool Parcel::ReadBoolUnaligned()
{
    return Read<bool>();
}

int8_t Parcel::ReadInt8()
{
    int32_t temp = Read<int32_t>();
    return static_cast<int8_t>(temp);
}

int16_t Parcel::ReadInt16()
{
    int32_t temp = Read<int32_t>();
    return static_cast<int16_t>(temp);
}

int32_t Parcel::ReadInt32()
{
    return Read<int32_t>();
}

int64_t Parcel::ReadInt64()
{
    return Read<int64_t>();
}

uint8_t Parcel::ReadUint8()
{
    uint32_t temp = Read<uint32_t>();
    return static_cast<uint8_t>(temp);
}

uint16_t Parcel::ReadUint16()
{
    uint32_t temp = Read<uint32_t>();
    return static_cast<uint16_t>(temp);
}

uint32_t Parcel::ReadUint32()
{
    return Read<uint32_t>();
}

uint64_t Parcel::ReadUint64()
{
    return Read<uint64_t>();
}

float Parcel::ReadFloat()
{
    return Read<float>();
}

double Parcel::ReadDouble()
{
    return Read<double>();
}

template <typename T>
bool Parcel::ReadPadded(T &value)
{
    int32_t temp;
    bool result = Read<int32_t>(temp);
    if (result) {
        value = static_cast<T>(temp);
    }

    return result;
}

bool Parcel::ReadBool(bool &value)
{
    return ReadPadded<bool>(value);
}

bool Parcel::ReadInt8(int8_t &value)
{
    return ReadPadded<int8_t>(value);
}

bool Parcel::ReadInt8Unaligned(int8_t &value)
{
    return Read<int8_t>(value);
}

bool Parcel::ReadInt16(int16_t &value)
{
    return ReadPadded<int16_t>(value);
}

bool Parcel::ReadInt16Unaligned(int16_t &value)
{
    return Read<int16_t>(value);
}

bool Parcel::ReadInt32(int32_t &value)
{
    return Read<int32_t>(value);
}

bool Parcel::ReadInt64(int64_t &value)
{
    return Read<int64_t>(value);
}

bool Parcel::ReadUint8(uint8_t &value)
{
    return ReadPadded<uint8_t>(value);
}

bool Parcel::ReadUint8Unaligned(uint8_t &value)
{
    return Read<uint8_t>(value);
}

bool Parcel::ReadUint16(uint16_t &value)
{
    return ReadPadded<uint16_t>(value);
}

bool Parcel::ReadUint16Unaligned(uint16_t &value)
{
    return Read<uint16_t>(value);
}

bool Parcel::ReadUint32(uint32_t &value)
{
    return Read<uint32_t>(value);
}

bool Parcel::ReadUint64(uint64_t &value)
{
    return Read<uint64_t>(value);
}

bool Parcel::ReadFloat(float &value)
{
    return Read<float>(value);
}

bool Parcel::ReadDouble(double &value)
{
    return Read<double>(value);
}

uintptr_t Parcel::ReadPointer()
{
    return Read<binder_uintptr_t>();
}

const char *Parcel::ReadCString()
{
    size_t oldCursor = readCursor_;
    const size_t avail = GetReadableBytes();
    const char* cstr = reinterpret_cast<const char*>(data_ + readCursor_);
    // is the string's trailing NUL within the parcel's valid bounds?
    const char* eos = reinterpret_cast<const char*>(memchr(cstr, 0, avail));
    if (eos != nullptr) {
        const size_t dataLength = eos - cstr;
        readCursor_ += (dataLength + 1);
        SkipBytes(GetPadSize(dataLength + 1));
        return cstr;
    }
    readCursor_ = oldCursor;
    return nullptr;
}

const std::string Parcel::ReadString()
{
    int32_t dataLength = 0;
    size_t oldCursor = readCursor_;

    if (!Read<int32_t>(dataLength) || dataLength < 0) {
        return std::string();
    }

    size_t readCapacity = dataLength + 1;
    if ((readCapacity > (size_t)dataLength) && (readCapacity <= GetReadableBytes())) {
        const uint8_t *dest = ReadBuffer(readCapacity);
        if (dest != nullptr) {
            const auto *str = reinterpret_cast<const char *>(dest);
            SkipBytes(GetPadSize(readCapacity));
            if (str[dataLength] == 0) {
                return std::string(str, dataLength);
            }
        }
    }

    readCursor_ = oldCursor;
    return std::string();
}

bool Parcel::ReadString(std::string &value)
{
    int32_t dataLength = 0;
    size_t oldCursor = readCursor_;

    if (!Read<int32_t>(dataLength) || dataLength < 0) {
        value = std::string();
        return false;
    }

    size_t readCapacity = dataLength + 1;
    if ((readCapacity > (size_t)dataLength) && (readCapacity <= GetReadableBytes())) {
        const uint8_t *dest = ReadBuffer(readCapacity);
        if (dest != nullptr) {
            const auto *str = reinterpret_cast<const char *>(dest);
            SkipBytes(GetPadSize(readCapacity));
            if (str[dataLength] == 0) {
                value = std::string(str, dataLength);
                return true;
            }
        }
    }

    readCursor_ = oldCursor;
    value = std::string();
    return false;
}

const std::u16string Parcel::ReadString16()
{
    int32_t dataLength = 0;
    size_t oldCursor = readCursor_;

    if (!Read<int32_t>(dataLength) || dataLength < 0) {
        return std::u16string();
    }

    size_t readCapacity = (dataLength + 1) * sizeof(char16_t);
    if ((readCapacity > (size_t)dataLength) && (readCapacity <= GetReadableBytes())) {
        const uint8_t *str = ReadBuffer(readCapacity);
        if (str != nullptr) {
            const auto *u16Str = reinterpret_cast<const char16_t *>(str);
            SkipBytes(GetPadSize(readCapacity));
            if (u16Str[dataLength] == 0) {
                return std::u16string(u16Str, dataLength);
            }
        }
    }

    readCursor_ = oldCursor;
    return std::u16string();
}

bool Parcel::ReadString16(std::u16string &value)
{
    int32_t dataLength = 0;
    size_t oldCursor = readCursor_;

    if (!Read<int32_t>(dataLength) || dataLength < 0) {
        value = std::u16string();
        return false;
    }

    size_t readCapacity = (dataLength + 1) * sizeof(char16_t);
    if (readCapacity <= GetReadableBytes()) {
        const uint8_t *str = ReadBuffer(readCapacity);
        if (str != nullptr) {
            const auto *u16Str = reinterpret_cast<const char16_t *>(str);
            SkipBytes(GetPadSize(readCapacity));
            if (u16Str[dataLength] == 0) {
                value = std::u16string(u16Str, dataLength);
                return true;
            }
        }
    }

    readCursor_ = oldCursor;
    value = std::u16string();
    return false;
}

const std::u16string Parcel::ReadString16WithLength(int32_t &readLength)
{
    int32_t dataLength = 0;
    size_t oldCursor = readCursor_;

    if (!Read<int32_t>(dataLength)) {
        return std::u16string();
    }

    if (dataLength < 0) {
        readLength = dataLength;
        return std::u16string();
    }

    size_t readCapacity = (dataLength + 1) * sizeof(char16_t);
    if ((readCapacity > (size_t)dataLength) && (readCapacity <= GetReadableBytes())) {
        const uint8_t *str = ReadBuffer(readCapacity);
        if (str != nullptr) {
            const auto *u16Str = reinterpret_cast<const char16_t *>(str);
            SkipBytes(GetPadSize(readCapacity));
            if (u16Str[dataLength] == 0) {
                readLength = dataLength;
                return std::u16string(u16Str, dataLength);
            }
        }
    }

    readCursor_ = oldCursor;
    return std::u16string();
}

const std::string Parcel::ReadString8WithLength(int32_t &readLength)
{
    int32_t dataLength = 0;
    size_t oldCursor = readCursor_;

    if (!Read<int32_t>(dataLength)) {
        return std::string();
    }

    if (dataLength < 0) {
        readLength = dataLength;
        return std::string();
    }

    size_t readCapacity = (dataLength + 1) * sizeof(char);
    if ((readCapacity > (size_t)dataLength) && (readCapacity <= GetReadableBytes())) {
        const uint8_t *str = ReadBuffer(readCapacity);
        if (str != nullptr) {
            const auto *u8Str = reinterpret_cast<const char *>(str);
            SkipBytes(GetPadSize(readCapacity));
            if (u8Str[dataLength] == 0) {
                readLength = dataLength;
                return std::string(u8Str, dataLength);
            }
        }
    }

    readCursor_ = oldCursor;
    return std::string();
}

void *DefaultAllocator::Alloc(size_t size)
{
    return malloc(size);
}

void DefaultAllocator::Dealloc(void *data)
{
    if (data != nullptr) {
        free(data);
    }
}

void *DefaultAllocator::Realloc(void *data, size_t newSize)
{
    return realloc(data, newSize);
}

template <typename T1, typename T2>
bool Parcel::WriteVector(const std::vector<T1> &val, bool (Parcel::*Write)(T2))
{
    if (val.size() > INT_MAX) {
        return false;
    }

    if (!this->WriteInt32(static_cast<int32_t>(val.size()))) {
        return false;
    }

    for (const auto &v : val) {
        if (!(this->*Write)(v)) {
            return false;
        }
    }

    size_t padSize = this->GetPadSize(val.size() * sizeof(T1));
    this->WritePadBytes(padSize);
    return true;
}

bool Parcel::WriteBoolVector(const std::vector<bool> &val)
{
    return WriteVector(val, &Parcel::WriteBool);
}

bool Parcel::WriteInt8Vector(const std::vector<int8_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt8Unaligned);
}

bool Parcel::WriteInt16Vector(const std::vector<int16_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt16);
}

bool Parcel::WriteInt32Vector(const std::vector<int32_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt32);
}

bool Parcel::WriteInt64Vector(const std::vector<int64_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt64);
}

bool Parcel::WriteUInt8Vector(const std::vector<uint8_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint8Unaligned);
}

bool Parcel::WriteUInt16Vector(const std::vector<uint16_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint16Unaligned);
}

bool Parcel::WriteUInt32Vector(const std::vector<uint32_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint32);
}

bool Parcel::WriteUInt64Vector(const std::vector<uint64_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint64);
}

bool Parcel::WriteFloatVector(const std::vector<float> &val)
{
    return WriteVector(val, &Parcel::WriteFloat);
}

bool Parcel::WriteDoubleVector(const std::vector<double> &val)
{
    return WriteVector(val, &Parcel::WriteDouble);
}

bool Parcel::WriteStringVector(const std::vector<std::string> &val)
{
    return WriteVector(val, &Parcel::WriteString);
}

bool Parcel::WriteString16Vector(const std::vector<std::u16string> &val)
{
    return WriteVector(val, &Parcel::WriteString16);
}

template <typename T>
bool Parcel::ReadVector(std::vector<T> *val, bool (Parcel::*Read)(T &))
{
    if (val == nullptr) {
        return false;
    }

    int32_t len = this->ReadInt32();
    if (len < 0) {
        return false;
    }

    size_t readAbleSize = this->GetReadableBytes();
    size_t size = static_cast<size_t>(len);
    if ((size > readAbleSize) || (size > val->max_size())) {
        UTILS_LOGE("Failed to read vector, size = %{public}zu, readAbleSize = %{public}zu", size, readAbleSize);
        return false;
    }
    val->resize(size);
    if (val->size() < size) {
        return false;
    }

    for (auto &v : *val) {
        if (!(this->*Read)(v)) {
            return false;
        }
    }

    size_t padSize = this->GetPadSize(size * sizeof(T));
    this->SkipBytes(padSize);
    return true;
}

bool Parcel::ReadBoolVector(std::vector<bool> *val)
{
    if (val == nullptr) {
        return false;
    }

    int32_t len = this->ReadInt32();
    if (len < 0) {
        return false;
    }

    size_t readAbleSize = this->GetReadableBytes();
    size_t size = static_cast<size_t>(len);
    if ((size > readAbleSize) || (val->max_size() < size)) {
        UTILS_LOGE("Failed to read bool vector, size = %{public}zu, readAbleSize = %{public}zu", size, readAbleSize);
        return false;
    }
    val->resize(size);
    if (val->size() < size) {
        return false;
    }

    for (size_t i = 0; i < size; ++i) {
        (*val)[i] = ReadBool();
    }

    size_t padSize = this->GetPadSize(size * sizeof(bool));
    this->SkipBytes(padSize);
    return true;
}

bool Parcel::ReadInt8Vector(std::vector<int8_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt8Unaligned);
}

bool Parcel::ReadInt16Vector(std::vector<int16_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt16);
}

bool Parcel::ReadInt32Vector(std::vector<int32_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt32);
}

bool Parcel::ReadInt64Vector(std::vector<int64_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt64);
}

bool Parcel::ReadUInt8Vector(std::vector<uint8_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint8Unaligned);
}

bool Parcel::ReadUInt16Vector(std::vector<uint16_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint16Unaligned);
}

bool Parcel::ReadUInt32Vector(std::vector<uint32_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint32);
}

bool Parcel::ReadUInt64Vector(std::vector<uint64_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint64);
}

bool Parcel::ReadFloatVector(std::vector<float> *val)
{
    return ReadVector(val, &Parcel::ReadFloat);
}

bool Parcel::ReadDoubleVector(std::vector<double> *val)
{
    return ReadVector(val, &Parcel::ReadDouble);
}

bool Parcel::ReadStringVector(std::vector<std::string> *val)
{
    if (val == nullptr) {
        return false;
    }

    int32_t len = this->ReadInt32();
    if (len < 0) {
        return false;
    }

    size_t readAbleSize = this->GetReadableBytes();
    size_t size = static_cast<size_t>(len);
    if ((size > readAbleSize) || (val->max_size() < size)) {
        UTILS_LOGE("Failed to read string vector, size = %{public}zu, readAbleSize = %{public}zu", size, readAbleSize);
        return false;
    }
    val->resize(size);
    if (val->size() < size) {
        return false;
    }

    for (auto &v : *val) {
        v = ReadString();
    }

    return true;
}

bool Parcel::ReadString16Vector(std::vector<std::u16string> *val)
{
    if (val == nullptr) {
        return false;
    }

    int32_t len = this->ReadInt32();
    if (len < 0) {
        return false;
    }

    size_t readAbleSize = this->GetReadableBytes();
    size_t size = static_cast<size_t>(len);
    if ((size > readAbleSize) || (val->max_size() < size)) {
        UTILS_LOGE("Failed to read u16string vector, size = %{public}zu, readAbleSize = %{public}zu",
            size, readAbleSize);
        return false;
    }

    val->resize(size);
    if (val->size() < size) {
        return false;
    }

    for (auto &v : *val) {
        v = ReadString16();
    }

    return true;
}
}  // namespace OHOS
