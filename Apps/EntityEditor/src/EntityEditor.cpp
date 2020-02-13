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

#include "Core/RTTI/RTTIEditorHelpers.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"
#include "Render/Platform/RenderPass/RenderPassParticle.h"
#include "Render/Platform/RenderPass/RenderPassShadow.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"
#include "GameFramework/Component/PerspectiveCameraComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"
#include "widgets/TextEditor.h"

//////////////////////////////////////////////////////////////////////////

void EntityEditorApplication::OnPostCreate()
{
    Application::OnPostCreate();

    window->Recentre();
    //window->SetWindowPosition(int2(-1920, 200));

    editor = std::make_shared<TextEditor>();
    editor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());

    Ref<RenderPassShadow> shadowPass = RenderPassManager::GetPassAsType<RenderPassShadow>(RenderPassShadow::Id);
    shadowPass->AddDirectionalLight("sun", float3(1.0f, 0.5f, 1.0f), 4.0f);

    light = CreateObject<Object>(ObjectId::Create("light"));
    //Ref<LightComponent> lightComponent = light->CreateComponent<LightComponent>("RootTransform");
    //lightComponent->position = float3(0.0f, 16.0f, 0.0f);

    controller = CreateObject<Object>(ObjectId::Create("controller"));

    Ref<TransformComponent> controllerTransform = controller->CreateComponent<TransformComponent>("RootTransform");
    controllerTransform->position = float3(8.0f);

    Ref<PerspectiveCameraComponent> controllerCamera = controller->CreateComponent<PerspectiveCameraComponent>("Camera");
    controllerCamera->SetTarget(float3(0.0f));
    controllerCamera->SetAsMainCamera();

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

    bool save = false;

    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("File"))
        {
            save = ImGui::MenuItem("Save...");
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    // save dialog
    {
    
        static const char* saveModal = "Save To File";
    
        if (save)
        {
            ImGui::OpenPopup(saveModal);
        }
    
        if (ImGui::BeginPopupModal(saveModal, (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Entity Name:");
            ImGui::SameLine();
    
            static char filenameBuffer[32] = "";
            ImGui::InputText("##filename_edit", filenameBuffer, 32);
    
            const bool saveItem = ImGui::Button("Save");
    
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
    
            ImGui::EndPopup();
        }
    }

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
            managed->OnConstruct();

            delete[] componentIdBuffer;
            componentIdBuffer = new char[32]{ '\0' };
        }

        ImGui::Separator();

        //RTTI::DisplayEdit<Ref<RTTIObject>>("Entity", std::dynamic_pointer_cast<RTTIObject>(entity));

        entity->ForEachComponent([](const ComponentId& id, Ref<Component> component) -> void
        {
            if (ImGui::TreeNode(id.GetStringId().c_str()))
            {
                TypeBase* type = TypeRegister::GetRegisteredType(component->GetTypeName());
                do {
                    const bool typeHasProperties = type->GetPropertyCount();
                    if (typeHasProperties && ImGui::TreeNode(type->GetTypeName().c_str()))
                    {
                        for (TypeBase::property_iterator it = type->property_begin(); it != type->property_end(); ++it)
                        {
                            it->second->DisplayEdit(component.get());
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

    editor->Render("Test");
}

//////////////////////////////////////////////////////////////////////////

void main()
{
    Application::Main<EntityEditorApplication>();
}

//////////////////////////////////////////////////////////////////////////
