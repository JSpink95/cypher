//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/TypeRegister.h"
#include "Core/TypeId.h"

//////////////////////////////////////////////////////////////////////////

#include <functional>

//////////////////////////////////////////////////////////////////////////

class Mesh;
class Material;
class RTTIObject;
class VertexArray;
class PropertyBase;
class ComponentRefBase;

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    bool IsRefType(const std::string& typeName);
    std::string TrimRefModifier(const std::string& typeName);

    template<typename T>
    bool DisplayEditBox(void* owner, PropertyBase* prop, const std::string& id, T& value) { return false; }

    template<typename T>
    std::string ToString(const T& value) { return "not implemented"; }

    template<typename T>
    bool SetValue(const std::string& value, T& editable) { return false; }

    // forward declare concrete implementations - DisplayEditBox
    template<> bool DisplayEditBox<bool>(void* owner, PropertyBase* prop, const std::string& id, bool& editable);
    template<> bool DisplayEditBox<s32>(void* owner, PropertyBase* prop, const std::string& id, s32& editable);
    template<> bool DisplayEditBox<f32>(void* owner, PropertyBase* prop, const std::string& id, f32& editable);
    template<> bool DisplayEditBox<float2>(void* owner, PropertyBase* prop, const std::string& id, float2& editable);
    template<> bool DisplayEditBox<float3>(void* owner, PropertyBase* prop, const std::string& id, float3& editable);
    template<> bool DisplayEditBox<std::string>(void* owner, PropertyBase* prop, const std::string& id, std::string& editable);
    template<> bool DisplayEditBox<Ref<Material>>(void* owner, PropertyBase* prop, const std::string& id, Ref<Material>& editable);
    template<> bool DisplayEditBox<Ref<Mesh>>(void* owner, PropertyBase* prop, const std::string& id, Ref<Mesh>& editable);

    // forward declare concrete implementations - ToString
    template<> std::string ToString<bool>(const bool& value);
    template<> std::string ToString<s32>(const s32& value);
    template<> std::string ToString<f32>(const f32& value);
    template<> std::string ToString<float2>(const float2& value);
    template<> std::string ToString<float3>(const float3& value);
    template<> std::string ToString<std::string>(const std::string& value);
    template<> std::string ToString<Ref<Material>>(const Ref<Material>& value);
    template<> std::string ToString<Ref<Mesh>>(const Ref<Mesh>& value);
    template<> std::string ToString<ObjectId>(const ObjectId& value);
    template<> std::string ToString<ComponentId>(const ComponentId& value);

    // forward declare concrete implementations - SetValue
    template<> bool SetValue<bool>(const std::string& value, bool& editable);
    template<> bool SetValue<s32>(const std::string& value, s32& editable);
    template<> bool SetValue<f32>(const std::string& value, f32& editable);
    template<> bool SetValue<float2>(const std::string& value, float2& editable);
    template<> bool SetValue<float3>(const std::string& value, float3& editable);
    template<> bool SetValue<std::string>(const std::string& value, std::string& editable);
    template<> bool SetValue<Ref<Material>>(const std::string& value, Ref<Material>& editable);
    template<> bool SetValue<Ref<Mesh>>(const std::string& value, Ref<Mesh>& editable);
    template<> bool SetValue<ObjectId>(const std::string& value, ObjectId& editable);
    template<> bool SetValue<ComponentId>(const std::string& value, ComponentId& editable);
}

//////////////////////////////////////////////////////////////////////////

class IPropertyChangedListener
{
public:
    virtual void OnPropertyChanged(PropertyBase* prop) {}
};

////////////////////////////////////////////////////////////////////////////

class MetaData
{
public:
    friend class MetaParser;

public:
    void LoadMetaData(const std::string& string);

public:
    bool HasMetaData(const std::string& id) const;
    float2 GetAsFloat2(const std::string& id) const;

private:
    std::unordered_map<std::string, std::string> meta;
};

////////////////////////////////////////////////////////////////////////////

class PropertyBase
{
public:
    PropertyBase(const size_t inOffset, const std::string& inPropertyName, const std::string inTypeName, const std::string& inMetaString)
        : offset(inOffset)
        , propertyName(inPropertyName)
        , typeName(inTypeName)
    {
        meta.LoadMetaData(inMetaString);
    }

    virtual ~PropertyBase() {}

public:
    virtual bool IsRTTIObjectProperty() const;
    virtual bool IsListProperty() const;
    virtual bool IsMapProperty() const;
    virtual bool IsRefType() const;

public:
    void* AsVoidPointer(void* base);
    const void* AsVoidPointer(void* base) const;
    RTTIObject* AsRTTIObject(void* base);

    template<typename T> inline T* AsType(void* base)
    {
        char* bytes = reinterpret_cast<char*>(base) + offset;
        return reinterpret_cast<T*>(bytes);
    }

    template<typename T> inline const T* AsType(void* base) const
    {
        char* bytes = reinterpret_cast<char*>(base) + offset;
        return reinterpret_cast<const T*>(bytes);
    }

public:
    TypeBase* GetType();

public:
    virtual std::string ToString(void* base) = 0;
    virtual void SetValue(void* base, void* data) = 0;
    virtual void SetValue(void* base, const std::string& value) = 0;
    virtual bool DisplayEditBox(void* base) = 0;

public:
    const size_t offset;
    const std::string propertyName;
    const std::string typeName;
    MetaData meta;
};

////////////////////////////////////////////////////////////////////////////

class Property_ListBase : public PropertyBase
{
public:
    using ListIteratorFunction = std::function<void(const s32, void*)>;

public:
    Property_ListBase(const size_t inOffset, const std::string& inPropertyName, const std::string& inValueTypeName)
        : PropertyBase(inOffset, inPropertyName, inValueTypeName, "")
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
    virtual void AddValue(void* base, void* data) = 0;
    virtual void AddValue(void* base, const std::string& value) = 0;
    virtual void ForEachItem(void* base, ListIteratorFunction predicate) = 0;
    virtual std::string ValueToString(void* base, const s32 index) = 0;

public:

    virtual inline void SetValue(void* base, void* data) override final
    {
        printf("[Property_ListBase::SetValue(void*,void*)] - Not implemented.\n");
    }

    virtual inline void SetValue(void* base, const std::string& value) override final
    {
        printf("[Property_ListBase::SetValue(void*,const std::string&)] - Not implemented.\n");
    }

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
        : PropertyBase(inOffset, inPropertyName, inValueTypeName, "")
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
    virtual const void* CreateKeyFromString(void* base, const std::string& key) = 0;
    virtual void EmplaceValue(void* base, const void* key, void* value) = 0;
    virtual void EmplaceValueFromString(void* base, const void* key, const std::string& value) = 0;

    virtual size_t Count(void* base) const = 0;
    virtual void ForEachItem(void* base, MapIteratorFunction predicate) = 0;
    virtual std::string KeyToString(void* base, const void* k) = 0;
    virtual std::string ValueToString(void* base, const void* k) = 0;

public:

    virtual inline void SetValue(void* base, void* data) override final
    {
        printf("[Property_MapBase::SetValue(void*,void*)] - Not implemented.\n");
    }

    virtual inline void SetValue(void* base, const std::string& value) override final
    {
        printf("[Property_MapBase::SetValue(void*,const std::string&)] - Not implemented.\n");
    }

public:
    const std::string keyTypeName;
    const std::string valueTypeName;
};

////////////////////////////////////////////////////////////////////////////

template<typename T>
class Property : public PropertyBase
{
public:
    Property(const size_t offset, const std::string& inPropertyName, const std::string& inTypeName, const std::string& meta)
        : PropertyBase(offset, inPropertyName, inTypeName, meta)
    {}

    virtual ~Property() {}

public:

    virtual std::string ToString(void* base) override
    {
        T& editable = *AsType<T>(base);
        return RTTI::ToString<T>(editable);
    }

    virtual void SetValue(void* base, void* data) override
    {
        T& editable = *AsType<T>(base);
        editable = *reinterpret_cast<T*>(data);
    }

    virtual void SetValue(void* base, const std::string& value) override
    {
        T& editable = *AsType<T>(base);
        RTTI::SetValue<T>(value, editable);
    }

    virtual bool DisplayEditBox(void* base) override
    {
        T& editable = *AsType<T>(base);
        return RTTI::DisplayEditBox<T>(base, this, propertyName, editable);
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
        const list_type* list = AsType<list_type>(base);
        return list->size();
    }

    virtual void AddValue(void* base, void* data) override
    {
        list_type* list = AsType<list_type>(base);
        list->push_back(*reinterpret_cast<TValue*>(data));
    }

    virtual void AddValue(void* base, const std::string& value) override
    {
        TValue v;
        RTTI::SetValue<TValue>(value, v);

        list_type* list = AsType<list_type>(base);
        list->push_back(v);
    }

    virtual void ForEachItem(void* base, ListIteratorFunction predicate) override
    {
        list_type* list = AsType<list_type>(base);
        for (size_t index = 0; index < list->size(); ++index)
        {
            TValue* value = &list->at(index);
            predicate(index, (void*)value);
        }
    }
    
    virtual std::string ValueToString(void* base, const s32 index) override
    {
        list_type* list = AsType<list_type>(base);
        if (index < 0 || index >= list->size())
            return "out of bounds";

        return RTTI::ToString(list->at(index));
    }

public:
    virtual bool IsRefType() const override
    {
        return RTTI::IsRefType(valueTypeName);
    }

    virtual std::string ToString(void* base) override
    {
        return "todo";
    }

    virtual bool DisplayEditBox(void* base) override
    {
        const bool valueTypeIsRef = RTTI::IsRefType(valueTypeName);
        const bool isRTTIObjectType = TypeRegister::IsRegisteredType(RTTI::TrimRefModifier(valueTypeName));

        // #todo - implement list edit

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
    virtual const void* CreateKeyFromString(void* base, const std::string& key) override
    {
        map_type* map = AsType<map_type>(base);

        TKey newKey;
        RTTI::SetValue<TKey>(key, newKey);

        auto it = map->emplace(newKey, TValue()).first;
        return reinterpret_cast<const void*>(&it->first);
    }

    virtual void EmplaceValue(void* base, const void* key, void* value) override
    {
        map_type* map = AsType<map_type>(base);
        const TKey* k = reinterpret_cast<const TKey*>(key);

        auto it = map->find(*k);
        if (it != map->end())
        {
            it->second = *reinterpret_cast<TValue*>(value);
        }
    }
    
    virtual void EmplaceValueFromString(void* base, const void* key, const std::string& value)
    {
        map_type* map = AsType<map_type>(base);
        const TKey* k = reinterpret_cast<const TKey*>(key);

        auto it = map->find(*k);
        if (it != map->end())
        {
            TValue v;
            RTTI::SetValue<TValue>(value, v);

            it->second = v;
        }
    }

    virtual size_t Count(void* base) const override
    {
        const map_type* map = AsType<map_type>(base);
        return map->size();
    }

    virtual void ForEachItem(void* base, MapIteratorFunction predicate) override
    {
        map_type* map = AsType<map_type>(base);
        for (auto it = map->begin(); it != map->end(); ++it)
        {
            const TKey* key = &it->first;
            TValue* value = &it->second;
            predicate(reinterpret_cast<const void*>(key), reinterpret_cast<void*>(value));
        }
    }

    virtual std::string KeyToString(void* base, const void* k) override
    {
        map_type* map = AsType<map_type>(base);
        const TKey* key = reinterpret_cast<const TKey*>(k);

        return RTTI::ToString<TKey>(*key);
    }

    virtual std::string ValueToString(void* base, const void* k) override
    {
        map_type* map = AsType<map_type>(base);
        const TKey* key = reinterpret_cast<const TKey*>(k);

        auto it = map->find(*key);
        if (it != map->end())
        {
            return RTTI::ToString(it->second);
        }

        return "";
    }

public:
    virtual bool IsRefType() const override
    {
        return RTTI::IsRefType(valueTypeName);
    }

    virtual std::string ToString(void* base) override
    {
        return "todo";
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
    AutoPropertyRegister(const size_t offset, const std::string& name, const std::string& typeName, const std::string& meta)
        : type(offset, name, typeName, meta)
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
