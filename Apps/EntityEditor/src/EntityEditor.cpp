//////////////////////////////////////////////////////////////////////////
//    File        	: EntityEditorApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [06/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "EntityEditor.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/Components.h"
#include "Core/ObjectManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

//////////////////////////////////////////////////////////////////////////

void EntityEditorApplication::OnPostCreate()
{
    Application::OnPostCreate();

    window->SetWindowPosition(int2(1920, 200));

    entity = CreateObject<Object>(ObjectId::Create("entity"));

    Ref<TransformComponent> transform = entity->CreateComponent<TransformComponent>("RootTransform");
}

//////////////////////////////////////////////////////////////////////////

void EntityEditorApplication::OnImGuiRender()
{
    Application::OnImGuiRender();

    if (entity == nullptr)
    {
        return;
    }

    std::vector<TypeBase*> registeredComponentList;
    TypeRegister::GetRegisteredTypesOfBase<Component>(registeredComponentList);

    std::vector<std::string> registeredComponentListNames;
    auto componentTypeToNameTransfomer = [](TypeBase* type) -> std::string { return type->GetTypeName(); };
    std::transform(registeredComponentList.begin(), registeredComponentList.end(), std::back_inserter(registeredComponentListNames), componentTypeToNameTransfomer);

    auto vectorStringGetter = [](void* data, int idx, const char** out) -> bool
    {
        std::vector<std::string>* strings = reinterpret_cast<std::vector<std::string>*>(data);
        if (idx < strings->size() && idx >= 0)
        {
            *out = strings->at(idx).c_str();
            return true;
        }

        return false;
    };

    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("File"))
        {
            const bool save = ImGui::MenuItem("Save...");
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);
    ImGui::SetNextWindowPos(ImVec2(4, 22));
    if (ImGui::Begin("Entity", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
    {
        static char* componentIdBuffer = new char[32]{ '\0' };
        ImGui::InputText("Id", componentIdBuffer, 32);

        static int selected = 0;
        ImGui::Combo("##component_list", &selected, vectorStringGetter, &registeredComponentListNames, registeredComponentListNames.size());

        ImGui::SameLine();
        const bool addComponent = ImGui::Button("+##add_component");

        if (addComponent && componentIdBuffer[0] != '\0')
        {
            TypeBase* componentType = registeredComponentList.at(selected);
            Component* component = reinterpret_cast<Component*>(componentType->New());
            component->SetOwner(entity.get());
            component->SetId(componentIdBuffer);

            Ref<Component> managed;
            managed.reset(component);

            entity->AddComponent(managed);

            delete[] componentIdBuffer;
            componentIdBuffer = new char[32]{ '\0' };
        }

        ImGui::Separator();

        entity->ForEachComponent([](const ComponentId& id, Ref<Component> component) -> void
        {
            if (ImGui::TreeNode(id.GetStringId().c_str()))
            {
                TypeBase* type = TypeRegister::GetRegisteredType(component->GetTypeName());
                do {
                    const bool typeHasProperties = true;
                    if (typeHasProperties && ImGui::TreeNode(type->GetTypeName().c_str()))
                    {
                        for (TypeBase::property_iterator it = type->property_begin(); it != type->property_end(); ++it)
                        {
                            it->second->DisplayEditBox(component.get());
                        }

                        ImGui::TreePop();
                    }

                    type = type->GetBaseType();
                } while (type != nullptr);

                ImGui::TreePop();
            }
        });
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

//////////////////////////////////////////////////////////////////////////

void main()
{
    Application::Main<EntityEditorApplication>();
}

//////////////////////////////////////////////////////////////////////////
