//////////////////////////////////////////////////////////////////////////
//    File        	: StaticMeshComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/StaticMeshComponent.h"
#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/Thread/GameThread.h"
#include "Render/Platform/Renderer.h"
#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::OnConstruct()
{
    Super::OnConstruct();

    ownerTransform = owner->FindFirstComponentOfType<TransformComponent>();
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

    fmat4 transform(1.0f);
    if (!ownerTransform.expired())
    {
        transform = ownerTransform.lock()->CalculateTransformMatrix();
    }

    transform = glm::scale(transform, scale);

    if (mesh != nullptr && material != nullptr)
    {
        Renderer::Submit(material, mesh, transform);
    }
}

//////////////////////////////////////////////////////////////////////////
