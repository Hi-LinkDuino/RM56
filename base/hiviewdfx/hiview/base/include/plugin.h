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
#ifndef HIVIEW_BASE_PLUGIN_H
#define HIVIEW_BASE_PLUGIN_H

#include <atomic>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "defines.h"
#include "dynamic_module.h"
#include "event.h"
#include "event_loop.h"
#include "plugin_extra_info.h"

class HiEvent;
namespace OHOS {
namespace HiviewDFX {
class HiviewContext;
class DllExport Plugin : public EventHandler, public std::enable_shared_from_this<Plugin> {
public:
    enum class PluginType {
        STATIC,
        DYNAMIC,
        PROXY,
    };
public:
    Plugin() : handle_(DynamicModuleDefault), context_(nullptr){};
    virtual ~Plugin();
    // do not store the event in the callback
    // create a new one through copy constructor is preferred
    virtual bool OnEvent(std::shared_ptr<Event>& event) override;
    virtual bool CanProcessEvent(std::shared_ptr<Event> event) override;
    virtual bool CanProcessMoreEvents() override;
    bool OnEventProxy(std::shared_ptr<Event> event) override;
    virtual std::string GetHandlerInfo() override;

    // check whether the plugin should be loaded in current environment
    // the plugin will not be load if return false
    // called by plugin framework in main thread
    virtual bool ReadyToLoad()
    {
        return true;
    };

    // the plugin instance will be stored in platform after calling this function
    // and other loaded plugin can pass event to this plugin
    // called by plugin framework in main thread
    virtual void OnLoad(){};

    // release resource and clear pending workloads
    // after calling this function ,the reference of this plugin will be deleted
    // called by plugin framework in main thread
    virtual void OnUnload(){};

    // dump plugin info from dump command line
    virtual void Dump(int fd __UNUSED, const std::vector<std::string>& cmds __UNUSED) {};

    // create an event with specific type
    virtual std::shared_ptr<Event> GetEvent(Event::MessageType type)
    {
        auto event = std::make_shared<Event>(name_);
        event->messageType_ = type;
        return event;
    };

    // called by audit module
    virtual std::string GetPluginInfo();

    // Listener callback
    virtual void OnEventListeningCallback(const Event &msg)
    {
        return;
    }

    // Make sure that you insert non-overlayed range
    void AddEventListenerInfo(uint32_t type, const EventListener::EventIdRange &range = EventListener::EventIdRange(0));
    void AddEventListenerInfo(uint32_t type, const std::set<EventListener::EventIdRange> &listenerInfo);
    bool GetEventListenerInfo(uint32_t type, std::set<EventListener::EventIdRange> &listenerInfo);

    void AddEventListenerInfo(uint32_t type, const std::string& eventName);
    void AddEventListenerInfo(uint32_t type, const std::set<std::string> &eventNames);
    bool GetEventListenerInfo(uint32_t type, std::set<std::string> &eventNames);

    // reinsert the event into the workloop
    // delay in seconds
    void DelayProcessEvent(std::shared_ptr<Event> event, uint64_t delay);

    const std::string& GetName();
    const std::string& GetVersion();
    void SetName(const std::string& name);
    void SetVersion(const std::string& version);
    void BindWorkLoop(std::shared_ptr<EventLoop> loop);
    void UpdateActiveTime();
    void UpdateTimeByDelay(time_t delay);
    std::shared_ptr<EventLoop> GetWorkLoop();

    HiviewContext* GetHiviewContext()
    {
        return context_;
    };

    void SetHiviewContext(HiviewContext* context)
    {
        std::call_once(contextSetFlag_, [&]() { context_ = context; });
    };

    void SetHandle(DynamicModule handle)
    {
        handle_ = handle;
        if (handle_ != nullptr) {
            type_ = PluginType::DYNAMIC;
        }
    };

    void SetBundleName(const std::string& bundle)
    {
        bundle_ = bundle;
    }

    bool IsBundlePlugin()
    {
        return bundle_.empty();
    }

    bool HasLoaded()
    {
        return loaded_;
    }

    void SetType(PluginType type)
    {
        type_ = type;
    }

    PluginType GetType() const
    {
        return type_;
    }

    time_t GetLastActiveTime() const
    {
        return lastActiveTime_;
    }
protected:
    std::string name_;
    std::string bundle_;
    std::string version_;
    std::shared_ptr<EventLoop> workLoop_;
    PluginType type_ = PluginType::STATIC;
    std::atomic<time_t> lastActiveTime_;

private:
    DynamicModule handle_;
    // the reference of the plugin platform
    // always available in framework callbacks
    HiviewContext* context_;
    std::once_flag contextSetFlag_;
    std::atomic<bool> loaded_;
};
class HiviewContext {
public:
    struct InstanceInfo {
        std::weak_ptr<Plugin> plugin;
        std::weak_ptr<EventListener> listener;
        bool isPlugin;
    };
    
    virtual ~HiviewContext(){};
    // post event to broadcast queue, the event will be delivered to all plugin that concern this event
    virtual void PostUnorderedEvent(std::shared_ptr<Plugin> plugin __UNUSED, std::shared_ptr<Event> event __UNUSED) {};

    // register listener to unordered broadcast queue
    virtual void RegisterUnorderedEventListener(std::weak_ptr<EventListener> listener __UNUSED) {};

     // register dynamic listener to queue
    virtual void RegisterDynamicListenerInfo(std::weak_ptr<Plugin> listener __UNUSED) {};

    // send a event to a specific plugin and wait the return of the OnEvent.
    virtual bool PostSyncEventToTarget(std::shared_ptr<Plugin> caller __UNUSED, const std::string& callee __UNUSED,
                                       std::shared_ptr<Event> event __UNUSED)
    {
        return true;
    }

    // send a event to a specific plugin and return at once
    virtual void PostAsyncEventToTarget(std::shared_ptr<Plugin> caller __UNUSED, const std::string& callee __UNUSED,
                                        std::shared_ptr<Event> event __UNUSED) {};

    // post event to distributed communicator plugin, to send to remote device
    virtual int32_t PostEventToRemote(std::shared_ptr<Plugin> caller __UNUSED, const std::string& deviceId __UNUSED,
        const std::string& targetPlugin __UNUSED, std::shared_ptr<Event> event __UNUSED)
    {
        return 0;
    }
    // request plugin platform to release plugin instance
    // do not recommend unload an plugin in pipeline scheme
    // platform will check the reference count if other module still holding the reference of this module
    virtual void RequestUnloadPlugin(std::shared_ptr<Plugin> caller __UNUSED) {};

    // publish plugin capacity and get remote capacity
    virtual void PublishPluginCapacity(PluginCapacityInfo &pluginCapacityInfo __UNUSED) {};
    virtual void GetRemoteByCapacity(const std::string& plugin __UNUSED, const std::string& capacity __UNUSED,
        std::list<std::string> &deviceIdList __UNUSED) {};

    // get the shared event loop reference
    virtual std::shared_ptr<EventLoop> GetSharedWorkLoop()
    {
        return nullptr;
    }

    // get predefined pipeline list
    virtual std::list<std::weak_ptr<Plugin>> GetPipelineSequenceByName(const std::string& name __UNUSED)
    {
        return std::list<std::weak_ptr<Plugin>>(0);
    }

    // check if all non-pending loaded plugins are loaded
    virtual bool IsReady()
    {
        return false;
    }

    enum class DirectoryType {
        CONFIG_DIRECTORY,
        CLOUD_UPDATE_DIRECTORY,
        WORK_DIRECTORY,
        PERSIST_DIR,
    };
    // get hiview available directory
    virtual std::string GetHiViewDirectory(DirectoryType type __UNUSED)
    {
        return "";
    }
    virtual std::string GetHiviewProperty(const std::string& key __UNUSED, const std::string& defaultValue)
    {
        return defaultValue;
    }

    virtual bool SetHiviewProperty(const std::string& key __UNUSED, const std::string& value __UNUSED,
        bool forceUpdate __UNUSED)
    {
        return true;
    }

    virtual void AppendPluginToPipeline(const std::string& pluginName __UNUSED,
                                        const std::string& pipelineName __UNUSED) {};
    virtual void RequestLoadBundle(const std::string& bundleName __UNUSED) {};
    virtual std::shared_ptr<Plugin> InstancePluginByProxy(std::shared_ptr<Plugin> proxy __UNUSED)
    {
        return nullptr;
    }

    virtual std::shared_ptr<Plugin> GetPluginByName(const std::string& name)
    {
        return nullptr;
    }

    virtual void AddListenerInfo(uint32_t type, const std::string& name,
    const std::set<std::string>& eventNames, const std::set<EventListener::EventIdRange>& listenerInfo) {};

    virtual void AddListenerInfo(uint32_t type, std::weak_ptr<Plugin> plugin,
        const std::set<std::string>& eventNames, const std::set<EventListener::EventIdRange>& listenerInfo) {};

    virtual std::vector<std::weak_ptr<InstanceInfo>> GetListenerInfo(uint32_t type,
        const std::string& eventNames, uint32_t eventId)
    {
        return std::vector<std::weak_ptr<InstanceInfo>>();
    }

    virtual bool GetListenerInfo(uint32_t type, const std::string& name,
        std::set<EventListener::EventIdRange> &listenerInfo)
    {
        return false;
    }

    virtual bool GetListenerInfo(uint32_t type, const std::string& name, std::set<std::string> &eventNames)
    {
        return false;
    }
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_PLUGIN_H
