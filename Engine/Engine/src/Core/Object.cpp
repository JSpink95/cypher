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

#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

//////////////////////////////////////////////////////////////////////////

#include <fstream>

//////////////////////////////////////////////////////////////////////////

#define BINARY_SERIALISATION_VERSION 1u

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ObjectTickFunction, TickFunction)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(Object, RTTIObject)
    RTTI_PROPERTY_MAP(Object, std::unordered_map, ComponentId, Ref<Component>, components)
    RTTI_PROPERTY(Object, ObjectTickFunction, tickFunction)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void ObjectTickFunction::ExecuteTick(const f32 dt)
{
    Super::ExecuteTick(dt);

    if (object != nullptr)
    {
        object->OnTick(dt);
    }
}

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
    tickFunction.object = this;
    GameThread::RegisterTickFunction(&tickFunction);

    for (auto component : components)
        component.second->OnConstruct();
}

//////////////////////////////////////////////////////////////////////////

void Object::OnDestruct()
{
    GameThread::DeregisterTickFunction(&tickFunction);

    for (auto component : components)
        component.second->OnDestruct();
}

//////////////////////////////////////////////////////////////////////////

void Object::OnTick(const f32 dt)
{

}

//////////////////////////////////////////////////////////////////////////

void Object::OnPhysicsUpdate(const f32 dt)
{
}

//////////////////////////////////////////////////////////////////////////

void Object::OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride/* = nullptr*/)
{
}

//////////////////////////////////////////////////////////////////////////

void Object::AddComponent(Ref<Component> component)
{
    components.emplace(component->GetId(), component);
}

//////////////////////////////////////////////////////////////////////////

void Object::SetTickEnabled(const bool enabled)
{
    tickFunction.enabled = enabled;
}

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    //////////////////////////////////////////////////////////////////////////

    std::tuple<ComponentId, Ref<Component>> CreateComponentFromXML(Object* owner, pugi::xml_node node)
    {
        const std::string classname = node.attribute("type").as_string("invalid");
        const std::string name = node.attribute("name").as_string("invalid");

        TypeBase* type = TypeRegister::GetRegisteredType(classname);
        if (type == nullptr)
        {
            LOG_ERROR("Type does not exist %s\n", classname.c_str());
            return { ComponentId::Create(owner->GetId(), "invalid"), nullptr };
        }

        if (!type->IsTypeOf(Component::ClassName))
        {
            LOG_ERROR("Type is not typeof Component %s\n", classname.c_str());
            return { ComponentId::Create(owner->GetId(), "invalid"), nullptr };
        }
        
        Component* component = (Component*)type->New();

        Ref<Component> refComponent;
        refComponent.reset(component);

        refComponent->SetOwner(owner);
        refComponent->SetId(name);

        for (pugi::xml_node propertyNode : node)
        {
            // set property values here
            const std::string propertyName = propertyNode.attribute("name").as_string();
            const std::string propertyValue = propertyNode.attribute("value").as_string();

            if (PropertyBase* prop = type->FindProperty(propertyName))
            {
                prop->SetValue(component, propertyValue);
            }
        }

        return { refComponent->GetId(), refComponent };
    }

    //////////////////////////////////////////////////////////////////////////

    Ref<Object> LoadObjectFromXML(const std::string& filepath)
    {
        const std::string realpath = FileVolumeManager::GetRealPathFromVirtualPath(filepath).fullpath;

        pugi::xml_document file;
        file.load_file(realpath.c_str());

        pugi::xml_node root = file.root().child("object");

        ObjectId id = ObjectId::Create(root.attribute("id").as_string());

        Ref<Object> object = std::make_shared<Object>();
        object->SetId(id);

        if (object == nullptr)
        {
            LOG_ERROR("Failed to create object! %s\n", id.GetStringId().c_str());
            return nullptr;
        }

        for (pugi::xml_node node : root)
        {
            const std::string nodename = node.name();
            if (nodename == "component")
            {
                auto[componentId, component] = CreateComponentFromXML(object.get(), node);
                object->components.emplace(componentId, component);
            }
        }

        object->OnConstruct();
        return object;
    }

    //////////////////////////////////////////////////////////////////////////

    void SaveComponentToXML(pugi::xml_node& node, Ref<Component> component)
    {
        const std::string classname = component->GetTypeName();
        const std::string componentName = component->GetInstanceName();

        node.append_attribute("type").set_value(classname.c_str());
        node.append_attribute("name").set_value(componentName.c_str());

        TypeBase* type = TypeRegister::GetRegisteredType(classname);

        do {

            for (TypeBase::property_iterator it = type->property_begin(); it != type->property_end(); ++it)
            {
                pugi::xml_node prop = node.append_child("property");
                prop.append_attribute("name").set_value(it->first.c_str());
                prop.append_attribute("value").set_value(it->second->ToString(component.get()).c_str());
            }

            type = type->GetBaseType();
        } while (type != nullptr);
    }

    //////////////////////////////////////////////////////////////////////////

    void SaveObjectToXML(Ref<Object> object, const std::string& filepath)
    {
        const std::string realpath = FileVolumeManager::GetRealPathFromVirtualPath(filepath).fullpath;

        pugi::xml_document file;

        pugi::xml_node root = file.root().append_child("object");
        root.append_attribute("id").set_value(object->GetId().GetStringId().c_str());

        object->ForEachComponent([&](const ComponentId& id, Ref<Component> component) -> void
        {
            pugi::xml_node node = root.append_child("component");
            SaveComponentToXML(node, component);
        });

        file.save_file(realpath.c_str());
    }

    //////////////////////////////////////////////////////////////////////////

    // 32 bits
    struct BinaryHeader
    {
        u32 version;
    };

    //////////////////////////////////////////////////////////////////////////

    // 92 bits
    struct BinaryComponentHeader
    {
        u32 version, byteSize;
        char classname[64];
    };

    //////////////////////////////////////////////////////////////////////////

    template<typename T> void GenericBinaryWriteToFile(std::ofstream& stream, T* value)
    {
        stream.write(reinterpret_cast<const char*>(value), sizeof(T));
    }

    //////////////////////////////////////////////////////////////////////////

    void SaveObjectToBinary(Ref<Object> object, const std::string& filepath)
    {
        const std::string realpath = FileVolumeManager::GetRealPathFromVirtualPath(filepath).fullpath;

        BinaryHeader header;
        header.version = BINARY_SERIALISATION_VERSION;

        std::ofstream file(realpath.c_str(), std::ios::binary);
        if (file.is_open())
        {
            GenericBinaryWriteToFile<BinaryHeader>(file, &header);

            object->ForEachComponent([&](const ComponentId& id, Ref<Component> component)
            {
                BinaryComponentHeader componentHeader;
                componentHeader.version = BINARY_SERIALISATION_VERSION;
                componentHeader.byteSize = component->GetByteSize();
                std::memcpy(componentHeader.classname, component->GetTypeName().c_str(), glm::min(64u, component->GetTypeName().length()));

                GenericBinaryWriteToFile<BinaryComponentHeader>(file, &componentHeader);

                const char* bytes = reinterpret_cast<const char*>(component.get());
                file.write(bytes, component->GetByteSize());
            });

            file.close();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
