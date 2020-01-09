//////////////////////////////////////////////////////////////////////////
//    File        	: TypeId.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Object;
class Component;

//////////////////////////////////////////////////////////////////////////

struct HashedString
{
public:
    static inline HashedString Create(const std::string& id)
    {
        return HashedString(id);
    }

public:
    HashedString() {}

protected:
    HashedString(const std::string& id);

public:
    inline bool operator==(const HashedString& other) const { return hashedId == other.hashedId; };
    inline bool operator<=(const HashedString& other) const { return hashedId <= other.hashedId; };
    inline bool operator<(const HashedString& other) const { return hashedId < other.hashedId; };

public:
    const std::string& GetStringId() const { return stringId; }
    const u64 GetHashedId() const { return hashedId; }

private:
    std::string stringId;
    u64 hashedId;
};

//////////////////////////////////////////////////////////////////////////

using ObjectId = HashedString;

//////////////////////////////////////////////////////////////////////////

struct ComponentId: public HashedString
{
public:
    static inline ComponentId Create(const ObjectId& ownerId, const std::string& id)
    {
        return ComponentId(ownerId, id);
    }

public:
    ComponentId() {}

private:
    ComponentId(const ObjectId& ownerId, const std::string& id)
        : HashedString(ownerId.GetStringId() + ":" + id)
    {

    }
};

//////////////////////////////////////////////////////////////////////////

// Specialize std::hash
namespace std
{
    template<> struct hash<HashedString>
    {
        size_t operator()(const HashedString& oid) const noexcept
        {
            return (size_t)oid.GetHashedId();
        }
    };

    template<> struct hash<ComponentId>
    {
        size_t operator()(const ComponentId& oid) const noexcept
        {
            return (size_t)oid.GetHashedId();
        }
    };
}

//////////////////////////////////////////////////////////////////////////

template<typename Value>
using ClassIdHashMap = std::unordered_map<ClassId, Value, std::hash<ClassId>>;

//////////////////////////////////////////////////////////////////////////

template<typename Value>
using ObjectIdHashMap = std::unordered_map<ObjectId, Value, std::hash<ObjectId>>;

using GenericObjectHashMap = ObjectIdHashMap<Ref<Object>>;

//////////////////////////////////////////////////////////////////////////

template<typename Value>
using ComponentHashMap = std::unordered_map<ComponentId, Value, std::hash<ComponentId>>;

using GenericComponentHashMap = ComponentHashMap<Ref<Component>>;

//////////////////////////////////////////////////////////////////////////
