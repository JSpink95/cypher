//////////////////////////////////////////////////////////////////////////
//    File        	: ObjectManager.h
//    Created By    : Jack Spink
//    Created On 	: [21/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"
#include "Core/TypeId.h"
#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Object;

//////////////////////////////////////////////////////////////////////////

class ObjectManager : public Singleton<ObjectManager>
{
public:
    void RegisterObject(Ref<Object> newObject);
    void DeregisterObject(const ObjectGuid& guid);

    Ref<Object> GetStrongRef(const ObjectGuid& guid);
    WeakRef<Object> GetWeakRef(const ObjectGuid& guid);

public:
    virtual void OnDelete() override;

private:
    std::unordered_map<ObjectGuid, Ref<Object>> objects;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<ObjectManager> GetObjectManager() { return ObjectManager::Get(); }

//////////////////////////////////////////////////////////////////////////

template<typename TObject>
static inline Ref<TObject> CreateObject(const std::string& id = "")
{
    Ref<ObjectManager> manager = GetObjectManager();
    if (manager)
    {
        Ref<TObject> newObject = std::make_shared<TObject>();
        newObject->SetSelf(newObject);

        manager->RegisterObject(newObject);
        newObject->OnConstruct();

        return newObject;
    }
    
    printf("[ObjectManager] has not been initialised!\n");
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

static inline void DestroyObject(const ObjectGuid& guid, const std::string& id = "")
{
    Ref<ObjectManager> manager = GetObjectManager();
    if (manager)
    {
        Ref<Object> object = manager->GetStrongRef(guid);
        //object->OnDestroy(); #todo - implement

        manager->DeregisterObject(guid);
    }
}

//////////////////////////////////////////////////////////////////////////
