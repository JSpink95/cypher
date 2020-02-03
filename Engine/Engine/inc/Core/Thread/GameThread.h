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

#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <future>
#include <functional>

//////////////////////////////////////////////////////////////////////////

class Object;

//////////////////////////////////////////////////////////////////////////

class TickFunction : public RTTIObject
{
    DECLARE_DERIVED(TickFunction, RTTIObject)
public:
    virtual void ExecuteTick(f32 dt) {}

public:
    bool enabled = false;
};

//////////////////////////////////////////////////////////////////////////

class GameThread
    : public BaseThread
    , public Singleton<GameThread>
{
public:
    static void RegisterTickFunction(TickFunction* tick);
    static void DeregisterTickFunction(TickFunction* tick);

private:
    void RegisterTickFunctionImpl(TickFunction* tick);
    void DeregisterTickFunctionImpl(TickFunction* tick);

private:
    using ObjectIdHashMap = std::unordered_map<ObjectId, WeakRef<Object>>;

    ObjectIdHashMap registeredObjects;
    std::vector<TickFunction*> ticks;

private:
    virtual void ThreadLoop() override;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<GameThread> GetGameThread() { return GetSingleton<GameThread>(); }

//////////////////////////////////////////////////////////////////////////
