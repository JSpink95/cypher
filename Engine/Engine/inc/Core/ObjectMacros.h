//////////////////////////////////////////////////////////////////////////
//    File        	: ObjectMacros.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/RTTI/TypeRegister.h"

//////////////////////////////////////////////////////////////////////////

#include <string>
#include <functional>

//////////////////////////////////////////////////////////////////////////

#define DEFINE_CLASS_UID(Class)                                                 \
public: static inline const std::string ClassName = #Class;                     \
public: static inline ClassId ClassUID()                                        \
{                                                                               \
    return (ClassId)&ClassUID;                                                  \
}

//////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE_COMPONENT(Base)                                            \
DEFINE_CLASS_UID(Base)                                                          \
public: virtual inline ClassId GetClassUID() { return ClassUID(); }             \
public: virtual inline std::string GetTypeName() { return ClassName; }          \
public: virtual inline bool IsTypeOf(ClassId id)                                \
{                                                                               \
    return id == ClassUID();                                                    \
}

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED_COMPONENT(Derived, Base)                                \
DEFINE_CLASS_UID(Derived)                                                       \
private: using Super = Base;                                                    \
public: virtual inline ClassId GetClassUID() override { return ClassUID(); }    \
public: virtual inline std::string GetTypeName() override { return ClassName; } \
public: virtual inline bool IsTypeOf(ClassId id) override                       \
{                                                                               \
    return (ClassUID() == id) || Super::IsTypeOf(id);                           \
}

//////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE_OBJECT(Base)                                               \
DEFINE_CLASS_UID(Base)                                                          \
private: template<typename T> friend Ref<T> CreateObject(const ObjectId& id);   \
private: using SelfType = Base;                                                 \
protected: WeakRef<SelfType> self;                                              \
public: inline void SetSelf(Ref<Base> newSelf) { self = newSelf; }              \
public: inline Ref<Object> GetStrongRef() { return self.lock(); }               \
public: inline WeakRef<Object> GetWeakRef() { return self; }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED_OBJECT(Derived, Base)                                   \
DEFINE_CLASS_UID(Derived)                                                       \
private: template<typename T> friend Ref<T> CreateObject(const ObjectId& id);   \
private: using Super = Base;                                                    \
private: using SelfType = Derived;                                              \

//////////////////////////////////////////////////////////////////////////
