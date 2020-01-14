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

    void AddProperty(const std::string& id, BaseProperty* prop)
    {
        properties.emplace(id, prop);
    }

private:
    std::unordered_map<std::string, BaseProperty*> properties;
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
    TypeRegister()
    {
        printf("created new TypeRegister singleton!\n");
    }

    static inline void RegisterType(BaseType* type)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            tr->RegisterTypeImpl(type);
        }
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
