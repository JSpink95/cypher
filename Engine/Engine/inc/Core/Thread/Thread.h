//////////////////////////////////////////////////////////////////////////
//    File        	: Thread.h
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <mutex>
#include <atomic>
#include <thread>
#include <future>
#include <vector>
#include <wtypes.h>

//////////////////////////////////////////////////////////////////////////

namespace Thread
{
    DWORD GetCurrentId();
    void SetName(DWORD dwThreadID, char* threadName);
    void SetName(char* threadName);
}

//////////////////////////////////////////////////////////////////////////

class BaseThread
{
public:
    BaseThread();
    virtual ~BaseThread();

    virtual bool IsCurrentThread() const;

    template<typename TObject>
    void PushThreadTask(TObject* object, void(TObject::*task)());

    void PushThreadTask(std::function<void()> task);

protected:
    std::atomic_bool running;
    //std::thread handle;
    std::future<void> handle;

    std::mutex taskMutex;
    std::vector<std::function<void()>> tasks;

protected:
    virtual void DispatchThreadTasks();
    virtual void ThreadLoop() = 0;
};

//////////////////////////////////////////////////////////////////////////

template<typename TObject>
void BaseThread::PushThreadTask(TObject* object, void(TObject::*task)())
{
    PushThreadTask(std::bind(task, object));
}

//////////////////////////////////////////////////////////////////////////
