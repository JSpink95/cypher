//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/TypeRegister.h"

//////////////////////////////////////////////////////////////////////////

class BaseProperty
{
public:
    virtual ~BaseProperty() {}

    virtual void SetFromBlob(void* base, void* blob) = 0;
    virtual void SetFromString(void* base, const std::string& value) = 0;

public:
    size_t offset = 0;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class Property: public BaseProperty
{
public:
    void SetFromStrongType(void* base, const T& value)
    {
        *(T*)(reinterpret_cast<char*>(base) + offset) = value;
    }

    // maybe we can get away with doing some kind of cast...
    virtual void SetFromBlob(void* base, void* blob) override
    {
        T& strongType = *(T*)blob;
        SetFromStrongType(base, strongType);
    }

    // needs concrete implementation per type.
    virtual void SetFromString(void* base, const std::string& string) override
    {
        // implemented elsewhere
    }
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class RefProperty: public BaseProperty
{
public:
    using RefType = Ref<T>;

public:
    void SetFromStrongType(void* base, const RefType& value)
    {
        *(RefType*)(reinterpret_cast<char*>(base) + offset) = value;
    }

    // maybe we can get away with doing some kind of cast...
    virtual void SetFromBlob(void* base, void* blob) override
    {
        // this doesn't work for ref types
    }

    // needs concrete implementation per type.
    virtual void SetFromString(void* base, const std::string& string) override
    {
        // implemented elsewhere
    }
};

//////////////////////////////////////////////////////////////////////////

template<typename T, typename TProperty>
class AutoPropertyRegister
{
public:
    AutoPropertyRegister(const std::string& id, const size_t offset)
    {
        TypeRegister::GetRegisteredType<T>()->AddProperty(id, &prop);
        prop.offset = offset;
    }

private:
    Property<TProperty> prop;
};

//////////////////////////////////////////////////////////////////////////

template<typename T, typename TProperty>
class AutoRefPropertyRegister
{
public:
    AutoRefPropertyRegister(const std::string& id, const size_t offset)
    {
        TypeRegister::GetRegisteredType<T>()->AddProperty(id, &prop);
        prop.offset = offset;
    }

private:
    RefProperty<TProperty> prop;
};

//////////////////////////////////////////////////////////////////////////
