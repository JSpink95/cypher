//////////////////////////////////////////////////////////////////////////
//    File        	: Serialiser.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/Serialise/Serialiser.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/ObjectManager.h"

//////////////////////////////////////////////////////////////////////////

void Serialiser::RegisterComponentDeserialiseFunctionImpl(const std::string& classId, ComponentDeserialiseFunction function)
{
    if (deserialisationFunctions.find(classId) == deserialisationFunctions.end())
    {
        deserialisationFunctions.emplace(classId, function);
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<Object> Serialiser::LoadObjectFromBlueprintImpl(const std::string& filepath)
{
    pugi::xml_document document;
    auto result = document.load_file(filepath.c_str());

    if (!result)
    {
        // error
        return nullptr;
    }

    pugi::xml_node root = document.root();
    const std::string id = root.attribute("id").as_string("-error-id");

    Ref<Object> object = CreateObject<Object>(ObjectId::Create(id));

    for (auto element : root)
    {
        const std::string elementName = element.name();

        if (elementName == "component")
        {
            const std::string componentId = element.attribute("id").as_string("-error-id");
            const std::string componentType = element.attribute("type").as_string("-error-id");

            auto it = deserialisationFunctions.find(componentType);
            if (it != deserialisationFunctions.end())
            {
                Ref<Component> createdComponent = deserialisationFunctions.at(componentType)(object, componentId, element);
            }
        }
    }

    return object;
}

//////////////////////////////////////////////////////////////////////////
