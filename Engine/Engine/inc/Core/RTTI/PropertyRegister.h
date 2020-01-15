//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/TypeRegister.h"

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    template<typename T>
    void SetValueFromString(const std::string& value, T& editable);

    template<typename T>
    void ShowEditBox(const std::string& id, T& value);
}

//////////////////////////////////////////////////////////////////////////

class BaseProperty
{
public:
    virtual ~BaseProperty() {}

    virtual void SetFromBlob(void* base, void* blob) = 0;
    virtual void SetFromString(void* base, const std::string& value) = 0;

public:
    virtual void ShowEditBox(const std::string& id, void* base) = 0;

public:
    size_t offset = 0;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class Property: public BaseProperty
{
public:

    inline T& GetValueFromContainer(void* base)
    {
        unsigned char* bytes = reinterpret_cast<unsigned char*>(base);
        return *(T*)(bytes + offset);
    }

    inline void SetFromStrongType(void* base, const T& value)
    {
        unsigned char* bytes = reinterpret_cast<unsigned char*>(base);
        *(T*)(bytes + offset) = value;
    }

    // maybe we can get away with doing some kind of cast...
    virtual void SetFromBlob(void* base, void* blob) override
    {
        T& strongType = *(T*)blob;
        SetFromStrongType(base, strongType);
    }

public:
    // needs concrete implementation per type.
    virtual void SetFromString(void* base, const std::string& string) override
    {
        RTTI::SetValueFromString<T>(string, GetValueFromContainer(base));
    }

    virtual void ShowEditBox(const std::string& id, void* base) override
    {
        RTTI::ShowEditBox<T>(id, GetValueFromContainer(base));
    }
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class RefProperty: public BaseProperty
{
public:
    using RefType = Ref<T>;

public:
    inline RefType& GetValueFromContainer(void* base)
    {
        unsigned char* bytes = reinterpret_cast<unsigned char*>(base);
        return *(RefType*)(bytes + offset);
    }

    inline void SetFromStrongType(void* base, const RefType& value)
    {
        unsigned char* bytes = reinterpret_cast<unsigned char*>(base);
        *(RefType*)(bytes + offset) = value;
    }

    // maybe we can get away with doing some kind of cast...
    virtual void SetFromBlob(void* base, void* blob) override
    {
        // this doesn't work for ref types
    }

public:
    // needs concrete implementation per type.
    virtual void SetFromString(void* base, const std::string& string) override
    {
        RTTI::SetValueFromString<RefType>(string, GetValueFromContainer(base));
    }

    virtual void ShowEditBox(const std::string& id, void* base) override
    {
        RTTI::ShowEditBox<RefType>(id, GetValueFromContainer(base));
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
