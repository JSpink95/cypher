//////////////////////////////////////////////////////////////////////////
//    File        	: Serialiser.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

#include <functional>

//////////////////////////////////////////////////////////////////////////

class Object;
class Component;

//////////////////////////////////////////////////////////////////////////

using ComponentDeserialiseFunction = std::function<Ref<Component>(Ref<Object>, const std::string&, pugi::xml_node)>;

//////////////////////////////////////////////////////////////////////////

class Serialiser: public Singleton<Serialiser>
{
public:

    static inline void RegisterComponentDeserialiseFunction(const std::string& classId, ComponentDeserialiseFunction function)
    {
        if (Ref<Serialiser> serialiser = Get())
        {
            serialiser->RegisterComponentDeserialiseFunctionImpl(classId, function);
        }
    }

    static inline Ref<Object> LoadObjectFromBlueprint(const std::string& filepath)
    {
        if (Ref<Serialiser> serialiser = Get())
        {
            return serialiser->LoadObjectFromBlueprintImpl(filepath);
        }

        return nullptr;
    }

private:
    void RegisterComponentDeserialiseFunctionImpl(const std::string& classId, ComponentDeserialiseFunction function);
    Ref<Object> LoadObjectFromBlueprintImpl(const std::string& filepath);

private:
    std::unordered_map<std::string, ComponentDeserialiseFunction> deserialisationFunctions;
};

//////////////////////////////////////////////////////////////////////////
