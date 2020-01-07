//////////////////////////////////////////////////////////////////////////
//    File        	: ObjectMacros.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////


#define DEFINE_CLASS_UID                                                        \
public: static inline ClassId ClassUID()                                        \
{                                                                               \
    return (ClassId)&ClassUID;                                                  \
}

//////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE_COMPONENT(Base)                                            \
DEFINE_CLASS_UID                                                                \
public: virtual inline ClassId GetClassUID() { return ClassUID(); }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED_COMPONENT(Derived, Base)                                \
DEFINE_CLASS_UID                                                                \
private: using Super = Base;                                                    \
public: virtual inline ClassId GetClassUID() override { return ClassUID(); }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE_OBJECT(Base)                                               \
DEFINE_CLASS_UID                                                                \
private: template<typename T> friend Ref<T> CreateObject(const std::string& id);\
private: using SelfType = Base;                                                 \
protected: WeakRef<SelfType> self;                                              \
public: inline void SetSelf(Ref<Base> newSelf) { self = newSelf; }              \
public: inline Ref<Object> GetStrongRef() { return self.lock(); }               \
public: inline WeakRef<Object> GetWeakRef() { return self; }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED_OBJECT(Derived, Base)                                   \
DEFINE_CLASS_UID                                                                \
private: template<typename T> friend Ref<T> CreateObject(const std::string& id);\
private: using Super = Base;                                                    \
private: using SelfType = Derived;                                              \

//////////////////////////////////////////////////////////////////////////
