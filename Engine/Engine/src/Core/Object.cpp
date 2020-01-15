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
    if (enabled)
    {
        GetGameThread()->AddObject(self);
    }
    else
    {
        GetGameThread()->RemoveObject(self);
    }
}

//////////////////////////////////////////////////////////////////////////

namespace ImGui
{
    void ShowObjectProperties(Ref<Object> object)
    {
        if (object == nullptr)
            return;

        auto foreachComponent = [object](ComponentId id, Ref<Component> component) -> void
        {
            if (component == nullptr)
                return;

            const std::string componentId = component->GetId().GetStringId();
            const std::string trimmedId = componentId.substr(componentId.find_first_of(":") + 1);

            if (ImGui::TreeNode(trimmedId.c_str()))
            {
                if (BaseType* type = TypeRegister::GetRegisteredType(component->GetTypeName()))
                {
                    for (BaseType::iterator it = type->begin(); it != type->end(); ++it)
                    {
                        it->second->ShowEditBox(it->first, (void*)component.get());
                    }
                }

                ImGui:TreePop();
            }
        };

        if (ImGui::TreeNode(object->GetId().GetStringId().c_str()))
        {
            object->ForEachComponent(foreachComponent);
            ImGui::TreePop();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
