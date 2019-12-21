//////////////////////////////////////////////////////////////////////////
//    File        	: PhysicsThread.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/PhysicsThread.h"
#include "Core/Object.h"
#include "Core/Application.h"

//////////////////////////////////////////////////////////////////////////

using highres_clock = std::chrono::high_resolution_clock;
using time_point = std::chrono::time_point<highres_clock>;
using milliseconds = std::chrono::milliseconds;

//////////////////////////////////////////////////////////////////////////

static constexpr const f32 dt = 1.0f / 60.0f;
static constexpr const milliseconds targetFrameTime = milliseconds(16);

//////////////////////////////////////////////////////////////////////////

void* PhysicsMemAlloc::allocate(size_t alloc)
{
    return malloc(alloc);
}

//////////////////////////////////////////////////////////////////////////

void PhysicsMemAlloc::deallocate(void* data)
{
    free(data);
}

//////////////////////////////////////////////////////////////////////////

bool PhysicsThread::IsObjectRegisteredForTick(Object* object) const
{
    auto it = objects.find(object->GetInstanceId());
    return it != objects.end();
}

//////////////////////////////////////////////////////////////////////////

void PhysicsThread::AddObject(WeakRef<Object> object)
{
    if (Ref<Object> o = object.lock())
    {
        objects.emplace(o->GetInstanceId(), o);
    }
}

//////////////////////////////////////////////////////////////////////////

void PhysicsThread::RemoveObject(WeakRef<Object> object)
{
    if (Ref<Object> o = object.lock())
    {
        objects.erase(o->GetInstanceId());
    }
}

//////////////////////////////////////////////////////////////////////////

void PhysicsThread::RemoveObject(ObjectGuid id)
{
    objects.erase(id);
}

//////////////////////////////////////////////////////////////////////////

void PhysicsThread::ThreadLoop()
{
    Thread::SetName("* Physics Thread");

    while (running)
    {
        GetApplication()->OnPrePhysics();

        time_point frameStart = highres_clock::now();
        DispatchThreadTasks();

        for (auto object : objects)
        {
            if (Ref<Object> o = object.second.lock())
            {
                o->OnPhysicsUpdate(dt);
            }
        }

        time_point frameEnd = highres_clock::now();

        GetApplication()->OnPostPhysics();

        milliseconds totalFrameTime = std::chrono::duration_cast<milliseconds>(frameEnd - frameStart);
        if (totalFrameTime < targetFrameTime)
        {
            milliseconds sleepDuration = std::chrono::duration_cast<milliseconds>(targetFrameTime - totalFrameTime);
            std::this_thread::sleep_for(sleepDuration);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
