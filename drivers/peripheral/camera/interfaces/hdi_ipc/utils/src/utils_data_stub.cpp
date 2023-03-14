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

#include "utils_data_stub.h"

namespace OHOS::Camera {
bool UtilsDataStub::EncodeCameraMetadata(const std::shared_ptr<CameraStandard::CameraMetadata> &metadata,
    MessageParcel &data)
{
    if (metadata == nullptr) {
        return false;
    }

    bool bRet = true;
    uint32_t tagCount = 0;
    common_metadata_header_t *meta = metadata->get();
    if (meta != nullptr) {
        tagCount = CameraStandard::GetCameraMetadataItemCount(meta);
        bRet = (bRet && data.WriteInt32(static_cast<int32_t>(tagCount)));
        camera_metadata_item_entry_t *item = CameraStandard::GetMetadataItems(meta);
        for (uint32_t i = 0; i < tagCount; i++, item++) {
            camera_metadata_item_t entry;
            int ret = CameraStandard::FindCameraMetadataItem(meta, item->item, &entry);
            if (ret == -ENOENT) {
                return false;
            }

            bRet = (bRet && data.WriteUint32(static_cast<uint32_t>(entry.index)));
            bRet = (bRet && data.WriteUint32(entry.item));
            bRet = (bRet && data.WriteUint8(static_cast<uint8_t>(entry.data_type)));
            bRet = (bRet && data.WriteUint32(static_cast<uint32_t>(entry.count)));
            bRet = (bRet && UtilsDataStub::WriteMetadata(entry, data));
        }
    } else {
        bRet = data.WriteInt32(tagCount);
    }
    return bRet;
}

void UtilsDataStub::DecodeCameraMetadata(MessageParcel &data, std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    int32_t tagCount = data.ReadInt32();
    if (tagCount <= 0) {
        return;
    }

    int32_t metadataSize = 0;
    std::vector<camera_metadata_item_t> entrys;
    for (int32_t i = 0; i < tagCount; i++) {
        camera_metadata_item_t entry;
        entry.index = static_cast<size_t>(data.ReadUint32());
        entry.item = static_cast<uint32_t>(data.ReadUint32());
        entry.data_type = static_cast<uint8_t>(data.ReadUint8());
        entry.count = static_cast<size_t>(data.ReadUint32());
        ReadMetadata(entry, data);
        metadataSize++;

        entrys.push_back(entry);
    }

    metadata = std::make_shared<CameraStandard::CameraMetadata>(tagCount, metadataSize);
    common_metadata_header_t *meta = metadata->get();
    for (auto &entry : entrys) {
        void *buffer = nullptr;
        UtilsDataStub::EntryDataToBuffer(entry, &buffer);
        if (buffer != nullptr) {
            (void)CameraStandard::AddCameraMetadataItem(meta, entry.item, buffer, entry.count);
        }
    }
}

bool UtilsDataStub::EncodeStreamInfo(const std::shared_ptr<StreamInfo> &pInfo, MessageParcel &parcel)
{
    bool bRet = true;
    bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->streamId_)));
    bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->width_)));
    bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->height_)));
    bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->format_)));
    bRet = (bRet = (bRet && parcel.WriteInt32(pInfo->intent_)));
    bRet = (bRet && parcel.WriteBool(pInfo->tunneledMode_));
    bool bufferQueueFlag = (pInfo->bufferQueue_ != nullptr) ? true : false;
    bRet = (bRet && parcel.WriteBool(bufferQueueFlag));
    if (bufferQueueFlag) {
        bRet = (bRet && parcel.WriteRemoteObject(pInfo->bufferQueue_->AsObject()));
    }
    bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->minFrameDuration_)));
    bRet = (bRet && parcel.WriteInt32(pInfo->encodeType_));
    return bRet;
}

void UtilsDataStub::DecodeStreamInfo(MessageParcel &parcel, std::shared_ptr<StreamInfo> &pInfo)
{
    pInfo->streamId_ = static_cast<int>(parcel.ReadInt32());
    pInfo->width_ = static_cast<int>(parcel.ReadInt32());
    pInfo->height_ = static_cast<int>(parcel.ReadInt32());
    pInfo->format_ = static_cast<int>(parcel.ReadInt32());
    pInfo->intent_ = static_cast<StreamIntent>(parcel.ReadInt32());
    pInfo->tunneledMode_ = parcel.ReadBool();
    bool bufferQueueFlag = parcel.ReadBool();
    if (bufferQueueFlag) {
        sptr<IRemoteObject> remoteBufferProducer = parcel.ReadRemoteObject();
        pInfo->bufferQueue_ = OHOS::iface_cast<OHOS::IBufferProducer>(remoteBufferProducer);
    }
    pInfo->minFrameDuration_ = static_cast<int>(parcel.ReadInt32());
    pInfo->encodeType_ = static_cast<EncodeType>(parcel.ReadInt32());
}

int32_t UtilsDataStub::GetDataSize(uint8_t type)
{
    int32_t size = 0;
    if (type == META_TYPE_BYTE) {
        size = sizeof(uint8_t);
    } else if (type == META_TYPE_INT32) {
        size = sizeof(int32_t);
    } else if (type == META_TYPE_FLOAT) {
        size = sizeof(float);
    } else if (type == META_TYPE_INT64) {
        size = sizeof(int64_t);
    } else if (type == META_TYPE_DOUBLE) {
        size = sizeof(double);
    } else if (type == META_TYPE_RATIONAL) {
        size = sizeof(camera_rational_t);
    } else {
        size = 0;
    }
    return size;
}

bool UtilsDataStub::WriteMetadata(const camera_metadata_item_t &entry, MessageParcel &data)
{
    if (entry.data_type == META_TYPE_BYTE) {
        std::vector<uint8_t> buffers;
        for (size_t i = 0; i < entry.count; i++) {
            buffers.push_back(*(entry.data.u8 + i));
        }
        data.WriteUInt8Vector(buffers);
    } else if (entry.data_type == META_TYPE_INT32) {
        std::vector<int32_t> buffers;
        for (size_t i = 0; i < entry.count; i++) {
            buffers.push_back(*(entry.data.i32 + i));
        }
        data.WriteInt32Vector(buffers);
    } else if (entry.data_type == META_TYPE_FLOAT) {
        std::vector<float> buffers;
        for (size_t i = 0; i < entry.count; i++) {
            buffers.push_back(*(entry.data.f + i));
        }
        data.WriteFloatVector(buffers);
    } else if (entry.data_type == META_TYPE_INT64) {
        std::vector<int64_t> buffers;
        for (size_t i = 0; i < entry.count; i++) {
            buffers.push_back(*(entry.data.i64 + i));
        }
        data.WriteInt64Vector(buffers);
    } else if (entry.data_type == META_TYPE_DOUBLE) {
        std::vector<double> buffers;
        for (size_t i = 0; i < entry.count; i++) {
            buffers.push_back(*(entry.data.d + i));
        }
        data.WriteDoubleVector(buffers);
    } else if (entry.data_type == META_TYPE_RATIONAL) {
        std::vector<int32_t> buffers;
        for (size_t i = 0; i < entry.count; i++) {
            buffers.push_back((*(entry.data.r + i)).numerator);
            buffers.push_back((*(entry.data.r + i)).denominator);
        }
        data.WriteInt32Vector(buffers);
    }

    return true;
}

bool UtilsDataStub::ReadMetadata(camera_metadata_item_t &entry, MessageParcel &data)
{
    if (entry.data_type == META_TYPE_BYTE) {
        std::vector<uint8_t> buffers;
        data.ReadUInt8Vector(&buffers);
        entry.data.u8 = new(std::nothrow) uint8_t[entry.count];
        if (entry.data.u8 != nullptr) {
            for (size_t i = 0; i < entry.count; i++) {
                entry.data.u8[i] = buffers.at(i);
            }
        }
    } else if (entry.data_type == META_TYPE_INT32) {
        std::vector<int32_t> buffers;
        data.ReadInt32Vector(&buffers);
        entry.data.i32 = new(std::nothrow) int32_t[entry.count];
        if (entry.data.i32 != nullptr) {
            for (size_t i = 0; i < entry.count; i++) {
                entry.data.i32[i] = buffers.at(i);
            }
        }
    } else if (entry.data_type == META_TYPE_FLOAT) {
        std::vector<float> buffers;
        data.ReadFloatVector(&buffers);
        entry.data.f = new(std::nothrow) float[entry.count];
        if (entry.data.f != nullptr) {
            for (size_t i = 0; i < entry.count; i++) {
                entry.data.f[i] = buffers.at(i);
            }
        }
    } else if (entry.data_type == META_TYPE_INT64) {
        std::vector<int64_t> buffers;
        data.ReadInt64Vector(&buffers);
        entry.data.i64 = new(std::nothrow) int64_t[entry.count];
        if (entry.data.i64 != nullptr) {
            for (size_t i = 0; i < entry.count; i++) {
                entry.data.i64[i] = buffers.at(i);
            }
        }
    } else if (entry.data_type == META_TYPE_DOUBLE) {
        std::vector<double> buffers;
        data.ReadDoubleVector(&buffers);
        entry.data.d = new(std::nothrow) double[entry.count];
        if (entry.data.d != nullptr) {
            for (size_t i = 0; i < entry.count; i++) {
                entry.data.d[i] = buffers.at(i);
            }
        }
    } else if (entry.data_type == META_TYPE_RATIONAL) {
        std::vector<int32_t> buffers;
        data.ReadInt32Vector(&buffers);
        entry.data.r = new(std::nothrow) camera_rational_t[entry.count];
        if (entry.data.r != nullptr) {
            for (size_t i = 0, j = 0;
                i < entry.count && j < static_cast<size_t>(buffers.size());
                i++, j += 2) { // 2:Take two elements from the buffer vector container
                entry.data.r[i].numerator = buffers.at(j);
                entry.data.r[i].denominator = buffers.at(j + 1);
            }
        }
    }
    return true;
}

void UtilsDataStub::EntryDataToBuffer(const camera_metadata_item_t &entry, void **buffer)
{
    if (entry.data_type == META_TYPE_BYTE) {
        *buffer = (void*)entry.data.u8;
    } else if (entry.data_type == META_TYPE_INT32) {
        *buffer = (void*)entry.data.i32;
    } else if (entry.data_type == META_TYPE_FLOAT) {
        *buffer = (void*)entry.data.f;
    } else if (entry.data_type == META_TYPE_INT64) {
        *buffer = (void*)entry.data.i64;
    } else if (entry.data_type == META_TYPE_DOUBLE) {
        *buffer = (void*)entry.data.d;
    } else if (entry.data_type == META_TYPE_RATIONAL) {
        *buffer = (void*)entry.data.r;
    }
}
}
