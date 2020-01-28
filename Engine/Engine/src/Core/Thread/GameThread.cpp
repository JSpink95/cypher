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

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

#ifdef _OPENGL
    #include "imgui_impl_opengl3.h"
    #include "imgui_impl_glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

bool GameThread::IsObjectRegistered(WeakRef<Object> object)
{
	if (Ref<GameThread> thread = GetGameThread())
	{
		return thread->IsObjectRegisteredImpl(object);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void GameThread::AddObject(WeakRef<Object> object)
{
	if (Ref<GameThread> thread = GetGameThread())
	{
		thread->AddObjectImpl(object);
	}
}

//////////////////////////////////////////////////////////////////////////

void GameThread::RemoveObject(WeakRef<Object> object)
{
	if (Ref<GameThread> thread = GetGameThread())
	{
		thread->RemoveObjectImpl(object);
	}
}

//////////////////////////////////////////////////////////////////////////

void GameThread::AddObjectImpl(WeakRef<Object> object)
{
    if (!object.expired())
    {
        Ref<Object> obj = object.lock();
        ObjectId id = obj->GetId();

        if (registeredObjects.find(id) == registeredObjects.end())
        {
            registeredObjects.emplace(id, object);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void GameThread::RemoveObjectImpl(WeakRef<Object> object)
{
    if (!object.expired())
    {
        Ref<Object> obj = object.lock();
        registeredObjects.erase(obj->GetId());
    }
}

//////////////////////////////////////////////////////////////////////////

bool GameThread::IsObjectRegisteredImpl(WeakRef<Object> object)
{
	if (Ref<Object> obj = object.lock())
	{
		return registeredObjects.find(obj->GetId()) != registeredObjects.end();
	}

	return false;
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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        GetApplication()->OnImGuiRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        GetApplication()->OnPresent();

        // briefly sleep so as not to run _too_ fast
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // if we have thread tasks
    DispatchThreadTasks();
}

//////////////////////////////////////////////////////////////////////////

