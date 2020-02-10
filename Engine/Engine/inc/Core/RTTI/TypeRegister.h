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

class PropertyBase;

//////////////////////////////////////////////////////////////////////////

class TypeBase
{
public:
    using property_iterator = std::unordered_map<std::string, PropertyBase*>::iterator;
    using const_property_iterator = std::unordered_map<std::string, PropertyBase*>::const_iterator;

public:
    TypeBase(const std::string& inTypeName, ClassId inTypeId)
        : typeName(inTypeName)
        , typeId(inTypeId)
    {}

    virtual ~TypeBase() {}

public:
    virtual size_t GetSizeInBytes() const = 0;
    virtual void* New() = 0;

public:
    inline const std::string& GetTypeName() const { return typeName; }
    inline const ClassId& GetTypeId() const { return typeId; }

    inline void SetBaseType(const std::string& inBaseTypeName, const ClassId& inBaseTypeId)
    {
        baseTypeName = inBaseTypeName;
        baseTypeId = inBaseTypeId;
    }

public:
    void AddProperty(PropertyBase* newProperty);
    PropertyBase* FindProperty(const std::string& id);
    size_t GetPropertyCount() const;

    TypeBase* GetBaseType() const;
    bool IsTypeOf(const std::string& className) const;

public:

    inline property_iterator property_begin() { return properties.begin(); }
    inline property_iterator property_end() { return properties.end(); }

    inline const_property_iterator property_begin() const { return properties.begin(); }
    inline const_property_iterator property_end() const { return properties.end(); }

private:
    const std::string typeName;
    const ClassId typeId;

    std::string baseTypeName;
    ClassId baseTypeId;

    std::unordered_map<std::string, PropertyBase*> properties;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class Type: public TypeBase
{
public:
    static inline const std::string TypeName = T::ClassName;
    static inline const ClassId TypeId = T::ClassUID();
    static inline const size_t ByteSize = sizeof(T);

public:
    Type()
        : TypeBase(TypeName, T::ClassUID())
    {}

    virtual ~Type() {}

public:
    inline virtual size_t GetSizeInBytes() const override
    {
        return ByteSize;
    }

    inline virtual void* New() override
    {
        return new T;
    }
};

//////////////////////////////////////////////////////////////////////////

class TypeRegister : public AutoConstructSingleton<TypeRegister>
{
public:
    static inline void RegisterType(TypeBase* type)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            tr->RegisterTypeImpl(type);
        }
    }

    static inline bool IsRegisteredType(const std::string& typeName)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            return tr->IsRegisteredTypeImpl(typeName);
        }

        return false;
    }

    static inline bool IsRegisteredTypeOf(const std::string& typeName, ClassId classId)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            return tr->IsRegisteredTypeOfImpl(typeName, classId);
        }

        return false;
    }

    static inline TypeBase* GetRegisteredType(const std::string& typeName)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            return tr->GetRegisteredTypeImpl(typeName);
        }

        return nullptr;
    }

    static inline void GetRegisteredTypesOfBase(const std::string& baseTypeName, std::vector<TypeBase*>& types)
    {
        if (Ref<TypeRegister> tr = TypeRegister::Get())
        {
            return tr->GetRegisteredTypesOfBaseImpl(baseTypeName, types);
        }
    }

    template<typename T>
    static inline Type<T>* GetRegisteredType()
    {
        return dynamic_cast<Type<T>*>(GetRegisteredType(Type<T>::TypeName));
    }

    template<typename T>
    static inline void GetRegisteredTypesOfBase(std::vector<TypeBase*>& types)
    {
        GetRegisteredTypesOfBase(Type<T>::TypeName, types);
    }

private:
    void RegisterTypeImpl(TypeBase* type);
    bool IsRegisteredTypeImpl(const std::string& typeName);
    bool IsRegisteredTypeOfImpl(const std::string& typeName, ClassId classId);
    TypeBase* GetRegisteredTypeImpl(const std::string& typeName);
    void GetRegisteredTypesOfBaseImpl(const std::string& baseTypeName, std::vector<TypeBase*>& types);

private:
    std::unordered_map<std::string, TypeBase*> types;
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
        type.SetBaseType(TBase::ClassName, TBase::ClassUID());
    }

private:
    Type<TDerived> type;
};

//////////////////////////////////////////////////////////////////////////
