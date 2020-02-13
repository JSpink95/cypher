//////////////////////////////////////////////////////////////////////////
//    File        	: MaterialEditor.cpp
//    Created By    : Jack Spink
//    Created On 	: [06/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "MaterialEditor.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"
#include "widgets/TextEditor.h"

//////////////////////////////////////////////////////////////////////////

static constexpr const s32 ImGui_BorderlessImmovableWindow = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

//////////////////////////////////////////////////////////////////////////

namespace ShaderInfo
{
    enum Enum : u8
    {
        Vertex, Geometry, Pixel, Max
    };

    std::string ToString(Enum e)
    {
        static std::unordered_map<Enum, std::string> strings =
        {
            { Vertex, "Vertex" },
            { Geometry, "Geometry" },
            { Pixel, "Pixel" },
        };

        auto it = strings.find(e);
        if (it != strings.end())
        {
            return it->second;
        }

        return "invalid";
    }
}

//////////////////////////////////////////////////////////////////////////

struct MenuStateFlags
{
    bool newFile : 1;
    bool loadFile : 1;
    bool saveFile : 1;
    s32 padding : 5;

    MenuStateFlags()
        : newFile(false)
        , loadFile(false)
        , saveFile(false)
    {}
};

//////////////////////////////////////////////////////////////////////////

struct MaterialDataMenuFlags
{
    bool addShader : 1;
    s32 padding : 7;

    MaterialDataMenuFlags()
        : addShader(false)
    {}
};

//////////////////////////////////////////////////////////////////////////

void MaterialEditorApplication::OnPostCreate()
{
    Application::OnPostCreate();

    window->Recentre();
    window->SetWindowPosition(int2(-1600, 180));

    editor = std::make_shared<TextEditor>();
    editor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

//////////////////////////////////////////////////////////////////////////

void MaterialEditorApplication::OnImGuiRender()
{
    Application::OnImGuiRender();

    MenuStateFlags menuStateFlags;
    MaterialDataMenuFlags materialDataMenuFlags;
    ShaderInfo::Enum previousActiveShader = activeShaderEdit;

    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("File"))
        {
            /**

            *---------*
            | New...  |
            |---------|
            | Load... |
            | Save... |
            *---------*
            
            */
            menuStateFlags.newFile = ImGui::MenuItem("New...");

            ImGui::Separator();

            menuStateFlags.loadFile = ImGui::MenuItem("Load...");
            menuStateFlags.saveFile = ImGui::MenuItem("Save...");

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);

    ImGui::SetNextWindowPos(ImVec2(4, 22));
    if (ImGui::Begin("##material_edit", (bool*)0, ImGui_BorderlessImmovableWindow))
    {
        ImGui::Combo("##shadertypes", &selectedShaderDropDown, "Vertex\0Geometry\0Pixel\0\0");
        ImGui::SameLine();
        materialDataMenuFlags.addShader = ImGui::Button("Add Shader");

        ImGui::Separator();

        for (auto& pair: sources)
        {
            ImGui::Text((ShaderInfo::ToString(pair.first) + ":").c_str());
            ImGui::SameLine();
            if (ImGui::Selectable("Edit"))
            {
                activeShaderEdit = pair.first;
            }
        }
    }
    ImGui::End();

    if (materialDataMenuFlags.addShader && selectedShaderDropDown != -1)
    {
        ShaderInfo::Enum newShader = (ShaderInfo::Enum)selectedShaderDropDown;
        sources.emplace(newShader, "");
    }

    if (activeShaderEdit != previousActiveShader)
    {

    }

    ImGui::PopStyleVar();
}

//////////////////////////////////////////////////////////////////////////

void main()
{
    Application::Main<MaterialEditorApplication>();
}

//////////////////////////////////////////////////////////////////////////
