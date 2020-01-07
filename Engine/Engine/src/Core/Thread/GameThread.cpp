//////////////////////////////////////////////////////////////////////////
//    File        	: GameThread.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/GameThread.h"
#include "Core/Thread/Thread.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/Application.h"
#include "Core/Utility/TimeUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

void GameThread::AddObject(WeakRef<Object> object)
{
    if (!object.expired())
    {
        Ref<Object> obj = object.lock();
        ObjectGuid guid = obj->GetInstanceId();

        if (registeredObjects.find(guid) == registeredObjects.end())
        {
            registeredObjects.emplace(guid, object);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void GameThread::RemoveObject(WeakRef<Object> object)
{
    if (!object.expired())
    {
        Ref<Object> obj = object.lock();
        ObjectGuid guid = obj->GetInstanceId();

        registeredObjects.erase(guid);
    }
}

//////////////////////////////////////////////////////////////////////////

void GameThread::RemoveObject(ObjectGuid id)
{
    registeredObjects.erase(id);
}

//////////////////////////////////////////////////////////////////////////

bool GameThread::IsObjectRegistered(Object* object)
{
    return registeredObjects.find(object->GetInstanceId()) != registeredObjects.end();
}

//////////////////////////////////////////////////////////////////////////

void GameThread::ThreadLoop()
{
    using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::duration<f32>;

    Thread::SetName("* Game Thread");

    static constexpr f32 timeDilation = 0.1f;

    GetApplication()->RenderCreate();

    time_point frameStart = clock::now();
    while (running)
    {
        time_point now = clock::now();
        const f32 dt = std::chrono::duration_cast<duration>(now - frameStart).count();
        frameStart = now;

        Time::UpdateTime(dt);

        DispatchThreadTasks();

        // update
        GetApplication()->OnPreUpdate(dt);

        for (auto object : registeredObjects)
        {
            WeakRef<Object> obj = object.second;
            if (!obj.expired())
            {
                obj.lock()->OnUpdate(dt);
            }
        }

        GetApplication()->OnPostUpdate();

        // render
        GetApplication()->OnPreRender();

        RenderPassManager::Render();

        GetApplication()->OnPostRender();

        // briefly sleep so as not to run _too_ fast
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // if we have thread tasks
    DispatchThreadTasks();
}

//////////////////////////////////////////////////////////////////////////

