/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

/**
 * @file media_service.h
 *
 * @brief Declares the class of the media service framework, including attributes and methods.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef MEDIA_SERVICE_H
#define MEDIA_SERVICE_H

#include <stdint.h>
#include <deque>
#include <vector>
#include <string>
#include "log.h"
namespace stub {
#define MEDIA_SERVICE_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;                 \
    void operator=(const TypeName &) = delete

#define MEDIA_SERVICE_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName() = delete;                                       \
    MEDIA_SERVICE_DISALLOW_COPY_AND_ASSIGN(TypeName)

class MediaService {
public:
    struct MediaPlayer {
        uint8_t itemType_ {0xFF};
        uint8_t majorType_ {0xFF};
        uint8_t playStatus_ {0xFF};
        uint16_t playerId_ {0xFFFF};
        uint32_t subType_ {0xFFFFFFFF};
        std::string name_ {};
        std::vector<uint8_t> features_ {};
    };

    struct MediaItem {
        uint8_t itemType_ {0xFF};
        uint8_t mediaType_ {0xFF};
        uint8_t playable_ {0xFF};
        uint64_t uid_ {0xFFFFFFFFFFFFFFFF};
        std::string name_ {};
        std::vector<uint32_t> attributes_ {};
        std::vector<std::string> values_ {};
    };

    class IObserver {
    public:
        IObserver() = default;
        virtual ~IObserver() = default;

        virtual void OnConnectionStateChanged(const std::string &addr, int state) = 0;

        virtual void OnPressButton(const std::string &addr, uint8_t button) = 0;

        virtual void OnReleaseButton(const std::string &addr, uint8_t button) = 0;

        virtual void OnSetAddressedPlayer(const std::string &addr, uint8_t label, int status) = 0;

        virtual void OnSetBrowsedPlayer(const std::string &addr, uint16_t uidCounter, uint32_t numOfItems,
            const std::vector<std::string> &folderNames, uint8_t label, int status) = 0;

        virtual void OnGetPlayerAppSettingAttributes(
            const std::string &addr, const std::deque<uint8_t> &attributes, uint8_t label) = 0;

        virtual void OnGetPlayerAppSettingValues(
            const std::string &addr, const std::deque<uint8_t> &values, uint8_t label) = 0;

        virtual void OnGetPlayerAppSettingCurrentValue(const std::string &addr, const std::deque<uint8_t> &attributes,
            const std::deque<uint8_t> &values, uint8_t label, uint8_t context) = 0;

        virtual void OnSetPlayerAppSettingCurrentValue(const std::string &addr, uint8_t label) = 0;

        virtual void OnGetPlayerAppSettingAttributeText(const std::string &addr, const std::vector<uint8_t> &attributes,
            const std::vector<std::string> &attrStr, uint8_t label) = 0;

        virtual void OnGetPlayerAppSettingValueText(const std::string &addr, const std::vector<uint8_t> &values,
            const std::vector<std::string> &valueStr, uint8_t label) = 0;

        virtual void OnGetElementAttributes(const std::string &addr, const std::vector<uint32_t> &attribtues,
            const std::vector<std::string> &values, uint8_t label) = 0;

        virtual void OnGetPlayStatus(const std::string &addr, uint32_t songLength, uint32_t songPosition,
            uint8_t playStatus, uint8_t label, uint8_t context) = 0;

        virtual void OnPlayItem(const std::string &addr, uint8_t label, int status) = 0;

        virtual void OnAddToNowPlaying(const std::string &addr, uint8_t label, int status) = 0;

        virtual void OnChangePath(const std::string &addr, uint32_t numOfItems, uint8_t label, int status) = 0;

        virtual void OnGetMediaPlayers(const std::string &addr, uint16_t uidCounter,
            const std::vector<stub::MediaService::MediaPlayer> &items, uint8_t label, int status) = 0;

        virtual void OnGetFolderItems(const std::string &addr, uint16_t uidCounter,
            const std::vector<stub::MediaService::MediaItem> &items, uint8_t label, int status) = 0;

        virtual void OnGetItemAttributes(const std::string &addr, const std::vector<uint32_t> &attributes,
            const std::vector<std::string> &values, uint8_t label, int status) = 0;

        virtual void OnGetTotalNumberOfItems(
            const std::string &addr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status) = 0;

        virtual void OnSetAbsoluteVolume(const std::string &addr, uint8_t volume, uint8_t label) = 0;

        virtual void OnGetSelectedTrack(const std::string &addr, uint64_t uid, uint8_t label) = 0;

        virtual void OnGetAddressedPlayer(
            const std::string &addr, uint16_t playerId, uint16_t uidCounter, uint8_t label) = 0;

        virtual void OnGetUidCounter(const std::string &addr, uint16_t uidCounter, uint8_t label) = 0;

        virtual void OnGetCurrentAbsoluteVolume(const std::string &addr, uint8_t volume, uint8_t label) = 0;
    };

    static MediaService *GetInstance();

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/
    void RegisterObserver(MediaService::IObserver *observer)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
    }
    void UnregisterObserver(MediaService::IObserver *observer)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
    }

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/
    void SetActiveDevice(const std::string &addr)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
    }

    /******************************************************************
     * BUTTON OPERATION                                               *
     ******************************************************************/
    int PressButton(const std::string &addr, uint8_t button, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int ReleaseButton(const std::string &addr, uint8_t button, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }

    /******************************************************************
     * Media Player Selection                                         *
     ******************************************************************/
    int SetAddressedPlayer(const std::string &addr, uint16_t playerId, uint16_t uidCounter, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int SetBrowsedPlayer(const std::string &addr, uint16_t playerId, uint16_t uidCounter, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }

    /******************************************************************
     * Capabilities                                                   *
     ******************************************************************/
    std::vector<uint8_t> GetCapabilities(const std::string &addr, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        std::vector<uint8_t> tmp;
        return tmp;
    }

    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/
    int GetPlayerAppSettingAttributes(const std::string &addr, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetPlayerAppSettingValues(const std::string &addr, uint8_t attribute, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetPlayerAppSettingCurrentValue(
        const std::string &addr, const std::deque<uint8_t> &attributes, uint8_t label, uint8_t context)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int SetPlayerAppSettingCurrentValue(const std::string &addr, const std::deque<uint8_t> &attributes,
        const std::deque<uint8_t> &values, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetPlayerAppSettingAttributeText(
        const std::string &addr, const std::vector<uint8_t> &attributes, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetPlayerAppSettingValueText(
        const std::string &addr, uint8_t attribute, const std::vector<uint8_t> &values, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }

    /******************************************************************
     * MEDIA INFORMATION PDUS                                         *
     ******************************************************************/
    int GetElementAttributes(
        const std::string &addr, uint64_t identifier, const std::vector<uint32_t> &attributes, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }

    /******************************************************************
     * PLAY                                                           *
     ******************************************************************/
    int GetPlayStatus(const std::string &addr, uint8_t label, uint8_t context)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int PlayItem(const std::string &addr, uint8_t scope, uint64_t uid, uint16_t uidCounter, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int AddToNowPlaying(const std::string &addr, uint8_t scope, uint64_t uid, uint16_t uidCounter, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }

    /******************************************************************
     * OPERATE THE VIRTUAL FILE SYSTEM                                *
     ******************************************************************/
    int ChangePath(const std::string &addr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetFolderItems(const std::string &addr, uint8_t scope, uint32_t startItem, uint32_t endItem,
        const std::vector<uint32_t> &attributes, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetItemAttributes(const std::string &addr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
        const std::vector<uint32_t> &attributes, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetTotalNumberOfItems(const std::string &addr, uint8_t scope, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }

    /******************************************************************
     * ABSOLUTE VOLUME                                                *
     ******************************************************************/
    int SetAbsoluteVolume(const std::string &addr, uint8_t volume, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetSelectedTrack(const std::string addr, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetAddressedPlayer(const std::string &addr, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetUidCounter(const std::string &addr, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    int GetCurrentAbsoluteVolume(const std::string &addr, uint8_t label)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
        return -1;
    }
    void SetPlaybackInterval(const std::string &addr, uint32_t interval)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
    }
    void ResponseNotification(uint16_t events)
    {
        LOG_DEBUG("[AVRCP TG] MediaService::%{public}s", __func__);
    }

private:
    MediaService() = default;
    ~MediaService() = default;

    MEDIA_SERVICE_DISALLOW_COPY_AND_ASSIGN(MediaService);
};
}  // namespace stub
#endif  // MEDIA_SERVICE_H