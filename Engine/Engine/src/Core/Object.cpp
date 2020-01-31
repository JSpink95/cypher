//////////////////////////////////////////////////////////////////////////
//    File        	: Object.cpp
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(Object, RTTIObject)
    RTTI_PROPERTY_MAP(Object, std::unordered_map, ComponentId, Ref<Component>, components)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

Object::Object()
{
}

//////////////////////////////////////////////////////////////////////////

Object::~Object()
{

}

//////////////////////////////////////////////////////////////////////////

void Object::OnConstruct()
{
    for (auto component : components)
        component.second->OnConstruct();
}

//////////////////////////////////////////////////////////////////////////

void Object::OnDestruct()
{
    for (auto component : components)
        component.second->OnDestruct();
}

//////////////////////////////////////////////////////////////////////////

void Object::OnUpdate(const f32 dt)
{
    for (auto component : components)
        component.second->OnUpdate(dt);
}

//////////////////////////////////////////////////////////////////////////

void Object::OnPhysicsUpdate(const f32 dt)
{
}

//////////////////////////////////////////////////////////////////////////

void Object::OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride/* = nullptr*/)
{
    for (auto component : components)
        component.second->OnRender(pass, materialOverride);
}

//////////////////////////////////////////////////////////////////////////

void Object::SetTickEnabled(bool const enabled)
{
    //if (enabled)
    //{
    //    GetGameThread()->AddObject(self);
    //}
    //else
    //{
    //    GetGameThread()->RemoveObject(self);
    //}
}

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    std::tuple<ComponentId, Ref<Component>> CreateComponentFromXML(const ObjectId& owner, pugi::xml_node node)
    {
        return { ComponentId::Create(owner, "invalid"), nullptr };
    }

    Ref<Object> LoadObjectFromXML(const std::string& filepath)
    {
        pugi::xml_document file;
        file.load_file(filepath.c_str());

        pugi::xml_node root = file.root();

        ObjectId id = ObjectId::Create(root.attribute("id").as_string());

        Ref<Object> object = std::make_shared<Object>();
        object->SetId(id);

        if (object == nullptr)
        {
            printf("Failed to create object! %s.\n", id.GetStringId());
            return nullptr;
        }

        for (pugi::xml_node node : root)
        {
            if (node.name() == "component")
            {
                const std::string classname = node.attribute("type").as_string("invalid");
                auto[componentId, component] = CreateComponentFromXML(id, node);

                object->components.emplace(componentId, component);
            }
        }

        object->OnConstruct();
        return object;
    }
}

//////////////////////////////////////////////////////////////////////////
