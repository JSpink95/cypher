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

#define DEFINE_SERIALISATION_TYPES(Class)                                                   \
public: using PropertySetterFunction = std::function<void(Ref<Class>,const std::string&)>; 

//////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE_COMPONENT(Base)                                            \
DEFINE_CLASS_UID(Base)                                                          \
DEFINE_SERIALISATION_TYPES(Base)                                                \
public: virtual inline ClassId GetClassUID() { return ClassUID(); }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED_COMPONENT(Derived, Base)                                \
DEFINE_CLASS_UID(Derived)                                                       \
private: using Super = Base;                                                    \
DEFINE_SERIALISATION_TYPES(Derived)                                             \
public: virtual inline ClassId GetClassUID() override { return ClassUID(); }

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
