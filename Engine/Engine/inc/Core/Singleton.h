//////////////////////////////////////////////////////////////////////////
//    File        	: Singleton.h
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

//
// Typical singleton. Explicitly Created/Deleted.
//

template<typename T>
class Singleton
{
public:
    using SingletonType = T;

public:

    static void Create();
    static void Delete();

    static inline Ref<T> Get() { return instance; }

public:
    virtual void OnCreate() {}
    virtual void OnDelete() {}

private:
    static inline Ref<T> instance = nullptr;
};

//////////////////////////////////////////////////////////////////////////

// 
// Special case singleton. implicitly Created/Deleted.
// 

template<typename T>
class AutoConstructSingleton
{
public:
    static inline Ref<T> Get()
    {
        static Ref<T> instance = std::make_shared<T>();
        return instance;
    }
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
inline void Singleton<T>::Create()
{
    if (instance == nullptr)
    {
        instance = std::make_shared<T>();
        instance->OnCreate();
    }
}

//////////////////////////////////////////////////////////////////////////

template<typename T>
inline void Singleton<T>::Delete()
{
    if (instance != nullptr)
    {
        instance->OnDelete();
        instance = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

template<typename T>
static inline Ref<T> GetSingleton() { return T::Get(); }

//////////////////////////////////////////////////////////////////////////
