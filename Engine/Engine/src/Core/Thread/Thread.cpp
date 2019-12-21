//////////////////////////////////////////////////////////////////////////
//    File        	: Thread.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/Thread.h"

//
// Usage: SetThreadName (-1, "MainThread");
//
#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////

namespace Thread
{
    DWORD GetCurrentId()
    {
        return ::GetThreadId(::GetCurrentThread());
    }

    void SetName(DWORD dwThreadID, char* threadName)
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }

    void SetName(char* threadName)
    {
        SetName(GetCurrentId(), threadName);
    }
}

//////////////////////////////////////////////////////////////////////////

BaseThread::BaseThread()
    : running(true)
{
    //handle = std::thread(&BaseThread::ThreadLoop, this);
    handle = std::async(&BaseThread::ThreadLoop, this);
}

//////////////////////////////////////////////////////////////////////////

BaseThread::~BaseThread()
{
    running = false;
    handle.get();
    //if (handle.joinable()) handle.join();
}

//////////////////////////////////////////////////////////////////////////

bool BaseThread::IsCurrentThread() const
{
    return false;
    //return (std::this_thread::get_id() == handle.get_id());
}

//////////////////////////////////////////////////////////////////////////

void BaseThread::PushThreadTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> guard(taskMutex);
    tasks.push_back(task);
}

//////////////////////////////////////////////////////////////////////////

void BaseThread::DispatchThreadTasks()
{
    std::lock_guard<std::mutex> guard(taskMutex);
    for (auto task : tasks) task();
    tasks.clear();
}

//////////////////////////////////////////////////////////////////////////
