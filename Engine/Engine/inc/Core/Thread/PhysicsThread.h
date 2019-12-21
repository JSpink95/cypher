//////////////////////////////////////////////////////////////////////////
//    File        	: PhysicsThread.h
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

// #experiment - memory allocation
class PhysicsMemAlloc
{
public:
    void* allocate(size_t alloc);
    void deallocate(void* data);

    template<typename T>
    inline T* allocate() { return static_cast<T*>(allocate(sizeof(T))); }

public:
    static PhysicsMemAlloc& Instance()
    {
        static PhysicsMemAlloc instance;
        return instance;
    }
};

//////////////////////////////////////////////////////////////////////////

class PhysicsThread
    : public BaseThread
    , public Singleton<PhysicsThread>
{
public:
    PhysicsThread() = default;
    virtual ~PhysicsThread() = default;

    void AddObject(WeakRef<Object> object);
    void RemoveObject(WeakRef<Object> object);
    void RemoveObject(ObjectGuid id);

    bool IsObjectRegisteredForTick(Object* object) const;

protected:
    virtual void ThreadLoop() override;

private:
    using ObjectGuidMap = std::unordered_map<ObjectGuid, WeakRef<Object>>;

    ObjectGuidMap objects;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<PhysicsThread> GetPhysicsThread() { return GetSingleton<PhysicsThread>(); }

//////////////////////////////////////////////////////////////////////////
