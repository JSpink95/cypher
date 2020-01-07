//////////////////////////////////////////////////////////////////////////
//    File        	: TypeId.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <guiddef.h>
#include <type_traits>
#include <cstdint>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

using ObjectGuid = GUID;

//////////////////////////////////////////////////////////////////////////

struct ObjectId
{
    const std::string stringId;
    const u64 hashedId;

    ObjectId(const std::string& id);
    inline bool operator==(const ObjectId& other) const { return hashedId == other.hashedId; };
    inline bool operator<=(const ObjectId& other) const { return hashedId <= other.hashedId; };
    inline bool operator<(const ObjectId& other) const { return hashedId < other.hashedId; };
};

//////////////////////////////////////////////////////////////////////////

// Specialize std::hash
namespace std
{
    template<> struct hash<GUID>
    {
        size_t operator()(const GUID& guid) const noexcept
        {
#ifdef _WIN64
            using guid_alignment = std::uint64_t;
#elif defined(_WIN32)
            using guid_alignment = std::uint32_t;
#endif
            static_assert(std::alignment_of<GUID>::value >= std::alignment_of<guid_alignment>::value, "GUID Not aligned to correct size");

            const guid_alignment* p = reinterpret_cast<const guid_alignment*>(&guid);
            std::hash<guid_alignment> hash;
            return hash(p[0]) ^ hash(p[1]);
        }
    };

    template<> struct hash<ObjectId>
    {
        size_t operator()(const ObjectId& oid) const noexcept
        {
            return (u32)oid.hashedId;
        }
    };
}

//////////////////////////////////////////////////////////////////////////

template<typename Value>
using ClassIdHashMap = std::unordered_map<ClassId, Value, std::hash<ClassId>>;

//////////////////////////////////////////////////////////////////////////

template<typename Value>
using ObjectIdHashMap = std::unordered_map<ObjectGuid, Value, std::hash<ObjectGuid>>;

//////////////////////////////////////////////////////////////////////////
