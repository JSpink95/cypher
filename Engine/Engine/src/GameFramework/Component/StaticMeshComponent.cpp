//////////////////////////////////////////////////////////////////////////
//    File        	: StaticMeshComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/StaticMeshComponent.h"
#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/RTTI/RTTI.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(StaticMeshComponent, TransformComponent)
    RTTI_PROPERTY(StaticMeshComponent, Ref<Material>, material)
    RTTI_PROPERTY(StaticMeshComponent, Ref<Mesh>, mesh)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

StaticMeshComponent::StaticMeshComponent()
{
    parentTransform.componentName = "RootTransform";
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride)
{
    Super::OnRender(pass, materialOverride);

    fmat4 transform = CalculateTransformMatrix();
    if (mesh != nullptr)
    {
        mesh->Render(material, transform);
    }
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::SetMesh(const std::string& name)
{
    mesh = MeshLibrary::GetMesh(name);
}

//////////////////////////////////////////////////////////////////////////

Ref<Component> StaticMeshComponent::Create(Ref<Object> object, pugi::xml_node node)
{
    Ref<StaticMeshComponent> staticMesh = object->CreateComponent<StaticMeshComponent>("StaticMesh");

    return staticMesh;
}

//////////////////////////////////////////////////////////////////////////
