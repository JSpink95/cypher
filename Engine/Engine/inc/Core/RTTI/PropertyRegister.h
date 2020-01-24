//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/TypeRegister.h"

//////////////////////////////////////////////////////////////////////////

#include <functional>

//////////////////////////////////////////////////////////////////////////

class Material;
class RTTIObject;
class VertexArray;
class PropertyBase;
class ComponentRefBase;
class StaticMesh;

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    bool IsRefType(const std::string& typeName);
    std::string TrimRefModifier(const std::string& typeName);

    template<typename T>
    bool DisplayEditBox(void* owner, PropertyBase* prop, const std::string& id, T& value) { return false; }

    // forward declare concrete implementations
    template<> bool DisplayEditBox<bool>(void* owner, PropertyBase* prop, const std::string& id, bool& editable);
    template<> bool DisplayEditBox<s32>(void* owner, PropertyBase* prop, const std::string& id, s32& editable);
    template<> bool DisplayEditBox<f32>(void* owner, PropertyBase* prop, const std::string& id, f32& editable);
    template<> bool DisplayEditBox<float2>(void* owner, PropertyBase* prop, const std::string& id, float2& editable);
    template<> bool DisplayEditBox<float3>(void* owner, PropertyBase* prop, const std::string& id, float3& editable);
    template<> bool DisplayEditBox<std::string>(void* owner, PropertyBase* prop, const std::string& id, std::string& editable);
    template<> bool DisplayEditBox<Ref<Material>>(void* owner, PropertyBase* prop, const std::string& id, Ref<Material>& editable);
    template<> bool DisplayEditBox<StaticMesh>(void* owner, PropertyBase* prop, const std::string& id, StaticMesh& editable);
}

//////////////////////////////////////////////////////////////////////////

class IPropertyChangedListener
{
public:
    virtual void OnPropertyChanged(PropertyBase* prop) {}
};

////////////////////////////////////////////////////////////////////////////

class PropertyBase
{
public:
    PropertyBase(const size_t inOffset, const std::string& inPropertyName, const std::string inTypeName)
        : offset(inOffset)
        , propertyName(inPropertyName)
        , typeName(inTypeName)
    {}

    virtual ~PropertyBase() {}

public:
    virtual bool IsRTTIObjectProperty() const;
    virtual bool IsListProperty() const;
    virtual bool IsMapProperty() const;

    virtual bool IsRefType() const;

    void* AsVoidPointer(void* base);
    const void* AsVoidPointer(void* base) const;

    TypeBase* GetType();
    RTTIObject* AsRTTIObject(void* base);

public:
    virtual bool DisplayEditBox(void* base) = 0;

public:
    const size_t offset;
    const std::string propertyName;
    const std::string typeName;
};

////////////////////////////////////////////////////////////////////////////

class Property_ListBase : public PropertyBase
{
public:
    using ListIteratorFunction = std::function<void(void*)>;

public:
    Property_ListBase(const size_t inOffset, const std::string& inPropertyName, const std::string& inValueTypeName)
        : PropertyBase(inOffset, inPropertyName, inValueTypeName)
        , valueTypeName(inValueTypeName)
    {}

    virtual ~Property_ListBase() {}

public:
    virtual bool IsRTTIObjectProperty() const override;
    virtual bool IsListProperty() const override;

public:
    virtual bool IsValueRefType() const;
    virtual bool IsValueRTTIObjectType() const;

public:
    virtual size_t Count(void* base) const = 0;
    virtual void ForEachItem(void* base, ListIteratorFunction predicate) = 0;

public:
    const std::string valueTypeName;
};

////////////////////////////////////////////////////////////////////////////

class Property_MapBase : public PropertyBase
{
public:
    using MapIteratorFunction = std::function<void(const void*, void*)>;

public:
    Property_MapBase(const size_t inOffset, const std::string& inPropertyName, const std::string& inKeyTypeName, const std::string& inValueTypeName)
        : PropertyBase(inOffset, inPropertyName, inValueTypeName)
        , keyTypeName(inKeyTypeName)
        , valueTypeName(inValueTypeName)
    {}

    virtual ~Property_MapBase() {}

public:
    virtual bool IsRTTIObjectProperty() const override;
    virtual bool IsMapProperty() const override;

public:
    virtual bool IsKeyRefType() const;
    virtual bool IsKeyRTTIObjectType() const;
    virtual bool IsValueRefType() const;
    virtual bool IsValueRTTIObjectType() const;

public:
    TypeBase* GetKeyType();
    TypeBase* GetValueType();

public:
    virtual size_t Count(void* base) const = 0;
    virtual void ForEachItem(void* base, MapIteratorFunction predicate) = 0;

public:
    const std::string keyTypeName;
    const std::string valueTypeName;
};

////////////////////////////////////////////////////////////////////////////

template<typename T>
class Property : public PropertyBase
{
public:
    Property(const size_t offset, const std::string& inPropertyName, const std::string& inTypeName)
        : PropertyBase(offset, inPropertyName, inTypeName)
    {}

    virtual ~Property() {}

public:
    virtual bool DisplayEditBox(void* base) override
    {
        return RTTI::DisplayEditBox<T>(base, this, propertyName, *(T*)AsVoidPointer(base));
    }
};

////////////////////////////////////////////////////////////////////////////

template<template<typename> typename TList, typename TValue>
class Property_List : public Property_ListBase
{
public:
    using list_type = TList<TValue>;

public:
    Property_List(const size_t offset, const std::string& inPropertyName, const std::string& inValueTypeName)
        : Property_ListBase(offset, inPropertyName, inValueTypeName)
    { }

    virtual ~Property_List() {}

public:

    virtual size_t Count(void* base) const override
    {
        const list_type* list = (const list_type*)AsVoidPointer(base);
        return list->size();
    }

    virtual void ForEachItem(void* base, ListIteratorFunction predicate) override
    {
        list_type* list = (list_type*)AsVoidPointer(base);
        for (size_t index = 0; index < list->size(); ++index)
        {
            TValue* value = &list->at(index);
            predicate((void*)value);
        }
    }

public:
    virtual bool IsRefType() const override
    {
        return RTTI::IsRefType(valueTypeName);
    }

    virtual bool DisplayEditBox(void* base) override
    {
        const bool valueTypeIsRef = RTTI::IsRefType(valueTypeName);
        const bool isRTTIObjectType = TypeRegister::IsRegisteredType(RTTI::TrimRefModifier(valueTypeName));

        return false;
    }
};

////////////////////////////////////////////////////////////////////////////

template<template<typename, typename> typename TMap, typename TKey, typename TValue>
class Property_Map: public Property_MapBase
{
public:
    using map_type = TMap<TKey, TValue>;

public:
    Property_Map(const size_t offset, const std::string& inPropertyName, const std::string& inKeyTypeName, const std::string& inValueTypeName)
        : Property_MapBase(offset, inPropertyName, inKeyTypeName, inValueTypeName)
    {

    }

    virtual ~Property_Map() {}

public:
    virtual size_t Count(void* base) const override
    {
        map_type* map = (map_type*)AsVoidPointer(base);
        return map->size();
    }

    virtual void ForEachItem(void* base, MapIteratorFunction predicate) override
    {
        map_type* map = (map_type*)AsVoidPointer(base);
        for (auto it = map->begin(); it != map->end(); ++it)
        {
            const TKey* key = &it->first;
            TValue* value = &it->second;
            predicate((const void*)key, (void*)value);
        }
    }

public:
    virtual bool IsRefType() const override
    {
        return RTTI::IsRefType(valueTypeName);
    }

    virtual bool DisplayEditBox(void* base) override
    {
        const bool isRTTIObjectType = TypeRegister::IsRegisteredType(valueTypeName);
        return false;
    }
};

////////////////////////////////////////////////////////////////////////////

template<typename TOwnerType, typename TPropertyType>
class AutoPropertyRegister
{
public:
    AutoPropertyRegister(const size_t offset, const std::string& name, const std::string& typeName)
        : type(offset, name, typeName)
    {
        TypeRegister::GetRegisteredType<TOwnerType>()->AddProperty(&type);
    }

private:
    Property<TPropertyType> type;
};

////////////////////////////////////////////////////////////////////////////

template<typename TOwnerType, template<typename> typename TList, typename TValue>
class AutoPropertyRegister_List
{
public:
    AutoPropertyRegister_List(const size_t offset, const std::string& name, const std::string& valueName)
        : type(offset, name, valueName)
    {
        TypeRegister::GetRegisteredType<TOwnerType>()->AddProperty(&type);
    }

private:
    Property_List<TList, TValue> type;
};

////////////////////////////////////////////////////////////////////////////

template<typename TOwnerType, template<typename, typename> typename TMap, typename TKey, typename TValue>
class AutoPropertyRegister_Map
{
public:
    AutoPropertyRegister_Map(const size_t offset, const std::string& name, const std::string& keyName, const std::string& valueName)
        : type(offset, name, keyName, valueName)
    {
        TypeRegister::GetRegisteredType<TOwnerType>()->AddProperty(&type);
    }

private:
    Property_Map<TMap, TKey, TValue> type;
};

////////////////////////////////////////////////////////////////////////////
