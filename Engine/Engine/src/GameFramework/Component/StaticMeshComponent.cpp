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

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"
#include "Render/Platform/RenderPass/RenderPassLit.h"
#include "Render/Platform/RenderPass/RenderPassUnlit.h"

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

void StaticMeshComponent::OnConstruct()
{
    Super::OnConstruct();

    renderFunction.enabled = true;
    RenderPassManager::GetPassAsType<RenderPassUnlit>(RenderPassUnlit::Id)->AddRenderFunction(&renderFunction, false);
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride)
{
    Super::OnRender(pass, materialOverride);

    if (mesh != nullptr && material != nullptr)
    {
        fmat4 transform = CalculateTransformMatrix();
        mesh->Render(material, transform);
    }
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::SetMaterial(const std::string& name)
{
    material = MaterialLibrary::GetMaterial(name);
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::SetMaterial(Ref<Material> newMaterial)
{
    material = newMaterial;
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::SetMesh(const std::string& name)
{
    mesh = MeshLibrary::GetMesh(name);
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::SetMesh(Ref<Mesh> newMesh)
{
    mesh = newMesh;
}

//////////////////////////////////////////////////////////////////////////

void StaticMeshComponent::SetRenderPass(const HashedString& newRenderPassId)
{
}

//////////////////////////////////////////////////////////////////////////

Ref<Component> StaticMeshComponent::Create(Ref<Object> object, pugi::xml_node node)
{
    Ref<StaticMeshComponent> staticMesh = object->CreateComponent<StaticMeshComponent>("StaticMesh");

    return staticMesh;
}

//////////////////////////////////////////////////////////////////////////
