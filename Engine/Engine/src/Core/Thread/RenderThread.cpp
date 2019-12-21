//////////////////////////////////////////////////////////////////////////
//    File        	: RenderThread.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/RenderThread.h"
#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"
#include "Core/Application.h"

//////////////////////////////////////////////////////////////////////////

void RenderThread::AssignContext(Ref<Window> renderContext)
{
    auto AssignContextToRenderThread = [renderContext, this]() -> void
    {
        context = renderContext;
        context->ForceFocused();
    };

    PushThreadTask(AssignContextToRenderThread);
}

//////////////////////////////////////////////////////////////////////////

bool RenderThread::IsObjectRegisteredForTick(Object* object) const
{
    return dispatcher.IsRegisteredForEvent(object);
}

//////////////////////////////////////////////////////////////////////////

void RenderThread::RegisterGameObjectForTick(Object* object)
{
    std::lock_guard<std::mutex> guard(dispatcherMutex);
    //dispatcher.AddListener(object, &Object::OnRender);
}

//////////////////////////////////////////////////////////////////////////

void RenderThread::DeregisterGameObjectForTick(Object* object)
{
    std::lock_guard<std::mutex> guard(dispatcherMutex);
    dispatcher.RemoveListener(object);
}

//////////////////////////////////////////////////////////////////////////

void RenderThread::ThreadLoop()
{
    using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<clock>;
    using milliseconds = std::chrono::milliseconds;

    static constexpr const f32 dt = 1.0f / 60.0f;
    static constexpr const milliseconds targetFrameTime = milliseconds(16);

    Thread::SetName("* Render Thread");

    while (running)
    {
        time_point frameStart = clock::now();

        GetApplication()->OnPreRender();

        {
            std::lock_guard<std::mutex> guard(dispatcherMutex);
            dispatcher.Dispatch();
        }

        DispatchThreadTasks();

        GetApplication()->OnPostRender();

        time_point frameEnd = clock::now();

        milliseconds totalFrameTime = std::chrono::duration_cast<milliseconds>(frameEnd - frameStart);
        if (totalFrameTime < targetFrameTime)
        {
            milliseconds sleepDuration = std::chrono::duration_cast<milliseconds>(targetFrameTime - totalFrameTime);
            std::this_thread::sleep_for(sleepDuration);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
