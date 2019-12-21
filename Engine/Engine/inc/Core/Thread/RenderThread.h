//////////////////////////////////////////////////////////////////////////
//    File        	: RenderThread.h
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/Thread.h"
#include "Core/Singleton.h"
#include "Core/Events/EventDispatcher.h"

//////////////////////////////////////////////////////////////////////////

class Object;
class Window;

//////////////////////////////////////////////////////////////////////////

class RenderThread
    : public BaseThread
    , public Singleton<RenderThread>
{
public:
    RenderThread() = default;
    virtual ~RenderThread() = default;

    void AssignContext(Ref<Window> renderContext);

    bool IsObjectRegisteredForTick(Object* object) const;
    void RegisterGameObjectForTick(Object* object);
    void DeregisterGameObjectForTick(Object* object);

protected:
    virtual void ThreadLoop() override;

private:
    EventDispatcher<Object, std::function<void()>, 0u> dispatcher;
    std::mutex dispatcherMutex;

    Ref<Window> context;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<RenderThread> GetRenderThread() { return GetSingleton<RenderThread>(); }

//////////////////////////////////////////////////////////////////////////
