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

////////////////////////////////////////////////////////////////////////////

#define DECLARE_CLASS(Class)                                                    \
private: using ThisClass = Class;                                               \
public: static inline const std::string ClassName = #Class;                     \
public: static inline ClassId ClassUID()                                        \
{                                                                               \
    return (ClassId)&ClassUID;                                                  \
}                                                                               \
public: virtual inline ClassId GetClassUID()                                    \
{                                                                               \
    return ClassUID();                                                          \
}                                                                               \
public: virtual inline const std::string& GetTypeName()                         \
{                                                                               \
    return ClassName;                                                           \
}                                                                               \
public: virtual inline const u32& GetByteSize()                                 \
{                                                                               \
    return sizeof(Class);                                                       \
}                                                                               \
private: static inline Ref<Class> iaresource = std::make_shared<Class>();
//private: Class(){}                                                              \

////////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE(Base)                                                      \
DECLARE_CLASS(Base)                                                             \
public: virtual inline bool IsTypeOf(ClassId id)                                \
{                                                                               \
    return id == ClassUID();                                                    \
}

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED(Derived, Base)                                          \
DECLARE_CLASS(Derived)                                                          \
private: using Super = Base;                                                    \
public: virtual inline bool IsTypeOf(ClassId id) override                       \
{                                                                               \
    return id == ClassUID() || Super::IsTypeOf(id);                             \
}

////////////////////////////////////////////////////////////////////////////

#define DECLARE_COMPONENT(Derived, Base)                                        \
DECLARE_DERIVED(Derived, Base)

//////////////////////////////////////////////////////////////////////////

#define DECLARE_OBJECT(Derived, Base)                                           \
DECLARE_DERIVED(Derived, Base)                                                  \
private: template<typename T> friend Ref<T> CreateObject(const ObjectId& id);

////////////////////////////////////////////////////////////////////////////
