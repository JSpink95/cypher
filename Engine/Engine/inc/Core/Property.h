//////////////////////////////////////////////////////////////////////////
//    File        	: Property.h
//    Created By    : Jack Spink
//    Created On 	: [4/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class IProperty
{
public:
    virtual ~IProperty() {}

    virtual std::string GetPropertyId() const = 0;
    virtual u32 GetPropertyTypeId() const = 0;
};

//////////////////////////////////////////////////////////////////////////

// this class pretty much just keeps track of properties...
class PropertyContainer
{
public:
    using PropertyMap = std::unordered_map<std::string, IProperty*>;

public:
    virtual ~PropertyContainer() {}

    void AddProperty(IProperty* newProperty);
    void RemoveProperty(IProperty* property);

    template<typename TPredicate>
    void ForEachProperty(TPredicate predicate)
    {
        for (auto prop : properties)
            predicate(prop->first, prop->second);
    }

private:
    std::unordered_map<std::string, IProperty*> properties;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class Property : public IProperty
{
public:
    using ThisType = Property<T>;

public:

    static u32 PropertyTypeId()
    {
        static u32 id;
        return &id;
    }

public:

    virtual std::string GetPropertyId() const
    {
        return id;
    }

    virtual u32 GetPropertyTypeId() const
    {
        return PropertyTypeId();
    }

public:
    void Create(PropertyContainer* container, const std::string& pid)
    {
        id = pid;
        container->AddProperty(this);
    }

    void Destroy(PropertyContainer* container)
    {
        container->RemoveProperty(this);
    }

private:
    std::string id;
};

//////////////////////////////////////////////////////////////////////////

using Int32Property = Property<s32>;
using UInt32Property = Property<u32>;
using FloatProperty = Property<f32>;
using StringProperty = Property<std::string>;

//////////////////////////////////////////////////////////////////////////
