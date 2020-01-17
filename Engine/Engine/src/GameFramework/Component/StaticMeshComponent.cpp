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

#include "Render/Platform/Renderer.h"
#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(StaticMeshComponent, TransformComponent)
    RTTI_PROPERTY(StaticMeshComponent, Ref<Material>, material)
    RTTI_PROPERTY(StaticMeshComponent, Ref<VertexArray>, mesh)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

StaticMeshComponent::StaticMeshComponent()
{
    parentTransform.componentName = "RootComponent";
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::OnConstruct()
{
    Super::OnConstruct();
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::OnDestruct()
{
    Super::OnDestruct();

    if (mesh != nullptr)
    {
        mesh->OnDestroy();
        mesh = nullptr;
    }

    if (material != nullptr)
    {
        material->OnDestroy();
        material = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride)
{
    Super::OnRender(pass, materialOverride);

    fmat4 transform = CalculateTransformMatrix();
    if (mesh != nullptr && material != nullptr)
    {
        Renderer::Submit(material, mesh, transform);
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<Component> StaticMeshComponent::Create(Ref<Object> object, pugi::xml_node node)
{
    Ref<StaticMeshComponent> staticMesh = object->CreateComponent<StaticMeshComponent>("StaticMesh");

    return staticMesh;
}

//////////////////////////////////////////////////////////////////////////
