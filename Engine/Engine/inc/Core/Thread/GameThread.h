//////////////////////////////////////////////////////////////////////////
//    File        	: GameThread.h
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Events/EventDispatcher.h"
#include "Core/Thread/Thread.h"
#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <future>
#include <functional>

//////////////////////////////////////////////////////////////////////////

class Object;

//////////////////////////////////////////////////////////////////////////

class GameThread
    : public BaseThread
    , public Singleton<GameThread>
{
public:
    GameThread() = default;
    virtual ~GameThread() = default;

    void AddObject(WeakRef<Object> object);
    void RemoveObject(WeakRef<Object> object);
    void RemoveObject(ObjectGuid id);

    bool IsObjectRegistered(Object* object);

private:
    using ObjectGuidMap = std::unordered_map<ObjectGuid, WeakRef<Object>>;

    ObjectGuidMap registeredObjects;

private:
    virtual void ThreadLoop() override;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<GameThread> GetGameThread() { return GetSingleton<GameThread>(); }

//////////////////////////////////////////////////////////////////////////
