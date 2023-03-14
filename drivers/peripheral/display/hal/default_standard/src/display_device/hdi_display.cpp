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

#include "hdi_display.h"
#include <vector>
#include "display_common.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
uint32_t HdiDisplay::mIdleId = 0;
std::unordered_set<uint32_t> HdiDisplay::mIdSets;

uint32_t HdiDisplay::GetIdleId()
{
    const uint32_t oldIdleId = mIdleId;
    uint32_t id = INVALIDE_DISPLAY_ID;
    // ensure the mIdleId not INVALIDE_DISPLAY_ID
    mIdleId = mIdleId % INVALIDE_DISPLAY_ID;
    do {
        auto iter = mIdSets.find(mIdleId);
        if (iter == mIdSets.end()) {
            id = mIdleId;
            break;
        }
        mIdleId = (mIdleId + 1) % INVALIDE_DISPLAY_ID;
    } while (oldIdleId != mIdleId);
    mIdSets.emplace(id);
    mIdleId++;
    return id;
}


int32_t HdiDisplay::Init()
{
    DISPLAY_LOGD();
    uint32_t id = GetIdleId();
    DISPLAY_CHK_RETURN((id == INVALIDE_DISPLAY_ID), DISPLAY_FAILURE, DISPLAY_LOGE("have no id to used"));
    mId = id;
    auto layer = CreateHdiLayer(LAYER_TYPE_GRAPHIC);
    DISPLAY_CHK_RETURN((layer.get() == nullptr), DISPLAY_FAILURE, DISPLAY_LOGE("can not create hdi layer for client"));
    mClientLayer = std::move(layer);
    DISPLAY_LOGD("the id is %{public}d", id);
    return DISPLAY_SUCCESS;
}


HdiDisplay::~HdiDisplay()
{
    mIdSets.erase(mId);
}

int32_t HdiDisplay::SetLayerZorder(uint32_t layerId, uint32_t zorder)
{
    DISPLAY_LOGD("layerId:%{public}d zorder:%{public}d size:%{public}zu", layerId, zorder, mLayers.size());
    auto iter = mLayersMap.find(layerId);
    DISPLAY_CHK_RETURN((iter == mLayersMap.end()), DISPLAY_FAILURE,
        DISPLAY_LOGE("can not find the layer %{public}d", layerId));
    auto layer = mLayersMap[layerId].get();
    if (layer->GetZorder() == zorder) {
        DISPLAY_LOGD("zorder no change layerId %{public}d, zorder %{public}d", layerId, zorder);
        return DISPLAY_SUCCESS;
    }
    // reset to sort
    auto zRange = mLayers.equal_range(layer);
    DISPLAY_LOGD("zorder range : zRange.first %{public}p  zRange.second %{public}p", *zRange.first, *zRange.second);
    for (auto c = zRange.first; c != zRange.second; c++) {
        if (*c == layer) {
            DISPLAY_LOGD("erase layer:%{public}p", layer);
            mLayers.erase(c);
            break;
        }
    }
    layer->SetLayerZorder(zorder);
    mLayers.emplace(layer);
    DISPLAY_LOGD("SetLayerZorder size:%{public}zu", mLayers.size());
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::CreateLayer(const LayerInfo *layerInfo, uint32_t *layerId)
{
    DISPLAY_LOGD();
    int ret;
    DISPLAY_CHK_RETURN((layerInfo == nullptr), DISPLAY_PARAM_ERR, DISPLAY_LOGE("LayerInfo is null"));
    DISPLAY_CHK_RETURN((layerId == nullptr), DISPLAY_PARAM_ERR, DISPLAY_LOGE("layerId is null"));
    auto layer = CreateHdiLayer(layerInfo->type);
    DISPLAY_CHK_RETURN((layer.get() == nullptr), DISPLAY_FAILURE, DISPLAY_LOGE("can not create hdi layer"));
    ret = layer->Init();
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("Layer Init failed"));
    *layerId = layer->GetId();
    mLayers.insert(layer.get());
    mLayersMap.emplace(layer->GetId(), std::move(layer));
    DISPLAY_LOGD("mLayers size %{public}zu", mLayers.size());
    DISPLAY_LOGD("mLayerMap size %{public}zu", mLayersMap.size());
    return DISPLAY_SUCCESS;
}

std::unique_ptr<HdiLayer> HdiDisplay::CreateHdiLayer(LayerType type)
{
    DISPLAY_LOGD();
    return std::make_unique<HdiLayer>(type);
}


int32_t HdiDisplay::CloseLayer(uint32_t layerId)
{
    DISPLAY_LOGD("layerId %{public}d", layerId);
    auto iter = mLayersMap.find(layerId);
    DISPLAY_CHK_RETURN((iter == mLayersMap.end()), DISPLAY_FAILURE,
        DISPLAY_LOGE("can not find the layer id %{public}d", layerId));
    auto layer = iter->second.get();
    auto zRange = mLayers.equal_range(layer);
    DISPLAY_LOGD("zorder range:zRange.first %{public}p, zRange.second %{public}p", *zRange.first, *zRange.second);
    for (auto c = zRange.first; c != zRange.second; c++) {
        if (*c == layer) {
            DISPLAY_LOGD("erase layer:%{public}p", layer);
            mLayers.erase(c);
            break;
        }
    }
    mLayersMap.erase(layerId);
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::GetDisplayCompChange(uint32_t *num, uint32_t *layers, int32_t *type)
{
    DISPLAY_CHK_RETURN((num == nullptr), DISPLAY_FAILURE, DISPLAY_LOGE("the num is nullptr"));
    *num = mChangeLayers.size();
    if ((layers == nullptr) && (type == nullptr)) {
        return DISPLAY_SUCCESS;
    }
    DISPLAY_LOGD("set the layers and type");
    for (uint32_t i = 0; i < mChangeLayers.size(); i++) {
        HdiLayer *layer = mChangeLayers[i];
        if (layers != nullptr) {
            *(layers + i) = layer->GetId();
        }
        if (type != nullptr) {
            *(type + i) = layer->GetCompositionType();
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::GetDisplayReleaseFence(uint32_t *num, uint32_t *layers, int32_t *fences)
{
    DISPLAY_CHK_RETURN((num == nullptr), DISPLAY_FAILURE, DISPLAY_LOGE("the num is nullptr"));
    *num = mLayers.size();
    if ((layers == nullptr) && (fences == nullptr)) {
        return DISPLAY_SUCCESS;
    }
    DISPLAY_LOGD("set the layer fences");
    int i = 0;
    for (auto layer : mLayers) {
        if (layers != nullptr) {
            *(layers + i) = layer->GetId();
        }
        if (fences != nullptr) {
            *(fences + i) = dup(layer->GetReleaseFenceFd());
        }
        DISPLAY_LOGD("layer id %{public}d fencefd %{public}d", layer->GetId(), layer->GetReleaseFenceFd());
        i++;
    }
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::PrepareDisplayLayers(bool *needFlushFb)
{
    DISPLAY_LOGD();
    mChangeLayers.clear();
    std::vector<HdiLayer *> layers;
    for (auto c : mLayers) {
        if (c->IsVisible()) {
            layers.push_back(c);
        }
    }
    DISPLAY_LOGD(" mLayers  size %{public}zu layers size %{public}zu", mLayers.size(), layers.size());

    mComposer->Prepare(layers, *mClientLayer);
    // get the change layers
    for (auto &layer : layers) {
        if (layer->GetDeviceSelect() != layer->GetCompositionType()) {
            DISPLAY_LOGD("layer change");
            layer->SetLayerCompositionType(layer->GetDeviceSelect());
        }
        mChangeLayers.push_back(layer);
    }
    *needFlushFb = true;
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::Commit(int32_t *fence)
{
    DISPLAY_LOGD();
    mComposer->Commit(false);
    *fence = dup(mClientLayer->GetReleaseFenceFd());
    DISPLAY_LOGD("the release fence is %{public}d", *fence);
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::SetDisplayClientBuffer(const BufferHandle *buffer, int32_t fence)
{
    mClientLayer->SetLayerBuffer(buffer, fence);
    return DISPLAY_SUCCESS;
}

HdiLayer *HdiDisplay::GetHdiLayer(uint32_t id)
{
    DISPLAY_LOGD("id : %{public}d", id);
    auto iter = mLayersMap.find(id);
    DISPLAY_CHK_RETURN((iter == mLayersMap.end()), nullptr, DISPLAY_LOGE("can not find the layer %{public}d", id));
    return iter->second.get();
}

VsyncCallBack::VsyncCallBack(VBlankCallback cb, void *data) : mVBlankCb(cb), mData(data)
{
    DISPLAY_LOGD("VsyncCallBack %{public}p", cb);
}

void VsyncCallBack::Vsync(unsigned int sequence, uint64_t ns)
{
    DISPLAY_CHK_RETURN_NOT_VALUE((mVBlankCb == nullptr), DISPLAY_LOGE("the callback is nullptr"));
    mVBlankCb(sequence, ns, mData);
}
}
}
}
