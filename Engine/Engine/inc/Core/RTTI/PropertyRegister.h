//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/TypeRegister.h"

//////////////////////////////////////////////////////////////////////////

class BaseProperty;
class ComponentRefBase;

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    template<typename T>
    void SetValueFromString(const std::string& value, T& editable);

    template<typename T>
    bool ShowEditBox(void* owner, BaseProperty* prop, const std::string& id, T& value);

    bool ShowComponentRefEditBox(void* owner, BaseProperty* prop, ClassId classId, const std::string& id, ComponentRefBase* editable);
}

//////////////////////////////////////////////////////////////////////////

class IPropertyChangedListener
{
public:
    virtual void OnPropertyChanged(BaseProperty* prop) {}
};

//////////////////////////////////////////////////////////////////////////

class BaseProperty
{
public:
    virtual ~BaseProperty() {}

public:
    virtual void SetFromBlob(void* base, void* blob) = 0;
    virtual void SetFromString(void* base, const std::string& value) = 0;

public:
    virtual bool ShowEditBox(const std::string& id, void* base) = 0;

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

    virtual bool ShowEditBox(const std::string& id, void* base) override
    {
        return RTTI::ShowEditBox<T>(base, this, id, GetValueFromContainer(base));
    }
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class PropertyWithNotify : public Property<T>
{
public:
    virtual bool ShowEditBox(const std::string& id, void* base) override
    {
        const bool changed = Property<T>::ShowEditBox(id, base);

        if (changed)
        {
            if (IPropertyChangedListener* listener = (IPropertyChangedListener*)base)
            {
                listener->OnPropertyChanged(this);
            }
        }

        return changed;
    }
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class PropertyComponentRef : public BaseProperty
{
public:

    inline ComponentRefBase* GetValueFromContainer(void* base)
    {
        unsigned char* bytes = reinterpret_cast<unsigned char*>(base);
        return (ComponentRefBase*)(bytes + offset);
    }

public:

    // maybe we can get away with doing some kind of cast...
    virtual void SetFromBlob(void* base, void* blob) override
    {
        //T& strongType = *(T*)blob;
        //SetFromStrongType(base, strongType);
    }

public:
    // needs concrete implementation per type.
    virtual void SetFromString(void* base, const std::string& string) override
    {
        //RTTI::SetValueFromString<T>(string, GetValueFromContainer(base));
    }

    virtual bool ShowEditBox(const std::string& id, void* base) override
    {
        ComponentRefBase* editable = GetValueFromContainer(base);
        return RTTI::ShowComponentRefEditBox(base, this, T::ClassUID(), id, editable);
        //return RTTI::ShowEditBox<T>(base, this, id, GetValueFromContainer(base));
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
class AutoPropertyRegisterWithNotify
{
public:
    AutoPropertyRegisterWithNotify(const std::string& id, const size_t offset)
    {
        TypeRegister::GetRegisteredType<T>()->AddProperty(id, &prop);
        prop.offset = offset;
    }

private:
    PropertyWithNotify<TProperty> prop;
};

//////////////////////////////////////////////////////////////////////////

template<typename T, typename TProperty>
class AutoPropertyRegisterComponentRef
{
public:
    AutoPropertyRegisterComponentRef(const std::string& id, const size_t offset)
    {
        TypeRegister::GetRegisteredType<T>()->AddProperty(id, &prop);
        prop.offset = offset;
    }

private:
    PropertyComponentRef<TProperty> prop;
};

//////////////////////////////////////////////////////////////////////////
