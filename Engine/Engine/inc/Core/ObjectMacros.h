//////////////////////////////////////////////////////////////////////////
//    File        	: ObjectMacros.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE_COMPONENT(Base)                                            \
public: virtual GUID GetClassId() const { return Base::ClassId; }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED_COMPONENT(Derived, Base)                                \
private: using Super = Base;                                                    \
public: virtual GUID GetClassId() const override { return Derived::ClassId; }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_BASE_OBJECT(Base)                                               \
private: template<typename T> friend Ref<T> CreateObject(const std::string& id);\
private: using SelfType = Base;                                                 \
protected: WeakRef<SelfType> self;                                              \
public: inline void SetSelf(Ref<Base> newSelf) { self = newSelf; }              \
public: inline Ref<Object> GetStrongRef() { return self.lock(); }             \
public: inline WeakRef<Object> GetWeakRef() { return self; }

//////////////////////////////////////////////////////////////////////////

#define DECLARE_DERIVED_OBJECT(Derived, Base)                                   \
private: template<typename T> friend Ref<T> CreateObject(const std::string& id);\
private: using Super = Base;                                                    \
private: using SelfType = Derived;                                              \

//////////////////////////////////////////////////////////////////////////
