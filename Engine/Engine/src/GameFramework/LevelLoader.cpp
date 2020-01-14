//////////////////////////////////////////////////////////////////////////
//    File        	: LevelLoader.cpp
//    Created By    : Jack Spink
//    Created On 	: [23/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/LevelLoader.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/ObjectManager.h"
#include "Core/Utility/ParseUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/PhysicsObject.h"
#include "Physics/Geometry/BoxCollisionShape.h"
#include "Physics/Geometry/SphereCollisionShape.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Effects/LightManager.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"
#include "GameFramework/Component/LightComponent.h"
#include "GameFramework/Component/TransformComponent.h"
#include "GameFramework/Component/StaticMeshComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

#include <functional>

//////////////////////////////////////////////////////////////////////////

void ParseRenderPassString(const std::string& rp, std::vector<RenderPassType::Enum>& passes)
{
    static std::unordered_map<std::string, RenderPassType::Enum> stringToType = 
    {
        std::make_pair("opaque", RenderPassType::Opaque),
        std::make_pair("transparent", RenderPassType::Transparent),
        std::make_pair("shadow", RenderPassType::Shadow),
        std::make_pair("particle", RenderPassType::Particle),
    };

    std::string pass = "";
    for (u32 idx = 0; idx < rp.length(); ++idx)
    {
        const char c = rp.at(idx);
        if (c != '|')
            pass += c;

        if (c == '|' || idx == (rp.length() - 1))
        {
            auto it = stringToType.find(pass);
            if (it != stringToType.end())
                passes.push_back(it->second);

            pass = "";
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void ReadObject(pugi::xml_node data, Ref<GameObject> o)
{
    for (pugi::xml_node node : data)
    {
        const std::string nodeName = node.name();
        if (nodeName == "renderpass")
        {
            const std::string renderpass = node.attribute("value").as_string();
            std::vector<RenderPassType::Enum> passes;
            ParseRenderPassString(renderpass, passes);

            for (RenderPassType::Enum rp : passes)
            {
                RenderPassManager::AddObjectToPass(rp, o);
            }
        }
        else if (nodeName == "transform")
        {
            o->transform->position = ParseFloat3(node.attribute("position").as_string());
            o->transform->rotation = ParseFloat3(node.attribute("rotation").as_string());
        }
        else if (nodeName == "staticmesh")
        {
            Ref<StaticMeshComponent> staticmesh = o->CreateComponent<StaticMeshComponent>("");
            staticmesh->SetMesh(MeshLibrary::GetMesh(node.attribute("mesh").as_string()));
            staticmesh->SetMaterial(MaterialLibrary::GetMaterial(node.attribute("material").as_string()));
            staticmesh->SetScale(ParseFloat3(node.attribute("scale").as_string()));
        }
        else if (nodeName == "light")
        {
            Ref<LightComponent> light = o->CreateComponent<LightComponent>("");
            light->GetLightInstance()->SetColor(ParseFloat4(node.attribute("color").as_string()));
            light->GetLightInstance()->SetRadius(node.attribute("radius").as_float());
        }
        else if (nodeName == "particlesystem")
        {
            // #todo - create particle system
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void ReadPhysicsObject(pugi::xml_node data, Ref<PhysicsObject> po)
{
    ReadObject(data, po);

    auto parseCollisionShape = [](pugi::xml_node node) -> Ref<BaseCollisionShape>
    {
        const std::string type = node.attribute("type").as_string();
        if (type == "box")
        {
            return std::make_shared<BoxCollision>(ParseFloat3(node.attribute("size").as_string()));
        }
        else if (type == "sphere")
        {
            return std::make_shared<SphereCollision>(node.attribute("radius").as_float());
        }

        return nullptr;
    };

    for (pugi::xml_node node : data)
    {
        const std::string nodeName = node.name();
        if (nodeName == "collision")
        {
            f32 mass = node.attribute("mass").as_float();
            Ref<BaseCollisionShape> collision = parseCollisionShape(node);

            po->Initialise(mass, po->transform->position, po->transform->rotation, collision);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

bool ReadEntity(pugi::xml_node node, Ref<Object> object)
{
    using ComponentCreator = std::function<Ref<Component>(Ref<Object>, pugi::xml_node)>;
    static std::unordered_map<std::string, ComponentCreator> componentCreatorFactory =
    {
        std::make_pair("TransformComponent", TransformComponent::Create),
        std::make_pair("StaticMeshComponent", StaticMeshComponent::Create),
        //std::make_pair("RigidBodyComponent", StaticMeshComponent::Create),
    };

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(node.attribute("blueprint").as_string());
    if (!result)
        return false;

    pugi::xml_node root = doc.root().child("blueprint");
    for (pugi::xml_node node : root)
    {
        const std::string name = node.name();
        if (name == "update")
        {

        }
        else if (name == "render")
        {

        }
        else if (name == "component")
        {
            const std::string id = node.attribute("id").as_string();
            const std::string componentType = node.attribute("type").as_string();

            auto it = componentCreatorFactory.find(componentType);
            if (it != componentCreatorFactory.end())
            {
                Ref<Component> component = it->second(object, node);
            }
        }
        else
        {
#if DEBUG
            printf("Unrecognised blueprint property type %s.\n", name.c_str());
#endif
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void LevelLoader::LoadFromFile(const std::string& filepath, Level& level)
{

    int id = 0;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());
    if (result)
    {
        pugi::xml_node root = doc.root().child("map");
        for (pugi::xml_node node : root)
        {
            const std::string nodeName = node.name();
            if (nodeName == "spawnpoint")
            {
                level.spawnpoint = ParseFloat3(node.attribute("value").as_string());
            }
            else if (nodeName == "gameobject")
            {
                Ref<GameObject> newObject = CreateObject<GameObject>(ObjectId::Create(std::to_string(id++)));
                ReadObject(node, newObject);

                level.objects.push_back(newObject);
            }
            else if (nodeName == "physicsobject")
            {
                Ref<PhysicsObject> newObject = CreateObject<PhysicsObject>(ObjectId::Create(std::to_string(id++)));
                ReadPhysicsObject(node, newObject);

                level.objects.push_back(newObject);
            }
            else if (nodeName == "entity")
            {
                Ref<Object> object = CreateObject<Object>(ObjectId::Create(std::to_string(id++)));
                if (ReadEntity(node, object))
                {
                    level.objects.push_back(object);
                }

            }
            else
            {
                printf("Invalid nodeName '%s'.\n", nodeName.c_str());
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
