//////////////////////////////////////////////////////////////////////////
//    File        	: TypeRegister.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class BaseProperty;

//////////////////////////////////////////////////////////////////////////

class BaseType
{
public:
    virtual ~BaseType() {}

public:
    virtual const std::string& GetTypeId() const = 0;
    virtual const size_t GetTypeSize() const = 0;

public:

    const std::string& GetBaseId() const
    {
        return baseId;
    }

public:

    void SetBaseId(const std::string& newBaseId)
    {
        baseId = newBaseId;
    }

    void AddProperty(const std::string& id, BaseProperty* prop)
    {
        properties.emplace(id, prop);
    }

public:

    // stl compatibility

    using property_map = std::unordered_map<std::string, BaseProperty*>;
    using iterator = property_map::iterator;
    using const_iterator = property_map::const_iterator;

    iterator begin() { return properties.begin(); }
    const_iterator begin() const { return properties.begin(); }

    iterator end() { return properties.end(); }
    const_iterator end() const { return properties.end(); }

private:
    property_map properties;
    std::string baseId;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class Type: public BaseType
{
public:
    static inline const std::string TypeId = T::ClassName;
    static inline const size_t TypeSize = sizeof(T);

public:
    virtual inline const std::string& GetTypeId() const override { return TypeId; };
    virtual inline const size_t GetTypeSize() const override { return TypeSize; };
};

//////////////////////////////////////////////////////////////////////////

class TypeRegister : public AutoConstructSingleton<TypeRegister>
{
public:
    static inline void RegisterType(BaseType* type)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            tr->RegisterTypeImpl(type);
        }
    }

    static inline BaseType* GetRegisteredType(const std::string& typeId)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            return tr->GetRegisteredTypeImpl(typeId);
        }

        return nullptr;
    }

    template<typename T>
    static inline Type<T>* GetRegisteredType()
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            return tr->GetRegisteredTypeImpl<T>();
        }

        return nullptr;
    }

private:
    inline void RegisterTypeImpl(BaseType* type)
    {
        types.emplace(type->GetTypeId(), type);
    }

    inline BaseType* GetRegisteredTypeImpl(const std::string& typeId)
    {
        auto it = types.find(typeId);
        if (it != types.end())
        {
            return it->second;
        }

        return nullptr;
    }

    template<typename T>
    inline Type<T>* GetRegisteredTypeImpl()
    {
        auto it = types.find(Type<T>::TypeId);
        if (it != types.end())
        {
            return static_cast<Type<T>*>(it->second);
        }

        return nullptr;
    }

private:
    std::unordered_map<std::string, BaseType*> types;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class AutoTypeRegister
{
public:
    AutoTypeRegister()
    {
        TypeRegister::RegisterType(&type);
    }

private:
    Type<T> type;
};

//////////////////////////////////////////////////////////////////////////

template<typename TDerived, typename TBase>
class AutoTypeRegisterWithBase
{
public:
    AutoTypeRegisterWithBase()
    {
        TypeRegister::RegisterType(&type);
        type.SetBaseId(TBase::ClassName);
    }

private:
    Type<TDerived> type;
};

//////////////////////////////////////////////////////////////////////////
