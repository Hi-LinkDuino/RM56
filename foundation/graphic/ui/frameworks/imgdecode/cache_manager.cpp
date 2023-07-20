/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "imgdecode/cache_manager.h"
#include "gfx_utils/graphic_log.h"
#include "hal_tick.h"
#include "securec.h"

#include "gfx_utils/file.h"

#include "gfx_utils/mem_api.h"

#if (BES_IMG_PRELOAD_ENABLE == 1)
#include "cmsis_os2.h"

#include <fstream>

using namespace std;
#endif //BES_IMG_PRELOAD_ENABLE

namespace OHOS {
#if (BES_IMG_PRELOAD_ENABLE == 1)
/** 
 * @brief 获取资源锁
 * @param lock 资源
 * @param timeOutMs 执行的时长，单位为毫秒。当大于 0 时，表示以帧为时间间隔去尝试获取锁；0 表示只尝试获取锁，并不堵塞等待。为负值时，表示总是重复尝试，直到成功
 * @return 是否成功拿到锁，单位为毫秒
 */
bool TryLockInTime(pthread_mutex_t& lock, int32_t timeOutMs)
{
    bool ret = true;
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    if (timeOutMs < 0) {
        while (pthread_mutex_lock(&lock) != EOK);
    } else if (timeOutMs == 0) {
        ret = pthread_mutex_trylock(&lock) == EOK;
    } else {
        uint32_t time = HALTick::GetInstance().GetTime();
        while (pthread_mutex_trylock(&lock) != EOK) {
            osDelay(2);
            if (HALTick::GetInstance().GetTime() - time < timeOutMs) continue;
            ret = false;
            break;
        }
        return ret;
    }
#endif
    return ret;
}

/**
 * @brief 释放资源锁
 * @param lock
 */
void Unlock(pthread_mutex_t& lock)
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock);
#endif
}

void CacheManager::UiIdleListener::OnFrameStart()
{
    TryLockInTime(drawLock_, -1);
}

void CacheManager::UiIdleListener::OnFrameEnd(uint32_t duration)
{
    if (!cm_->isUiDrawStart_) {
        cm_->isUiDrawStart_ = true;
    } else {
        uint32_t time = HALTick::GetInstance().GetTime();
        if (TryLockInTime(preloadLock_, DRAW_FRAME_TIME_MS - duration)) {
            //一帧最大时间内可占用的时长
            uint32_t preloadTime = DRAW_FRAME_TIME_MS - duration - (HALTick::GetInstance().GetTime() - time);
            cm_->PreloadInTime(cm_->curPreloadEntry_, preloadTime);
            Unlock(preloadLock_);
        }
    }

    Unlock(drawLock_);
}

CacheManager::CacheManager() : size_(0), entryArr_(nullptr)
{
    uiIdleListener_ = new UiIdleListener(this);
    FrameEventManager::GetInstance().RegisterFrameListener(uiIdleListener_);

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
        pthread_mutex_init(&drawLock_, nullptr);
        pthread_mutex_init(&preloadLock_, nullptr);
#endif
}

CacheManager::~CacheManager() 
{
    if (uiIdleListener_ != nullptr) {
        FrameEventManager::GetInstance().UnregisterFrameListener(uiIdleListener_);
        delete uiIdleListener_;
        uiIdleListener_ = nullptr;
    }

    ListNode<PreloadEntry*>* node = preloadBootList_.Begin();
    PreloadEntry* entry;
    while (node != nullptr && node != preloadBootList_.End()) {
        entry = node->data_;
        if (entry == nullptr) break;
        delete entry;
        entry = nullptr;
        node = preloadBootList_.Remove(node);
    }

    node = preloadIdleList_.Begin();
    while (node != nullptr && node != preloadIdleList_.End()) {
        entry = node->data_;
        if (entry == nullptr) break;
        delete entry;
        entry = nullptr;
        node = preloadIdleList_.Remove(node);
    }

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&drawLock_);
    pthread_mutex_destroy(&drawLock_);

    pthread_mutex_unlock(&preloadLock_);
    pthread_mutex_destroy(&preloadLock_);
#endif
}

PreloadResult CacheManager::PreloadBeforeUiLaunch()
{
    if (!canBootPreload_) return PreloadResult::TERMINAL;
    PreloadResult ret = ParsePreloadManifest(PRELOAD_BOOT_PATH, &CacheManager::LoadPreloadRes);
    if (ret != PreloadResult::IO_ERROR) {
        canBootPreload_ = false;
        PreloadResult retCode = ParsePreloadManifest(PRELOAD_IDLE_PATH, &CacheManager::SavePreloadResPath);
        if (retCode == PreloadResult::IO_ERROR) return ret;

        Evaluate();
    }

    return ret;
}

//TODO: 2023年4月20日：Seems bad performance
PreloadResult CacheManager::ParsePreloadManifest(const char* filePath, void (*onRead)(const char* path, uint16_t len))
{
#ifdef _WIN32
    int32_t fd = open(filePath, O_RDONLY | O_BINARY);
#else
    int32_t fd = open(filePath, O_RDONLY);
#endif
    if (fd == -1) return PreloadResult::IO_ERROR;

    struct stat info = {0};
    if (stat(filePath, &info) != 0) return PreloadResult::IO_ERROR;
    uint32_t fsize = info.st_size;

    if (fsize == 0) return PreloadResult::ENDED;

    //拒绝读取大文件
    if (fsize > BES_IMG_PRELOAD_MANIFEST_SIZE_LIMIT) return PreloadResult::TERMINAL;

    char* buffer = reinterpret_cast<char*>(UIMalloc(fsize));
    if (buffer == nullptr) return PreloadResult::TERMINAL;

    uint32_t readCount = read(fd, buffer, fsize);
    if (readCount != fsize) return PreloadResult::TERMINAL;

    uint8_t CR = 0x0D;
    uint8_t LF = 0x0A;
    uint32_t startPos = 0;
    uint32_t uriLen = 0;
    uint16_t pathLen = 0;
    uint16_t rootDirLen = strlen(PRELOAD_ROOT_DIR);

    char *path = nullptr;
    //开始读取每一个路径，以 CR，LF 或 CRLF 为一个路径的结束标志
    //Windows（CRLF） / Linux（LF） / OSX（CR 或 LF） 中存在差异
    for (uint32_t i = 0; i < fsize; i++) {
        if (buffer[i] == CR || buffer[i] == LF) {
            uriLen = i - startPos;

            if (buffer[i] == CR) {
                if (i < fsize - 1 && buffer[i + 1] == LF) i++;  //针对 Windows / 旧版 OSX，跳过后面可能出现的 LF
            }
        } else if (i == fsize - 1) {
            uriLen = i + 1 - startPos;
        } else {
            continue;
        }

        //复用内存
        uint16_t newLen = uriLen + rootDirLen;
        if (path == nullptr || pathLen < newLen) {
            UIFree(path);
            path = reinterpret_cast<char*>(UIMalloc(newLen + 1));
        }
        pathLen = newLen;

        do {
            if (path == nullptr || memcpy_s(path, rootDirLen, PRELOAD_ROOT_DIR, rootDirLen) != EOK) break;
            if (memcpy_s(path + rootDirLen, uriLen, buffer + startPos, uriLen) != EOK) break;

            path[pathLen] = '\0'; //memcpy_s() 不会添加 \0

            //文件
            if (path[pathLen - 1] != '/') {
                OnPathRead(path, pathLen, onRead);
                break;
            }

            //目录
            DIR* dir;
            if ((dir = opendir(path)) == nullptr) break;

            char* subFilePath = nullptr;
            uint16_t subFileLen = 0;
            struct dirent* ent;
            while ((ent = readdir(dir)) != nullptr) {
                if (ent->d_name == nullptr 
                        || !strcmp(ent->d_name, "") 
                        || !strcmp(ent->d_name, ".") 
                        || !strcmp(ent->d_name, "..")) continue;

                //目录深度为 1
                uint16_t nameLen = strlen(ent->d_name);
                uint16_t suffixLen = sizeof(PRELOAD_SRC_SUFFIX) / sizeof(char);
                if (nameLen <= suffixLen) continue;
                bool isDir = false;
                uint16_t startPos = nameLen - suffixLen;
                for (size_t i = 0; i < suffixLen; i++) {
                    if (ent->d_name[startPos + i] == PRELOAD_SRC_SUFFIX[i]) continue;
                    isDir = true;
                    break;
                }
                if (isDir) continue;

                uint16_t fullLen = pathLen + nameLen;
                if (subFilePath == nullptr || subFileLen < fullLen) {
                    UIFree(subFilePath);
                    subFilePath = reinterpret_cast<char*>(UIMalloc(fullLen + 1)); //+ 1：末尾的'\0'
                    subFileLen = fullLen;
                }

                if (memcpy_s(subFilePath, pathLen, path, pathLen) != EOK
                    || memcpy_s(subFilePath + pathLen, nameLen, ent->d_name, nameLen) != EOK) continue;

                subFilePath[fullLen] = '\0';
                OnPathRead(subFilePath, fullLen, onRead);
            }

            UIFree(subFilePath);
            subFilePath = nullptr;

            closedir(dir);
        } while (false);

        startPos = i + 1;
    }

    UIFree(path);
    path = nullptr;

    UIFree(buffer);
    buffer = nullptr;

    return PreloadResult::ENDED;
}

void CacheManager::Evaluate()
{
    uint16_t cacheIndex = preloadBootList_.Size();
    char* targetPath = nullptr;
    uint32_t maxSize = 0;
    uint32_t headerSize = sizeof(ImageHeader);
    struct stat info = {0};
    ListNode<PreloadEntry*>* node;
    PreloadEntry* entry;
    node = preloadIdleList_.Begin();
    while (node != nullptr && node != preloadIdleList_.End()) {
        entry = node->data_;
        if (entry == nullptr) {
            node = preloadIdleList_.Remove(node);
            continue;
        }

        if (targetPath == nullptr) targetPath = entry->path;

        if (stat(entry->path, &info) != 0) {
            //移除无法使用的资源数据
            delete entry;
            entry = nullptr;
            node = preloadIdleList_.Remove(node);
            continue;
        }

        if (info.st_size <= headerSize) {
            delete entry;
            entry = nullptr;
            node = preloadIdleList_.Remove(node);
            continue;
        }

        entry->totalBytes = info.st_size - headerSize;
        entry->seq = cacheIndex;
        cacheIndex++;

        if (info.st_size > maxSize) {
            maxSize = info.st_size;
            targetPath = entry->path;
        }

        node = node->next_;
    }

    preloadCount_ = cacheIndex;

#ifdef _WIN32
#else
    if (GetEvaluateFromHistory()) return;
#endif

#ifdef _WIN32
    int32_t fd = open(targetPath, O_RDONLY | O_BINARY);
#else
    int32_t fd = open(targetPath, O_RDONLY);
#endif
    if (fd == -1) return;

    uint8_t *buf = reinterpret_cast<uint8_t*>(UIMalloc(IO_READ_BYTES_MAX));
    if (buf == nullptr) return;

    uint16_t evaluateLen = sizeof(bytesInTime);
    memset(bytesInTime, 0, evaluateLen);
    uint32_t timeMs;
    uint32_t readSize;
    uint32_t dataLen = IO_READ_BYTES_MIN;
    for (size_t i = 0; i < EVALUATE_IO_COUNT; i++) {
        if (dataLen >= IO_READ_BYTES_MAX) {
            bytesInTime[i][0] = bytesInTime[MATH_MAX(i - 1, 0)][0];
            bytesInTime[i][1] = IO_READ_BYTES_MAX;
            continue;
        }

        timeMs = HALTick::GetInstance().GetTime();
        lseek(fd, 0, SEEK_SET);
        readSize = read(fd, buf, dataLen);
        if (readSize < dataLen) {
            if (i == 0) {
                //文件比最小目标数据量还要小
                bytesInTime[i][0] = 0;
                bytesInTime[i][1] = IO_READ_BYTES_MIN;
            } else {
                //视为无用数据
                bytesInTime[i][0] = 0;
                bytesInTime[i][1] = IO_READ_BYTES_MIN;
            }
            continue;
        }

        bytesInTime[i][0] = HALTick::GetInstance().GetTime() - timeMs;
        bytesInTime[i][1] = readSize;

        if (dataLen < 1024) {           //1k
            dataLen = 1024;
        } else if (dataLen < 2048) {   //2k
            dataLen = 2048;
        } else if (dataLen < 4096)  {  //4k
            dataLen += 2048;
        } else if (dataLen < 16384)  { //16k
            dataLen += 4096;
        } else if (dataLen < 65535) {  //64k
            dataLen += 8192;
        } else if (dataLen < 131072) { //128k
            dataLen += 16384;
        } else if (dataLen < 262144) { //256k
            dataLen += 32768;
        } else if (dataLen < 524288) { //512k
            dataLen += 65536;
        } else {
            dataLen += 262144;
        }

        if (dataLen > IO_READ_BYTES_MAX) dataLen = IO_READ_BYTES_MAX;
    }

    UIFree(buf);
    buf = nullptr;

    if (fd != -1) {
        close(fd);
        fd = -1;
    }

#ifdef _WIN32
#else
    ofstream outfile;
    outfile.open(PRELOAD_EVALUATE_PATH, ios_base::out | ios_base::trunc);
    if (!outfile.fail() && !outfile.bad()) {
        char header[1] = { 0 };
        outfile.write(header, sizeof(header));
        outfile.write(reinterpret_cast<char*>(bytesInTime), evaluateLen);
    }
    outfile.close();
#endif
}

bool CacheManager::GetEvaluateFromHistory()
{
    char header[1] = { 0 };
    uint8_t headerLen = sizeof(header);
    uint32_t dataLen = sizeof(bytesInTime);

#ifdef _WIN32
    int32_t fd = open(PRELOAD_EVALUATE_PATH, O_RDONLY | O_BINARY);
#else
    int32_t fd = open(PRELOAD_EVALUATE_PATH, O_RDONLY);
#endif

    if (fd == -1) return false;

    if (read(fd, header, headerLen) < headerLen || header[0] >= BES_IMG_PRELOAD_EVALUATE_EXPIRED) {
        return false;
    }

    uint32_t* buf = reinterpret_cast<uint32_t*>(UIMalloc(dataLen));
    if (buf == nullptr) return false;

    lseek(fd, headerLen, SEEK_SET);
    int16_t readCount = read(fd, bytesInTime, dataLen);
    UIFree(buf);
    buf = nullptr;
    if (readCount == dataLen) {
        //更新文件头
        ofstream outfile;
        outfile.open(PRELOAD_EVALUATE_PATH, ios_base::in);
        if (!outfile.fail() && !outfile.bad()) {
            header[0] += 1;
            outfile.seekp(0);
            outfile.write(header, headerLen);
        }
        outfile.close();

        return true;
    }

    return false;
}

void CacheManager::LoadPreloadRes(const char* path, uint16_t len)
{
    CacheManager &cm = CacheManager::GetInstance();

    if (cm.IsDuplicatePreload(path)) return;

    Style style;
    int16_t index = cm.DoPreloadFully(path, cm.preloadBootList_.Size(), style);
    if (index == CODE_NOT_PRELOAD) return;

    cm.DoSavePreloadPath(cm.preloadBootList_, path, len);
    PreloadEntry* entry = cm.preloadBootList_.Back();
    entry->seq = index;
    entry->loadBytes = cm.entryArr_[index].GetImageInfo().dataSize;
    entry->totalBytes = entry->loadBytes;
}

void CacheManager::SavePreloadResPath(const char* path, uint16_t len)
{
    CacheManager &cm = CacheManager::GetInstance();
    cm.DoSavePreloadPath(cm.preloadIdleList_, path, len);
}

void CacheManager::PreloadWhenIdle()
{
    if (!canIdlePreload_) return;
    canIdlePreload_ = false;

    //2023年3月30日 需要等待第一帧绘制完成，才开始空闲时预加载任务
    while (!isUiDrawStart_) {
        osDelay(DRAW_FRAME_TIME_MS);
    }

    if (preloadIdleList_.Size() == 0) return;

    Style style;
    uint16_t preloadLimit = GetPreloadLimit();
    uint16_t processCount = 0;
    PreloadEntry* entry;
    ListNode<PreloadEntry*>* node = preloadIdleList_.Begin();
    while (node != nullptr && node != preloadIdleList_.End()) {
        entry = node->data_;

        if (entry == nullptr) {
            processCount++;
            node = node->next_;
            continue;
        }

        //跳过不可用资源
        if (entry->retryCount == 0) {
            processCount++;
            node = node->next_;
            continue;
        }

        //缓存池容量检查
        if (entry->seq >= preloadLimit) {
            processCount = UINT32_MAX;
            break;
        }

        if (IsPreloaded(entry)) {
            processCount++;
            node = node->next_;
            continue;
        }

        CacheEntry &targetEntry = entryArr_[entry->seq];
        FileImgDecoder::ImgResDsc &targetDsc = targetEntry.dsc_;

        if (entry->loadBytes == 0) {
            uint32_t time = HALTick::GetInstance().GetTime();
            RetCode ret = TryDecode(entry->path, style, targetEntry);
            if (ret != RetCode::OK) {
                node = node->next_;
                continue;
            }
            lseek(targetDsc.fd, sizeof(ImageHeader), SEEK_SET);

            targetEntry.dsc_.timeToOpen = time;

            entry->ClearData();
        }

        if (entry->data == nullptr) {
            //注意，开辟的内存首地址要求为 ALIGN_ADDRESS_NUM 的倍数
            entry->data = reinterpret_cast<uint8_t*>(aligned_malloc(entry->totalBytes, ALIGN_ADDRESS_NUM));
            if (entry->data == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "Not enough memory to preload[%s]! need %u byte", entry->path, entry->totalBytes);
                //内存不足，不仅放弃继续预加载，还裁剪预加载列表
                PrunePreload(node);
                preloadCount_ = preloadBootList_.Size() + preloadIdleList_.Size();
                break;
            }
        }
        curPreloadEntry_ = entry;

        bool processed = false;
        while (true) {
            // 让 UI 线程更多地获得时间片
            osDelay(DRAW_FRAME_TIME_MS);

            uint32_t time = HALTick::GetInstance().GetTime();
            TryLockInTime(drawLock_, -1);
            TryLockInTime(preloadLock_, -1);

            if (IsPreloaded(entry)) {
                processed = true;
                break;
            }

            PreloadResult result = PreloadInTime(entry, DEF_PRELOAD_TIME_MS);
            if (result == PreloadResult::IO_ERROR) {
                processed = false;
                break;
            }

            Unlock(drawLock_);
            Unlock(preloadLock_);
        }

        curPreloadEntry_ = nullptr;
        if (processed) {
            processCount++;
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "Failed to preload [%s]!", entry->path);

            entry->retryCount = MATH_MAX(entry->retryCount - 1, 0);
            entry->ClearData();
            targetDsc.decoder->Close(targetDsc);
        }

        if (node != nullptr) node = node->next_;

        Unlock(drawLock_);
        Unlock(preloadLock_);
    }

    uint16_t targetSize = preloadIdleList_.Size();
    if (processCount < targetSize) {
        canIdlePreload_ = true;
        PreloadWhenIdle();
    } else {
        if (uiIdleListener_ != nullptr) {
            FrameEventManager::GetInstance().UnregisterFrameListener(uiIdleListener_);
            delete uiIdleListener_;
            uiIdleListener_ = nullptr;
        }

        HILOG_INFO(HILOG_MODULE_ACE, "Preload done(%u). process[%u/%u]", HALTick::GetInstance().GetTime(), processCount, targetSize);
    }
}

PreloadResult CacheManager::PreloadInTime(PreloadEntry *target, int32_t duration)
{
    if (duration <= 0) return PreloadResult::TERMINAL;
    if (target == nullptr || target->seq == CODE_NOT_PRELOAD) return PreloadResult::TERMINAL;
    if (target->data == nullptr) return PreloadResult::TERMINAL;
    if (IsPreloaded(target)) return PreloadResult::ENDED;
    if (target->retryCount == 0) return PreloadResult::TERMINAL;
    
    uint32_t timeMs = HALTick::GetInstance().GetTime();

    uint32_t readSize = GetBestReadSizeByEvaluated(duration);
    if (readSize == 0) return PreloadResult::TERMINAL;

    CacheEntry &entry = entryArr_[target->seq];
    FileImgDecoder::ImgResDsc &dsc = entry.dsc_;
    ImageInfo &imgInfo = dsc.imgInfo;

    uint32_t durationOnce;
    int32_t readCount;
    while (true) {
        if (dsc.fd == -1) return PreloadResult::TERMINAL;

        durationOnce = HALTick::GetInstance().GetTime();

        readCount = read(dsc.fd, target->data + target->loadBytes, readSize);
        if (readCount > 0) target->loadBytes += readCount;

        if (readCount == -1) {
            if (!IsPreloaded(target)) {
                target->ClearData();
                return PreloadResult::TERMINAL;
            }
        }

        if (readCount < readSize) break;

        durationOnce = HALTick::GetInstance().GetTime() - durationOnce;

        //如果不足以进行再一次的读取，直接放弃
        if (HALTick::GetInstance().GetTime() - timeMs + durationOnce > duration) break;
    }

    if (IsPreloaded(target)) {
        //HILOG_INFO(HILOG_MODULE_ACE, "[%s](%u byte) is preloaded, seq=%u", target->path, target->totalBytes, target->seq);
        if (imgInfo.data != nullptr) {
            ImageCacheFree(imgInfo);
        }

        imgInfo.data = target->data;
        imgInfo.dataSize = target->totalBytes;
        entry.life_ = HALTick::GetInstance().GetTime();
        entry.persistent_ = true;
        dsc.inCache_ = true;
        close(dsc.fd);
        dsc.fd = -1;

        target->data = nullptr;
        return PreloadResult::ENDED;
    } else if (target->loadBytes > target->totalBytes) {
        return PreloadResult::TERMINAL;
    }

    return PreloadResult::ENDED;
}

void CacheManager::OnPathRead(const char* path, uint16_t pathLen, void (*onRead)(const char* path, uint16_t len))
{
    if ((preloadBootList_.Size() + preloadIdleList_.Size()) >= GetPreloadLimit()) return;

    char* fixedPath = FixSrcSuffix(path, strlen(PRELOAD_ROOT_DIR), pathLen);
    
    onRead(fixedPath == nullptr ? path : fixedPath, pathLen);

    UIFree(fixedPath);
    fixedPath = nullptr;
}

int16_t CacheManager::DoPreloadFully(const char* path, uint16_t cacheIndex, const Style& style)
{
    uint16_t hittedIndex = 0;
    RetCode ret = GetIndex(path, hittedIndex);
    if (ret == RetCode::OK) return hittedIndex;

    //越界检查
    if (cacheIndex >= GetPreloadLimit()) return CODE_NOT_PRELOAD;

    uint32_t time = HALTick::GetInstance().GetTime();
    if (TryDecode(path, style, entryArr_[cacheIndex]) == RetCode::OK) {
        if (ReadToCache(entryArr_[cacheIndex]) == RetCode::OK) {
            entryArr_[cacheIndex].dsc_.timeToOpen = time;
            entryArr_[cacheIndex].life_ = HALTick::GetInstance().GetTime();
            entryArr_[cacheIndex].persistent_ = true;

           // HILOG_INFO(HILOG_MODULE_ACE, "[%s] is preloaded, index=%u", path, cacheIndex);
            return cacheIndex;
        }
    }

    HILOG_ERROR(HILOG_MODULE_ACE, "Failed to preload[%s]!", path);
    return CODE_NOT_PRELOAD;
}

PreloadResult CacheManager::MaybePreloadSync(const char* path, const Style& style, CacheEntry& entry)
{
    PreloadEntry *preloadEntry = GetPreloadEntry(path, &preloadIdleList_);
    if (preloadEntry == nullptr || preloadEntry->seq == CODE_NOT_PRELOAD) return PreloadResult::TERMINAL;
    //2023年4月6日：首次读取资源并不在 UI 线程，必须获得锁以保证同步
    bool isUiThread = uiThreadId_ != pthread_self();
    if (!isUiThread) TryLockInTime(drawLock_, -1);

    TryLockInTime(preloadLock_, -1);
    preloadEntry = GetPreloadEntry(path, &preloadIdleList_);
    if (preloadEntry == nullptr || preloadEntry->seq == CODE_NOT_PRELOAD) return PreloadResult::TERMINAL;

    PreloadResult result;
    if (preloadEntry->loadBytes == 0) {
        //全量预加载
        int16_t index = DoPreloadFully(path, preloadEntry->seq, style);
        if (index == CODE_NOT_PRELOAD) {
            result = PreloadResult::TERMINAL;
        } else {
            entry = entryArr_[index];
            curPreloadEntry_ = nullptr;

            preloadEntry->loadBytes = entry.GetImageInfo().dataSize;
            preloadEntry->totalBytes = preloadEntry->loadBytes;

            result = PreloadResult::ENDED;
        }
    } else {
        //追加读取
        result = PreloadInTime(preloadEntry, INT32_MAX);
        if (result == PreloadResult::ENDED && IsPreloaded(preloadEntry)) {
            entry = entryArr_[preloadEntry->seq];
            curPreloadEntry_ = nullptr;
        } else {
            result = PreloadResult::TERMINAL;
        }
    }

    if (!isUiThread) Unlock(drawLock_);
    Unlock(preloadLock_);

    return result;
}

PreloadEntry* CacheManager::DoSavePreloadPath(List<PreloadEntry*>& list, const char* path, uint16_t len)
{
    //去重
    if (IsDuplicatePreload(path)) return nullptr;

    char* resPath = reinterpret_cast<char*>(UIMalloc(len + 1));
    if (resPath == nullptr || memcpy_s(resPath, len, path, len) != EOK) {
        UIFree(resPath);
        resPath = nullptr;
        return nullptr;
    }
    resPath[len] = '\0';

    PreloadEntry* entry = new PreloadEntry(resPath);
    list.PushBack(entry);

    return entry;
}

char* CacheManager::FixSrcSuffix(const char* path, uint16_t startPos, uint16_t& len)
{
    if (path == nullptr) return nullptr;
    if (len <= startPos) return nullptr;

    size_t suffixSize = sizeof(PRELOAD_SRC_SUFFIX) / sizeof(char);
    uint16_t splitLen = len;
    //试图找到 '.'。如果未找到，在末端拼接 PRELOAD_SRC_SUFFIX
    for (size_t i = len - 1; i >= startPos; i--) {
        if (path[i] != PRELOAD_SRC_SUFFIX[0]) continue;

        if (i + suffixSize < len) {
            //命中 ".bin"，不处理
            bool match = true;
            for (size_t j = 1; j < suffixSize; j++) {
                if (path[i + j] == PRELOAD_SRC_SUFFIX[j]) continue;
                match = false;
                break;
            }
            if (match) return nullptr;
        }

        splitLen = i;
        break;
    }
    
    char* tmp = reinterpret_cast<char*>(UIMalloc(splitLen));
    if (tmp == nullptr || memcpy_s(tmp, splitLen, path, splitLen) != EOK) {
        UIFree(tmp);
        tmp = nullptr;
        return nullptr;
    }

    char* fixedPath = reinterpret_cast<char*>(UIMalloc(splitLen + suffixSize + 1)); //+1：末端的'\0'
    if (fixedPath == nullptr || memcpy_s(fixedPath, splitLen, tmp, splitLen) != EOK) {
        UIFree(fixedPath);
        fixedPath = nullptr;
    }

    UIFree(tmp);
    tmp = nullptr;

    if (fixedPath == nullptr) return nullptr;

    for (size_t i = 0; i < suffixSize; i++) {
        fixedPath[splitLen + i] = PRELOAD_SRC_SUFFIX[i];
    }
    fixedPath[splitLen + suffixSize] = '\0';

    len = splitLen + suffixSize;

    return fixedPath;
}

void CacheManager::PrunePreload(ListNode<PreloadEntry*>* lastNode)
{
    ListNode<PreloadEntry*>* node = lastNode;
    while (true) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "Prune preload src[%s] cause by OUT OF MEMORY", node->data_ == nullptr ? "" : node->data_->path);
        node = preloadIdleList_.Remove(node);
        if (node == nullptr || node == preloadIdleList_.End()) break;
    }
}

uint32_t CacheManager::GetBestReadSizeByEvaluated(uint32_t duration)
{
    if (duration == 0 || EVALUATE_IO_COUNT == 0) return 0;

    uint32_t pos = EVALUATE_IO_COUNT - 1;
    int16_t l = 0, r = EVALUATE_IO_COUNT - 1, c = (l + r) / 2;
    while (l <= r) {
        if (bytesInTime[c][0] == duration) {
            pos = c;
            break;
        }

        if (bytesInTime[c][0] < duration) {
            l = c + 1;
            c = (l + r) / 2;
        } else {
            if (c - 1 >= 0 && bytesInTime[c - 1][0] <= duration) {
                pos = c - 1;
                break;
            }

            r = c - 1;
            if (r < l) {
                //未得到有效的评估值
                pos = 0;
                break;
            }

            c = (l + r) / 2;
        }
    }

    return bytesInTime[pos][1];
}

uint16_t CacheManager::GetPreloadLimit()
{
    return MATH_MAX(size_ * 4 / 5, 0); //最多使用 80% 的缓存池大小来作为常驻缓存区域
}

PreloadEntry* CacheManager::GetPreloadEntry(const char* path, const List<PreloadEntry*> *list)
{
    if (path == nullptr || list == nullptr || list->Size() == 0) return nullptr;

    PreloadEntry* entry;
    ListNode<PreloadEntry*>* node = list->Begin();
    while (node != nullptr && node != list->End()) {
        entry = node->data_;
        if (entry != nullptr) {
            if (!strcmp(entry->path, path)) return entry;
        }
        node = node->next_;
    }

    return nullptr;
}

bool CacheManager::IsDuplicatePreload(const char* path)
{
    PreloadEntry* entry;
    ListNode<PreloadEntry*>* node;
    
    node = preloadBootList_.Begin();
    while (node != nullptr && node != preloadBootList_.End()) {
        entry = node->data_;
        if (entry != nullptr) {
            if (!strcmp(entry->path, path)) return true;
        }
        node = node->next_;
    }

    node = preloadIdleList_.Begin();
    while (node != nullptr && node != preloadIdleList_.End()) {
        entry = node->data_;
        if (entry != nullptr) {
            if (!strcmp(entry->path, path)) return true;
        }
        node = node->next_;
    }

    return false;
}

bool CacheManager::IsPreloaded(const PreloadEntry* entry)
{
    return entry != nullptr && entry->loadBytes > 0 && entry->loadBytes == entry->totalBytes;
}

void CacheManager::ClearPreloadCache()
{
    for (uint16_t index = 0; index < GetSize(); index++) {
        if (entryArr_[index].dsc_.path != nullptr && entryArr_[index].persistent_) {
            entryArr_[index].dsc_.decoder->Close(entryArr_[index].dsc_);
            entryArr_[index].persistent_ = false;
            Clear(entryArr_[index]);
        }
    }

    preloadBootList_.Clear();
    preloadIdleList_.Clear();
    preloadCount_ = 0;
}

bool CacheManager::PruneCache(const char* excludePath, uint32_t num)
{
    if (excludePath == nullptr || num == 0) return false;

    int32_t index = -1;
    for (size_t i = preloadCount_; i < size_; i++) {
        if (entryArr_[i].dsc_.path == nullptr) continue;
        if (entryArr_[i].dsc_.imgInfo.data == nullptr) continue;
        if (!strcmp(entryArr_[i].dsc_.path, excludePath)) continue;
        if (index == -1) {
            index = i; 
            continue;
        }

        if (entryArr_[i].life_ < entryArr_[index].life_) index = i;
    }

    if (index == -1) return false;

    HILOG_INFO(HILOG_MODULE_ACE, "Prune cache[%s](%u)!", entryArr_[index].dsc_.path, entryArr_[index].dsc_.imgInfo.dataSize);
    entryArr_[index].Clear();
    num--;

    return num == 0 || PruneCache(excludePath, num);
}
#endif //BES_IMG_PRELOAD_ENABLE

const uint8_t* CacheEntry::GetImgData() const
{
    return dsc_.imgInfo.data;
}

RetCode CacheEntry::ReadLine(const Point& start, int16_t len, uint8_t* buf)
{
    RetCode ret;
    if (dsc_.decoder != nullptr) {
        ret = dsc_.decoder->ReadLine(dsc_, start, len, buf);
    } else {
        ret = RetCode::FAIL;
    }

    return ret;
}

void CacheEntry::Clear()
{
    if (dsc_.decoder != nullptr) {
        dsc_.decoder->Close(dsc_);
    }

    dsc_.decoder = nullptr;
    ClearSrc();
    dsc_.imgInfo.data = nullptr;
    dsc_.fd = -1;
    dsc_.srcType = IMG_SRC_UNKNOWN;
    life_ = 0;
}

void CacheEntry::ClearSrc()
{
    if (dsc_.srcType == IMG_SRC_FILE) {
        UIFree(const_cast<char*>(dsc_.path));
    }
    dsc_.path = nullptr;
}

RetCode CacheEntry::SetSrc(const char* path)
{
    ClearSrc();
    if (dsc_.srcType == IMG_SRC_FILE) {
        size_t strLen = strlen(path);
        if (strLen > MAX_SRC_LENGTH) {
            return RetCode::FAIL;
        }
        char* newStr = reinterpret_cast<char*>(UIMalloc(static_cast<uint32_t>(strLen) + 1));
        if (newStr == nullptr) {
            return RetCode::FAIL;
        }
        if (memcpy_s(newStr, strLen + 1, path, strLen) != EOK) {
            UIFree(newStr);
            newStr = nullptr;
            return RetCode::FAIL;
        }
        newStr[strLen] = '\0';
        dsc_.path = newStr;
    } else {
        dsc_.path = path;
    }
    return RetCode::OK;
}

CacheManager& CacheManager::GetInstance()
{
    static CacheManager instance;
    return instance;
}

RetCode CacheManager::Init(uint16_t size)
{
    if ((size == 0) || (size > MAX_CACHE_ENTRY_NUM)) {
        GRAPHIC_LOGE("Init failed due to invalid size[%u]. Range(0, %u]", size, MAX_CACHE_ENTRY_NUM);
        return RetCode::FAIL;
    }
    Reset();

#if (BES_IMG_PRELOAD_ENABLE == 1)
    ClearPreloadCache();
#endif //BES_IMG_PRELOAD_ENABLE

    if (entryArr_ != nullptr) {
        UIFree(reinterpret_cast<void*>(entryArr_));
        entryArr_ = nullptr;
    }

    uint32_t tmpCacheSize = size * sizeof(CacheEntry);
    entryArr_ = reinterpret_cast<CacheEntry*>(UIMalloc(tmpCacheSize));
    if (entryArr_ == nullptr) {
        size_ = 0;
        return RetCode::FAIL;
    }

    if (memset_s(entryArr_, tmpCacheSize, 0, tmpCacheSize) != EOK) {
        UIFree(reinterpret_cast<void*>(entryArr_));
        entryArr_ = nullptr;
        GRAPHIC_LOGE("Init failed due to OUT OF MEMORY");
        return RetCode::FAIL;
    }

    size_ = size;

    GRAPHIC_LOGI("Init success, cache pool size=%u", size_);

    return RetCode::OK;
}

RetCode CacheManager::Open(const char* path, const Style& style, CacheEntry& entry)
{
    if ((path == nullptr) || (GetSize() <= 0)) {
        return RetCode::FAIL;
    }

#if (BES_IMG_PRELOAD_ENABLE == 1)
    //当缓存未被关注时，其活跃值无意义，因此更新行为是多余的
#else
    AgingAll();
#endif //BES_IMG_PRELOAD_ENABLE

    uint16_t indexHitted = 0;
    RetCode ret = GetIndex(path, indexHitted);
    if (ret == RetCode::OK) {
        ret = ReadToCache(entryArr_[indexHitted]);
        //2023年4月12日：读取失败不应该使用该位置的缓存
        if (ret != RetCode::OK) return ret;

        entry = entryArr_[indexHitted];
        return RetCode::OK;
    }

#if (BES_IMG_PRELOAD_ENABLE == 1)
    PreloadResult result = MaybePreloadSync(path, style, entry);
    if (result == PreloadResult::ENDED) {
        return RetCode::OK;
    }
#endif //BES_IMG_PRELOAD_ENABLE

    SelectEntryToReplace(indexHitted);
    if (entryArr_[indexHitted].dsc_.path != nullptr) {
        entryArr_[indexHitted].dsc_.decoder->Close(entryArr_[indexHitted].dsc_);
    }

    uint32_t startTime = HALTick::GetInstance().GetTime();
    entryArr_[indexHitted].life_ = 0;

    ret = TryDecode(path, style, entryArr_[indexHitted]);
    if (ret == RetCode::OK) ret = ReadToCache(entryArr_[indexHitted]);

#if (BES_IMG_PRELOAD_ENABLE == 1)
    if (ret != RetCode::OK) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "OOM while load[%s]!", path);
        if (PruneCache(path, 1)) {
            ret = Open(path, style, entry);
        }
        return ret;
    }
    if (ret != RetCode::OK) return ret;

    entryArr_[indexHitted].dsc_.timeToOpen = startTime;
    entryArr_[indexHitted].life_ = HALTick::GetInstance().GetTime();
#else
    if (ret != RetCode::OK) return ret;

    entryArr_[indexHitted].life_ = HALTick::GetInstance().GetTime() - startTime;
#endif //BES_IMG_PRELOAD_ENABLE

    entry = entryArr_[indexHitted];
    return RetCode::OK;
}

RetCode CacheManager::Close(const char* path)
{
    if (path == nullptr) {
        return RetCode::FAIL;
    }

#if (BES_IMG_PRELOAD_ENABLE == 1)
    //从动态缓存区域始端开始检查
    for (uint16_t index = preloadCount_; index < GetSize(); index++) {
#else
    for (uint16_t index = 0; index < GetSize(); index++) {
#endif //BES_IMG_PRELOAD_ENABLE
        if (entryArr_[index].dsc_.srcType == IMG_SRC_FILE) {
            if (entryArr_[index].dsc_.path == nullptr) {
                continue;
            }
            if (strcmp(entryArr_[index].dsc_.path, path) == 0) {
                entryArr_[index].dsc_.decoder->Close(entryArr_[index].dsc_);
                Clear(entryArr_[index]);
                break;
            }
        } else {
            if (entryArr_[index].dsc_.path == path) {
                entryArr_[index].dsc_.decoder->Close(entryArr_[index].dsc_);
                Clear(entryArr_[index]);
                break;
            }
        }
    }

    return RetCode::OK;
}

bool CacheManager::GetImageHeader(const char* path, ImageHeader& header)
{
    CacheEntry entry;
    Style useless;
    RetCode ret = Open(path, useless, entry);
    if (ret != RetCode::OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Unknown src type of Image[%s]!", path);
        return false;
    }

    header = entry.GetImgHeader();
    return true;
}

RetCode CacheManager::Reset()
{
    if (entryArr_ == nullptr) {
        return RetCode::OK;
    }

#if (BES_IMG_PRELOAD_ENABLE == 1)
    //从从动态缓存区域始端开始检查
    for (uint16_t index = preloadCount_; index < GetSize(); index++) {
#else
    for (uint16_t index = 0; index < GetSize(); index++) {
#endif //BES_IMG_PRELOAD_ENABLE
        if (entryArr_[index].dsc_.path != nullptr) {
            entryArr_[index].dsc_.decoder->Close(entryArr_[index].dsc_);
            Clear(entryArr_[index]);
        }
    }

    return RetCode::OK;
}

RetCode CacheManager::ReadToCache(CacheEntry& entry)
{
    return entry.dsc_.decoder->ReadToCache(entry.dsc_);
}

void CacheManager::Clear(CacheEntry& entry)
{
    entry.Clear();
}

void CacheManager::AgingAll(int32_t time)
{
    for (uint16_t index = 0; index < GetSize(); index++) {
#if BES_IMG_PRELOAD_ENABLE == 1
    entryArr_[index].life_ = HALTick::GetInstance().GetTime();
#else
        if (entryArr_[index].life_ > INT32_MIN + AGING_INTERVAL) {
            entryArr_[index].life_ -= time;
        }
#endif //BES_IMG_PRELOAD_ENABLE
    }
}

RetCode CacheManager::GetIndex(const char* path, uint16_t& hittedIndex)
{
    for (uint16_t index = 0; index < GetSize(); index++) {
        bool hitted = false;
        if (entryArr_[index].dsc_.srcType == IMG_SRC_FILE) {
            if ((entryArr_[index].dsc_.path != nullptr) && !strcmp(path, entryArr_[index].dsc_.path)) {
                hitted = true;
            }
        } else {
            ImageInfo* imgDsc = reinterpret_cast<ImageInfo*>(const_cast<char*>(path));
            if ((entryArr_[index].dsc_.path == path) && (entryArr_[index].dsc_.imgInfo.data == imgDsc->data)) {
                hitted = true;
            }
        }

        if (hitted) {
#if (BES_IMG_PRELOAD_ENABLE == 1)
            bool isReady = true;
            if (index < preloadCount_) {
                isReady = IsPreloaded(GetPreloadEntry(path, &preloadBootList_)) || IsPreloaded(GetPreloadEntry(path, &preloadIdleList_));
            }
            if (!isReady) return RetCode::FAIL;

            entryArr_[index].life_ = HALTick::GetInstance().GetTime();
#else
            entryArr_[index].life_ += entryArr_[index].dsc_.timeToOpen * LIFE_GAIN_INTERVAL;
            if (entryArr_[index].life_ > LIFE_LIMIT) {
                entryArr_[index].life_ = LIFE_LIMIT;
            }
#endif //BES_IMG_PRELOAD_ENABLE

            hittedIndex = index;
            return RetCode::OK;
        }
    }

    return RetCode::FAIL;
}

RetCode CacheManager::SelectEntryToReplace(uint16_t& selectedIndex)
{
#if (BES_IMG_PRELOAD_ENABLE == 1)
    selectedIndex = preloadCount_;
    for (size_t index = preloadCount_; index < GetSize(); index++) {
        if (entryArr_[index].life_ < entryArr_[selectedIndex].life_) {
            selectedIndex = index;
        }
    }
#else
    selectedIndex = 0;
    for (uint16_t index = 0; index < GetSize(); index++) {
        if (entryArr_[index].life_ < entryArr_[selectedIndex].life_) {
            selectedIndex = index;
        }
    }
#endif //BES_IMG_PRELOAD_ENABLE

    return RetCode::OK;
}

RetCode CacheManager::TryDecode(const char* path, const Style& style, CacheEntry& entry)
{
    FileImgDecoder* decoder = &(FileImgDecoder::GetInstance());
    if (decoder == nullptr) {
        Clear(entry);
        return RetCode::FAIL;
    }

    entry.dsc_.srcType = IMG_SRC_FILE;
    RetCode ret = entry.SetSrc(path);
    if (ret != RetCode::OK) {
        Clear(entry);
        return ret;
    }
    entry.dsc_.decoder = decoder;

    ret = entry.dsc_.decoder->GetHeader(entry.dsc_);
    if (ret != RetCode::OK) {
        Clear(entry);
        return ret;
    }

    ret = entry.dsc_.decoder->Open(entry.dsc_);
    if (ret != RetCode::OK) {
        Clear(entry);
        return ret;
    }

    return ret;
}
} // namespace OHOS
