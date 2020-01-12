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

	static bool IsObjectRegistered(WeakRef<Object> object);
	static void AddObject(WeakRef<Object> object);
	static void RemoveObject(WeakRef<Object> object);

private:
    bool IsObjectRegisteredImpl(WeakRef<Object> object);
    void AddObjectImpl(WeakRef<Object> object);
    void RemoveObjectImpl(WeakRef<Object> object);

private:
    using ObjectIdHashMap = std::unordered_map<ObjectId, WeakRef<Object>>;

    ObjectIdHashMap registeredObjects;

private:
    virtual void ThreadLoop() override;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<GameThread> GetGameThread() { return GetSingleton<GameThread>(); }

//////////////////////////////////////////////////////////////////////////
