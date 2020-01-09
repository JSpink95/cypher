//////////////////////////////////////////////////////////////////////////
//    File        	: EventDispatcher.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/TypeId.h"

//////////////////////////////////////////////////////////////////////////

#include <functional>

//////////////////////////////////////////////////////////////////////////

// 
// the template argument TObject MUST support these functions:
// - GetInstanceId() -> ObjectId;
// 

template<typename TObject, typename TPredicate, u32 ParameterCount>
class EventDispatcher
{
public:
    static_assert(ParameterCount < 4u, "EventDispatcher::ParamterCount only supports a maximum of 3!");

    using HashMap = ObjectIdHashMap<TPredicate>;

public:
    bool IsRegisteredForEvent(TObject* object) const;

    template<typename TClassMemberFunction>
    void AddListener(TObject* object, TClassMemberFunction predicate);

    void RemoveListener(TObject* object);

    template<typename...TArgs>
    void Dispatch(TArgs... args)
    {
        for (auto& kv : listeners)
        {
            kv.second(args...);
        }
    }

private:
    HashMap listeners;
};

//////////////////////////////////////////////////////////////////////////

template<typename TObject, typename TPredicate, u32 ParameterCount>
inline bool EventDispatcher<TObject, TPredicate, ParameterCount>::IsRegisteredForEvent(TObject* object) const
{
    return object != nullptr && (listeners.find(object->GetId()) != listeners.end());
}

//////////////////////////////////////////////////////////////////////////

template<typename TObject, typename TPredicate, u32 ParameterCount>
template<typename TClassMemberFunction>
inline void EventDispatcher<TObject, TPredicate, ParameterCount>::AddListener(TObject* object, TClassMemberFunction predicate)
{
    if (!IsRegisteredForEvent(object))
    {
        if constexpr (ParameterCount == 0u)
        {
            TPredicate func = std::bind(predicate, object);
            listeners.emplace(std::make_pair(object->GetId(), func));
        }

        if constexpr (ParameterCount == 1u)
        {
            TPredicate func = std::bind(predicate, object, std::placeholders::_1);
            listeners.emplace(std::make_pair(object->GetId(), func));
        }

        if constexpr (ParameterCount == 2u)
        {
            TPredicate func = std::bind(predicate, object, std::placeholders::_1, std::placeholders::_2);
            listeners.emplace(std::make_pair(object->GetId(), func));
        }

        if constexpr (ParameterCount == 3u)
        {
            TPredicate func = std::bind(predicate, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            listeners.emplace(std::make_pair(object->GetId(), func));
        }
    }
}

//////////////////////////////////////////////////////////////////////////

template<typename TObject, typename TPredicate, u32 ParameterCount>
inline void EventDispatcher<TObject, TPredicate, ParameterCount>::RemoveListener(TObject* object)
{
    if (IsRegisteredForEvent(object))
    {
        listeners.erase(object->GetId());
    }
}

//////////////////////////////////////////////////////////////////////////
